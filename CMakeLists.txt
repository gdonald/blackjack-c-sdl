cmake_minimum_required(VERSION 3.10)

project(blackjack)

include(FindPackageHandleStandardArgs)

set(CMAKE_C_COMPILER clang)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(include ${SDL2_INCLUDE_DIRS} ${SDL2_GFX_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

file(GLOB sources src/*.c)
add_executable(blackjack ${sources})
target_link_libraries(blackjack ${SDL2_LIBRARIES} ${SDL2_GFX_LIBRARIES} ${SDL2_TTF_LIBRARIES})

add_custom_command(TARGET blackjack
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:blackjack> .)
