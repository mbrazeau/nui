file(GLOB nui_SRC
    "*.h"
    "*.cpp"
)
include_directories(../morphylib/include ../install/include ../PAWM/include ../pcl/include)
add_executable(nui ${nui_SRC})
target_link_libraries(nui ${NCL_LIB_FILE} pawm)
if (MSVC)
    target_link_libraries(nui wineditline)
else()
    target_link_libraries(nui ${PATH_LIB_EDIT} termcap)
endif()
install(TARGETS nui DESTINATION bin)
