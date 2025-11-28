
target_sources(app PRIVATE 
            ${CMAKE_CURRENT_SOURCE_DIR}/HAL_Clock.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/HAL_Power.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/HAL_GPS.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/HAL_Encoder.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/HAL_Buzz.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/HAL.cpp
)

target_include_directories(app PRIVATE 
            ${CMAKE_CURRENT_SOURCE_DIR}
            )

