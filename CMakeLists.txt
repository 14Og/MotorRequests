cmake_minimum_required(VERSION 3.15)
project(MotorRequests VERSION 0.1
                        DESCRIPTION "Lib foor making http requests to klipper API")

enable_testing()

set(CMAKE_CXX_STANDARD 17)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/inc/MotorRequests.h ${CMAKE_BINARY_DIR}/include/MotorRequests.h COPYONLY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/jsoncons/include)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})
set(JSONCONS_BUILD_TESTS OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/jsoncons)


add_library(${PROJECT_NAME} STATIC
            ${CMAKE_CURRENT_SOURCE_DIR}/inc/${PROJECT_NAME}.h
            ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}.cpp)

add_executable(example src/example.cpp
                src/MotorRequests.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl PRIVATE jsoncons)
target_link_libraries(example PRIVATE ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/jsoncons/include)

    

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
