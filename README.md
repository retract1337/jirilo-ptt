### 💡 Jirilo PTT
Implementation of PTT (Push to talk) for Wayland

As we know, it’s impossible to properly use PTT in Hyprland or any other Wayland compositor. So, I came up with the idea of creating an independent PTT script that can easily and globally work with any Wayland (or X11) compositor.

Jirilo PTT works with ALSA and PulseAudio backends, but you can easily add any other backend if necessary. 

How does Jirilo PTT work? It’s so fuckin simple! You just send multiple requests to the Jirilo PTT FIFO. When you stop, the script simply interrupts and mutes your microphone globally.

### ⚙️ Setup
Required stuff `PulseAudio` or `Alsa`, `make`, `clang` or `gcc`.

Edit `config.h` if necessary.

Simply run `make` in the Jirilo PTT's folder and then copy `ptt` to your `/usr/bin/` dir or somewhere else. If you want to build in debug mode with all the debug messages, run `make DEBUG=1`.

Run `ptt` or set it to be executed on system startup. For example, in Hyprland, you can do it by using `exec-once ptt`.

Then, bind a button to spam echo `"1" > /tmp/jirilo_ptt` into ptt's FIFO.

Also you can specify a command to execute on timeout. For example, you can use `notify-send "Microphone disabled"` to get a notification when microphone is disabled.

### 🔧 Modes
1 - Unmutes your micro,
0 - Mutes your micro,
ptt - Disables ptt,
status - Prints status.
