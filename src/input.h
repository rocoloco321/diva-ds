#pragma once
typedef struct
{
    u16 pressedKeys;
    u16 heldKeys;
    u16 releasedKeys;
}input_t;

typedef struct
{
    int x;
    int y;
} touch_pos_t;


typedef struct 
{
    touch_pos_t pressed;
    touch_pos_t held;
    touch_pos_t released;
} touch_t;


extern input_t gInputData;
extern touch_t gTouchData;

void input_init();
void input_update();

static inline u16 input_getPressedKeys()
{
    return gInputData.pressedKeys;
}

static inline u16 input_getHeldKeys()
{
    return gInputData.heldKeys;
}

static inline u16 input_getReleasedKeys()
{
    return gInputData.releasedKeys;
}

static inline BOOL input_isTouchValid()
{
    return !(gTouchData.pressed.x==-1 && gTouchData.pressed.y==-1);
}

static inline int input_getPressedTpX()
{
    return gTouchData.pressed.x;
}

static inline int input_getPressedTpY()
{
    return gTouchData.pressed.y;
}