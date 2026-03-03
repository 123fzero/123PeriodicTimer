#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>

#include "periodic_timer_settings.h"

typedef enum {
    PeriodicTimerViewSubmenu,
    PeriodicTimerViewMain,
    PeriodicTimerViewSettings,
    PeriodicTimerViewTimer,
    PeriodicTimerViewWidget,
} PeriodicTimerView;

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    View* main_view;
    VariableItemList* variable_item_list;
    View* timer_view;
    Widget* widget;
    NotificationApp* notifications;
    FuriTimer* timer;
    PeriodicTimerSettings settings;
} PeriodicTimerApp;
