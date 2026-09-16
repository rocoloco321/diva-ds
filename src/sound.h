#pragma once

#define SOUND_HEAP_SIZE 0x80000
#define STREAM_THREAD_PRIO 10

void sound_init();
void sound_load(int currScene);
void sound_main();
void sound_resume();
void sound_pause();
void sound_stop(int fadeOutFrames);
