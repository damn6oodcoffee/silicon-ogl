message(STATUS "Searching for wxWidgets and OpenGL...")

add_library(external_deps INTERFACE)
find_package(OpenGL REQUIRED)
find_package(wxWidgets COMPONENTS core base gl REQUIRED)
target_link_libraries(external_deps INTERFACE ${OpenGL_LIBRARIES})
target_link_libraries(external_deps INTERFACE wxWidgets::wxWidgets)
