cmake_minimum_required(VERSION 3.30)
project(Loonar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "-Debug")

# Set paths
set(VENDOR_DIR ${CMAKE_SOURCE_DIR}/vendor)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Add include directories
include_directories(${INCLUDE_DIR})
include_directories(${INCLUDE_DIR}/glm)
include_directories(${VENDOR_DIR}/bgfx.cmake/bx/include)
include_directories(${VENDOR_DIR}/bgfx.cmake/bimg/include)
include_directories(${VENDOR_DIR}/bgfx.cmake/bgfx/include)
include_directories(${VENDOR_DIR}/SDL/include)
include_directories(${VENDOR_DIR}/JoltPhysics)

# Add SDL
add_subdirectory(${VENDOR_DIR}/SDL EXCLUDE_FROM_ALL)

# Add bgfx
add_subdirectory(${VENDOR_DIR}/bgfx.cmake)

# Add JoltPhysics
add_subdirectory(${VENDOR_DIR}/JoltPhysics/build)

# Add glm
add_subdirectory(${VENDOR_DIR}/glm)

# Add Lua
add_subdirectory(${VENDOR_DIR}/Lua)
# Add main project executable
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)

# Platform-specific settings
if(WIN32)
    add_executable(Loonar WIN32 ${SRC_FILES})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /Zc:preprocessor")
    target_compile_definitions(Loonar PRIVATE "_CRT_SECURE_NO_WARNINGS")
elseif(APPLE)
    add_executable(Loonar ${SRC_FILES})
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREVIDEO_FRAMEWORK CoreVideo)
    target_link_libraries(Loonar PRIVATE ${COCOA_FRAMEWORK} ${IOKIT_FRAMEWORK} ${COREVIDEO_FRAMEWORK})
elseif(UNIX)
    add_executable(Loonar ${SRC_FILES})
    target_link_libraries(Loonar PRIVATE pthread dl)
endif()

target_include_directories(Loonar PUBLIC ${INCLUDE_DIR})
target_link_libraries(Loonar PRIVATE bgfx glm SDL2main SDL2-static Jolt lua_static)
set_target_properties(Loonar PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Set output directories
set_target_properties(Loonar PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
