cmake_minimum_required(VERSION 3.0)
project(boxology)

set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "9")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

add_definitions(-std=c++11 -DBOXOLOGY_VERSION=${VERSION}) 

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(QT_RC ${CMAKE_SOURCE_DIR}/rc/resources.qrc)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_policy(SET CMP0020 NEW) # automatic linking of qtmain.lib on Windows

# for UUIDs
find_package(Boost 1.43.0 REQUIRED)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Svg REQUIRED)

file(GLOB_RECURSE SRC src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.hpp)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src # for json/json.h
    )

add_executable(${PROJECT_NAME} ${SRC} ${HEADERS_MOC} ${HEADERS} ${HEADERS_UI} ${QT_RC})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Svg)

