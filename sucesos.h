//
// Created by mlste on 9/5/2019.
//

#ifndef SUCESOS_H
#define SUCESOS_H

typedef enum {
    SUC_NUM_0,
    SUC_NUM_1,
    SUC_NUM_2,
    SUC_NUM_3,
    SUC_NUM_4,
    SUC_NUM_5,
    SUC_NUM_6,
    SUC_NUM_7,
    SUC_NUM_8,
    SUC_NUM_9,
    SUC_TAP_DETECTED,
    SUC_MOVE_CURSOR_LEFT,
    SUC_MOVE_CURSOR_RIGHT,
    SUC_INC_DIS_INTENSITY,
    SUC_SCROLL_UP,
    SUC_SCROLL_DOWN,
    SUC_DELAY,
    SUC_SWIPE_INTERRUPTED,
    SUC_SWIPE_END,
    SUC_N} suceso_t;

/*
static const char * suc_debug_codes[] = \
{
    "SUC_NUM_0",
    "SUC_NUM_1",
    "SUC_NUM_2",
    "SUC_NUM_3",
    "SUC_NUM_4",
    "SUC_NUM_5",
    "SUC_NUM_6",
    "SUC_NUM_7",
    "SUC_NUM_8",
    "SUC_NUM_9",
    "SUC_TAP_DETECTED",
    "SUC_MOVE_CURSOR_LEFT",
    "SUC_MOVE_CURSOR_RIGHT",
    "SUC_INC_DIS_INTENSITY",
    "SUC_N"
};
*/


#endif //SUCESOS_H
