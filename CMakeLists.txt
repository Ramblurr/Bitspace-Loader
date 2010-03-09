cmake_minimum_required(VERSION 2.6)
project(loader)
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

find_package(Qt4 REQUIRED)
find_package(Bitspace REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

set(CMAKE_CXX_FLAGS "-g -Wall")
set(LOADER_INCLUDES
    ${QT_INCLUDES}
    ${BITSPACE_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src
)

add_subdirectory(src)


