file(GLOB nui_SRC
    "*.h"
    "*.cpp"
)
include_directories(../morphylib/include ../install/include ../PAWM/include)
add_executable(nui ${nui_SRC})
target_link_libraries(nui ${CMAKE_SOURCE_DIR}/../install/lib/ncl/libncl.a ${PATH_LIB_EDIT} termcap bsd pawm)

install(TARGETS nui DESTINATION bin)
