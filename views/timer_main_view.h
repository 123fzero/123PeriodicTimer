#pragma once

#include <gui/view.h>

typedef enum {
    TimerMainFieldMinutes,
    TimerMainFieldSeconds,
} TimerMainField;

typedef struct {
    uint32_t minutes;
    uint32_t seconds;
    TimerMainField selected_field;
} TimerMainModel;

View* timer_main_view_alloc(void);
void timer_main_view_free(View* view);
