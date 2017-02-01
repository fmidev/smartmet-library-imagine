cmake_minimum_required(VERSION 3.7.1)
project(imagine)

set(CMAKE_MFC_FLAG 0)

add_definitions( 
    -D_UNICODE 
    -DUNICODE
    -DIMAGINE_IGNORE_FORMATS
    )

include_directories( 
    $ENV{BOOST_INCLUDE_DIR}
    $ENV{LIBS_FMI}/imagine/include
    $ENV{LIBS_FMI}/newbase/include
    )

file( GLOB CPP_FILES source/*.cpp )
file( GLOB H_FILES include/*.h )

add_library( 
    ${PROJECT_NAME} STATIC
    ${CPP_FILES} 
    ${H_FILES} 
    )
