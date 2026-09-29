#ifndef TERMINAL_SIZE_H
#define TERMINAL_SIZE_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

typedef struct {
    int width;
    int height;
    int left;
    int top;
} TerminalViewport;

static inline TerminalViewport terminal_viewport(void) {
    int columns = 80;
    int rows = 24;

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
        columns = info.srWindow.Right - info.srWindow.Left + 1;
        rows = info.srWindow.Bottom - info.srWindow.Top + 1;
    }
#else
    struct winsize size;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 &&
        size.ws_col > 0 && size.ws_row > 0) {
        columns = size.ws_col;
        rows = size.ws_row;
    }
#endif

    TerminalViewport viewport;
    viewport.width = columns * 85 / 100;
    viewport.height = rows * 85 / 100;
    if (viewport.width < 1) viewport.width = 1;
    if (viewport.height < 1) viewport.height = 1;
    viewport.left = (columns - viewport.width) / 2 + 1;
    viewport.top = (rows - viewport.height) / 2 + 1;
    return viewport;
}

#endif
