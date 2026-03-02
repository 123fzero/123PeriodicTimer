# PeriodicTimer Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Build a Flipper Zero periodic timer app inspired by iOS "Periodic Timer" — set an interval, it repeats with beep/vibration alerts forever until stopped.

**Architecture:** ViewDispatcher + SceneManager with 3 scenes (Main, Timer, Settings). Two custom views (main interval picker, timer session). VariableItemList for settings. FuriTimer periodic 1s. Settings persisted via FlipperFormat.

**Tech Stack:** C, Flipper Zero SDK (ufbt), FuriTimer, FlipperFormat, Canvas API

---

### Task 1: Project scaffold — application.fam, app header, entry point

**Files:**
- Create: `application.fam`
- Create: `periodic_timer_app.h`
- Create: `periodic_timer.c`

**Step 1: Create application.fam**

```python
App(
    appid="periodic_timer",
    name="PeriodicTimer",
    apptype=FlipperAppType.EXTERNAL,
    entry_point="periodic_timer_app",
    stack_size=2 * 1024,
    fap_version=(1, 0),
    fap_icon="icon.png",
    fap_category="Tools",
    fap_description="Repeating interval timer with vibration and sound alerts",
    fap_author="@colinfrl",
    requires=[
        "gui",
        "notification",
        "storage",
    ],
)
```

**Step 2: Create periodic_timer_app.h**

```c
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
```

**Step 3: Create periodic_timer.c (entry point with alloc/free)**

```c
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
    view_dispatcher_add_view(
        app->view_dispatcher, PeriodicTimerViewMain, app->main_view);

    // Timer session view
    app->timer_view = timer_session_view_alloc();
    view_set_context(app->timer_view, app);
    view_dispatcher_add_view(
        app->view_dispatcher, PeriodicTimerViewTimer, app->timer_view);

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
```

**Step 4: Commit**

```bash
git add application.fam periodic_timer_app.h periodic_timer.c
git commit -m "feat: add project scaffold with app manifest, header, and entry point"
```

---

### Task 2: Settings — persistence and configuration

**Files:**
- Create: `periodic_timer_settings.h`
- Create: `periodic_timer_settings.c`

**Step 1: Create periodic_timer_settings.h**

```c
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define INTERVAL_MIN_MIN     0
#define INTERVAL_MIN_MAX     59
#define INTERVAL_MIN_DEFAULT 0

#define INTERVAL_SEC_MIN     0
#define INTERVAL_SEC_MAX     59
#define INTERVAL_SEC_DEFAULT 30

#define SETTINGS_FILE_PATH    APP_DATA_PATH("settings.conf")
#define SETTINGS_FILE_TYPE    "PeriodicTimer Settings"
#define SETTINGS_FILE_VERSION 1

typedef enum {
    PeriodicTimerVibroOff,
    PeriodicTimerVibroShort,
    PeriodicTimerVibroLong,
    PeriodicTimerVibroCount,
} PeriodicTimerVibroMode;

typedef enum {
    PeriodicTimerSoundOff,
    PeriodicTimerSoundOn,
    PeriodicTimerSoundCount,
} PeriodicTimerSoundMode;

typedef enum {
    PeriodicTimerDisplayCountdown,
    PeriodicTimerDisplayIntervals,
    PeriodicTimerDisplayTotalTime,
    PeriodicTimerDisplayCount,
} PeriodicTimerDisplayMode;

typedef struct {
    uint32_t interval_min;
    uint32_t interval_sec;
    uint32_t vibro_mode;
    uint32_t sound_mode;
    uint32_t display_mode;
} PeriodicTimerSettings;

bool periodic_timer_settings_load(PeriodicTimerSettings* settings);
bool periodic_timer_settings_save(const PeriodicTimerSettings* settings);

// Helper: total interval in seconds (minimum 1)
static inline uint32_t periodic_timer_interval_total(const PeriodicTimerSettings* s) {
    uint32_t total = s->interval_min * 60 + s->interval_sec;
    return total > 0 ? total : 1;
}
```

**Step 2: Create periodic_timer_settings.c**

```c
#include "periodic_timer_settings.h"
#include <furi.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

bool periodic_timer_settings_save(const PeriodicTimerSettings* settings) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    bool success = false;

    do {
        if(!flipper_format_file_open_always(ff, SETTINGS_FILE_PATH)) break;
        if(!flipper_format_write_header_cstr(ff, SETTINGS_FILE_TYPE, SETTINGS_FILE_VERSION)) break;
        if(!flipper_format_write_uint32(ff, "IntervalMin", &settings->interval_min, 1)) break;
        if(!flipper_format_write_uint32(ff, "IntervalSec", &settings->interval_sec, 1)) break;
        if(!flipper_format_write_uint32(ff, "VibroMode", &settings->vibro_mode, 1)) break;
        if(!flipper_format_write_uint32(ff, "SoundMode", &settings->sound_mode, 1)) break;
        if(!flipper_format_write_uint32(ff, "DisplayMode", &settings->display_mode, 1)) break;
        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}

bool periodic_timer_settings_load(PeriodicTimerSettings* settings) {
    settings->interval_min = INTERVAL_MIN_DEFAULT;
    settings->interval_sec = INTERVAL_SEC_DEFAULT;
    settings->vibro_mode = PeriodicTimerVibroShort;
    settings->sound_mode = PeriodicTimerSoundOn;
    settings->display_mode = PeriodicTimerDisplayCountdown;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    bool success = false;

    do {
        if(!flipper_format_file_open_existing(ff, SETTINGS_FILE_PATH)) break;

        FuriString* filetype = furi_string_alloc();
        uint32_t version = 0;
        if(!flipper_format_read_header(ff, filetype, &version)) {
            furi_string_free(filetype);
            break;
        }
        if(furi_string_cmp_str(filetype, SETTINGS_FILE_TYPE) != 0 ||
           version != SETTINGS_FILE_VERSION) {
            furi_string_free(filetype);
            break;
        }
        furi_string_free(filetype);

        flipper_format_read_uint32(ff, "IntervalMin", &settings->interval_min, 1);
        flipper_format_read_uint32(ff, "IntervalSec", &settings->interval_sec, 1);
        flipper_format_read_uint32(ff, "VibroMode", &settings->vibro_mode, 1);
        flipper_format_read_uint32(ff, "SoundMode", &settings->sound_mode, 1);
        flipper_format_read_uint32(ff, "DisplayMode", &settings->display_mode, 1);

        if(settings->interval_min > INTERVAL_MIN_MAX) {
            settings->interval_min = INTERVAL_MIN_DEFAULT;
        }
        if(settings->interval_sec > INTERVAL_SEC_MAX) {
            settings->interval_sec = INTERVAL_SEC_DEFAULT;
        }
        if(settings->vibro_mode >= PeriodicTimerVibroCount) {
            settings->vibro_mode = PeriodicTimerVibroShort;
        }
        if(settings->sound_mode >= PeriodicTimerSoundCount) {
            settings->sound_mode = PeriodicTimerSoundOn;
        }
        if(settings->display_mode >= PeriodicTimerDisplayCount) {
            settings->display_mode = PeriodicTimerDisplayCountdown;
        }

        success = true;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}
```

**Step 3: Commit**

```bash
git add periodic_timer_settings.h periodic_timer_settings.c
git commit -m "feat: add settings persistence with FlipperFormat"
```

---

### Task 3: Scene infrastructure — scene header, config, and stubs

**Files:**
- Create: `scenes/periodic_timer_scene.h`
- Create: `scenes/periodic_timer_scene_config.c`
- Create: `scenes/periodic_timer_scene_main.c` (stub)
- Create: `scenes/periodic_timer_scene_timer.c` (stub)
- Create: `scenes/periodic_timer_scene_settings.c` (stub)

**Step 1: Create scenes/periodic_timer_scene.h**

```c
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
```

**Step 2: Create scenes/periodic_timer_scene_config.c**

```c
#include "../periodic_timer_app.h"
#include "periodic_timer_scene.h"

void periodic_timer_scene_main_on_enter(void*);
bool periodic_timer_scene_main_on_event(void*, SceneManagerEvent);
void periodic_timer_scene_main_on_exit(void*);

void periodic_timer_scene_timer_on_enter(void*);
bool periodic_timer_scene_timer_on_event(void*, SceneManagerEvent);
void periodic_timer_scene_timer_on_exit(void*);

void periodic_timer_scene_settings_on_enter(void*);
bool periodic_timer_scene_settings_on_event(void*, SceneManagerEvent);
void periodic_timer_scene_settings_on_exit(void*);

void (*const periodic_timer_scene_on_enter_handlers[])(void*) = {
    periodic_timer_scene_main_on_enter,
    periodic_timer_scene_timer_on_enter,
    periodic_timer_scene_settings_on_enter,
};

bool (*const periodic_timer_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    periodic_timer_scene_main_on_event,
    periodic_timer_scene_timer_on_event,
    periodic_timer_scene_settings_on_event,
};

void (*const periodic_timer_scene_on_exit_handlers[])(void*) = {
    periodic_timer_scene_main_on_exit,
    periodic_timer_scene_timer_on_exit,
    periodic_timer_scene_settings_on_exit,
};

const SceneManagerHandlers periodic_timer_scene_handlers = {
    .on_enter_handlers = periodic_timer_scene_on_enter_handlers,
    .on_event_handlers = periodic_timer_scene_on_event_handlers,
    .on_exit_handlers = periodic_timer_scene_on_exit_handlers,
    .scene_num = PeriodicTimerSceneCount,
};
```

**Step 3: Create scene stubs (all 3)**

`scenes/periodic_timer_scene_main.c`:
```c
#include "../periodic_timer_app.h"
#include "periodic_timer_scene.h"

void periodic_timer_scene_main_on_enter(void* context) {
    UNUSED(context);
}

bool periodic_timer_scene_main_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void periodic_timer_scene_main_on_exit(void* context) {
    UNUSED(context);
}
```

`scenes/periodic_timer_scene_timer.c`:
```c
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
```

`scenes/periodic_timer_scene_settings.c`:
```c
#include "../periodic_timer_app.h"
#include "periodic_timer_scene.h"

void periodic_timer_scene_settings_on_enter(void* context) {
    UNUSED(context);
}

bool periodic_timer_scene_settings_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void periodic_timer_scene_settings_on_exit(void* context) {
    UNUSED(context);
}
```

**Step 4: Commit**

```bash
git add scenes/
git commit -m "feat: add scene infrastructure with config and stubs"
```

---

### Task 4: Main view — interval picker with custom draw/input

**Files:**
- Create: `views/timer_main_view.h`
- Create: `views/timer_main_view.c`

**Step 1: Create views/timer_main_view.h**

```c
#pragma once

#include <gui/view.h>

typedef enum {
    TimerMainFieldMinutes,
    TimerMainFieldSeconds,
} TimerMainField;

typedef struct {
    uint32_t minutes;
    uint32_t seconds;
    TimerMainField selected_field;
} TimerMainModel;

View* timer_main_view_alloc(void);
void timer_main_view_free(View* view);
```

**Step 2: Create views/timer_main_view.c**

```c
#include "timer_main_view.h"
#include "../periodic_timer_app.h"
#include "../scenes/periodic_timer_scene.h"
#include <gui/elements.h>
#include <stdio.h>

static void timer_main_view_draw(Canvas* canvas, void* model) {
    TimerMainModel* m = model;
    canvas_clear(canvas);

    // Title
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignBottom, "Periodic Timer");

    // Interval display: MM : SS
    char min_buf[4];
    char sec_buf[4];
    snprintf(min_buf, sizeof(min_buf), "%02lu", (unsigned long)m->minutes);
    snprintf(sec_buf, sizeof(sec_buf), "%02lu", (unsigned long)m->seconds);

    canvas_set_font(canvas, FontBigNumbers);

    // Minutes
    uint8_t min_x = 20;
    uint8_t num_y = 38;
    if(m->selected_field == TimerMainFieldMinutes) {
        // Draw selection box around minutes
        canvas_draw_rframe(canvas, min_x - 3, num_y - 17, 30, 22, 2);
    }
    canvas_draw_str(canvas, min_x, num_y, min_buf);

    // Colon
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, num_y - 4, AlignCenter, AlignBottom, ":");

    // Seconds
    uint8_t sec_x = 82;
    canvas_set_font(canvas, FontBigNumbers);
    if(m->selected_field == TimerMainFieldSeconds) {
        canvas_draw_rframe(canvas, sec_x - 3, num_y - 17, 30, 22, 2);
    }
    canvas_draw_str(canvas, sec_x, num_y, sec_buf);

    // Labels under numbers
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, min_x + 13, 46, AlignCenter, AlignBottom, "min");
    canvas_draw_str_aligned(canvas, sec_x + 13, 46, AlignCenter, AlignBottom, "sec");

    // Bottom hints
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 62, "[Settings]");
    canvas_draw_str_aligned(canvas, 126, 62, AlignRight, AlignBottom, "[Start >]");
}

static bool timer_main_view_input(InputEvent* event, void* context) {
    PeriodicTimerApp* app = context;

    if(event->type != InputTypeShort && event->type != InputTypeRepeat) {
        return false;
    }

    if(event->key == InputKeyOk) {
        // Save current interval to settings before starting
        with_view_model(
            app->main_view,
            TimerMainModel * m,
            {
                app->settings.interval_min = m->minutes;
                app->settings.interval_sec = m->seconds;
            },
            false);
        periodic_timer_settings_save(&app->settings);
        view_dispatcher_send_custom_event(
            app->view_dispatcher, PeriodicTimerCustomEventStart);
        return true;
    }

    if(event->key == InputKeyLeft || event->key == InputKeyRight) {
        with_view_model(
            app->main_view,
            TimerMainModel * m,
            {
                if(event->key == InputKeyLeft) {
                    m->selected_field = TimerMainFieldMinutes;
                } else {
                    m->selected_field = TimerMainFieldSeconds;
                }
            },
            true);
        return true;
    }

    if(event->key == InputKeyUp || event->key == InputKeyDown) {
        with_view_model(
            app->main_view,
            TimerMainModel * m,
            {
                if(m->selected_field == TimerMainFieldMinutes) {
                    if(event->key == InputKeyUp) {
                        m->minutes = (m->minutes + 1) % 60;
                    } else {
                        m->minutes = (m->minutes + 59) % 60;
                    }
                } else {
                    if(event->key == InputKeyUp) {
                        m->seconds = (m->seconds + 1) % 60;
                    } else {
                        m->seconds = (m->seconds + 59) % 60;
                    }
                }
            },
            true);
        return true;
    }

    return false;
}

View* timer_main_view_alloc(void) {
    View* view = view_alloc();
    view_allocate_model(view, ViewModelTypeLocking, sizeof(TimerMainModel));
    view_set_draw_callback(view, timer_main_view_draw);
    view_set_input_callback(view, timer_main_view_input);
    return view;
}

void timer_main_view_free(View* view) {
    view_free(view);
}
```

**Step 3: Commit**

```bash
git add views/timer_main_view.h views/timer_main_view.c
git commit -m "feat: add main view with interval picker (MM:SS, Up/Down/Left/Right)"
```

---

### Task 5: Timer session view — 3 display modes, progress bar, pause

**Files:**
- Create: `views/timer_session_view.h`
- Create: `views/timer_session_view.c`

**Step 1: Create views/timer_session_view.h**

```c
#pragma once

#include <gui/view.h>

typedef struct {
    uint32_t seconds_remaining;
    uint32_t interval_count;
    uint32_t total_elapsed;
    uint32_t interval_total;
    uint32_t display_mode;
    bool paused;
} TimerSessionModel;

View* timer_session_view_alloc(void);
void timer_session_view_free(View* view);
```

**Step 2: Create views/timer_session_view.c**

```c
#include "timer_session_view.h"
#include "../periodic_timer_app.h"
#include "../periodic_timer_settings.h"
#include <gui/elements.h>
#include <stdio.h>

static void timer_session_view_draw(Canvas* canvas, void* model) {
    TimerSessionModel* m = model;
    canvas_clear(canvas);

    // Header: "Periodic Timer" left, interval right
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 10, "Periodic Timer");

    char interval_buf[16];
    uint32_t iv_min = m->interval_total / 60;
    uint32_t iv_sec = m->interval_total % 60;
    snprintf(
        interval_buf,
        sizeof(interval_buf),
        "%02lu:%02lu",
        (unsigned long)iv_min,
        (unsigned long)iv_sec);
    canvas_draw_str_aligned(canvas, 126, 10, AlignRight, AlignBottom, interval_buf);

    // Center: depends on display mode
    canvas_set_font(canvas, FontBigNumbers);
    char center_buf[32];

    if(m->paused) {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignBottom, "PAUSED");
    } else if(m->display_mode == PeriodicTimerDisplayCountdown) {
        uint32_t cd_min = m->seconds_remaining / 60;
        uint32_t cd_sec = m->seconds_remaining % 60;
        if(cd_min > 0) {
            snprintf(
                center_buf,
                sizeof(center_buf),
                "%lu:%02lu",
                (unsigned long)cd_min,
                (unsigned long)cd_sec);
        } else {
            snprintf(center_buf, sizeof(center_buf), "%lu", (unsigned long)cd_sec);
        }
        canvas_draw_str_aligned(canvas, 64, 36, AlignCenter, AlignBottom, center_buf);
    } else if(m->display_mode == PeriodicTimerDisplayIntervals) {
        snprintf(center_buf, sizeof(center_buf), "%lu", (unsigned long)m->interval_count);
        canvas_draw_str_aligned(canvas, 64, 36, AlignCenter, AlignBottom, center_buf);
    } else {
        // Total time
        uint32_t t_min = m->total_elapsed / 60;
        uint32_t t_sec = m->total_elapsed % 60;
        snprintf(
            center_buf,
            sizeof(center_buf),
            "%lu:%02lu",
            (unsigned long)t_min,
            (unsigned long)t_sec);
        canvas_draw_str_aligned(canvas, 64, 36, AlignCenter, AlignBottom, center_buf);
    }

    // Mode label under the number
    canvas_set_font(canvas, FontSecondary);
    const char* mode_label = "";
    if(m->display_mode == PeriodicTimerDisplayCountdown) {
        mode_label = "countdown";
    } else if(m->display_mode == PeriodicTimerDisplayIntervals) {
        mode_label = "intervals";
    } else {
        mode_label = "total time";
    }
    if(!m->paused) {
        canvas_draw_str_aligned(canvas, 64, 44, AlignCenter, AlignBottom, mode_label);
    }

    // Progress bar
    uint8_t bar_x = 14;
    uint8_t bar_y = 47;
    uint8_t bar_w = 100;
    uint8_t bar_h = 6;
    elements_frame(canvas, bar_x, bar_y, bar_w, bar_h);
    if(m->interval_total > 0) {
        uint32_t elapsed_in_interval = m->interval_total - m->seconds_remaining;
        uint8_t fill_w =
            (uint8_t)(elapsed_in_interval * (bar_w - 2) / m->interval_total);
        if(fill_w > 0) {
            canvas_draw_box(canvas, bar_x + 1, bar_y + 1, fill_w, bar_h - 2);
        }
    }

    // Hints
    canvas_set_font(canvas, FontSecondary);
    if(m->paused) {
        canvas_draw_str(canvas, 2, 62, "[OK]=Resume");
    } else {
        canvas_draw_str(canvas, 2, 62, "[OK]=Pause");
    }
    canvas_draw_str_aligned(canvas, 126, 62, AlignRight, AlignBottom, "[<]=Stop");
}

static bool timer_session_view_input(InputEvent* event, void* context) {
    PeriodicTimerApp* app = context;

    if(event->type != InputTypeShort) return false;

    if(event->key == InputKeyOk) {
        bool is_paused = false;
        with_view_model(
            app->timer_view,
            TimerSessionModel * m,
            {
                m->paused = !m->paused;
                is_paused = m->paused;
            },
            true);
        if(is_paused) {
            if(app->timer) furi_timer_stop(app->timer);
        } else {
            if(app->timer) furi_timer_start(app->timer, furi_kernel_get_tick_frequency());
        }
        return true;
    }

    if(event->key == InputKeyUp || event->key == InputKeyDown) {
        with_view_model(
            app->timer_view,
            TimerSessionModel * m,
            {
                if(event->key == InputKeyUp) {
                    m->display_mode = (m->display_mode + 1) % PeriodicTimerDisplayCount;
                } else {
                    m->display_mode =
                        (m->display_mode + PeriodicTimerDisplayCount - 1) %
                        PeriodicTimerDisplayCount;
                }
            },
            true);
        // Save display mode preference
        with_view_model(
            app->timer_view,
            TimerSessionModel * m,
            {
                app->settings.display_mode = m->display_mode;
            },
            false);
        periodic_timer_settings_save(&app->settings);
        return true;
    }

    return false;
}

View* timer_session_view_alloc(void) {
    View* view = view_alloc();
    view_allocate_model(view, ViewModelTypeLocking, sizeof(TimerSessionModel));
    view_set_draw_callback(view, timer_session_view_draw);
    view_set_input_callback(view, timer_session_view_input);
    return view;
}

void timer_session_view_free(View* view) {
    view_free(view);
}
```

**Step 3: Commit**

```bash
git add views/timer_session_view.h views/timer_session_view.c
git commit -m "feat: add timer session view with 3 display modes and progress bar"
```

---

### Task 6: Main scene — wire up interval picker, start, settings navigation

**Files:**
- Modify: `scenes/periodic_timer_scene_main.c`

**Step 1: Implement the main scene**

Replace `scenes/periodic_timer_scene_main.c` entirely:

```c
#include "../periodic_timer_app.h"
#include "../views/timer_main_view.h"
#include "periodic_timer_scene.h"

void periodic_timer_scene_main_on_enter(void* context) {
    PeriodicTimerApp* app = context;

    // Initialize main view model from settings
    with_view_model(
        app->main_view,
        TimerMainModel * m,
        {
            m->minutes = app->settings.interval_min;
            m->seconds = app->settings.interval_sec;
            m->selected_field = TimerMainFieldSeconds;
        },
        true);

    view_dispatcher_switch_to_view(app->view_dispatcher, PeriodicTimerViewMain);
}

bool periodic_timer_scene_main_on_event(void* context, SceneManagerEvent event) {
    PeriodicTimerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case PeriodicTimerCustomEventStart:
            // Validate interval is not 0:00
            {
                uint32_t total = periodic_timer_interval_total(&app->settings);
                if(total > 0) {
                    scene_manager_next_scene(app->scene_manager, PeriodicTimerSceneTimer);
                }
            }
            consumed = true;
            break;
        case PeriodicTimerCustomEventSettings:
            scene_manager_next_scene(app->scene_manager, PeriodicTimerSceneSettings);
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
    UNUSED(context);
}
```

**Step 2: Commit**

```bash
git add scenes/periodic_timer_scene_main.c
git commit -m "feat: implement main scene with interval picker and navigation"
```

---

### Task 7: Timer scene — FuriTimer, alerts, infinite repeat

**Files:**
- Modify: `scenes/periodic_timer_scene_timer.c`

**Step 1: Implement the timer scene**

Replace `scenes/periodic_timer_scene_timer.c` entirely:

```c
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
```

**Step 2: Commit**

```bash
git add scenes/periodic_timer_scene_timer.c
git commit -m "feat: implement timer scene with infinite repeat, alerts, and backlight"
```

---

### Task 8: Settings scene — vibration, sound, display mode

**Files:**
- Modify: `scenes/periodic_timer_scene_settings.c`

**Step 1: Implement the settings scene**

Replace `scenes/periodic_timer_scene_settings.c` entirely:

```c
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
```

**Step 2: Commit**

```bash
git add scenes/periodic_timer_scene_settings.c
git commit -m "feat: implement settings scene with vibration, sound, and display mode"
```

---

### Task 9: Icon and navigation fix — Back from main opens settings

**Files:**
- Create: `icon.png` (10x10 timer icon)
- Modify: `views/timer_main_view.c` — add Back key handling for settings navigation

**Step 1: Create icon.png**

Create a 10x10 pixel 1-bit PNG with a simple timer/clock icon. Use ImageMagick or copy from PuffPacer and modify.

```bash
# Option: copy PuffPacer icon as placeholder
cp /Users/colinfrl/work/123fzero/123PuffPacer/icon.png icon.png
```

**Step 2: Add Back key handling in main view for settings**

In `views/timer_main_view.c`, in the `timer_main_view_input` function, add handling for the Back key to send a settings event. Add this before the final `return false`:

```c
    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        // Don't consume - let SceneManager handle it for app exit
        // Settings accessible via long press
        return false;
    }
```

Actually, since we want Back on the main screen to exit the app (handled by SceneManager), and Settings to be accessible, let's add a long-press Back for settings. Update the input function to handle `InputTypeLong` on Back:

In `timer_main_view_input`, before the existing `InputTypeShort`/`InputTypeRepeat` guard, add:

```c
    if(event->key == InputKeyBack && event->type == InputTypeLong) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, PeriodicTimerCustomEventSettings);
        return true;
    }
```

**Step 3: Commit**

```bash
git add icon.png views/timer_main_view.c
git commit -m "feat: add app icon and settings navigation via long-press Back"
```

---

### Task 10: README with ASCII UI screenshots

**Files:**
- Create: `README.md`

**Step 1: Create README.md**

```markdown
# PeriodicTimer — Repeating Interval Timer for Flipper Zero

Set any interval from 1 second to 59:59 — the timer repeats at your chosen interval until you stop it, alerting you with a beep and vibration each time.

## Why?

Sometimes you need a simple repeating timer: time-boxing tasks, pacing activities, cooking intervals, meditation, or anything that needs a periodic reminder. PeriodicTimer does one thing well — beeps at your interval, forever, until you stop it.

## Features

- **Flexible interval** — from 00:01 to 59:59 (minutes:seconds)
- **Infinite repeat** — runs until you stop it
- **3 display modes** — Countdown, Intervals count, Total time
- **Vibration control** — Off / Short / Long
- **Sound control** — Off / On
- **Pause/resume** with OK button
- **Progress bar** — visual indicator of current interval
- **Persistent settings** — saved to SD card, remembered between launches
- **Backlight stays on** during active timer

## Screenshots

```
┌────────────────────────────┐   ┌────────────────────────────┐
│      Periodic Timer        │   │ Vibration         < Short >│
│                            │   │ Sound                < On >│
│       [00] : [30]          │   │ Display Mode  < Countdown >│
│        min   sec           │   │                            │
│                            │   │                            │
│ [Settings]       [Start >] │   │                            │
│                            │   │                            │
└────────────────────────────┘   └────────────────────────────┘
         Main Screen                       Settings

┌────────────────────────────┐   ┌────────────────────────────┐
│ Periodic Timer     00:30   │   │ Periodic Timer     00:30   │
│                            │   │                            │
│           18               │   │           5                │
│                            │   │                            │
│       countdown            │   │        intervals           │
│    [███████░░░░░░░]        │   │    [███████░░░░░░░]        │
│ [OK]=Pause    [<]=Stop     │   │ [OK]=Pause    [<]=Stop     │
└────────────────────────────┘   └────────────────────────────┘
     Timer: Countdown Mode           Timer: Intervals Mode

┌────────────────────────────┐   ┌────────────────────────────┐
│ Periodic Timer     00:30   │   │ Periodic Timer     00:30   │
│                            │   │                            │
│         2:30               │   │         PAUSED             │
│                            │   │                            │
│       total time           │   │                            │
│    [███████░░░░░░░]        │   │    [███████░░░░░░░]        │
│ [OK]=Pause    [<]=Stop     │   │ [OK]=Resume   [<]=Stop     │
└────────────────────────────┘   └────────────────────────────┘
    Timer: Total Time Mode            Timer: Paused
```

## Install

### From .fap file

1. Download `periodic_timer.fap` from [Releases](../../releases)
2. Copy to your Flipper Zero SD card: `SD Card/apps/Tools/`
3. Open on Flipper: `Applications → Tools → PeriodicTimer`

### Build from source

```bash
# Install ufbt if you haven't
pip install ufbt

# Clone and build
git clone https://github.com/123fzero/123PeriodicTimer.git
cd 123PeriodicTimer
ufbt

# Build and launch on connected Flipper
ufbt launch
```

## Usage

1. Open PeriodicTimer from Applications → Tools
2. Set the interval using **Up/Down** (change value) and **Left/Right** (switch min/sec)
3. Press **OK** to start the timer
4. Flipper alerts you on each interval with beep and vibration
5. Press **OK** to pause/resume, **Back** to stop
6. Long-press **Back** on main screen for **Settings** (vibration, sound, display mode)
7. Use **Up/Down** during timer to switch display mode

## Controls

### Main Screen
| Button | Action |
|--------|--------|
| Up/Down | Change minutes or seconds value |
| Left/Right | Switch between minutes and seconds |
| OK | Start timer |
| Back (long) | Open settings |
| Back (short) | Exit app |

### Timer Screen
| Button | Action |
|--------|--------|
| OK | Pause / Resume |
| Up/Down | Switch display mode |
| Back | Stop timer, return to main |

## Tested On

- **Firmware:** Momentum (mntm-012)
- **API:** 87.1
- **Hardware:** Flipper Zero (f7)

Should also work on official firmware and other custom firmwares (Unleashed, RogueMaster, Xtreme) with compatible API versions.

## License

MIT
```

**Step 2: Commit**

```bash
git add README.md
git commit -m "docs: add README with feature list, ASCII screenshots, and usage guide"
```

---

### Task 11: CLAUDE.md — technical context for future development

**Files:**
- Create: `CLAUDE.md`

**Step 1: Create CLAUDE.md**

```markdown
# PeriodicTimer — Flipper Zero App

Repeating interval timer inspired by the iOS app "Periodic Timer".

## What It Does
- Set interval from 00:01 to 59:59 (minutes:seconds)
- Timer repeats infinitely with beep/vibration alerts
- 3 display modes: Countdown, Intervals count, Total Time
- Vibration control: Off / Short / Long
- Sound control: Off / On
- Pause/resume with OK button
- Settings persisted to SD card

## Technical Details
- Flipper Zero, SDK (ufbt), API 87.1
- Firmware tested: Momentum mntm-012
- Language: C
- Architecture: ViewDispatcher + SceneManager
- 3 scenes: Main, Timer, Settings
- Widgets: 2 custom Views (main picker, timer session), VariableItemList (settings)
- Timer: FuriTimer periodic 1s
- Notifications: separate vibro and sound sequences, configurable
- Settings: FlipperFormat v1 at /ext/apps_data/periodic_timer/settings.conf
- Category: Tools
- appid: periodic_timer
- Icon: 10x10 timer

## Build
```bash
ufbt          # build
ufbt launch   # build + deploy + run
ufbt format   # format code
ufbt lint     # lint code
```

## Communication
- Speak Russian with the developer
- Write code and documentation in English
```

**Step 2: Commit**

```bash
git add CLAUDE.md
git commit -m "docs: add CLAUDE.md with technical context"
```

---

### Task 12: Final review — build test and code formatting

**Step 1: Run ufbt lint**

```bash
ufbt lint
```

Fix any issues found.

**Step 2: Run ufbt format**

```bash
ufbt format
```

**Step 3: Run ufbt build**

```bash
ufbt
```

Fix any compilation errors.

**Step 4: Commit any fixes**

```bash
git add -A
git commit -m "chore: fix lint and formatting issues"
```
