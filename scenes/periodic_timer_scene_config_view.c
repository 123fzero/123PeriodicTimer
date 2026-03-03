#include "../periodic_timer_app.h"
#include "../views/timer_main_view.h"
#include "periodic_timer_scene.h"

void periodic_timer_scene_config_on_enter(void* context) {
    PeriodicTimerApp* app = context;

    with_view_model(
        app->main_view,
        TimerMainModel * m,
        {
            m->minutes = app->settings.interval_min;
            m->seconds = app->settings.interval_sec;
            m->selected_field = TimerMainFieldSeconds;
        },
        true);

    view_dispatcher_switch_to_view(app->view_dispatcher, PeriodicTimerViewMain);
}

bool periodic_timer_scene_config_on_event(void* context, SceneManagerEvent event) {
    PeriodicTimerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PeriodicTimerCustomEventStartTimer) {
            uint32_t total = periodic_timer_interval_total(&app->settings);
            if(total > 0) {
                scene_manager_next_scene(app->scene_manager, PeriodicTimerSceneTimer);
            }
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, PeriodicTimerSceneMainMenu);
        consumed = true;
    }

    return consumed;
}

void periodic_timer_scene_config_on_exit(void* context) {
    UNUSED(context);
}
