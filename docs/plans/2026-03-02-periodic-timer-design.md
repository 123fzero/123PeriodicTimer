# PeriodicTimer for Flipper Zero — Design Document

## Concept

Universal repeating interval timer inspired by the iOS app "Periodic Timer". Set an interval — Flipper beeps/vibrates every time the interval elapses. Repeats infinitely until stopped.

## Architecture

- **Pattern**: ViewDispatcher + SceneManager (standard Flipper Zero)
- **Language**: C
- **SDK**: Flipper Zero SDK (ufbt)
- **3 Scenes**: Main, Timer, Settings
- **Widgets**: 2 Custom Views (main, timer), VariableItemList (settings)
- **Timer**: FuriTimer periodic 1s

## Screens

### 1. Main Screen — interval setup + start

```
┌────────────────────────────┐
│      Periodic Timer        │
│                            │
│        00 : 30             │
│        min  sec            │
│                            │
│                            │
│ [Settings]       [Start >] │
└────────────────────────────┘
```

- Left/Right: switch between minutes and seconds
- Up/Down: change value
- OK: Start timer
- Back long: Settings

### 2. Timer Screen — active timer with 3 display modes

```
┌────────────────────────────┐
│ Periodic Timer     00:30   │
│                            │
│       Interval: 5          │
│                            │
│    [███████░░░░░░░]        │
│ [OK]=Pause    [<]=Stop     │
└────────────────────────────┘
     Mode: Intervals

┌────────────────────────────┐
│ Periodic Timer     00:30   │
│                            │
│       Total: 02:30         │
│                            │
│    [███████░░░░░░░]        │
│ [OK]=Pause    [<]=Stop     │
└────────────────────────────┘
     Mode: Total Time

┌────────────────────────────┐
│ Periodic Timer     00:30   │
│                            │
│          0:18              │
│                            │
│    [███████░░░░░░░]        │
│ [OK]=Pause    [<]=Stop     │
└────────────────────────────┘
     Mode: Countdown
```

- Header: app name + set interval
- Center: depends on mode
- Progress bar: current interval progress
- OK: Pause/Resume
- Back: Stop → return to Main
- Up/Down: switch display mode

### 3. Settings Screen — VariableItemList

```
┌────────────────────────────┐
│ Vibration         < Short >│
│ Sound                < On >│
│ Display Mode  < Countdown >│
│                            │
└────────────────────────────┘
```

## Data Models

### Settings (persisted to SD card)
- `interval_min`: 0-59, default 0
- `interval_sec`: 0-59, default 30
- `vibro_mode`: Off/Short/Long, default Short
- `sound_mode`: Off/On, default On
- `display_mode`: Intervals/TotalTime/Countdown, default Countdown

### Timer Model (runtime)
- `seconds_remaining`: countdown within current interval
- `interval_count`: completed intervals
- `total_elapsed`: total seconds elapsed
- `paused`: bool

## Notifications
- Beep (C7 note) on each interval completion (if sound on)
- Vibration on each interval completion (if vibro not off)
- Backlight stays on during timer

## File Structure
```
periodic_timer/
├── application.fam
├── periodic_timer.c
├── periodic_timer_app.h
├── periodic_timer_settings.h
├── periodic_timer_settings.c
├── scenes/
│   ├── periodic_timer_scene.h
│   ├── periodic_timer_scene_config.c
│   ├── periodic_timer_scene_main.c
│   ├── periodic_timer_scene_timer.c
│   └── periodic_timer_scene_settings.c
├── views/
│   ├── timer_main_view.h
│   ├── timer_main_view.c
│   ├── timer_session_view.h
│   └── timer_session_view.c
├── icon.png
└── README.md
```

## Settings Persistence
- FlipperFormat v2 on SD card
- Path: `/ext/apps_data/periodic_timer/settings.conf`
- Load on startup, save on change
