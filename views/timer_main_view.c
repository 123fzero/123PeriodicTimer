#include "timer_main_view.h"
#include "../periodic_timer_app.h"
#include "../scenes/periodic_timer_scene.h"
#include <gui/elements.h>
#include <stdio.h>

static void timer_main_view_draw(Canvas* canvas, void* model) {
    TimerMainModel* m = model;
    canvas_clear(canvas);

    // Title
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignBottom, "123PeriodicTimer");

    // Interval display: MM : SS
    char min_buf[4];
    char sec_buf[4];
    snprintf(min_buf, sizeof(min_buf), "%02lu", (unsigned long)m->minutes);
    snprintf(sec_buf, sizeof(sec_buf), "%02lu", (unsigned long)m->seconds);

    canvas_set_font(canvas, FontBigNumbers);

    // Minutes
    uint8_t min_x = 20;
    uint8_t num_y = 38;
    if(m->selected_field == TimerMainFieldMinutes) {
        canvas_draw_rframe(canvas, min_x - 3, num_y - 17, 30, 22, 2);
    }
    canvas_draw_str(canvas, min_x, num_y, min_buf);

    // Colon
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, num_y - 4, AlignCenter, AlignBottom, ":");

    // Seconds
    uint8_t sec_x = 82;
    canvas_set_font(canvas, FontBigNumbers);
    if(m->selected_field == TimerMainFieldSeconds) {
        canvas_draw_rframe(canvas, sec_x - 3, num_y - 17, 30, 22, 2);
    }
    canvas_draw_str(canvas, sec_x, num_y, sec_buf);

    // Labels under numbers
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, min_x + 13, 46, AlignCenter, AlignBottom, "min");
    canvas_draw_str_aligned(canvas, sec_x + 13, 46, AlignCenter, AlignBottom, "sec");

    // Bottom hints
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 62, "[<] Back");
    canvas_draw_str_aligned(canvas, 126, 62, AlignRight, AlignBottom, "[OK] Start");
}

static bool timer_main_view_input(InputEvent* event, void* context) {
    PeriodicTimerApp* app = context;

    if(event->type != InputTypeShort && event->type != InputTypeRepeat) {
        return false;
    }

    if(event->key == InputKeyOk) {
        // Save current interval to settings before starting
        with_view_model(
            app->main_view,
            TimerMainModel * m,
            {
                app->settings.interval_min = m->minutes;
                app->settings.interval_sec = m->seconds;
            },
            false);
        periodic_timer_settings_save(&app->settings);
        view_dispatcher_send_custom_event(app->view_dispatcher, PeriodicTimerCustomEventStartTimer);
        return true;
    }

    if(event->key == InputKeyLeft || event->key == InputKeyRight) {
        with_view_model(
            app->main_view,
            TimerMainModel * m,
            {
                if(event->key == InputKeyLeft) {
                    m->selected_field = TimerMainFieldMinutes;
                } else {
                    m->selected_field = TimerMainFieldSeconds;
                }
            },
            true);
        return true;
    }

    if(event->key == InputKeyUp || event->key == InputKeyDown) {
        with_view_model(
            app->main_view,
            TimerMainModel * m,
            {
                if(m->selected_field == TimerMainFieldMinutes) {
                    if(event->key == InputKeyUp) {
                        m->minutes = (m->minutes + 1) % 60;
                    } else {
                        m->minutes = (m->minutes + 59) % 60;
                    }
                } else {
                    if(event->key == InputKeyUp) {
                        m->seconds = (m->seconds + 1) % 60;
                    } else {
                        m->seconds = (m->seconds + 59) % 60;
                    }
                }
            },
            true);
        return true;
    }

    return false;
}

View* timer_main_view_alloc(void) {
    View* view = view_alloc();
    view_allocate_model(view, ViewModelTypeLocking, sizeof(TimerMainModel));
    view_set_draw_callback(view, timer_main_view_draw);
    view_set_input_callback(view, timer_main_view_input);
    return view;
}

void timer_main_view_free(View* view) {
    view_free(view);
}
