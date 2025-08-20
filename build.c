// build options

#define WARN_IGNORE "-Wno-override-init"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define NOB_IMPLEMENTATION

#include "nob.h"
#include "raylib.h"

enum {
    BUILD_OPT_GDB               = 1,
    BUILD_OPT_ADDRESS_SANITIZER = 2,
    BUILD_OPT_DEBUG_MODE        = 4,
    BUILD_OPT_BAKE_ASSETES      = 8,
    BUILD_OPT_RELEASE           = 16,
};

enum  {
    PLATFORM_LINUX,
    PLATFORM_LIN_TO_WIN,
    PLATFORM_WINDOWS,
};

enum { EXEC_OUT, EXEC_IN };

#define INOUT_MAKE(l,r) (const char*[2]) { l, r }

#define FLAG(F) flag(argc, argv, (F))
bool flag(int argc, char** argv, const char* flagname) {
    for(int i = 0; i < argc; i++)
        if (strcmp(argv[i], flagname) == 0) 
            return true;
    return false;
}

int build_main(int agrc, char** argv, const char** inout, int platform, int opt) {
    Nob_Cmd cmd = {0};
    bool windows = 
        (platform & PLATFORM_LIN_TO_WIN)
        || (platform & PLATFORM_WINDOWS)
    ;

    if (windows)  {
        nob_cmd_append(&cmd, 
                (platform & PLATFORM_WINDOWS) ? "gcc" : "x86_64-w64-mingw32-gcc", 
                "-o", 
                inout[EXEC_OUT], inout[EXEC_IN], 
                "-Wall", "-Wextra",
                "-I./lib/rl-windows/include/", 
                "-L./lib/rl-windows/lib/",
        );
    }
    else {
        nob_cmd_append(&cmd, 
                "cc", "-o", inout[EXEC_OUT], inout[EXEC_IN], 
                "-Wall", "-Wextra"
                );
    }

    nob_cmd_append(&cmd, WARN_IGNORE);

    if (opt & BUILD_OPT_GDB)                
        nob_cmd_append(&cmd, "-ggdb");
    if (opt & BUILD_OPT_ADDRESS_SANITIZER)    
        nob_cmd_append(&cmd, "-fsanitize=address");
    if (opt & BUILD_OPT_DEBUG_MODE) nob_cmd_append(&cmd,"-D=DEBUG");
    if (opt & BUILD_OPT_BAKE_ASSETES) nob_cmd_append(&cmd,"-D=BAKE");


    nob_cmd_append(&cmd, "-lraylib");

    if (windows) 
        nob_cmd_append(&cmd, "-lwinmm", "-lgdi32");
    else 
        nob_cmd_append(&cmd, "-lm", "-lX11", "-lGL");

    return nob_cmd_run_sync(cmd);
} 

void run(const char* out) {
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, out);

    if (!nob_cmd_run_sync(cmd)) 
        nob_log(NOB_ERROR, "Failed to run exec");
}

void print_help(int argc, char** argv) {
    printf(
            "Asteroids2 Builder: "  "\n"
            "\t"        "USAGE: ./build <target> <arguments>" "\n"
            "\t"        "> TARGETS: " "\n"
            "\t\t"      "'win'      - target windows" "\n"
            "\t\t\t"    "[!] NOTE: this target is probably broken because i don't have windows :d" "\n"
            "\t\t"      "'lin'      - (DEFAULT) target linux(or other posix enviroment)" "\n"
            "\t\t"      "'lintowin' - target windows from linux enviroment (uses mingw)" "\n"
            "\t"        "> ARGUMENTS: " "\n"
            "\t\t"      "'bake'   - bake assets into executable" "\n"
            "\t\t"      "'debug'  - define DEBUG, adds debug tools/information" "\n"
            "\t\t"      "'release'- compiles with compiler optimisations enabled" "\n"
            "\t\t"      "'run'    - run executable after compilation" "\n"
            "\n"
            "\t"        "Examples:" "\n"
            "\t\t"      "./build debug run - linux debug build" "\n"
            "\t\t"      "./build lintowin bake debug run - make '.exe' debug baked build" "\n"

    );
    nob_log(NOB_INFO,"early quit after help message");
}

bool build_codegen(int opts, int platform) {
    Nob_Cmd cmd = {0};
    bool windows = 
        (platform & PLATFORM_LIN_TO_WIN)
        || (platform & PLATFORM_WINDOWS)
    ;

    nob_cmd_append(&cmd, 
            "cc", "-o",  
            "./codegen/gen",
            "./codegen/gen.c",
            "-Wall", "-Wextra",
            "-ggdb"
    );

    nob_cmd_append(&cmd, "-lraylib");

    if (platform & PLATFORM_WINDOWS) 
        nob_cmd_append(&cmd, "-lwinmm", "-lgdi32");
    else 
        nob_cmd_append(&cmd, "-lm", "-lX11", "-lGL");

    if (opts & BUILD_OPT_DEBUG_MODE)
        nob_cmd_append(&cmd, "-D=DEBUG");
    if (opts & BUILD_OPT_BAKE_ASSETES)
        nob_cmd_append(&cmd, "-D=BAKE");
    
    return nob_cmd_run_sync(cmd);
}

int main(int argc, char** argv) {
    NOB_GO_REBUILD_URSELF(argc,argv);

    int target = PLATFORM_LINUX;
    int opts = 0;
    bool force_debug = false;
    
    const char* inout[2] = {
        [EXEC_IN]   = "./main.c",
        [EXEC_OUT]  = "./bin/main"
    };

    if (FLAG("help")) {
        print_help(argc,argv);
        return 0;
    }

    if (FLAG("debug") || force_debug) {
        opts |= BUILD_OPT_DEBUG_MODE;
        opts |= BUILD_OPT_GDB;
        //opts |= BUILD_OPT_ADDRESS_SANITIZER;
    }

    if (FLAG("bake")) {
        opts |= BUILD_OPT_BAKE_ASSETES;
    }

    if (FLAG("lintowin")) 
        target = PLATFORM_LIN_TO_WIN;
    else if (FLAG("win")) 
        target = PLATFORM_WINDOWS;
    else if (FLAG("lin"))
        target = PLATFORM_LINUX;
    else  {
        nob_log(NOB_WARNING,"NO TARGET SPECIFIED, fallback is 'linux'");
        target = PLATFORM_LINUX;
    }

    bool windows = 
        (target & PLATFORM_LIN_TO_WIN)
        || (target & PLATFORM_WINDOWS)
    ;

    nob_log(NOB_INFO, "BUILDING CODE GENERATOR");
    if (!build_codegen(opts, target)) {
        nob_log(NOB_ERROR,"Failed to build asset codegen script!");
        return 1;
    }

    run("./codegen/gen");

    if (!build_main(argc,argv, inout, target, opts)) {
        nob_log(NOB_ERROR,"Build of main executable failed");
        return 1;
    }


    if (FLAG("run")) {
        if (windows)    run("./bin/main.exe");
        else            run("./bin/main");
    }
}
