#include <locale.h>
#include <ncurses.h>

int main(void)
{
    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

    printf("\033[?1003h");
    fflush(stdout);

    clear();
    mvprintw(0, 0, "Mouse test");
    mvprintw(1, 0, "Tap, drag, scroll, double tap...");
    mvprintw(2, 0, "Press 'q' to quit.");
    refresh();

    int ch;
    MEVENT ev;

    while ((ch = getch()) != 'q') {
        if (ch == KEY_MOUSE) {
            if (getmouse(&ev) == OK) {

                mvprintw(4, 0, "x=%3d y=%3d          ", ev.x, ev.y);
                mvprintw(5, 0, "bstate = 0x%08lx      ",
                         (unsigned long)ev.bstate);

                int row = 7;

#define SHOW(flag) \
    if (ev.bstate & flag) mvprintw(row++, 0, #flag);

                SHOW(BUTTON1_PRESSED);
                SHOW(BUTTON1_RELEASED);
                SHOW(BUTTON1_CLICKED);
                SHOW(BUTTON1_DOUBLE_CLICKED);
                SHOW(BUTTON1_TRIPLE_CLICKED);
                SHOW(BUTTON2_CLICKED);
                SHOW(BUTTON3_CLICKED);
                SHOW(BUTTON4_PRESSED);
                SHOW(BUTTON4_RELEASED);
                SHOW(BUTTON5_PRESSED);
                SHOW(BUTTON5_RELEASED);
                SHOW(REPORT_MOUSE_POSITION);

#undef SHOW

                while (row < 20)
                    mvprintw(row++, 0, "                              ");

                refresh();
            }
        }
    }

    printf("\033[?1003l");
    fflush(stdout);

    endwin();
    return 0;
}

