add_executable (MyHTTPServer "main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MyHTTPServer PROPERTY CXX_STANDARD 20)
endif()

add_subdirectory("src")

target_link_libraries(MyHTTPServer PRIVATE my_http_server_src)
