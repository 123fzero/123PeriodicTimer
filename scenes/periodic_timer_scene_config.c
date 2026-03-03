#include "../periodic_timer_app.h"
#include "periodic_timer_scene.h"

void periodic_timer_scene_main_on_enter(void*);
bool periodic_timer_scene_main_on_event(void*, SceneManagerEvent);
void periodic_timer_scene_main_on_exit(void*);

void periodic_timer_scene_config_on_enter(void*);
bool periodic_timer_scene_config_on_event(void*, SceneManagerEvent);
void periodic_timer_scene_config_on_exit(void*);

void periodic_timer_scene_settings_on_enter(void*);
bool periodic_timer_scene_settings_on_event(void*, SceneManagerEvent);
void periodic_timer_scene_settings_on_exit(void*);

void periodic_timer_scene_timer_on_enter(void*);
bool periodic_timer_scene_timer_on_event(void*, SceneManagerEvent);
void periodic_timer_scene_timer_on_exit(void*);

void periodic_timer_scene_about_on_enter(void*);
bool periodic_timer_scene_about_on_event(void*, SceneManagerEvent);
void periodic_timer_scene_about_on_exit(void*);

void (*const periodic_timer_scene_on_enter_handlers[])(void*) = {
    periodic_timer_scene_main_on_enter,
    periodic_timer_scene_config_on_enter,
    periodic_timer_scene_settings_on_enter,
    periodic_timer_scene_timer_on_enter,
    periodic_timer_scene_about_on_enter,
};

bool (*const periodic_timer_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    periodic_timer_scene_main_on_event,
    periodic_timer_scene_config_on_event,
    periodic_timer_scene_settings_on_event,
    periodic_timer_scene_timer_on_event,
    periodic_timer_scene_about_on_event,
};

void (*const periodic_timer_scene_on_exit_handlers[])(void*) = {
    periodic_timer_scene_main_on_exit,
    periodic_timer_scene_config_on_exit,
    periodic_timer_scene_settings_on_exit,
    periodic_timer_scene_timer_on_exit,
    periodic_timer_scene_about_on_exit,
};

const SceneManagerHandlers periodic_timer_scene_handlers = {
    .on_enter_handlers = periodic_timer_scene_on_enter_handlers,
    .on_event_handlers = periodic_timer_scene_on_event_handlers,
    .on_exit_handlers = periodic_timer_scene_on_exit_handlers,
    .scene_num = PeriodicTimerSceneCount,
};
