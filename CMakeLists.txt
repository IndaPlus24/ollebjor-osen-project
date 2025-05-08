cmake_minimum_required(VERSION 3.5)
project(Loonar LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "-Debug")

# Include FetchContent module
include(FetchContent)

# Set paths
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

if(MSVC)
    foreach(flag_var
    CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_DEBUG)
        string (REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endforeach()
endif()
if(MSVC)
    add_compile_options("/Zc:__cplusplus" "/permissive-" "/Zc:preprocessor" "/EHsc")
    add_compile_definitions("_CRT_SECURE_NO_WARNINGS")
endif()

# Fetch SDL
FetchContent_Declare(
  SDL
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-2.32.4
)
FetchContent_MakeAvailable(SDL)

# Fetch bgfx
FetchContent_Declare(
  bgfx
  GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git
  GIT_TAG master
)
FetchContent_MakeAvailable(bgfx)

# Fetch JoltPhysics
FetchContent_Declare(
  JoltPhysics
  GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
  GIT_TAG v5.3.0
  SOURCE_SUBDIR "Build"
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(BUILD_SAMPLES OFF CACHE BOOL "Build JoltPhysics samples" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "Build JoltPhysics unit tests" FORCE)
FetchContent_MakeAvailable(JoltPhysics)

# Fetch glm
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# Fetch Lua
FetchContent_Declare(
  lua
  GIT_REPOSITORY https://github.com/walterschell/Lua.git
  GIT_TAG v5.4.5
)
FetchContent_MakeAvailable(lua)

# Fetch sol2
FetchContent_Declare(
  sol2
  GIT_REPOSITORY https://github.com/ThePhD/sol2.git
  GIT_TAG v3.5.0
)
# Configure sol2 options
set(SOL2_ENABLE_INSTALL OFF CACHE BOOL "Enable installation for sol2" FORCE)
set(SOL2_SINGLE OFF CACHE BOOL "Use single header for sol2" FORCE)
set(SOL2_BUILD_LUA OFF CACHE BOOL "Build Lua for sol2" FORCE)
add_compile_definitions(SOL_USE_STD_OPTIONAL=1)
FetchContent_MakeAvailable(sol2)

# Fetch assimp
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.4.3
)
# set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Build Assimp tests" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Assimp shared libraries" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Assimp warnings as errors" FORCE)
FetchContent_MakeAvailable(assimp)

# Add include directories
include_directories(${INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIR}/glm)
include_directories(${bgfx_SOURCE_DIR}/bx/include)
include_directories(${bgfx_SOURCE_DIR}/bimg/include)
include_directories(${bgfx_SOURCE_DIR}/bgfx/include)
include_directories(${SDL_SOURCE_DIR}/include)
include_directories(${JoltPhysics_SOURCE_DIR})
include_directories(${lua_SOURCE_DIR})
include_directories(${sol2_SOURCE_DIR}/include)
include_directories(${ASSIMP_SOURCE_DIR}/include)
include_directories(${EXTERNAL_DIR})

# Compile shaders
include_directories(${SHADER_DIR}/include/generated)
set(BGFX_SHADER_INCLUDE_PATH ${SHADER_DIR}/include)
file(GLOB_RECURSE SHADER_FILES_VS shaders/vs_*.sc)
file(GLOB_RECURSE SHADER_FILES_FS shaders/fs_*.sc)
bgfx_compile_shaders(
    TYPE VERTEX
    SHADERS ${SHADER_FILES_VS}
    VARYING_DEF ${SHADER_DIR}/varying.def.sc
    OUTPUT_DIR ${SHADER_DIR}/include/generated
    AS_HEADERS
)
bgfx_compile_shaders(
    TYPE FRAGMENT
    SHADERS ${SHADER_FILES_FS}
    VARYING_DEF ${SHADER_DIR}/varying.def.sc
    OUTPUT_DIR ${SHADER_DIR}/include/generated
    AS_HEADERS
)

# Add main project executable
file(GLOB_RECURSE SRC_FILES ${SRC_DIR}/*.cpp)

# Platform-specific settings
if(WIN32)
    add_executable(Loonar ${SRC_FILES} ${SHADER_FILES_VS} ${SHADER_FILES_FS})
elseif(APPLE)
    add_executable(Loonar ${SRC_FILES} ${SHADER_FILES_VS} ${SHADER_FILES_FS})
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREVIDEO_FRAMEWORK CoreVideo)
    target_link_libraries(Loonar PRIVATE ${COCOA_FRAMEWORK} ${IOKIT_FRAMEWORK} ${COREVIDEO_FRAMEWORK})
elseif(UNIX)
    add_executable(Loonar ${SRC_FILES} ${SHADER_FILES_VS} ${SHADER_FILES_FS})
    target_link_libraries(Loonar PRIVATE pthread dl)
endif()

target_include_directories(Loonar PUBLIC ${INCLUDE_DIR})
target_link_libraries(Loonar PRIVATE bgfx glm SDL2main SDL2-static Jolt assimp lua_static sol2)
set_target_properties(Loonar PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Set output directories
set_target_properties(Loonar PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
