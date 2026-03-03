# 123PeriodicTimer — Repeating Interval Timer for Flipper Zero

123PeriodicTimer is a Flipper Zero repeating interval timer and recurring reminder app. Set any interval from 1 second to 59:59, then let the timer repeat continuously until you stop it, with beep and vibration alerts at every cycle.

If you are looking for a Flipper Zero interval timer, Flipper Zero repeating timer, Flipper Zero recurring alarm, or a simple Flipper Zero productivity timer for workouts, study sessions, breathing, cooking, or Pomodoro-style pacing, this app is built for that exact use case.

## Why?

Sometimes you need a simple repeating timer: time-boxing tasks, pacing activities, cooking intervals, meditation, or anything that needs a periodic reminder. 123PeriodicTimer does one thing well — beeps at your interval, forever, until you stop it.

## Best For

- **HIIT and workout intervals** on Flipper Zero
- **Pomodoro-style focus sessions** with repeating reminders
- **Breathing and meditation pacing** with gentle periodic alerts
- **Cooking and kitchen timing** when you need a recurring beep
- **General recurring reminders** for any short repeated task

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

## Common Use Cases

- Repeating work / break prompts
- Exercise rounds and recovery intervals
- Stretch, hydration, and posture reminders
- Tea, coffee, and kitchen countdown loops
- Habit-building routines that need a simple recurring alert

## Screenshots

```
┌────────────────────────────┐   ┌────────────────────────────┐
│     123PeriodicTimer       │   │ Vibration         < Short >│
│                            │   │ Sound                < On >│
│       [00] : [30]          │   │ Display Mode  < Countdown >│
│        min   sec           │   │                            │
│                            │   │                            │
│ [Settings]       [Start >] │   │                            │
│                            │   │                            │
└────────────────────────────┘   └────────────────────────────┘
         Main Screen                       Settings

┌────────────────────────────┐   ┌────────────────────────────┐
│123PeriodicTimer    00:30   │   │123PeriodicTimer    00:30   │
│                            │   │                            │
│           18               │   │           5                │
│                            │   │                            │
│       countdown            │   │        intervals           │
│    [███████░░░░░░░]        │   │    [███████░░░░░░░]        │
│ [OK]=Pause    [<]=Stop     │   │ [OK]=Pause    [<]=Stop     │
└────────────────────────────┘   └────────────────────────────┘
     Timer: Countdown Mode           Timer: Intervals Mode

┌────────────────────────────┐   ┌────────────────────────────┐
│123PeriodicTimer    00:30   │   │123PeriodicTimer    00:30   │
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
3. Open on Flipper: `Applications → Tools → 123PeriodicTimer`

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

1. Open 123PeriodicTimer from Applications → Tools
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

## SEO Keywords

Flipper Zero timer, Flipper Zero interval timer, Flipper Zero repeating timer, Flipper Zero recurring reminder, Flipper Zero productivity app, Flipper Zero workout timer, Flipper Zero HIIT timer, Flipper Zero Pomodoro timer, Flipper Zero meditation timer, Flipper Zero cooking timer, periodic timer for Flipper Zero, recurring alarm for Flipper Zero.

## License

MIT
