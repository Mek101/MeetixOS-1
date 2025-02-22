/**
 * @brief
 * This file is part of the MeetiX Operating System.
 * Copyright (c) 2017-2022, Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @developers
 * Marco Cicognani (marco.cicognani@meetixos.org)
 *
 * @license
 * GNU General Public License version 3
 */

#pragma once

#include <LibTC/Assertions.hh>
#include <LibTC/Cxx.hh>
#include <LibTC/Functional/Must.hh>
#include <LibTC/Functional/Try.hh>
#include <LibTC/Memory/Raw.hh>
#include <LibTC/Trait/TypeIntrinsics.hh>

namespace TC::Collection {
namespace Details {

template<typename Collection, typename T>
VectorIterator<Collection, T> VectorIterator<Collection, T>::begin(Collection& collection) {
    return VectorIterator{ collection, 0 };
}

template<typename Collection, typename T>
VectorIterator<Collection, T> VectorIterator<Collection, T>::end(Collection& collection) {
    return VectorIterator{ collection, collection.count() };
}

template<typename Collection, typename T>
VectorIterator<Collection, T>& VectorIterator<Collection, T>::operator++() {
    ++m_index;
    return *this;
}

template<typename Collection, typename T>
VectorIterator<Collection, T> VectorIterator<Collection, T>::operator++(int) {
    ++m_index;
    return VectorIterator{ m_collection, m_index - 1 };
}

template<typename Collection, typename T>
T& VectorIterator<Collection, T>::operator*() {
    VERIFY_FALSE(is_end());
    return m_collection->at(m_index);
}

template<typename Collection, typename T>
T const& VectorIterator<Collection, T>::operator*() const {
    VERIFY_FALSE(is_end());
    return m_collection->at(m_index);
}

template<typename Collection, typename T>
T* VectorIterator<Collection, T>::operator->() {
    return &operator*();
}

template<typename Collection, typename T>
T const* VectorIterator<Collection, T>::operator->() const {
    return &operator*();
}

template<typename Collection, typename T>
bool VectorIterator<Collection, T>::is_end() const {
    return m_index == end(*m_collection).index();
}

template<typename Collection, typename T>
usize VectorIterator<Collection, T>::index() const {
    return m_index;
}

template<typename Collection, typename T>
bool VectorIterator<Collection, T>::operator==(VectorIterator<Collection, T> const& rhs) const {
    return m_collection == rhs.m_collection && m_index == rhs.m_index;
}

template<typename Collection, typename T>
bool VectorIterator<Collection, T>::operator!=(VectorIterator<Collection, T> const& rhs) const {
    return m_collection != rhs.m_collection || m_index != rhs.m_index;
}

template<typename Collection, typename T>
bool VectorIterator<Collection, T>::operator<(VectorIterator<Collection, T> const& rhs) const {
    return m_index < rhs.m_index;
}

template<typename Collection, typename T>
bool VectorIterator<Collection, T>::operator>(VectorIterator<Collection, T> const& rhs) const {
    return m_index > rhs.m_index;
}

template<typename Collection, typename T>
bool VectorIterator<Collection, T>::operator<=(VectorIterator<Collection, T> const& rhs) const {
    return m_index <= rhs.m_index;
}

template<typename Collection, typename T>
bool VectorIterator<Collection, T>::operator>=(VectorIterator<Collection, T> const& rhs) const {
    return m_index >= rhs.m_index;
}

template<typename Collection, typename T>
VectorIterator<Collection, T>::VectorIterator(Collection& collection, usize index)
    : m_collection{ &collection }
    , m_index{ index } {
}

} /* namespace Details */

template<typename T>
Vector<T>::Vector(usize capacity) {
    ensure_capacity(capacity);
}

template<typename T>
Vector<T>::Vector(AdoptTag, T* data_storage, usize size)
    : m_data_storage{ data_storage }
    , m_data_capacity{ size }
    , m_values_count{ size } {
}

template<typename T>
Vector<T>::Vector(AdoptTag, T* data_storage, usize size, usize capacity)
    : m_data_storage{ data_storage }
    , m_data_capacity{ size }
    , m_values_count{ capacity } {
}

template<typename T>
Vector<T>::Vector(Vector const& rhs)
    : m_values_count{ rhs.m_values_count } {
    if ( !is_empty() ) {
        ensure_capacity(rhs.count());
        for ( usize i = 0; i < m_values_count; ++i )
            new (&m_data_storage[i]) T{ rhs.at(i) };
    }
}

template<typename T>
Vector<T>::Vector(Vector&& rhs) noexcept
    : m_data_storage{ exchange(rhs.m_data_storage, nullptr) }
    , m_data_capacity{ exchange(rhs.m_data_capacity, 0) }
    , m_values_count{ exchange(rhs.m_values_count, 0) } {
}

template<typename T>
Vector<T>::Vector(std::initializer_list<T> initializer_list) {
    ensure_capacity(initializer_list.size());
    for ( auto const& element : initializer_list )
        append_unchecked(move(element));
}

template<typename T>
Vector<T>::~Vector() {
    clear(KeepStorageCapacity::No);
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector const& rhs) {
    if ( this == &rhs )
        return *this;

    Vector vector{ rhs };
    swap(vector);
    return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector&& rhs) noexcept {
    Vector vector{ move(rhs) };
    swap(vector);
    return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(std::initializer_list<T> initializer_list) {
    Vector vector{ initializer_list };
    swap(vector);
    return *this;
}

template<typename T>
void Vector<T>::clear(KeepStorageCapacity keep_storage_capacity) {
    if ( m_data_storage != nullptr ) {
        /* call the destructors only for non-trivial types */
        if constexpr ( !Trait::TypeIntrinsics<T>::is_trivial() ) {
            for ( usize i = 0; i < m_values_count; ++i )
                m_data_storage[i].~T();
        }

        /* free the memory if requested */
        if ( keep_storage_capacity == KeepStorageCapacity::No ) {
            Memory::Raw::free_sized(m_data_storage, m_data_capacity);
            m_data_storage  = nullptr;
            m_data_capacity = 0;
        }
    }
    m_values_count = 0;
}

template<typename T>
void Vector<T>::swap(Vector& rhs) noexcept {
    Cxx::swap(m_data_storage, rhs.m_data_storage);
    Cxx::swap(m_data_capacity, rhs.m_data_capacity);
    Cxx::swap(m_values_count, rhs.m_values_count);
}

template<typename T>
void Vector<T>::insert_at(usize index, T const& value) {
    MUST(try_insert_at(index, T{ value }));
}

template<typename T>
void Vector<T>::insert_at(usize index, T&& value) {
    MUST(try_insert_at(index, move(value)));
}

template<typename T>
ErrorOr<void> Vector<T>::try_insert_at(usize index, T const& value) {
    return try_insert_at(index, T{ value });
}

template<typename T>
ErrorOr<void> Vector<T>::try_insert_at(usize index, T&& value) {
    if ( index > m_values_count )
        return EINVAL;

    TRY(try_ensure_capacity(m_values_count + 1));

    /* move the values after the insertion one place forward */
    if ( index < m_values_count ) {
        if constexpr ( Trait::TypeIntrinsics<T>::is_trivial() )
            __builtin_memmove(data_slot(index + 1), data_slot(index), (m_values_count - index) * sizeof(T));
        else {
            for ( usize i = m_values_count; i > index; --i ) {
                new (data_slot(i)) T{ move(m_data_storage[i - 1]) };
                at(i - 1).~T();
            }
        }
    }

    /* move the value into the memory */
    new (data_slot(index)) T{ move(value) };
    ++m_values_count;
    return {};
}

template<typename T>
template<typename Comparator>
void Vector<T>::insert_sorted(T const& value, Comparator comparator) {
    MUST(try_insert_sorted(value, comparator));
}

template<typename T>
void Vector<T>::insert_sorted(T const& value) {
    MUST(try_insert_sorted(value));
}

template<typename T>
template<typename Comparator>
ErrorOr<void> Vector<T>::try_insert_sorted(T const& value, Comparator comparator) {
    /* find the insertion index */
    usize insert_index = 0;
    for ( usize i = 0; i < m_values_count; ++i ) {
        if ( comparator(m_data_storage[i], value) )
            insert_index = i + 1;
        else
            break;
    }

    /* insert at the position */
    return try_insert_at(insert_index, value);
}

template<typename T>
ErrorOr<void> Vector<T>::try_insert_sorted(T const& value) {
    return try_insert_sorted(value, [](T const& a, T const& b) { return a < b; });
}

template<typename T>
void Vector<T>::prepend(T const& value) {
    MUST(try_prepend(T{ value }));
}

template<typename T>
void Vector<T>::prepend(T&& value) {
    MUST(try_prepend(move(value)));
}

template<typename T>
ErrorOr<void> Vector<T>::try_prepend(T const& value) {
    return try_prepend(T{ value });
}

template<typename T>
ErrorOr<void> Vector<T>::try_prepend(T&& value) {
    return try_insert_at(0, move(value));
}

template<typename T>
void Vector<T>::append(T const& value) {
    MUST(try_append(T{ value }));
}

template<typename T>
void Vector<T>::append(T&& value) {
    MUST(try_append(move(value)));
}

template<typename T>
ErrorOr<void> Vector<T>::try_append(T const& value) {
    return try_append(T{ value });
}

template<typename T>
ErrorOr<void> Vector<T>::try_append(T&& value) {
    return try_insert_at(m_values_count, move(value));
}

template<typename T>
void Vector<T>::append_unchecked(T const& value) {
    append_unchecked(T{ value });
}

template<typename T>
void Vector<T>::append_unchecked(T&& value) {
    new (data_slot(m_values_count)) T{ move(value) };
    ++m_values_count;
}

template<typename T>
template<typename... Args>
void Vector<T>::emplace_first(Args&&... args) {
    MUST(try_emplace_first(std::forward<Args>(args)...));
}

template<typename T>
template<typename... Args>
ErrorOr<void> Vector<T>::try_emplace_first(Args&&... args) {
    TRY(try_ensure_capacity(m_values_count + 1));

    /* move the values after the insertion one place forward */
    if constexpr ( Trait::TypeIntrinsics<T>::is_trivial() )
        __builtin_memmove(data_slot(1), m_data_storage, m_values_count * sizeof(T));
    else {
        for ( usize i = m_values_count; i > 0; --i ) {
            new (data_slot(i)) T{ move(m_data_storage[i - 1]) };
            at(i - 1).~T();
        }
    }

    /* move the value into the memory */
    new (data_slot(0)) T{ std::forward<Args>(args)... };
    ++m_values_count;
    return {};
}

template<typename T>
template<typename... Args>
void Vector<T>::emplace_last(Args&&... args) {
    MUST(try_emplace_last(std::forward<Args>(args)...));
}

template<typename T>
template<typename... Args>
ErrorOr<void> Vector<T>::try_emplace_last(Args&&... args) {
    TRY(try_ensure_capacity(m_values_count + 1));

    /* move the value into the memory */
    new (data_slot(m_values_count)) T{ std::forward<Args>(args)... };
    ++m_values_count;
    return {};
}

template<typename T>
T& Vector<T>::first() {
    return at(0);
}

template<typename T>
T const& Vector<T>::first() const {
    return at(0);
}

template<typename T>
T& Vector<T>::last() {
    return at(m_values_count - 1);
}

template<typename T>
T const& Vector<T>::last() const {
    return at(m_values_count - 1);
}

template<typename T>
T Vector<T>::take_first() {
    return take_at(0);
}

template<typename T>
T Vector<T>::take_last() {
    return take_at(m_values_count - 1);
}

template<typename T>
T Vector<T>::take_at(usize index) {
    VERIFY_LESS(index, m_values_count);

    T value{ move(m_data_storage[index]) };
    erase_at(index);
    return value;
}

template<typename T>
ErrorOr<void> Vector<T>::erase_at(usize index) {
    if ( index >= m_values_count )
        return EINVAL;

    /* shift all the values one position back */
    if constexpr ( Trait::TypeIntrinsics<T>::is_trivial() )
        __builtin_memmove(data_slot(index), data_slot(index + 1), (m_values_count - index - 1) * sizeof(T));
    else {
        at(index).~T();
        for ( usize i = index + 1; i < m_values_count; ++i ) {
            new (data_slot(i - 1)) T{ move(m_data_storage[i]) };
            m_data_storage[i].~T();
        }
    }
    --m_values_count;
    return {};
}

template<typename T>
ErrorOr<void> Vector<T>::erase_first_of(T const& value) {
    for ( usize i = 0; i < m_values_count; ++i ) {
        if ( m_data_storage[i] == value ) {
            TRY(erase_at(i));
            return {};
        }
    }
    return ENOENT;
}

template<typename T>
ErrorOr<usize> Vector<T>::erase_all_of(T const& value) {
    return erase_all_matches([&value](T const& current) { return current == value; });
}

template<typename T>
template<typename Matcher>
ErrorOr<usize> Vector<T>::erase_all_matches(Matcher matches) {
    usize erased_count = 0;
    for ( usize i = 0; i < m_values_count; ) {
        if ( matches(m_data_storage[i]) ) {
            erase_at(i);
            ++erased_count;
        } else
            ++i;
    }

    if ( erased_count > 0 )
        return erased_count;
    else
        return ENOENT;
}

template<typename T>
template<typename Comparator>
void Vector<T>::sort(Comparator comparator) {
    for ( usize i = 0; i + 1 < m_values_count; ++i ) {
        for ( usize j = i + 1; j < m_values_count; ++j ) {
            if ( comparator(m_data_storage[i], m_data_storage[j]) > 0 )
                Cxx::swap(m_data_storage[i], m_data_storage[j]);
        }
    }
}

template<typename T>
ErrorOr<void> Vector<T>::try_resize(usize new_count) {
    if ( new_count < m_values_count ) {
        for ( usize i = new_count; i < m_values_count; ++i )
            at(i).~T();
    } else if ( new_count > m_values_count ) {
        TRY(try_ensure_capacity(new_count));

        /* default construct the new values */
        for ( usize i = m_values_count; i < new_count; ++i )
            new (data_slot(i)) T{};
    }

    m_values_count = new_count;
    return {};
}

template<typename T>
ErrorOr<void> Vector<T>::try_ensure_capacity(usize capacity) {
    VERIFY_GREATER(capacity, 0);

    if ( m_data_capacity >= capacity )
        return {};

    /* for first time use the given capacity, otherwise increase the capacity logarithmically */
    usize new_capacity;
    if ( m_data_capacity == 0 )
        new_capacity = capacity == 0 ? 16 : capacity;
    else
        new_capacity = capacity + capacity / 4;

    /* allocate new memory and move the content into it */
    auto new_data_storage = TRY(Memory::Raw::clean_alloc_array<T>(new_capacity));
    if constexpr ( Trait::TypeIntrinsics<T>::is_trivial() )
        __builtin_memmove(new_data_storage, m_data_storage, m_values_count * sizeof(T));
    else {
        for ( usize i = 0; i < m_values_count; ++i ) {
            new (&new_data_storage[i]) T{ move(at(i)) };
            at(i).~T();
        }
    }

    /* destroy the previous buffer if exists and update the other fields */
    if ( m_data_storage )
        Memory::Raw::free_sized<T>(m_data_storage, m_data_capacity);

    m_data_storage  = new_data_storage;
    m_data_capacity = new_capacity;
    return {};
}

template<typename T>
void Vector<T>::resize(usize new_count) {
    MUST(try_resize(new_count));
}

template<typename T>
void Vector<T>::ensure_capacity(usize capacity) {
    MUST(try_ensure_capacity(capacity));
}

template<typename T>
typename Vector<T>::Iterator Vector<T>::begin() {
    return Iterator::begin(*this);
}

template<typename T>
typename Vector<T>::Iterator Vector<T>::end() {
    return Iterator::end(*this);
}

template<typename T>
typename Vector<T>::ConstIterator Vector<T>::begin() const {
    return ConstIterator::begin(*this);
}

template<typename T>
typename Vector<T>::ConstIterator Vector<T>::end() const {
    return ConstIterator::end(*this);
}

template<typename T>
bool Vector<T>::contains(T const& value) const {
    return index_of(value).is_present();
}

template<typename T>
Option<usize> Vector<T>::index_of(T const& value) const {
    return index_if([&value](auto const& v) { return v == value; });
}

template<typename T>
template<typename Callback>
Option<usize> Vector<T>::index_if(Callback callback) const {
    for ( usize i = 0; i < m_values_count; ++i ) {
        if ( callback(m_data_storage[i]) )
            return i;
    }
    return {};
}

template<typename T>
Option<T&> Vector<T>::find(T const& value) {
    auto index_or_none = index_of(value);
    if ( index_or_none.is_present() )
        return at(index_or_none.value());
    else
        return {};
}

template<typename T>
Option<T const&> Vector<T>::find(T const& value) const {
    auto index_or_none = index_of(value);
    if ( index_or_none.is_present() )
        return at(index_or_none.value());
    else
        return {};
}

template<typename T>
template<typename Callback>
Option<T&> Vector<T>::find_if(Callback callback) {
    auto index_or_none = index_if(callback);
    if ( index_or_none.is_present() )
        return at(index_or_none.value());
    else
        return {};
}

template<typename T>
template<typename Callback>
Option<T const&> Vector<T>::find_if(Callback callback) const {
    auto index_or_none = index_if(callback);
    if ( index_or_none.is_present() )
        return at(index_or_none.value());
    else
        return {};
}

template<typename T>
T& Vector<T>::at(usize index) {
    VERIFY_LESS(index, m_values_count);

    return m_data_storage[index];
}

template<typename T>
T const& Vector<T>::at(usize index) const {
    VERIFY_LESS(index, m_values_count);

    return m_data_storage[index];
}

template<typename T>
T& Vector<T>::operator[](usize index) {
    return at(index);
}

template<typename T>
T const& Vector<T>::operator[](usize index) const {
    return at(index);
}

template<typename T>
usize Vector<T>::count() const {
    return m_values_count;
}

template<typename T>
usize Vector<T>::capacity() const {
    return m_data_capacity;
}

template<typename T>
T* Vector<T>::data() {
    return m_data_storage;
}

template<typename T>
T const* Vector<T>::data() const {
    return m_data_storage;
}

template<typename T>
bool Vector<T>::is_empty() const {
    return m_values_count == 0;
}

template<typename T>
bool Vector<T>::any() const {
    return !is_empty();
}

template<typename T>
T* Vector<T>::data_slot(usize index) {
    return &m_data_storage[index];
}

} /* namespace TC::Collection */