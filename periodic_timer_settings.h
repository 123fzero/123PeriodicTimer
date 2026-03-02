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

static inline uint32_t periodic_timer_interval_total(const PeriodicTimerSettings* s) {
    uint32_t total = s->interval_min * 60 + s->interval_sec;
    return total > 0 ? total : 1;
}
