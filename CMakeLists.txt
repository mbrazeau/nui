file(GLOB nui_SRC
    "*.h"
    "*.cpp"
)
include_directories(${CMAKE_SOURCE_DIR}/../morphylib/include ${CMAKE_SOURCE_DIR}/../install/include ${CMAKE_SOURCE_DIR}/../PAWM/include ${CMAKE_SOURCE_DIR}/../pcl/include)
add_executable(nui ${nui_SRC})
target_link_libraries(nui ${PATH_LIB_NCL} pawm)
if (MSVC)
    target_link_libraries(nui edit_static)
    include_directories(${CMAKE_SOURCE_DIR}/../external/wineditline/include)
else()
    target_link_libraries(nui ${PATH_LIB_EDIT})
endif()
install(TARGETS nui DESTINATION bin)
