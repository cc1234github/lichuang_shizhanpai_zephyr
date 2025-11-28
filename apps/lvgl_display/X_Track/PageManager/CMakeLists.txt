

file(GLOB CURRENT_DIR_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
target_sources(app PRIVATE ${CURRENT_DIR_SRCS})
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
