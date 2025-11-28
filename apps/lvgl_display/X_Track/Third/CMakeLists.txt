
target_sources(app PRIVATE
	# ${CMAKE_CURRENT_SOURCE_DIR}/lv_anim_label/lv_anim_label.c

    ${CMAKE_CURRENT_SOURCE_DIR}/lv_poly_line/lv_poly_line.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/MapConv/MapConv.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MapConv/GPS_Transform/GPS_Transform.c
    ${CMAKE_CURRENT_SOURCE_DIR}/MapConv/TileSystem/TileSystem.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/minmea/minmea.c

    ${CMAKE_CURRENT_SOURCE_DIR}/PointContainer/PointContainer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/TileConv/TileConv.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/TrackFilter/TrackLineFilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackFilter/TrackPointFilter.cpp
)

target_include_directories(app 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Filters
    # ${CMAKE_CURRENT_SOURCE_DIR}/lv_anim_label
    ${CMAKE_CURRENT_SOURCE_DIR}/lv_poly_line
    ${CMAKE_CURRENT_SOURCE_DIR}/MapConv
    ${CMAKE_CURRENT_SOURCE_DIR}/MapConv/GPS_Transform
    ${CMAKE_CURRENT_SOURCE_DIR}/MapConv/TileSystem
    ${CMAKE_CURRENT_SOURCE_DIR}/minmea
    ${CMAKE_CURRENT_SOURCE_DIR}/PointContainer
    ${CMAKE_CURRENT_SOURCE_DIR}/TileConv
    ${CMAKE_CURRENT_SOURCE_DIR}/TrackFilter
)
