//
// Created by mek101 on 10/03/22.
//

#pragma once

#include <cstdlib>
#include <string>
#include <optional>

#include <Api/utils/local.hpp>

#include "ResourceResolver.hh"


enum class ResourceType {
    Error,
    Name,
    AbsolutePath,
    RelativePath
};

std::optional<std::string> get_working_directory() {
    Local work_dir_buffer { new char[PATH_MAX] };

    auto work_dir_status = s_get_working_directory(work_dir_buffer());
    if ( work_dir_status == GET_WORKING_DIRECTORY_SUCCESSFUL ) {

        auto work_dir = std::string{ work_dir_buffer() };
        if ( !work_dir.ends_with('/') ) {
            work_dir += '/';
        }
        return work_dir;
    }
    return std::nullopt;
}

ResourceType identify_type(const std::string& target) {
    // Too short
    if (target.length() < 1) {
        return ResourceType::Error;
    }

    if (target[0] == '/') {
        return ResourceType::AbsolutePath;
    } else if ( target.find('/') == std::string::npos ) {
        return ResourceType::Name;
    } else {
        return ResourceType::RelativePath;
    }
}

ResourceResolver::ResourceResolver(std::vector<std::string> include_dirs) {
    this->m_include_dirs = include_dirs;
}

ResourceResult<std::shared_ptr<ElfDynamicObject>> ResourceResolver::resolve(const std::string& target) {
    auto type = identify_type(target);

    switch ( type ) {
        case ResourceType::Name: {
            // Search and try loading the ELF form the included directories.
            for ( const auto& dir : this->m_include_dirs ) {
                auto load_res = this->get_from_path(target);
                if ( load_res.is_value() ) {
                    return load_res.value();
                }
            }
            return ResourceError::TargetNotFound;
        }
        case ResourceType::RelativePath: {
            // Try loading the ELF from the current working directory + the relative path.
            auto current_working_dir = get_working_directory();
            if ( current_working_dir.has_value() ) {
                return this->get_from_path(current_working_dir.value().append(target));
            }
            return ResourceError::TargetNotFound;
        }
        case ResourceType::AbsolutePath: {
            // Try loading the ELF from the specified path.
            return this->get_from_path(target);
        }
        case ResourceType::Error: {
            return ResourceError::BadTarget;
        }
        default: {
            return ResourceError::UnknownError;
        }
    }
}

ResourceResult<std::shared_ptr<ElfDynamicObject>> ResourceResolver::get_from_path(const std::string& target_path) {
    auto cached_val = this->m_cache.get(target_path);
    if ( cached_val != nullptr ) {
        return cached_val->value;
    }

    auto load_res = try_load(target_path);
    if ( load_res.is_value() ) {
        // Load the ELF file.
        auto loaded_elf = load_res.unwrap_value();

        // Now resolve and relocate it


        this->m_cache.add(target_path, ptr);
        return ptr;
    } else {
        return ResourceError::LoadError;
    }
}