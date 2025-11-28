

target_sources(app PRIVATE 
                ${CMAKE_CURRENT_SOURCE_DIR}/DataProc.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/DP_GPS.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/DP_SportStatus.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/DP_Clock.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/DP_Power.cpp
                )
target_include_directories(app PRIVATE 
                ${CMAKE_CURRENT_SOURCE_DIR}
                )