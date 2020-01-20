file(GLOB nui_SRC
    "*.h"
    "*.cpp"
)
include_directories(${CMAKE_SOURCE_DIR}/../mpl/include ${CMAKE_SOURCE_DIR}/../install/include)
add_executable(nui ${nui_SRC})
target_link_libraries(nui ${PATH_LIB_NCL} mpl)
if (MSVC)
    target_link_libraries(nui edit_static)
    include_directories(${CMAKE_SOURCE_DIR}/../external/wineditline/include)
else()
    target_link_libraries(nui ${PATH_LIB_EDIT})
endif()
install(TARGETS nui DESTINATION bin)
