#include "timer_session_view.h"
#include "../periodic_timer_app.h"
#include "../periodic_timer_settings.h"
#include <gui/elements.h>
#include <stdio.h>

static void timer_session_view_draw(Canvas* canvas, void* model) {
    TimerSessionModel* m = model;
    canvas_clear(canvas);

    // Header: "Periodic Timer" left, interval right
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 10, "Periodic Timer");

    char interval_buf[16];
    uint32_t iv_min = m->interval_total / 60;
    uint32_t iv_sec = m->interval_total % 60;
    snprintf(
        interval_buf,
        sizeof(interval_buf),
        "%02lu:%02lu",
        (unsigned long)iv_min,
        (unsigned long)iv_sec);
    canvas_draw_str_aligned(canvas, 126, 10, AlignRight, AlignBottom, interval_buf);

    // Center: depends on display mode
    canvas_set_font(canvas, FontBigNumbers);
    char center_buf[32];

    if(m->paused) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignBottom, "PAUSED");
    } else if(m->display_mode == PeriodicTimerDisplayCountdown) {
        uint32_t cd_min = m->seconds_remaining / 60;
        uint32_t cd_sec = m->seconds_remaining % 60;
        if(cd_min > 0) {
            snprintf(
                center_buf,
                sizeof(center_buf),
                "%lu:%02lu",
                (unsigned long)cd_min,
                (unsigned long)cd_sec);
        } else {
            snprintf(center_buf, sizeof(center_buf), "%lu", (unsigned long)cd_sec);
        }
        canvas_draw_str_aligned(canvas, 64, 36, AlignCenter, AlignBottom, center_buf);
    } else if(m->display_mode == PeriodicTimerDisplayIntervals) {
        snprintf(center_buf, sizeof(center_buf), "%lu", (unsigned long)m->interval_count);
        canvas_draw_str_aligned(canvas, 64, 36, AlignCenter, AlignBottom, center_buf);
    } else {
        // Total time
        uint32_t t_min = m->total_elapsed / 60;
        uint32_t t_sec = m->total_elapsed % 60;
        snprintf(
            center_buf,
            sizeof(center_buf),
            "%lu:%02lu",
            (unsigned long)t_min,
            (unsigned long)t_sec);
        canvas_draw_str_aligned(canvas, 64, 36, AlignCenter, AlignBottom, center_buf);
    }

    // Mode label under the number
    canvas_set_font(canvas, FontSecondary);
    const char* mode_label = "";
    if(m->display_mode == PeriodicTimerDisplayCountdown) {
        mode_label = "countdown";
    } else if(m->display_mode == PeriodicTimerDisplayIntervals) {
        mode_label = "intervals";
    } else {
        mode_label = "total time";
    }
    if(!m->paused) {
        canvas_draw_str_aligned(canvas, 64, 44, AlignCenter, AlignBottom, mode_label);
    }

    // Progress bar
    uint8_t bar_x = 14;
    uint8_t bar_y = 47;
    uint8_t bar_w = 100;
    uint8_t bar_h = 6;
    elements_frame(canvas, bar_x, bar_y, bar_w, bar_h);
    if(m->interval_total > 0) {
        uint32_t elapsed_in_interval = m->interval_total - m->seconds_remaining;
        uint8_t fill_w = (uint8_t)(elapsed_in_interval * (bar_w - 2) / m->interval_total);
        if(fill_w > 0) {
            canvas_draw_box(canvas, bar_x + 1, bar_y + 1, fill_w, bar_h - 2);
        }
    }

    // Hints
    canvas_set_font(canvas, FontSecondary);
    if(m->paused) {
        canvas_draw_str(canvas, 2, 62, "[OK]=Resume");
    } else {
        canvas_draw_str(canvas, 2, 62, "[OK]=Pause");
    }
    canvas_draw_str_aligned(canvas, 126, 62, AlignRight, AlignBottom, "[<]=Stop");
}

static bool timer_session_view_input(InputEvent* event, void* context) {
    PeriodicTimerApp* app = context;

    if(event->type != InputTypeShort) return false;

    if(event->key == InputKeyOk) {
        bool is_paused = false;
        with_view_model(
            app->timer_view,
            TimerSessionModel * m,
            {
                m->paused = !m->paused;
                is_paused = m->paused;
            },
            true);
        if(is_paused) {
            if(app->timer) furi_timer_stop(app->timer);
        } else {
            if(app->timer) furi_timer_start(app->timer, furi_kernel_get_tick_frequency());
        }
        return true;
    }

    if(event->key == InputKeyUp || event->key == InputKeyDown) {
        with_view_model(
            app->timer_view,
            TimerSessionModel * m,
            {
                if(event->key == InputKeyUp) {
                    m->display_mode = (m->display_mode + 1) % PeriodicTimerDisplayCount;
                } else {
                    m->display_mode = (m->display_mode + PeriodicTimerDisplayCount - 1) %
                                      PeriodicTimerDisplayCount;
                }
            },
            true);
        with_view_model(
            app->timer_view,
            TimerSessionModel * m,
            { app->settings.display_mode = m->display_mode; },
            false);
        periodic_timer_settings_save(&app->settings);
        return true;
    }

    return false;
}

View* timer_session_view_alloc(void) {
    View* view = view_alloc();
    view_allocate_model(view, ViewModelTypeLocking, sizeof(TimerSessionModel));
    view_set_draw_callback(view, timer_session_view_draw);
    view_set_input_callback(view, timer_session_view_input);
    return view;
}

void timer_session_view_free(View* view) {
    view_free(view);
}
