file(GLOB nui_SRC
    "*.h"
    "*.cpp"
)
set(EXENAME "morphy_nui_v${CMAKE_PROJECT_VERSION}")
if (BETA)
set(EXENAME "${EXENAME}b")
endif()
include_directories(${CMAKE_SOURCE_DIR}/../mpl/include ${CMAKE_SOURCE_DIR}/../install/include)
add_executable(${EXENAME} ${nui_SRC})
target_link_libraries(${EXENAME} ${PATH_LIB_NCL} mpl)
if (MSVC)
    target_link_libraries(${EXENAME} edit_static)
    include_directories(${CMAKE_SOURCE_DIR}/../external/wineditline/include)
else()
    target_link_libraries(${EXENAME} ${PATH_LIB_EDIT})
endif()
install(TARGETS ${EXENAME} DESTINATION bin)
