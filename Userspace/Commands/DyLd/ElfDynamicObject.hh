//
// Created by mek101 on 10/03/22.
//

#pragma once

#include <optional>
#include <vector>
#include <memory>

#include <Api/StdInt.h>
#include <Api/ELF32.h>
#include <LibTC/Functional/Result.hh>

#include "Definitions.hh"


class ElfDynamicObject {
    LoadedSegments prog_segments;
};

