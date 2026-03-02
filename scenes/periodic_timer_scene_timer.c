#include "../periodic_timer_app.h"
#include "periodic_timer_scene.h"

void periodic_timer_scene_timer_on_enter(void* context) {
    UNUSED(context);
}

bool periodic_timer_scene_timer_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void periodic_timer_scene_timer_on_exit(void* context) {
    UNUSED(context);
}
