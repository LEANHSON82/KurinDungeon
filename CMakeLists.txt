cmake_minimum_required(VERSION 3.16)
project(kurin LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(KURIN_WERROR "Treat compiler warnings as errors" OFF)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build raylib examples" FORCE)
add_subdirectory(thirdparty/raylib-5.0)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(kurin ${GAME_SOURCES})
target_compile_features(kurin PRIVATE cxx_std_17)
set_target_properties(kurin PROPERTIES CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(kurin PRIVATE -Wall -Wextra -Wshadow -Wpedantic)
    if(KURIN_WERROR)
        target_compile_options(kurin PRIVATE -Werror)
    endif()
elseif(MSVC)
    target_compile_options(kurin PRIVATE /W4)
    if(KURIN_WERROR)
        target_compile_options(kurin PRIVATE /WX)
    endif()
endif()

target_include_directories(kurin PRIVATE
    src/platform
    src/gameLayer
)
target_link_libraries(kurin PRIVATE raylib)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.26)
    set(KURIN_COPY_DIRECTORY copy_directory_if_different)
else()
    set(KURIN_COPY_DIRECTORY copy_directory)
endif()
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E ${KURIN_COPY_DIRECTORY}
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:kurin>/resources"
)
add_dependencies(copy_resources kurin)
