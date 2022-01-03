REMOTE_ARCHIVE=https://www.meetixos.org/Repository/Ports/libpng-1.6.37.tar.gz
UNPACKED_DIR=libpng-1.6.37
ARCHIVE=libpng-1.6.37.tar.gz

port_unpack() {
    tar -xf ${ARCHIVE}
}

port_build() {
    CPPFLAGS="-I$TOOLCHAIN_ROOT/include" \
    LDFLAGS="-L$MEETIX_LIBC_DIR"         \
    LIBS="$MEETIX_EXTRA_LIBS"            \
        ../$UNPACKED_DIR/configure --host=i686-pc-meetix --prefix="$TOOLCHAIN_ROOT" || exit 1

    make -j$BUILD_JOBS install || exit 1
}
