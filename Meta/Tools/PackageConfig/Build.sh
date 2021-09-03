#!/bin/bash

SYSROOT=$(realpath ../../../Toolchain/Local)
OUT_PATH="${SYSROOT}/bin"
OUT_FILE="${OUT_PATH}/meetix-pkg-config.sh"

sed -e 's|__SYSROOT__|'${SYSROOT}'|' meetix-pkg-config.sh.template >"${OUT_FILE}"
chmod +x "${OUT_FILE}"
