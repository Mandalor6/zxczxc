#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* List of platform features */
#ifdef _WIN32
#define OS "wn32"
#define IS_WINDOWS
#endif
#ifdef __linux
#define OS "linux"
#define IS_LINUX
#endif
#ifdef __APPLE__
#define OS "darwin"
#define IS_MACOS
#endif

const char *ARM = "arm";
const char *ARM64 = "arm64";
const char *X64 = "x64";

/* System, but with string replace */
int run(const char *cmd, ...) {
    char buf[2048];
    va_list args;
    va_start(args, cmd);
    vsprintf(buf, cmd, args);
    va_end(args);
    printf("--> %s\n\n", buf);
    return system(buf);
}

void build_lsquic(const char *arch) {
#ifndef IS_WINDOWS
    /* Build for x64 or arm/arm64 (depending on the host) */
    run("cd build && mkdir lsquic && cd lsquic && cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBORINGSSL_DIR=../../third_party/boringssl -DCMAKE_BUILD_TYPE=Release -DLSQUIC_BIN=Off ../../third_party/lsquic && make lsquic");
#else
    run("cd build && mkdir lsquic && cd lsquic && cmake -DCMAKE_TOOLCHAIN_FILE=c:/vcpkg/scripts/buildsystems/vcpkg.cmake  -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DBORINGSSL_DIR=../../third_party/boringssl -DCMAKE_BUILD_TYPE=Release -DLSQUIC_BIN=Off ../../third_party/lsquic && msbuild ALL_BUILD.vcxproj");
#endif
}

/* Build boringssl */
void build_boringssl(const char *arch) {

#ifdef IS_MACOS
    /* Build for x64 (the host) */
    run("cd build && mkdir -p x64/boringssl && cd x64/boringssl && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.14 ../../../third_party/boringssl && make crypto ssl");

    /* Build for arm64 (cross compile) */
    run("cd build && mkdir -p arm64/boringssl && cd arm64/boringssl && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64 ../../../third_party/boringssl && make crypto ssl");
#endif

#ifdef IS_LINUX
    /* Build for x64 or arm/arm64 (depending on the host) */
    run("cd build && mkdir -p x64/boringssl && cd x64/boringssl && cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=Release ../../../third_party/boringssl && make crypto ssl", arch, arch);
#endif

#ifdef IS_WINDOWS
    /* Build for x64 (the host) */
//    run("mkdir -p build/x64/boringssl");
    run("cd build && mkdir x64\\boringssl && cd x64/boringssl && cmake -DCMAKE_BUILD_TYPE=Release -GNinja ../../../third_party/boringssl && ninja crypto ssl");
#endif

}

int main() {
    printf("[Preparing]\n");
    printf("\n[Building]\n");
    const char *arch = X64;
#ifdef __arm__
    arch = ARM;
#endif
#ifdef __aarch64__
    arch = ARM64;
#endif
    /* Build for x64 and/or arm/arm64 */
//    build_boringssl(arch);

    build_lsquic(arch);
}
