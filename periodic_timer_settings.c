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
