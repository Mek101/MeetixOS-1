//
// Created by mek101 on 10/03/22.
//

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include <LibTC/Functional/Result.hh>

#include "ElfDynamicObject.hh"
#include "Load/ElfLoader.hh"
#include "utils/HashMap.hpp"


enum class ResourceError {
    BadTarget,
    TargetNotFound,
    LoadError,
    UnknownError
};

template <typename T>
using ResourceResult = TC::Functional::Result<T, ResourceError>;

class ResourceResolver {
public:
    explicit ResourceResolver(std::vector<std::string> include_dirs);

    ResourceResult<std::shared_ptr<ElfDynamicObject>> resolve(const std::string& target);

private:
    std::vector<std::string> m_include_dirs;
    HashMap<std::string, std::shared_ptr<ElfDynamicObject>> m_cache;

    ResourceResult<std::shared_ptr<ElfDynamicObject>> get_from_path(const std::string& target_path);
};
