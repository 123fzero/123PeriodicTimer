#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification_messages.h>

#include "periodic_timer_settings.h"

typedef enum {
    PeriodicTimerViewMain,
    PeriodicTimerViewTimer,
    PeriodicTimerViewSettings,
} PeriodicTimerView;

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    View* main_view;
    View* timer_view;
    VariableItemList* variable_item_list;
    NotificationApp* notifications;
    FuriTimer* timer;
    PeriodicTimerSettings settings;
} PeriodicTimerApp;
