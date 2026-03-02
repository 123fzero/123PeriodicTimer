#include "../periodic_timer_app.h"
#include "../views/timer_session_view.h"
#include "periodic_timer_scene.h"

static const NotificationSequence sequence_vibro_short = {
    &message_vibro_on,
    &message_delay_100,
    &message_vibro_off,
    NULL,
};

static const NotificationSequence sequence_vibro_long = {
    &message_vibro_on,
    &message_delay_500,
    &message_vibro_off,
    NULL,
};

static const NotificationSequence sequence_beep = {
    &message_note_c7,
    &message_delay_100,
    &message_sound_off,
    NULL,
};

static void periodic_timer_send_alert(PeriodicTimerApp* app) {
    if(app->settings.vibro_mode == PeriodicTimerVibroShort) {
        notification_message(app->notifications, &sequence_vibro_short);
    } else if(app->settings.vibro_mode == PeriodicTimerVibroLong) {
        notification_message(app->notifications, &sequence_vibro_long);
    }

    if(app->settings.sound_mode == PeriodicTimerSoundOn) {
        notification_message(app->notifications, &sequence_beep);
    }
}

static void periodic_timer_tick_callback(void* context) {
    PeriodicTimerApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, PeriodicTimerCustomEventTimerTick);
}

void periodic_timer_scene_timer_on_enter(void* context) {
    PeriodicTimerApp* app = context;

    uint32_t interval_total = periodic_timer_interval_total(&app->settings);

    with_view_model(
        app->timer_view,
        TimerSessionModel * m,
        {
            m->seconds_remaining = interval_total;
            m->interval_count = 0;
            m->total_elapsed = 0;
            m->interval_total = interval_total;
            m->display_mode = app->settings.display_mode;
            m->paused = false;
        },
        true);

    app->timer = furi_timer_alloc(periodic_timer_tick_callback, FuriTimerTypePeriodic, app);
    furi_timer_start(app->timer, furi_kernel_get_tick_frequency());

    notification_message(app->notifications, &sequence_display_backlight_enforce_on);

    view_dispatcher_switch_to_view(app->view_dispatcher, PeriodicTimerViewTimer);
}

bool periodic_timer_scene_timer_on_event(void* context, SceneManagerEvent event) {
    PeriodicTimerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PeriodicTimerCustomEventTimerTick) {
            bool alert = false;
            with_view_model(
                app->timer_view,
                TimerSessionModel * m,
                {
                    m->total_elapsed++;
                    if(m->seconds_remaining > 0) {
                        m->seconds_remaining--;
                    }
                    if(m->seconds_remaining == 0) {
                        m->interval_count++;
                        m->seconds_remaining = m->interval_total;
                        alert = true;
                    }
                },
                true);

            if(alert) {
                periodic_timer_send_alert(app);
            }
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        if(app->timer) {
            furi_timer_stop(app->timer);
        }
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, PeriodicTimerSceneMain);
        consumed = true;
    }

    return consumed;
}

void periodic_timer_scene_timer_on_exit(void* context) {
    PeriodicTimerApp* app = context;

    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);

    if(app->timer) {
        furi_timer_stop(app->timer);
        furi_timer_free(app->timer);
        app->timer = NULL;
    }
}
