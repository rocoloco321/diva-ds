#include "common.h"
#include "scene.h"
#include "sound_data.sadl"
#include "sound.h"

static NNSSndArc arc;
static u8 sndHeap[SOUND_HEAP_SIZE];

static NNSSndHeapHandle heap;

static u32 playingPos;
int strmID = -1;

//static NNSSndHandle bgmHandle;
static NNSSndHandle seHandle;
static NNSSndStrmHandle strmHandle;

void sound_init()
{
    NNS_SndInit();
    heap = NNS_SndHeapCreate(&sndHeap, sizeof(sndHeap));
    NNS_SndArcInit(&arc, "/data/Sound/sound_data.sdat", heap, FALSE);
    NNS_SndArcPlayerSetup(heap);
    NNS_SndArcStrmInit(STREAM_THREAD_PRIO, heap);
    //NNS_SndHandleInit(&bgmHandle);
    NNS_SndArcLoadSeqArc(SEQ_SE, heap);
    NNS_SndArcLoadBank(BANK_SE, heap);
    NNS_SndHandleInit(&seHandle);
    NNS_SndStrmHandleInit(&strmHandle);
}

void sound_load(int currScene)
{
    strmID = -1;
    switch (currScene)
    {
    case SCENE_TITLE:
        strmID = STRM_MARIOKART;
        break;
    
    default:
        break;
    }
    if(strmID != -1)
    {   
        playingPos = 0;
        NNS_SndArcStrmStop(&strmHandle, 0);

        NNS_SndArcStrmPrepare(&strmHandle, strmID, 0);
        if(currScene != SCENE_PLAY)
        {
            while(!NNS_SndArcStrmIsPrepared(&strmHandle));
            NNS_SndArcStrmStartPrepared(&strmHandle);
        }
    }
}

void sound_resume()
{
    while(!NNS_SndArcStrmIsPrepared(&strmHandle));
    NNS_SndArcStrmStartPrepared(&strmHandle);
}

void sound_pause()
{
    playingPos = NNS_SndArcStrmGetCurrentPlayingPos(&strmHandle);
    NNS_SndArcStrmStop(&strmHandle, 4);
    NNS_SndArcStrmPrepare(&strmHandle, strmID, playingPos);
}

void sound_stop(int fadeOutFrames)
{
    NNS_SndArcStrmStop(&strmHandle, fadeOutFrames);
}

void sound_main()
{
    NNS_SndMain();
}
