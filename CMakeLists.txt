cmake_minimum_required(VERSION 3.16)
project(mygame LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build raylib examples" FORCE)
add_subdirectory(thirdparty/raylib-5.0)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(mygame ${GAME_SOURCES})
target_compile_features(mygame PRIVATE cxx_std_17)
set_target_properties(mygame PROPERTIES CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(mygame PRIVATE -Wall -Wextra)
elseif(MSVC)
    target_compile_options(mygame PRIVATE /W4)
endif()

target_include_directories(mygame PRIVATE
    src/platform
    src/gameLayer
)
target_link_libraries(mygame PRIVATE raylib)

add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:mygame>/resources"
)
