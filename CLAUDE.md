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
