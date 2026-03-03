#pragma once

#include <gui/scene_manager.h>

typedef enum {
    PeriodicTimerSceneMainMenu,
    PeriodicTimerSceneConfig,
    PeriodicTimerSceneSettings,
    PeriodicTimerSceneTimer,
    PeriodicTimerSceneAbout,
    PeriodicTimerSceneCount,
} PeriodicTimerScene;

typedef enum {
    PeriodicTimerCustomEventOpenConfig,
    PeriodicTimerCustomEventOpenSettings,
    PeriodicTimerCustomEventOpenAbout,
    PeriodicTimerCustomEventStartTimer,
    PeriodicTimerCustomEventTimerTick,
} PeriodicTimerCustomEvent;

extern const SceneManagerHandlers periodic_timer_scene_handlers;
