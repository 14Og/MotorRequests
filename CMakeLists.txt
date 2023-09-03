cmake_minimum_required(VERSION 3.15)
project(MotorRequests VERSION 0.1
                        DESCRIPTION "Lib foor making http requests to klipper API")

include(CTest)
enable_testing()


# add_library(${PROJECT_NAME} STATIC
#             ${CMAKE_CURRENT_SOURCE_DIR}/inc/${PROJECT_NAME}.h
#             ${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}.cpp)

add_executable(${PROJECT_NAME} src/MotorRequests.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_link_libraries(${PROJECT_NAME} curl)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
