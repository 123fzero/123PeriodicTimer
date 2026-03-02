#pragma once

#include <gui/scene_manager.h>

typedef enum {
    PeriodicTimerSceneMain,
    PeriodicTimerSceneTimer,
    PeriodicTimerSceneSettings,
    PeriodicTimerSceneCount,
} PeriodicTimerScene;

typedef enum {
    PeriodicTimerCustomEventStart,
    PeriodicTimerCustomEventSettings,
    PeriodicTimerCustomEventTimerTick,
} PeriodicTimerCustomEvent;

extern const SceneManagerHandlers periodic_timer_scene_handlers;
