#include "raylib.h"
#define NOB_IMPLEMENTATION
#include "../nob.h"

enum {
    GEN_ERROR_NONE,
    GEN_ERROR_WIPE,
    GEN_ERROR_APPEND,
};

typedef unsigned int u32;
typedef const char* istr;

// TODO: probably clean this up idk
// NOTE: baking files enabled with '-DBAKE' on codegen executable (./codegen/gen)

void    generate_getter_definition  (Nob_String_Builder*, istr, istr, istr);
int     generate_filedata_headers   (istr, istr);

int main(void) {
    int result = generate_filedata_headers("./assets/", "./assets/assets.c");
    if(result != GEN_ERROR_NONE) {
        nob_log(NOB_ERROR,"FAILED AT GENERATING CODE FOR ASSETS!");
        return -1;
    }
}




void generate_getter_definition(
            Nob_String_Builder* sb, 
            const char* name,
            const char* type, 
            const char* file
        ) 
{
#define cmps(L,R) (L) && (R) && strcmp(L,R) == 0

    //char variable_code  [512]={0}; 
    char data           [512]={0};
    char s_channles     [512]={0};
    char s_frames       [512]={0};
    char s_sample_rt    [512]={0};
    char s_sample_sz    [512]={0};

    nob_sb_appendf(sb, "%s asset_get_%s(void) {\n", type, name);
    
    if (cmps(type, "Sound")) {
        TextCopy(data,       TextToUpper(TextFormat("%s_data", name)));
        TextCopy(s_channles, TextToUpper(TextFormat("%s_channels", name)));
        TextCopy(s_frames,   TextToUpper(TextFormat("%s_frame_count", name)));
        TextCopy(s_sample_sz,TextToUpper(TextFormat("%s_sample_size", name)));
        TextCopy(s_sample_rt,TextToUpper(TextFormat("%s_sample_rate", name)));

#ifdef BAKE
        nob_sb_appendf(sb, "\tWave v = {\n");
        nob_sb_appendf(sb, "\t\t"   ".data       = (void*)%s,\n", data);
        nob_sb_appendf(sb, "\t\t"   ".frameCount = %s,\n", s_frames);
        nob_sb_appendf(sb, "\t\t"   ".channels   = %s,\n", s_channles);
        nob_sb_appendf(sb, "\t\t"   ".sampleRate = %s,\n", s_sample_rt);
        nob_sb_appendf(sb, "\t\t"   ".sampleSize = %s,\n", s_sample_sz);
        nob_sb_appendf(sb, "\t};\n");
#endif

#if 0
        nob_sb_appendf(sb, 
                "\tWave v = LoadWaveFromMemory("
                "\".mp3\","
                "(const unsigned char*) %s,"
                //"%s*%s*sizeof(%s[0]));\n"
                //"%s);\n"
                "sizeof(%s));\n"
                ,
                data, data
        );
#endif
        //nob_sb_appendf(sb, "");
    }
    else {}
#ifdef BAKE
    nob_sb_appendf(sb, "\t%s it = LoadSoundFromWave(v);\n", type);
    nob_sb_appendf(sb, "\t"
            "if(!IsWaveValid(v)) "
            "TraceLog(LOG_ERROR,\"FAILED TO LOAD WAVE: %s\");\n", name);
#endif

#ifndef BAKE
    nob_sb_appendf(sb, "\t%s it = LoadSound(\"%s\");\n", type, file);
#endif

    nob_sb_appendf(sb, "\t"
            "if(!IsSoundValid(it)) "
            "TraceLog(LOG_ERROR,\"FAILED TO LOAD SOUND: %s\");\n", name);
    nob_sb_appendf(sb, "\treturn it;\n");

    nob_sb_append_cstr(sb, "}\n");
#undef cmps
}
// TODO: this function should tell if all the file defs exist
// if so we can skip regeneration of assets, unless -FAREG is given as flag
#if 0
bool generate_can_skip(const char* dpath) {

}
#endif

int generate_filedata_headers(const char* dpath, const char* fpath) {
#define cmps(L,R) (L) && (R) && strcmp(L,R) == 0
#define go_defer(...) do {{__VA_ARGS__} goto defer;} while(0)

    const char* header = 
        "// GENERATED DATA FOR ASSETS //\n"
    ;

    FILE* assets    = 0;
    int result      = 0;

    Nob_String_Builder includ  = {0};
    Nob_String_Builder gentrs  = {0};
    Nob_String_Builder typedf  = {0};
    Nob_String_Builder getter  = {0};
    Nob_String_Builder fndefs  = {0};
    Nob_String_Builder unload  = {0};
    FilePathList files = LoadDirectoryFiles(dpath);
    const char* wd = GetApplicationDirectory();
    nob_log(NOB_INFO, "Working directory: '%s'", wd);

    // generate struct definition
    nob_sb_append_cstr(&typedf, "typedef struct {\n");
    // generate function definition
    nob_sb_append_cstr(&getter, "Assets a2_assets_get(void) {\n");
    nob_sb_append_cstr(&getter, "\treturn (Assets) {\n");
    // generate unloader for every asset
    nob_sb_append_cstr(&unload, "void a2_assets_unload(Assets* a) {\n");
    
    nob_log(NOB_INFO, "---------- BAKING ASSETS: ----------");
    for(u32 i = 0; i < files.count; i++) {
        const char* fext      = GetFileExtension(files.paths[i]);
        const char* fname     = GetFileNameWithoutExt(files.paths[i]);
        //const char* fnamefull = GetFileName(files.paths[i]);
        char* type = "";
        char rval[512]= {0};
        char out[512] = {0};


        if(cmps(fext, ".mp3")) {
            type = "Sound";
            // generate the sound file
            Wave wv = LoadWave(files.paths[i]);
            if(IsWaveValid(wv)) {
                TextCopy(out,TextFormat("%s../assets/baked/%s.c",wd, fname));
                TextCopy(rval, TextFormat("asset_get_%s()",fname));
                
                nob_log(NOB_INFO, "\t codegen: create file '%s'", out);

#ifdef BAKE
                nob_sb_appendf(&includ, "#include \"%s\"\n", out);
#endif
                nob_sb_appendf(&unload, "\tUnloadSound(a->%s); a->%s = (%s){0};\n", 
                        fname,fname, type
                );
                generate_getter_definition(&fndefs, fname, type, files.paths[i]);
              
#ifdef BAKE
                ExportWaveAsCode(wv, out);
#endif
            } else {
                TextCopy(rval, "{0}");
                nob_log(NOB_ERROR, "codegen: failed to load wave for generation");
            }
        } 
        
        // File is not supported
        else {
            nob_log(NOB_WARNING, 
                    "Failed to process asset '%s',"
                    " unsuported format, will be set to `{0}`",
                    fext
            );
            continue;
        }

        nob_sb_appendf(&gentrs, "%s asset_get_%s(void);\n", type, fname);
        nob_sb_appendf(&typedf, "\t%s %s;\n", type, fname);
        nob_sb_appendf(&getter, "\t\t\t.%s = %s,\n", fname, rval);
        nob_log(NOB_INFO, "------------------------------------");
    }

    nob_sb_append_cstr(&unload, "}\n");
    nob_sb_append_cstr(&typedf, "} Assets;\n");
    nob_sb_append_cstr(&getter, "\t};\n");
    nob_sb_append_cstr(&getter, "}\n");

    // write to file
    assets = fopen(fpath, "w");
    if (!assets) go_defer( result = GEN_ERROR_APPEND; );
    
    fprintf(assets, 
            "#ifndef __ASSETS_H\n"
            "#define __ASSETS_H\n"
            "#include \"../raylib.h\"\n"
            "%s\n"
            "%.*s\n"
            "%.*s\n"
            "%.*s\n"
            "%.*s\n"
            "%.*s\n"
            "%.*s"
            "#endif//__ASSETS_H\n"
            , 
            header,

            (int)   includ.count,
                    includ.items,

            (int)   gentrs.count,
                    gentrs.items,

            (int)   typedf.count,
                    typedf.items,

            (int)   unload.count,
                    unload.items,

            (int)   getter.count,
                    getter.items,

            (int)   fndefs.count,
                    fndefs.items
    );

    fclose(assets);

#undef cmps
defer:
    UnloadDirectoryFiles(files);
    nob_sb_free(typedf);
    nob_sb_free(getter);
    return result;
}
