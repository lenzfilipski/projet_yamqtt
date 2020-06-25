cmake_minimum_required(VERSION 3.17)


# set the project name
project(yamqtt VERSION 0.1.0 LANGUAGES C)


set( serveur_dir "src/serveur" )
set( client_dir "src/client" )
set( EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin" )

set ( INSTALL_BIN_PATH bin )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/src" 
                     "${CMAKE_CURRENT_BINARY_DIR}" )

file( GLOB serveur_yamqtt "${serveur_dir}/*.c"
                          "${serveur_dir}/*.h" )
file( GLOB client_yamqtt "${client_dir}/*.c" )


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(serveur_yamqtt ${serveur_yamqtt})
add_executable(client_yamqtt ${client_yamqtt})

target_link_libraries(serveur_yamqtt PRIVATE Threads::Threads)
target_link_libraries(client_yamqtt PRIVATE Threads::Threads)

install( TARGETS serveur_yamqtt client_yamqtt
         RUNTIME DESTINATION ${INSTALL_BIN_PATH}  COMPONENT main )
