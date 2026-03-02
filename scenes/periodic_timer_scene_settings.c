#include "../periodic_timer_app.h"
#include "periodic_timer_scene.h"
#include <stdio.h>

static const char* const vibro_labels[] = {"Off", "Short", "Long"};
static const char* const sound_labels[] = {"Off", "On"};
static const char* const display_labels[] = {"Countdown", "Intervals", "Total Time"};

static void periodic_timer_scene_settings_vibro_changed(VariableItem* item) {
    PeriodicTimerApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.vibro_mode = index;
    variable_item_set_current_value_text(item, vibro_labels[index]);
    periodic_timer_settings_save(&app->settings);
}

static void periodic_timer_scene_settings_sound_changed(VariableItem* item) {
    PeriodicTimerApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.sound_mode = index;
    variable_item_set_current_value_text(item, sound_labels[index]);
    periodic_timer_settings_save(&app->settings);
}

static void periodic_timer_scene_settings_display_changed(VariableItem* item) {
    PeriodicTimerApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    app->settings.display_mode = index;
    variable_item_set_current_value_text(item, display_labels[index]);
    periodic_timer_settings_save(&app->settings);
}

void periodic_timer_scene_settings_on_enter(void* context) {
    PeriodicTimerApp* app = context;
    VariableItem* item;

    variable_item_list_reset(app->variable_item_list);

    // Vibration
    item = variable_item_list_add(
        app->variable_item_list,
        "Vibration",
        PeriodicTimerVibroCount,
        periodic_timer_scene_settings_vibro_changed,
        app);
    variable_item_set_current_value_index(item, app->settings.vibro_mode);
    variable_item_set_current_value_text(item, vibro_labels[app->settings.vibro_mode]);

    // Sound
    item = variable_item_list_add(
        app->variable_item_list,
        "Sound",
        PeriodicTimerSoundCount,
        periodic_timer_scene_settings_sound_changed,
        app);
    variable_item_set_current_value_index(item, app->settings.sound_mode);
    variable_item_set_current_value_text(item, sound_labels[app->settings.sound_mode]);

    // Display Mode
    item = variable_item_list_add(
        app->variable_item_list,
        "Display Mode",
        PeriodicTimerDisplayCount,
        periodic_timer_scene_settings_display_changed,
        app);
    variable_item_set_current_value_index(item, app->settings.display_mode);
    variable_item_set_current_value_text(item, display_labels[app->settings.display_mode]);

    view_dispatcher_switch_to_view(app->view_dispatcher, PeriodicTimerViewSettings);
}

bool periodic_timer_scene_settings_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void periodic_timer_scene_settings_on_exit(void* context) {
    PeriodicTimerApp* app = context;
    variable_item_list_reset(app->variable_item_list);
}
