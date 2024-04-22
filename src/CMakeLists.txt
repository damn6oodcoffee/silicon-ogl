add_executable(${PROJECT_NAME} WIN32
               Atom.cpp
               Atom.h
               Silicon.cpp
               Silicon.h
               MyApp.cpp
               MyApp.h   
               MyFrame.cpp 
               MyFrame.h  
               OGLPane.cpp 
               OGLPane.h  
               RenderTimer.cpp  
               RenderTimer.h
               )


target_link_libraries(${PROJECT_NAME} PRIVATE external_deps)