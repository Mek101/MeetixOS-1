//
// Created by mek101 on 10/03/22.
//

#pragma once

#include <functional>

#include <LibTC/Collection/Vector.hh>

#include "ElfLoader.hh"


namespace {
    template<typename T>
    ElfLoadResult<void> map_to_with(FileHandle fd, i64 file_offset, usize object_size, u8* buf) {
        auto seek_res = s_seek(fd, file_offset, FsSeekMode::FS_SEEK_SET);
        if ( seek_res == -1 ) {
            return ElfLoadError::IOError;
        }
        auto read_res = s_read(fd, buf, object_size);
        if ( read_res < 0 ) {
            return ElfLoadError::IOError;
        } else if ( read_res != object_size ) {
            return ElfLoadError::SizeError;
        }
    }
}

template<typename T>
ElfLoadResult<std::unique_ptr<T>> map_to_struct(FileHandle fd, i64 file_offset) {
    auto buf = new u8[sizeof(T)];
    auto map_res = map_to_with<T>(fd, file_offset, sizeof(T), buf);
    if ( map_res.is_error() ) {
        delete[] buf;
        return map_res.unwrap_error();
    }
    return std::unique_ptr<T> { reinterpret_cast<T*>(buf) };
}

template<typename T>
ElfLoadResult<void> map_to_struct_with(FileHandle fd, i64 file_offset, u8* buf) {
    auto map_res = map_to_with<T>(fd, file_offset, sizeof(T), buf);
    if ( map_res.is_error() ) {
        return map_res.unwrap_error();
    }
    return {};
}

template<typename T>
ElfLoadResult<std::unique_ptr<T[]>> map_to_table(FileHandle fd, i64 file_offset, u16 entry_size, u16 entry_count) {
    auto byte_size = entry_size * entry_count;
    auto buf = new u8[byte_size];
    auto map_res = map_to_with<T>(fd, file_offset, byte_size, buf);
    if ( map_res.is_error() ) {
        delete[] buf;
        return map_res.unwrap_error();
    }
    return std::unique_ptr<T[]> { reinterpret_cast<T[]>(buf) };
}

/*template<typename T>
class HeaderIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = ElfLoadResult<T>;
    using pointer           = ElfLoadResult<T>*;
    using reference         = ElfLoadResult<T>&;

    explicit HeaderIterator(std::unique_ptr<T[]> header_table_offsets, u16 headers_count, i32 fd, usize file_size) :
        HeaderIterator(std::shared_ptr { header_table_offsets }, headers_count, 0, fd, file_size) {};

    HeaderIterator(const HeaderIterator<T>& other) = default;

    reference operator*() const { return *this->buffer; }
    pointer operator->() { return this->buffer; }

    // Pre increment
    HeaderIterator& operator++() { this->load_next(); return *this; }

    // Postfix increment
    HeaderIterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }

    friend bool operator== (const HeaderIterator<T>& a, const HeaderIterator<T>& b) noexcept {
        return a.header_current == b.header_current &&
               a.headers_count == b.headers_count &&
               a.fd == b.fd &&
               a.file_size == b.file_size &&
               a.headers_tab == b.headers_tab;
    }

    friend bool operator!= (const HeaderIterator<T>& a, const HeaderIterator<T>& b) noexcept {
        return !operator==(a, b);
    }

    HeaderIterator begin() {
        return HeaderIterator { this->headers_tab, this->headers_count, 0 };
    }

    HeaderIterator end() {
        return HeaderIterator { this->headers_tab, this->headers_count, this->headers_count };
    }

private:
    std::shared_ptr<T[]> headers_tab; // Shared because the iterator must be copiable.
    i32 fd;
    usize file_size;
    u16 headers_count;
    u16 header_current;
    ElfLoadResult<T> buffer;

    explicit HeaderIterator(std::shared_ptr<T[]> header_table_offsets, u16 headers_count, u16 current, i32 fd, usize file_size) {
        this->headers_tab = std::shared_ptr { header_table_offsets };
        this->headers_count = headers_count;
        this->header_current = current;
        this->fd = fd;
        this->file_size = file_size;
    }

    void load_next() {
        // Get address inside the vector into which load the segment.
        auto load_address = reinterpret_cast<u8*>(buffer.value());
        // Load the segment into the vector buffer.
        this->buffer = map_to_struct_with<T>(fd, this->headers_tab[this->header_current], load_address);

        this->headers_current++;
    }
};*/


template<typename T>
ElfLoadResult<TC::Collection::Vector<T>> parse_headers(FileHandle fd, u32 header_table_offset, u16 entry_size, u16 entry_count, usize file_size, std::function<bool(const T&)> validator, std::function<const bool(const T&, const T&)> sorter) {
    if ( entry_size != sizeof (u32) ) {
        return ElfLoadError::UnsupportedElfError;
    }

    // Parse the program header table
    auto header_table_res = map_to_table<u32>(fd, header_table_offset, entry_size, entry_count);
    if ( header_table_res.is_error() ) {
        return header_table_res.unwrap_error();
    }
    auto header_table = header_table_res.unwrap_value();

    // Sort for faster access.
    std::sort(header_table.get(), header_table.get() + entry_count);

    auto headers_buffer = new T[entry_count];
    usize valid_headers  = 0;

    for ( u32 i = 0; i < entry_count; i++ ) {
        auto offset = header_table[i];
        if ( offset > file_size ) {
            delete[] headers_buffer;
            return ElfLoadError::SizeError;
        }

        // Calculate the address inside the buffer into which load the segment.
        auto load_address = reinterpret_cast<u8*>(&headers_buffer[valid_headers]);
        // Load the segment into the buffer.
        auto entry_res = map_to_struct_with<T>(fd, offset, load_address);
        if ( entry_res.is_error() ) {
            delete[] headers_buffer;
            return entry_res.unwrap_error();
        }

        // Filter the headers.
        if ( validator(headers_buffer[valid_headers + 1]) ) {
            valid_headers++;
        }
    }

    // Sort for faster access.
    std::sort(headers_buffer, &headers_buffer[entry_count], sorter);

    return TC::Collection::Vector { headers_buffer, valid_headers, entry_count };
}
