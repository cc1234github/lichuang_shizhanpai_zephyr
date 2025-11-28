

target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Account.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DataCenter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PingPongBuffer/PingPongBuffer.c
)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_SOURCE_DIR}/PingPongBuffer
                                )
