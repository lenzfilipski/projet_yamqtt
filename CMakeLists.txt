cmake_minimum_required(VERSION 3.17)


# set the project name
project(yamqtt VERSION 0.1.0 LANGUAGES C)


set( serveur_dir "src/serveur" )
set( client_dir "src/client" )
set( EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin" )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/src" 
                     "${CMAKE_CURRENT_BINARY_DIR}" )

file( GLOB serveur "${serveur_dir}/*.c"
                   "${serveur_dir}/*.h" )
file( GLOB client "${client_dir}/*.c" )


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(serveur ${serveur})
add_executable(client ${client})

target_link_libraries(serveur PRIVATE Threads::Threads)
target_link_libraries(client PRIVATE Threads::Threads)
