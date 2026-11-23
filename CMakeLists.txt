cmake_minimum_required(VERSION 3.16)
project(kurin LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(KURIN_BUILD_TESTS "Build the tests" ON)
option(KURIN_WERROR "Treat compiler warnings as errors" OFF)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build raylib examples" FORCE)
add_subdirectory(thirdparty/raylib-5.0)

function(kurin_warnings target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wshadow -Wpedantic)
        if(KURIN_WERROR)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4)
        if(KURIN_WERROR)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    endif()
endfunction()

# Everything except main.cpp goes into a library, so the tests can link the same code.
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM GAME_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
add_library(kurin_game STATIC ${GAME_SOURCES})
target_include_directories(kurin_game PUBLIC src)
target_compile_features(kurin_game PUBLIC cxx_std_17)
target_link_libraries(kurin_game PUBLIC raylib)
set_target_properties(kurin_game PROPERTIES CXX_EXTENSIONS OFF)
kurin_warnings(kurin_game)

add_executable(kurin src/main.cpp)
target_link_libraries(kurin PRIVATE kurin_game)
set_target_properties(kurin PROPERTIES CXX_EXTENSIONS OFF)
kurin_warnings(kurin)

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

if(KURIN_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    enable_testing()
    add_subdirectory(tests)
endif()
