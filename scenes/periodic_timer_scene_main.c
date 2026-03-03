#include "../periodic_timer_app.h"
#include "periodic_timer_scene.h"

enum {
    PeriodicTimerMainMenuIndexStartTimer = PeriodicTimerCustomEventOpenConfig,
    PeriodicTimerMainMenuIndexSettings = PeriodicTimerCustomEventOpenSettings,
    PeriodicTimerMainMenuIndexAbout = PeriodicTimerCustomEventOpenAbout,
};

static void periodic_timer_scene_main_menu_callback(void* context, uint32_t index) {
    PeriodicTimerApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void periodic_timer_scene_main_on_enter(void* context) {
    PeriodicTimerApp* app = context;

    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "123PeriodicTimer");
    submenu_add_item(
        app->submenu,
        "Start Timer",
        PeriodicTimerMainMenuIndexStartTimer,
        periodic_timer_scene_main_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Settings",
        PeriodicTimerMainMenuIndexSettings,
        periodic_timer_scene_main_menu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "About",
        PeriodicTimerMainMenuIndexAbout,
        periodic_timer_scene_main_menu_callback,
        app);
    submenu_set_selected_item(
        app->submenu,
        scene_manager_get_scene_state(app->scene_manager, PeriodicTimerSceneMainMenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, PeriodicTimerViewSubmenu);
}

bool periodic_timer_scene_main_on_event(void* context, SceneManagerEvent event) {
    PeriodicTimerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, PeriodicTimerSceneMainMenu, event.event);
        switch(event.event) {
        case PeriodicTimerCustomEventOpenConfig:
            scene_manager_next_scene(app->scene_manager, PeriodicTimerSceneConfig);
            consumed = true;
            break;
        case PeriodicTimerCustomEventOpenSettings:
            scene_manager_next_scene(app->scene_manager, PeriodicTimerSceneSettings);
            consumed = true;
            break;
        case PeriodicTimerCustomEventOpenAbout:
            scene_manager_next_scene(app->scene_manager, PeriodicTimerSceneAbout);
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
    PeriodicTimerApp* app = context;
    submenu_reset(app->submenu);
}
