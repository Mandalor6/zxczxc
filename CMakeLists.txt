cmake_minimum_required(VERSION 3.20)
project(network)

set(CMAKE_CXX_STANDARD 17)

if (NOT VCPKG_TOOLCHAIN)
    set(VCPKG_TOOLCHAIN "c:/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif ()

set(USE_TOOLCHAIN "")

message("VCPKG_TOOLCHAIN is set to: ${VCPKG_TOOLCHAIN}")

if (WIN32)
    set(OS_NAME "win")
    set(OS_ARCHS "x64")
    set(USE_TOOLCHAIN "-DCMAKE_TOOLCHAIN_FILE=${VCPKG_TOOLCHAIN}")
elseif (UNIX)
    set(OS_NAME "linux")
    set(OS_ARCHS "x64")
elseif (APPLE)
    set(OS_NAME "darwin")
    set(OS_ARCHS "x64" "arm64")
else ()
    message(FATAL_ERROR "Not supported system")
endif ()

## Try to find the path to vswhere.exe
#find_program(VSWHERE_EXECUTABLE vswhere)
#
#if (NOT VSWHERE_EXECUTABLE)
#    message(FATAL_ERROR "vswhere not found. Please install Visual Studio or adjust the path.")
#endif ()


set(NODE_VERSIONS "v16.0.0" "v18.0.0" "v20.0.0" "v21.0.0")
set(NODE_ABIS "93" "108" "115" "120")
set(ITERATE_ABI 0)

# Find the curl executable
find_program(CURL_EXECUTABLE NAMES curl)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/targets)
#file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/boringssl)

foreach (OS_ARCH IN LISTS OS_ARCHS)
    # Build boringssl
    # Путь к проекту BoringSSL
    set(BORINGSSL_SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/boringssl)

    # Выходная директория сборки
    set(BORINGSSL_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/boringssl)

    # Создаем директории сборки
    file(MAKE_DIRECTORY ${BORINGSSL_BUILD_DIR})

    # Команда сборки проекта BoringSSL с использованием Ninja
    #    execute_process(
    #            COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release -GNinja ${BORINGSSL_SOURCE_DIR} -DCMAKE_INSTALL_PREFIX=${BORINGSSL_BUILD_DIR}
    #            WORKING_DIRECTORY ${BORINGSSL_BUILD_DIR}
    #    )
    #
    #    execute_process(
    #            COMMAND ${CMAKE_COMMAND} --build . --target crypto ssl
    #            WORKING_DIRECTORY ${BORINGSSL_BUILD_DIR}
    #    )

    # Build lsquic
    # Путь к проекту lsquic
    set(LSQUIC_SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/lsquic)

    # Выходная директория сборки
    set(LSQUIC_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/lsquic)

    # Создаем директории сборки
    file(MAKE_DIRECTORY ${LSQUIC_BUILD_DIR})

    # Команда сборки проекта lsquic

    if (WIN32)
        execute_process(
                COMMAND ${CMAKE_COMMAND} -GNinja ${USE_TOOLCHAIN} -DLSQUIC_BIN=Off -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=Release -DBORINGSSL_DIR=${BORINGSSL_SOURCE_DIR} ${LSQUIC_SOURCE_DIR} -DCMAKE_INSTALL_PREFIX=${LSQUIC_BUILD_DIR}
                WORKING_DIRECTORY ${LSQUIC_BUILD_DIR}
        )
    else ()
        execute_process(
                COMMAND ${CMAKE_COMMAND} -DLSQUIC_BIN=Off -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=Release -DBORINGSSL_DIR=${BORINGSSL_SOURCE_DIR} ${LSQUIC_SOURCE_DIR} -DCMAKE_INSTALL_PREFIX=${LSQUIC_BUILD_DIR}
                WORKING_DIRECTORY ${LSQUIC_BUILD_DIR}
        )
    endif ()


    execute_process(
            COMMAND ${CMAKE_COMMAND} --build .
            WORKING_DIRECTORY ${LSQUIC_BUILD_DIR}
    )


    message("ARCH: ${OS_ARCH}")

    # Build addons
    foreach (NODE_VERSION IN LISTS NODE_VERSIONS)
        list(GET NODE_ABIS ${ITERATE_ABI} NODE_ABI)
        MATH(EXPR ITERATE_ABI "${ITERATE_ABI}+1")

        # Get node headers and lib
        set(HEADERS_URL "https://nodejs.org/dist/${NODE_VERSION}/node-${NODE_VERSION}-headers.tar.gz")
        set(LIB_URL "https://nodejs.org/dist/${NODE_VERSION}/win-x64/node.lib")

        execute_process(COMMAND ${CURL_EXECUTABLE} ${HEADERS_URL} -o "node-${NODE_VERSION}-headers.tar.gz")
        execute_process(COMMAND tar xzf node-${NODE_VERSION}-headers.tar.gz -C targets)
        execute_process(COMMAND ${CURL_EXECUTABLE} ${LIB_URL} -o "targets/node-${NODE_VERSION}/node.lib")

        # Build libs
        add_library("node-${NODE_ABI}" SHARED library.cpp)

        set_target_properties("node-${NODE_ABI}" PROPERTIES
                OUTPUT_NAME "node-${OS_NAME}-${NODE_ABI}"
                PREFIX ""
                SUFFIX ".node"
        )

        # Include dirs
        target_include_directories("node-${NODE_ABI}" PUBLIC include)
        target_include_directories("node-${NODE_ABI}" PUBLIC third_party/lsquic/include)
        target_include_directories("node-${NODE_ABI}" PUBLIC third_party/lsquic/wincompat)
        target_include_directories("node-${NODE_ABI}" PUBLIC third_party/boringssl/include)

        # Link libs
    endforeach ()
endforeach ()


#add_library(xxx SHARED library.cpp)

#set_target_properties(xxx PROPERTIES LIBRARY_OUTPUT_NAME "socket")
#set_target_properties(xxx PROPERTIES OUTPUT_NAME "socket")


#set_target_properties(xxx PROPERTIES
#        OUTPUT_NAME "soooooocket"
#        PREFIX ""
#)


#if (WIN32)
#    execute_process(COMMAND rm -r -fo * -e *.node)
#else (UNIX)
#    execute_process(COMMAND rm -rf $ (ls -I "*.node"))
#endif ()


#Win
#rm -r -fo * -e *.node
#cmake -DCMAKE_BUILD_TYPE=Release -GNinja ../../boringssl/

#Unix
#rm -rf $(ls -I "*.node")