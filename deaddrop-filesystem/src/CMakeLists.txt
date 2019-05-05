cmake_minimum_required(VERSION 3.10.2)
# ------------------------------------------------------------------------------
# CONFIGURATION
# ------------------------------------------------------------------------------
set(CMAKE_C_FLAGS -Wall)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(
        -DDEBUG
        -DCOLORIZE
    )
endif(CMAKE_BUILD_TYPE STREQUAL Debug)
add_definitions(
    -D_FILE_OFFSET_BITS=64
    -DHAVE_SETXATTR
)
include_directories(
    ddfs
    ddfs/util
    ddfs/crypto
)
# ------------------------------------------------------------------------------
# LIBDDFS
# ------------------------------------------------------------------------------
add_library(ddfs STATIC
    ddfs/util/log.c
    ddfs/util/path.c
    ddfs/util/convert.c
    ddfs/crypto/index.c
    ddfs/crypto/record.c
    ddfs/crypto/crypto.c
)
target_link_libraries(ddfs
    fuse
    gcrypt
)
# ------------------------------------------------------------------------------
# DDFS-MOUNT
# ------------------------------------------------------------------------------
add_executable(ddfs-mount
    ddfs/main.c
    ddfs/ops/read.c
    ddfs/ops/open.c
    ddfs/ops/chmod.c
    ddfs/ops/chown.c
    ddfs/ops/rmdir.c
    ddfs/ops/write.c
    ddfs/ops/fsync.c
    ddfs/ops/utime.c
    ddfs/ops/mkdir.c
    ddfs/ops/mknod.c
    ddfs/ops/xattr.c
    ddfs/ops/access.c
    ddfs/ops/create.c
    ddfs/ops/statfs.c
    ddfs/ops/unlink.c
    ddfs/ops/rename.c
    ddfs/ops/release.c
    ddfs/ops/getattr.c
    ddfs/ops/utimens.c
    ddfs/ops/opendir.c
    ddfs/ops/readdir.c
    ddfs/ops/fgetattr.c
    ddfs/ops/fsyncdir.c
    ddfs/ops/ftruncate.c
    ddfs/ops/releasedir.c
)
target_link_libraries(ddfs-mount
    ddfs
    fuse
)
