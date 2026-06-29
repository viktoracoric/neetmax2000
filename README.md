# neetmax2000
Remote control your PC from your Android phone, through the magic of Termux and ncurses!

# Compilation

`gcc sshremote.c -lncurses -o sshremote`

# Usage

`./sshremote user@host -p port -i priv_ssh_key`

| Tap                          | Left click                                         |
|------------------------------|----------------------------------------------------|
| Triple Tap                   | Right click                                        |
| Swipe up / down              | Scroll                                             |
| Double tap                   | Change direction of scroll (horizontal / vertical) |
| k                            | keyboard mode                                      |
| ESC (while in keyboard mode) | exit keyboard mode                                 |
| c                            | send commands (e.g. loginctl poweroff)             |
| q                            | quit                                               |

Project started as a thought experiment, inspired by [Stefan Sundin's SSH Remote](https://github.com/stefansundin/SSHRemote), an Android app (which is the best Android remote controller app I've seen).

Released under GPLv3, I am very curious how people will extend this idea, trying to bypass Termux terminal limitations was very fun!
