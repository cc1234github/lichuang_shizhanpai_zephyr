file(GLOB CURRENT_DIR_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/lv_anim_timeline_wrapper.c

    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/Dialplate/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StartUp/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/SystemInfos/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/_Template/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StatusBar/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/LiveMap/*.cpp
)
target_sources(app PRIVATE ${CURRENT_DIR_SRCS})




target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}

								${CMAKE_CURRENT_SOURCE_DIR}/Dialplate
								${CMAKE_CURRENT_SOURCE_DIR}/StartUp
								${CMAKE_CURRENT_SOURCE_DIR}/SystemInfos
								${CMAKE_CURRENT_SOURCE_DIR}/_Template
								${CMAKE_CURRENT_SOURCE_DIR}/StatusBar
								${CMAKE_CURRENT_SOURCE_DIR}/LiveMap
								)

# target_include_directories(app PRIVATE
#     ${ZEPHYR_BASE}/modules/lvgl/include
# )