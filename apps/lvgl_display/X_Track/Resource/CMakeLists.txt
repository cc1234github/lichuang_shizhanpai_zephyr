file(GLOB CURRENT_DIR_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Font/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/Image/*.c
)
target_sources(app PRIVATE ${CURRENT_DIR_SRCS})

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                            ${CMAKE_CURRENT_SOURCE_DIR}/Font
                            ${CMAKE_CURRENT_SOURCE_DIR}/Image
                            )

# target_include_directories(app PRIVATE
#     ${ZEPHYR_BASE}/modules/lvgl/include
# )