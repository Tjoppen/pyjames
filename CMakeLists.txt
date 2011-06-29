cmake_minimum_required(VERSION 2.6)
project(pyjames)

add_subdirectory(libjames)

add_executable(pyjames
    main.cpp
    Class.cpp
    BuiltInClasses.cpp
)

target_link_libraries(pyjames libjames)

# Account for the Xerces-C++ 3.1.x library being called xerces-c_3 on WIN32
if(WIN32)
    target_link_libraries(pyjames xerces-c_3)
else(WIN32)
    target_link_libraries(pyjames xerces-c)
endif(WIN32)

install(TARGETS pyjames DESTINATION bin)

