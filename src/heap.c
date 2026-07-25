#include "common.h"
#include "heap.h"

#define SYSTEM_HEAP_SIZE        64*1024
#define ROUND_UP(value, alignment) \
    (((u32)(value) + (alignment-1)) & ~(alignment-1))

#define ROUND_DOWN(value, alignment) \
    ((u32)(value) & ~(alignment-1))

NNSFndHeapHandle gMainHeap;
NNSFndHeapHandle gSceneHeap = NULL;

void heap_preInit()
{
    void* sysHeapMemory = OS_AllocFromMainArenaLo(SYSTEM_HEAP_SIZE, 16);
    u32 arenaLow = ROUND_UP(OS_GetMainArenaLo(), 16);
	u32 arenaHigh = ROUND_DOWN(OS_GetMainArenaHi(), 16);
    u32 appHeapSize = arenaHigh - arenaLow;
    void* appHeapMemory = OS_AllocFromMainArenaLo(appHeapSize, 16);
	gMainHeap = NNS_FndCreateExpHeapEx(appHeapMemory, appHeapSize, 1);
}

void initHeap()
{
	// Setup 2 vram blocks for textures.
	u32 mSzWork = NNS_GfdGetLnkTexVramManagerWorkSize(32);
	u32 szWork = NNS_GfdGetLnkPlttVramManagerWorkSize(32);

	void* mPMgrWork = heap_alloc(gMainHeap, mSzWork);
	void* pMgrWork = heap_alloc(gMainHeap, szWork);
	NNS_GfdInitLnkTexVramManager(0x40000, 0x20000, mPMgrWork, mSzWork, TRUE);
	NNS_GfdInitLnkPlttVramManager(0x10000, pMgrWork, szWork, TRUE);    
}

NNSFndHeapHandle heap_create(void* address, u32 size)
{
    return NNS_FndCreateExpHeapEx(address, size, NNS_FND_HEAP_OPT_0_CLEAR);
}

void* heap_alloc(NNSFndHeapHandle heap, u32 size)
{
    void* result = NULL;
    switch (heap->signature)
    {
        case NNSI_EXPHEAP_SIGNATURE:
            result = NNS_FndAllocFromExpHeap(heap, size);
            break;
        case NNSI_FRMHEAP_SIGNATURE:
            result = NNS_FndAllocFromFrmHeap(heap, size);
            break;
    }
    return result;
}

void* heap_allocEx(NNSFndHeapHandle heap, u32 size, u32 alignment)
{
    void* result = NULL;
    switch (heap->signature)
    {
        case NNSI_EXPHEAP_SIGNATURE:
            result = NNS_FndAllocFromExpHeapEx(heap, size, alignment);
            break;
        case NNSI_FRMHEAP_SIGNATURE:
            OSIntrMode oldIrq = OS_DisableInterrupts();
            result = NNS_FndAllocFromFrmHeapEx(heap, size, alignment);
            OS_RestoreInterrupts(oldIrq);
            break;
    }
    return result;
}

void heap_free(NNSFndHeapHandle heap, void* memoryBlock)
{
    switch (heap->signature)
    {
        case NNSI_EXPHEAP_SIGNATURE:
            NNS_FndFreeToExpHeap(heap, memoryBlock);
            break;
        case NNSI_FRMHEAP_SIGNATURE:
            break;
    }
}

void heap_destroySubHeap(NNSFndHeapHandle subHeap)
{
    switch (subHeap->signature)
    {
        case NNSI_EXPHEAP_SIGNATURE:
            NNS_FndDestroyExpHeap(subHeap);
            break;
        case NNSI_FRMHEAP_SIGNATURE:
            NNS_FndDestroyFrmHeap(subHeap);
            break;
    }
    NNSFndHeapHandle containingHeap = NNS_FndFindContainHeap(subHeap);
    if (containingHeap)
        heap_free(containingHeap, subHeap);
}

static inline u32 getAllocatableSize(NNSFndHeapHandle heap)
{
    u32 allocatableSize = 0;
    switch (heap->signature)
    {
        case NNSI_EXPHEAP_SIGNATURE:
            allocatableSize = NNS_FndGetAllocatableSizeForExpHeap(heap);
            break;
        case NNSI_FRMHEAP_SIGNATURE:
            allocatableSize = NNS_FndGetAllocatableSizeForFrmHeap(heap);
            break;
    }
    return allocatableSize;
}

NNSFndHeapHandle heap_createSubExpHeap(NNSFndHeapHandle heap)
{
    u32 allocatableSize = getAllocatableSize(heap);
    return heap_create(heap_alloc(heap, allocatableSize), allocatableSize);
}

NNSFndHeapHandle heap_createSubFrmHeap(NNSFndHeapHandle heap)
{
    return heap_createSubFrmHeapWithSize(heap, getAllocatableSize(heap));
}

NNSFndHeapHandle heap_createSubFrmHeapWithSize(NNSFndHeapHandle heap, u32 size)
{
    return NNS_FndCreateFrmHeapEx(heap_alloc(heap, size), size, NNS_FND_HEAP_OPT_0_CLEAR);
}

NNSFndHeapHandle heap_createSubFrmHeapWithSizeFromTail(NNSFndHeapHandle heap, u32 size)
{
    return NNS_FndCreateFrmHeapEx(heap_allocEx(heap, size, -4), size, NNS_FND_HEAP_OPT_0_CLEAR);
}

void heap_adjustSubFrmHeap(NNSFndHeapHandle heap)
{
    u32 newSize = NNS_FndAdjustFrmHeap(heap);
    NNSFndHeapHandle containingHeap = NNS_FndFindContainHeap(heap);
    NNS_FndResizeForMBlockExpHeap(containingHeap, heap, newSize);
}
