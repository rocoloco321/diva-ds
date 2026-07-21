#include "common.h"
#include "heap.h"
#include "print.h"

#include "archive.h"

static void* loadFileToHeap(const char* path, int alignment)
{
    BOOL ok;
    FSFile file;
    void* data = NULL;
    if (!FS_IsAvailable())
        return data;
    FS_InitFile(&file);
    ok = FS_OpenFileEx(&file, path, FS_FILEMODE_R);
    if (ok)
    {
        u32 length = FS_GetLength(&file);
        data = proc_allocEx(length, alignment);
        DC_StoreRange(data, length);
        // weird, the file is not even closed, and why would you
        // check for null after storing the cache (at potentially 0)
        if (!data)
        {
            NOCASH_Printf("Data was null!");
            ok = FALSE;
        }
        else
        {
            if (FS_ReadFile(&file, data, length) < 0)
            {
                NOCASH_Printf("FS_ReadFile failed!\n");
                proc_free(data);
                ok = FALSE;
            }
            FS_CloseFile(&file);
        }
    }
    else
    {
        int resCode = FS_GetResultCode(&file);
        NOCASH_Printf("FS_OpenFile failed!\n");
        NOCASH_Printf("Error code: %d\n", resCode);
        if(resCode == 11)
        {
            NOCASH_Printf("%s does not exist!", path);
        }
    }
    if (!ok)
    {
        NOCASH_Printf("OK WAS FALSE, SETTING DATA TO NULL!\n");
        data = NULL;
    }
    return data;
}

void* loadFileFast(const char* path)
{
    void* result = NULL;
    result = loadFileToHeap(path, 8);
    return result;
}