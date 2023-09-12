cmake_minimum_required(VERSION 3.15)
project(MotorRequests VERSION 0.1
                        DESCRIPTION "Lib foor making http requests to klipper API")

include(CTest)
enable_testing()

set(CMAKE_CXX_COMPILE_FEATURES "-lcurl")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/inc/MotorRequests.h ${CMAKE_BINARY_DIR}/include/MotorRequests.h COPYONLY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

add_library(${PROJECT_NAME} STATIC
            ${CMAKE_CURRENT_SOURCE_DIR}/inc/${PROJECT_NAME}.h
            ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}.cpp)

add_executable(example src/example.cpp
                                src/${CMAKE_PROJECT_NAME}.cpp)


target_link_libraries(${PROJECT_NAME} PRIVATE curl)
target_link_libraries(example PRIVATE curl)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
    

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
