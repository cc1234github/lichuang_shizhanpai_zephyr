
add_subdirectory(HAL)

add_subdirectory(DataCenter)
add_subdirectory(DataProc)
add_subdirectory(PageManager)
add_subdirectory(Pages)
add_subdirectory(Resource)
add_subdirectory(ResourceManager)

add_subdirectory(Third)

target_include_directories(app PRIVATE
    ${ZEPHYR_BASE}/modules/lvgl/include
    ${ZEPHYR_BASE}/modules/lvgl/src
)