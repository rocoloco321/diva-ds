#include "common.h"
#include "input.h"

input_t gInputData;
touch_t gTouchData;
TPData  raw_point;
TPData  disp_point;
BOOL pressed = false;


void input_init()
{
    gInputData.pressedKeys = 0;
    gInputData.heldKeys = 0;
    gInputData.releasedKeys = 0;
    MI_CpuFill32(&gTouchData, -1, sizeof(touch_t));
}

void input_update()
{
    u16 newKeyData = PAD_Read();
    gInputData.pressedKeys = newKeyData & ~gInputData.heldKeys;
    gInputData.releasedKeys = gInputData.heldKeys & ~newKeyData;
    gInputData.heldKeys = newKeyData;
    if(TP_RequestRawSampling(&raw_point) == 0)
    {
        TP_GetCalibratedPoint(&disp_point, &raw_point);
        if (disp_point.touch && disp_point.validity == TP_VALIDITY_VALID)
        {
            if(!input_isTouchValid())
            {
                gTouchData.pressed.x = disp_point.x;
                gTouchData.pressed.y = disp_point.y;
            }
            else
            {
                gTouchData.held.x = disp_point.x;
                gTouchData.held.y = disp_point.y;
            }
        }
        else
        {
            gTouchData.pressed.x = -1;
            gTouchData.pressed.y = -1;
            gTouchData.released.x = gTouchData.held.x;
            gTouchData.released.y = gTouchData.held.y;
            gTouchData.held.x = -1;
            gTouchData.held.y = -1;
        }
    }
}