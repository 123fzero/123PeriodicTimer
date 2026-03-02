#include "periodic_timer_app.h"
#include "scenes/periodic_timer_scene.h"
#include "views/timer_main_view.h"
#include "views/timer_session_view.h"

static bool periodic_timer_custom_event_callback(void* context, uint32_t event) {
    PeriodicTimerApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool periodic_timer_back_event_callback(void* context) {
    PeriodicTimerApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static PeriodicTimerApp* periodic_timer_alloc(void) {
    PeriodicTimerApp* app = malloc(sizeof(PeriodicTimerApp));

    periodic_timer_settings_load(&app->settings);

    app->scene_manager = scene_manager_alloc(&periodic_timer_scene_handlers, app);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, periodic_timer_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, periodic_timer_back_event_callback);

    // Main view (interval picker)
    app->main_view = timer_main_view_alloc();
    view_set_context(app->main_view, app);
    view_dispatcher_add_view(app->view_dispatcher, PeriodicTimerViewMain, app->main_view);

    // Timer session view
    app->timer_view = timer_session_view_alloc();
    view_set_context(app->timer_view, app);
    view_dispatcher_add_view(app->view_dispatcher, PeriodicTimerViewTimer, app->timer_view);

    // Settings (VariableItemList)
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        PeriodicTimerViewSettings,
        variable_item_list_get_view(app->variable_item_list));

    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->timer = NULL;

    return app;
}

static void periodic_timer_free(PeriodicTimerApp* app) {
    if(app->timer) {
        furi_timer_stop(app->timer);
        furi_timer_free(app->timer);
    }

    view_dispatcher_remove_view(app->view_dispatcher, PeriodicTimerViewMain);
    timer_main_view_free(app->main_view);

    view_dispatcher_remove_view(app->view_dispatcher, PeriodicTimerViewTimer);
    timer_session_view_free(app->timer_view);

    view_dispatcher_remove_view(app->view_dispatcher, PeriodicTimerViewSettings);
    variable_item_list_free(app->variable_item_list);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_NOTIFICATION);

    free(app);
}

int32_t periodic_timer_app(void* p) {
    UNUSED(p);
    PeriodicTimerApp* app = periodic_timer_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, PeriodicTimerSceneMain);
    view_dispatcher_run(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    periodic_timer_free(app);
    return 0;
}
