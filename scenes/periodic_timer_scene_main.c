#include "../periodic_timer_app.h"
#include "../views/timer_main_view.h"
#include "periodic_timer_scene.h"

void periodic_timer_scene_main_on_enter(void* context) {
    PeriodicTimerApp* app = context;

    // Initialize main view model from settings
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

bool periodic_timer_scene_main_on_event(void* context, SceneManagerEvent event) {
    PeriodicTimerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case PeriodicTimerCustomEventStart: {
            uint32_t total = periodic_timer_interval_total(&app->settings);
            if(total > 0) {
                scene_manager_next_scene(app->scene_manager, PeriodicTimerSceneTimer);
            }
        }
            consumed = true;
            break;
        case PeriodicTimerCustomEventSettings:
            scene_manager_next_scene(app->scene_manager, PeriodicTimerSceneSettings);
            consumed = true;
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        consumed = true;
    }

    return consumed;
}

void periodic_timer_scene_main_on_exit(void* context) {
    UNUSED(context);
}
