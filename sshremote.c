#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define SCROLL_STEP 25

typedef struct {
    char target[256];
    char port[32];
    char key[256];
    FILE *ssh;
} config_t;

int mouse_horizontal = 0;

FILE *open_ssh(config_t *cfg)
{
    char cmd[512];

    if (strlen(cfg->key) && strlen(cfg->port)) {
        snprintf(cmd, sizeof(cmd),
            "ssh -i %s -p %s %s 'DISPLAY=:0 bash'",
            cfg->key, cfg->port, cfg->target);
    }
    else if (strlen(cfg->key)) {
        snprintf(cmd, sizeof(cmd),
            "ssh -i %s %s 'DISPLAY=:0 bash'",
            cfg->key, cfg->target);
    }
    else if (strlen(cfg->port)) {
        snprintf(cmd, sizeof(cmd),
            "ssh -p %s %s 'DISPLAY=:0 bash'",
            cfg->port, cfg->target);
    }
    else {
        snprintf(cmd, sizeof(cmd),
            "ssh %s 'DISPLAY=:0 bash'",
            cfg->target);
    }

    return popen(cmd, "w");
}

void send_shell(config_t *cfg, const char *cmd)
{
    if (!cfg->ssh)
        return;

    fprintf(cfg->ssh, "%s\n", cmd);
    fflush(cfg->ssh);
}

void send_cmd(config_t *cfg, const char *cmd)
{
    if (!cfg->ssh) return;

    fprintf(cfg->ssh, "xdotool %s\n", cmd);
    fflush(cfg->ssh);
}

void move_mouse(config_t *cfg, int dy)
{
    char buf[64];
    if(mouse_horizontal == 0)
        snprintf(buf, sizeof(buf),
            "mousemove_relative -- 0 %d", dy);
    else
        snprintf(buf, sizeof(buf), "mousemove_relative -- %d 0", dy);

    send_cmd(cfg, buf);
}

void click(config_t *cfg, int button)
{
    char buf[32];
    snprintf(buf, sizeof(buf),
        "click %d", button);

    send_cmd(cfg, buf);
}

char* translate_special_chars(int key) {
    if(key == 127) return "BackSpace";
    if(key == 32) return "space";
    if(key == KEY_PPAGE) return "Prior";
    if(key == KEY_NPAGE) return "Next";

    return NULL;
}

void press(config_t *cfg, int key)
{
    char buf[32];
    if(key == 127 || key == 32 || key == KEY_PPAGE || key == KEY_NPAGE) {
        snprintf(buf, sizeof(buf), "key %s", translate_special_chars(key));
    }
    else {
        snprintf(buf, sizeof(buf),
            "key %c", key);
    }
    send_cmd(cfg, buf);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s user@host [-p port] [-i key]\n", argv[0]);
        return 1;
    }

    config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    strncpy(cfg.target, argv[1], sizeof(cfg.target)-1);

    for (int i = 2; i < argc; i++) {
        if (!strcmp(argv[i], "-p") && i+1 < argc)
            strncpy(cfg.port, argv[++i], sizeof(cfg.port)-1);

        else if (!strcmp(argv[i], "-i") && i+1 < argc)
            strncpy(cfg.key, argv[++i], sizeof(cfg.key)-1);
    }

    cfg.ssh = open_ssh(&cfg);
    if (!cfg.ssh) {
        printf("Failed to open SSH session\n");
        return 1;
    }

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS, NULL);

    printw("Tap=Left | TripleTap=Right\n");
    printw("Scroll=Move Mouse (primary control)\n");
    printw("DoubleTap=Change direction of scroll\n");
    printw("PGUP/PGDOWN in Termux=Scroll\n");
    printw("Press k to enter keyboard mode\n");
    printw("Press c to send commands\n");
    printw("Press q to quit\n");
    refresh();

    int ch;
    MEVENT ev;

    while ((ch = getch()) != 'q') {

        if (ch == 'c') {

            char command[512];

            move(LINES - 1, 0);
            clrtoeol();
            echo();
            nocbreak();

            printw("Command: ");
            refresh();

            getnstr(command, sizeof(command) - 1);

            noecho();
            cbreak();

            if (strlen(command) > 0)
                send_shell(&cfg, command);

            move(LINES - 1, 0);
            clrtoeol();
            refresh();

            continue;
        }

        if (ch == 'k') {

            int character;
            move(LINES - 1, 0);
            clrtoeol();
            printw("Keyboard mode on! Press ESC to exit");
            while ((character = getch()) != 27) {
        if(getmouse(&ev) == OK) continue;
            press(&cfg, character);
        }
            move(LINES - 1, 0);
            clrtoeol();
    }

    if(ch == KEY_PPAGE || ch == KEY_NPAGE) {
        press(&cfg, ch);
        continue;
    }

        if (ch != KEY_MOUSE)
            continue;

        if (getmouse(&ev) != OK)
            continue;

        if (ev.bstate & BUTTON4_PRESSED) {
            move_mouse(&cfg, -SCROLL_STEP);
        }

        if (ev.bstate & BUTTON5_PRESSED) {
            move_mouse(&cfg, SCROLL_STEP);
        }

        if (ev.bstate & BUTTON1_CLICKED) {
            click(&cfg, 1);
        }

        if (ev.bstate & BUTTON1_DOUBLE_CLICKED) {
            mouse_horizontal = !mouse_horizontal;
        }

        if (ev.bstate & BUTTON1_TRIPLE_CLICKED) {
            click(&cfg, 3);
        }
    }

    endwin();

    if (cfg.ssh)
        pclose(cfg.ssh);

    return 0;
}

