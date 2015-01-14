cmake_minimum_required(VERSION 2.8.11)
project(cjs)

set(codemate_VERSION_MAJOR 0)
set(codemate_VERSION_MINOR 1)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(QT5_WIDGETS REQUIRED Qt5Widgets)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# should do this outside 

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

if ( APPLE )
    set( ENV_LIB_PATHS ENV DYLD_LIBRARY_PATH )
elseif ( UNIX )
    set( ENV_LIB_PATHS ENV LD_LIBRARY_PATH )
elseif ( WIN32 )
    set( ENV_LIB_PATHS ENV PATH )
else ()
    set( ENV_LIB_PATHS "" )
endif()

# On Debian-based systems, llvm installs into /usr/lib/llvm-x.y.
file( GLOB SYS_LLVM_PATHS "/usr/lib/llvm*/lib" )
# Need TEMP because find_library does not work with an option variable
find_library( TEMP clang
    PATHS
    ${ENV_LIB_PATHS}
    /usr/lib
    /usr/lib/llvm
    ${SYS_LLVM_PATHS}
    /Library/Developer/CommandLineTools/usr/lib )
set( EXTERNAL_LIBCLANG_PATH ${TEMP} )
message("found libclang:" ${TEMP})

#configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)

file(GLOB SRCS "${PROJECT_SOURCE_DIR}/*.cc")
set(target cjs)

if (APPLE)
    add_executable(${target} MACOSX_BUNDLE ${SRCS})
else ()
    add_executable(${target} ${SRCS})
endif()

target_link_libraries(${target} ${EXTERNAL_LIBCLANG_PATH})
