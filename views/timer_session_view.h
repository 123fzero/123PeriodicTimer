#pragma once

#include <gui/view.h>

typedef struct {
    uint32_t seconds_remaining;
    uint32_t interval_count;
    uint32_t total_elapsed;
    uint32_t interval_total;
    uint32_t display_mode;
    bool paused;
} TimerSessionModel;

View* timer_session_view_alloc(void);
void timer_session_view_free(View* view);
