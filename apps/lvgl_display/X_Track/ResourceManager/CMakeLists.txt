
target_sources(app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/ResourceManager.cpp
)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# target_include_directories(app PRIVATE
#     ${ZEPHYR_BASE}/modules/lvgl/include
# )