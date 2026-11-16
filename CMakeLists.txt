cmake_minimum_required(VERSION 3.16)
project(kurin LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(KURIN_BUILD_TESTS "Build the core tests" ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build raylib examples" FORCE)
add_subdirectory(thirdparty/raylib-5.0)

function(kurin_warnings target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    endif()
endfunction()

add_library(ecs INTERFACE)
target_include_directories(ecs INTERFACE src)
target_compile_features(ecs INTERFACE cxx_std_17)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/core/*.cpp)
add_library(kurin_core STATIC ${CORE_SOURCES})
target_link_libraries(kurin_core PUBLIC ecs)
set_target_properties(kurin_core PROPERTIES CXX_EXTENSIONS OFF)
kurin_warnings(kurin_core)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS src/game/*.cpp src/platform/*.cpp)
add_executable(kurin ${GAME_SOURCES})
target_link_libraries(kurin PRIVATE kurin_core raylib)
set_target_properties(kurin PROPERTIES CXX_EXTENSIONS OFF)
kurin_warnings(kurin)

add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:kurin>/resources"
)
add_dependencies(copy_resources kurin)

if(KURIN_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    enable_testing()
    add_subdirectory(tests)
endif()
