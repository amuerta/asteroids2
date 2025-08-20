#ifndef __ASSETS_H
#define __ASSETS_H
#include "../raylib.h"
// GENERATED DATA FOR ASSETS //

#include "/home/amuerta/code/c/asteroids2/codegen/../assets/baked/engine.c"
#include "/home/amuerta/code/c/asteroids2/codegen/../assets/baked/gun.c"
#include "/home/amuerta/code/c/asteroids2/codegen/../assets/baked/rock.c"
#include "/home/amuerta/code/c/asteroids2/codegen/../assets/baked/shipbreak.c"
#include "/home/amuerta/code/c/asteroids2/codegen/../assets/baked/stop.c"
#include "/home/amuerta/code/c/asteroids2/codegen/../assets/baked/warp.c"

Sound asset_get_engine(void);
Sound asset_get_gun(void);
Sound asset_get_rock(void);
Sound asset_get_shipbreak(void);
Sound asset_get_stop(void);
Sound asset_get_warp(void);

typedef struct {
	Sound engine;
	Sound gun;
	Sound rock;
	Sound shipbreak;
	Sound stop;
	Sound warp;
} Assets;

void a2_assets_unload(Assets* a) {
	UnloadSound(a->engine); a->engine = (Sound){0};
	UnloadSound(a->gun); a->gun = (Sound){0};
	UnloadSound(a->rock); a->rock = (Sound){0};
	UnloadSound(a->shipbreak); a->shipbreak = (Sound){0};
	UnloadSound(a->stop); a->stop = (Sound){0};
	UnloadSound(a->warp); a->warp = (Sound){0};
}

Assets a2_assets_get(void) {
	return (Assets) {
			.engine = asset_get_engine(),
			.gun = asset_get_gun(),
			.rock = asset_get_rock(),
			.shipbreak = asset_get_shipbreak(),
			.stop = asset_get_stop(),
			.warp = asset_get_warp(),
	};
}

Sound asset_get_engine(void) {
	Wave v = {
		.data       = (void*)ENGINE_DATA,
		.frameCount = ENGINE_FRAME_COUNT,
		.channels   = ENGINE_CHANNELS,
		.sampleRate = ENGINE_SAMPLE_RATE,
		.sampleSize = ENGINE_SAMPLE_SIZE,
	};
	Sound it = LoadSoundFromWave(v);
	if(!IsWaveValid(v)) TraceLog(LOG_ERROR,"FAILED TO LOAD WAVE: engine");
	if(!IsSoundValid(it)) TraceLog(LOG_ERROR,"FAILED TO LOAD SOUND: engine");
	return it;
}
Sound asset_get_gun(void) {
	Wave v = {
		.data       = (void*)GUN_DATA,
		.frameCount = GUN_FRAME_COUNT,
		.channels   = GUN_CHANNELS,
		.sampleRate = GUN_SAMPLE_RATE,
		.sampleSize = GUN_SAMPLE_SIZE,
	};
	Sound it = LoadSoundFromWave(v);
	if(!IsWaveValid(v)) TraceLog(LOG_ERROR,"FAILED TO LOAD WAVE: gun");
	if(!IsSoundValid(it)) TraceLog(LOG_ERROR,"FAILED TO LOAD SOUND: gun");
	return it;
}
Sound asset_get_rock(void) {
	Wave v = {
		.data       = (void*)ROCK_DATA,
		.frameCount = ROCK_FRAME_COUNT,
		.channels   = ROCK_CHANNELS,
		.sampleRate = ROCK_SAMPLE_RATE,
		.sampleSize = ROCK_SAMPLE_SIZE,
	};
	Sound it = LoadSoundFromWave(v);
	if(!IsWaveValid(v)) TraceLog(LOG_ERROR,"FAILED TO LOAD WAVE: rock");
	if(!IsSoundValid(it)) TraceLog(LOG_ERROR,"FAILED TO LOAD SOUND: rock");
	return it;
}
Sound asset_get_shipbreak(void) {
	Wave v = {
		.data       = (void*)SHIPBREAK_DATA,
		.frameCount = SHIPBREAK_FRAME_COUNT,
		.channels   = SHIPBREAK_CHANNELS,
		.sampleRate = SHIPBREAK_SAMPLE_RATE,
		.sampleSize = SHIPBREAK_SAMPLE_SIZE,
	};
	Sound it = LoadSoundFromWave(v);
	if(!IsWaveValid(v)) TraceLog(LOG_ERROR,"FAILED TO LOAD WAVE: shipbreak");
	if(!IsSoundValid(it)) TraceLog(LOG_ERROR,"FAILED TO LOAD SOUND: shipbreak");
	return it;
}
Sound asset_get_stop(void) {
	Wave v = {
		.data       = (void*)STOP_DATA,
		.frameCount = STOP_FRAME_COUNT,
		.channels   = STOP_CHANNELS,
		.sampleRate = STOP_SAMPLE_RATE,
		.sampleSize = STOP_SAMPLE_SIZE,
	};
	Sound it = LoadSoundFromWave(v);
	if(!IsWaveValid(v)) TraceLog(LOG_ERROR,"FAILED TO LOAD WAVE: stop");
	if(!IsSoundValid(it)) TraceLog(LOG_ERROR,"FAILED TO LOAD SOUND: stop");
	return it;
}
Sound asset_get_warp(void) {
	Wave v = {
		.data       = (void*)WARP_DATA,
		.frameCount = WARP_FRAME_COUNT,
		.channels   = WARP_CHANNELS,
		.sampleRate = WARP_SAMPLE_RATE,
		.sampleSize = WARP_SAMPLE_SIZE,
	};
	Sound it = LoadSoundFromWave(v);
	if(!IsWaveValid(v)) TraceLog(LOG_ERROR,"FAILED TO LOAD WAVE: warp");
	if(!IsSoundValid(it)) TraceLog(LOG_ERROR,"FAILED TO LOAD SOUND: warp");
	return it;
}
#endif//__ASSETS_H
