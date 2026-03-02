# PeriodicTimer — Flipper Zero App

Repeating interval timer inspired by the iOS app "Periodic Timer".

## What It Does
- Set interval from 00:01 to 59:59 (minutes:seconds)
- Timer repeats infinitely with beep/vibration alerts
- 3 display modes: Countdown, Intervals count, Total Time (switch with Up/Down during timer)
- Vibration control: Off / Short / Long
- Sound control: Off / On
- Pause/resume with OK button
- Settings persisted to SD card
- Backlight stays on during active timer

## Technical Details
- Flipper Zero, SDK (ufbt), API 87.1
- Firmware tested: Momentum mntm-012
- Language: C
- Architecture: ViewDispatcher + SceneManager
- 3 scenes: Main (interval picker), Timer (infinite repeat with alerts), Settings
- 2 custom Views: timer_main_view (MM:SS picker), timer_session_view (3 display modes + progress bar)
- 1 built-in widget: VariableItemList (settings)
- Timer: FuriTimer periodic 1s, sends custom event on each tick
- Notifications: separate vibro (short 100ms / long 500ms) and sound (C7 beep) sequences
- Settings: FlipperFormat v1 at /ext/apps_data/periodic_timer/settings.conf
- Category: Tools
- appid: periodic_timer
- Icon: 10x10 (shared with PuffPacer, replace with custom timer icon)

## File Structure
```
├── application.fam              # FAP manifest
├── periodic_timer.c             # Entry point, alloc/free
├── periodic_timer_app.h         # App struct, view enums
├── periodic_timer_settings.h    # Settings struct, enums, constants
├── periodic_timer_settings.c    # FlipperFormat save/load
├── scenes/
│   ├── periodic_timer_scene.h        # Scene + custom event enums
│   ├── periodic_timer_scene_config.c # Handler wiring
│   ├── periodic_timer_scene_main.c   # Interval picker + navigation
│   ├── periodic_timer_scene_timer.c  # FuriTimer, alerts, infinite repeat
│   └── periodic_timer_scene_settings.c # Vibro/Sound/Display mode
├── views/
│   ├── timer_main_view.h/c      # MM:SS picker with selection frame
│   └── timer_session_view.h/c   # 3 display modes, progress bar, pause
├── icon.png
├── README.md
└── CLAUDE.md
```

## Build
```bash
ufbt          # build .fap
ufbt launch   # build + deploy + run on connected Flipper
ufbt lint     # check formatting
```

## Formatting
`ufbt format` is broken (missing colorlog). Use clang-format directly:
```bash
~/.ufbt/toolchain/arm64-darwin/bin/clang-format -i *.c *.h scenes/*.c scenes/*.h views/*.c views/*.h
```

## Communication
- Speak Russian with the developer
- Write code and documentation in English
