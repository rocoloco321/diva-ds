#pragma once

extern NNSFndHeapHandle gMainHeap;
extern NNSFndHeapHandle gSceneHeap;

void heap_preInit();
void initHeap();

NNSFndHeapHandle heap_create(void* address, u32 size);
void* heap_alloc(NNSFndHeapHandle heap, u32 size);
void* heap_allocEx(NNSFndHeapHandle heap, u32 size, u32 alignment);
void heap_free(NNSFndHeapHandle heap, void* memoryBlock);
void heap_destroySubHeap(NNSFndHeapHandle subHeap);
NNSFndHeapHandle heap_createSubExpHeap(NNSFndHeapHandle heap);
NNSFndHeapHandle heap_createSubFrmHeap(NNSFndHeapHandle heap);
NNSFndHeapHandle heap_createSubFrmHeapWithSize(NNSFndHeapHandle heap, u32 size);
NNSFndHeapHandle heap_createSubFrmHeapWithSizeFromTail(NNSFndHeapHandle heap, u32 size);
void heap_adjustSubFrmHeap(NNSFndHeapHandle heap);

static inline NNSFndHeapHandle getMainHeap()
{
    return gMainHeap;
}

static inline NNSFndHeapHandle getSceneHeap()
{
    if(gSceneHeap == NULL)
    {
        return getMainHeap();
    }
    return gSceneHeap;
}

#define proc_alloc(size)				heap_alloc(getSceneHeap(), (size))
#define proc_allocEx(size, alignment)	heap_allocEx(getSceneHeap(), (size), (alignment))
#define proc_free(memoryBlock)			heap_free(getSceneHeap(), (memoryBlock))
