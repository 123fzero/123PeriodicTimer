#include "../periodic_timer_app.h"
#include "periodic_timer_scene.h"

void periodic_timer_scene_about_on_enter(void* context) {
    PeriodicTimerApp* app = context;

    widget_reset(app->widget);
    widget_add_text_scroll_element(
        app->widget,
        0,
        0,
        128,
        64,
        "123PeriodicTimer v1.1.1\n"
        "Flipper Zero repeating\n"
        "interval timer\n\n"
        "Recurring alerts for\n"
        "work, workouts, focus,\n"
        "breathing, and cooking\n\n"
        "Author: 123fzero\n"
        "Year: 2026\n\n"
        "github.com/123fzero\n"
        "/123PeriodicTimer");

    view_dispatcher_switch_to_view(app->view_dispatcher, PeriodicTimerViewWidget);
}

bool periodic_timer_scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void periodic_timer_scene_about_on_exit(void* context) {
    PeriodicTimerApp* app = context;
    widget_reset(app->widget);
}
