#ifndef __A2_SOUND_BUFFER_H
#define __A2_SOUND_BUFFER_H


#define SOUND_BUFFER_CAPACITY 32

typedef struct {
    Sound  buffer [SOUND_BUFFER_CAPACITY];
    size_t current;
} SoundBuffer;

typedef struct {
    float volume, pitch, pan;
    bool  play_globally;
} SoundBufferOpt;


void a2_playsound_opt(SoundBuffer* sb, Sound s, SoundBufferOpt opt);
#define a2_playsound(SB, S, ...) \
    a2_playsound_opt(SB, S, (SoundBufferOpt) { \
            .volume = 1.0, .pitch = 1.0, .pan = 0.5,  \
            __VA_ARGS__}\
    )


// aka a2_playsound(sb, sound, .pan = 1.0, .pitch = 2.0, .volume = 1.0)
// but can be called as just a2_playsound(sb, sound);
void a2_playsound_opt(SoundBuffer* sb, Sound s, SoundBufferOpt opts) {
    if (sb->current >= SOUND_BUFFER_CAPACITY)
        sb->current = 0;

    if (opts.play_globally) {
        PlaySound(s);
        return;
    }

    sb->buffer[sb->current] = LoadSoundAlias(s);
    SetSoundVolume(sb->buffer[sb->current], opts.volume);
    SetSoundPitch(sb->buffer[sb->current],  opts.pitch);
    SetSoundPan(sb->buffer[sb->current],    opts.pan);
    PlaySound(sb->buffer[sb->current]);
    sb->current++;
}

#endif//__A2_SOUND_BUFFER_H
