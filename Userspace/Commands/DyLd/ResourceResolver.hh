//
// Created by mek101 on 10/03/22.
//

#include <string>
#include <vector>
#include <optional>
#include <memory>

#include <LibTC/Functional/Result.hh>

#include "ElfDynamicObject.hh"
#include "Load/ElfLoader.hh"
#include "utils/HashMap.hpp"

enum ResourceError {
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

    ResourceResult<std::shared_ptr<ElfDynamicObject>> resolve(std::string &target);

private:
    std::vector<std::string> include_dirs;
    HashMap<std::string, std::shared_ptr<ElfDynamicObject>> cache;

    ResourceResult<std::shared_ptr<ElfDynamicObject>> get_from_path(std::string &target_path);
};
