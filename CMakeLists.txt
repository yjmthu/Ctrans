cmake_minimum_required(VERSION 3.2.1)

project(YOUDAOFANYI VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    add_compile_options("/utf-8")
endif()

set(CMAKE_BUILD_TYPE "Release")

set(YOUDAOFANYI_BINARY_DIR ./build)
set(YOUDAOFANYI_SOURCE_DIR ./)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# add_compile_options(-std=c++11 -Wall -Werror)

set(CMAKE_INCLUDE_CURRENT_DIR ON)  #当前include 打开

if(WIN32)
    add_subdirectory(utf8console)
endif()

set(YJSON_DIR ${CMAKE_SOURCE_DIR}/../YJson)

include_directories(
    ${YJSON_DIR}
)

add_subdirectory(${YJSON_DIR} yjson)

if (WIN32)
    include_directories(
        C:/MSVC2020/Boost/x64/include/boost-1_78
        C:/MSVC2020/OpenSSL/x64/include
        ${YJSON_DIR}
    )

    link_directories(
        C:/MSVC2020/Boost/x64/lib
        C:/MSVC2020/OpenSSL/x64/lib
    )
elseif(UNIX)
    include_directories(
        ${YJSON_DIR}
    )
endif()

set(SRC_LIST
    main.cpp
)

add_executable(yd ${SRC_LIST} )

if(WIN32)
    target_link_libraries(yd
        Wininet
        utf8_console
        libssl_static
        libcrypto_static
    #    ws2_32
        Crypt32
        yjson
    )
elseif(UNIX)
    target_link_libraries(yd
        ssl
        crypto
        yjson
    )
endif()

