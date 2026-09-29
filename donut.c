#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "terminal_size.h"

static volatile sig_atomic_t running = 1;

static void stop(int signal_number) {
    (void)signal_number;
    running = 0;
}

static void render(char *pixels, float *depth, TerminalViewport viewport,
                   float A, float B) {
    const int width = viewport.width;
    const int height = viewport.height;
    const size_t count = (size_t)width * height;
    const float scale_y = fminf(height * 0.60f, width * 0.30f);
    const float scale_x = 2.0f * scale_y;
    const float center_x = (width - 1) * 0.5f;
    const float center_y = (height - 1) * 0.5f;
    const float e = sinf(A), g = cosf(A);
    const float m = cosf(B), n = sinf(B);
    const char shades[] = ".,-~:;=!*#$@";
    const int last_shade = (int)sizeof(shades) - 2;

    memset(pixels, ' ', count);
    memset(depth, 0, count * sizeof(*depth));

    for (float j = 0; j < 6.2831853f; j += 0.07f) {
        const float d = cosf(j), f = sinf(j), h = d + 2.0f;
        for (float i = 0; i < 6.2831853f; i += 0.02f) {
            const float c = sinf(i), l = cosf(i);
            const float D = 1.0f / (c * h * e + f * g + 5.0f);
            const float t = c * h * g - f * e;
            const int x = (int)lroundf(center_x + scale_x * D * (l * h * m - t * n));
            const int y = (int)lroundf(center_y + scale_y * D * (l * h * n + t * m));
            if (x < 0 || x >= width || y < 0 || y >= height) continue;

            const size_t index = (size_t)y * width + x;
            if (D <= depth[index]) continue;
            depth[index] = D;

            int shade = (int)(8.0f * ((f * e - c * d * g) * m -
                                       c * d * e - f * g - l * d * n));
            if (shade < 0) shade = 0;
            if (shade > last_shade) shade = last_shade;
            pixels[index] = shades[shade];
        }
    }
}

int main(void) {
    TerminalViewport viewport = {0, 0, 0, 0};
    char *pixels = NULL;
    float *depth = NULL;
    float A = 0.0f, B = 0.0f;
    int status = 0;

    signal(SIGINT, stop);
    signal(SIGTERM, stop);
    printf("\x1b[?25l");

    while (running) {
        TerminalViewport next = terminal_viewport();
        if (next.width != viewport.width || next.height != viewport.height ||
            next.left != viewport.left || next.top != viewport.top) {
            const size_t count = (size_t)next.width * next.height;
            char *new_pixels = malloc(count);
            float *new_depth = malloc(count * sizeof(*new_depth));
            if (!new_pixels || !new_depth) {
                free(new_pixels);
                free(new_depth);
                status = 1;
                break;
            }
            free(pixels);
            free(depth);
            pixels = new_pixels;
            depth = new_depth;
            viewport = next;
            printf("\x1b[2J");
        }

        render(pixels, depth, viewport, A, B);
        for (int row = 0; row < viewport.height; ++row) {
            printf("\x1b[%d;%dH", viewport.top + row, viewport.left);
            fwrite(pixels + (size_t)row * viewport.width, 1, viewport.width, stdout);
        }
        fflush(stdout);
        A += 0.07f;
        B += 0.035f;
#ifdef _WIN32
        Sleep(30);
#else
        usleep(30000);
#endif
    }

    printf("\x1b[?25h\x1b[0m\n");
    fflush(stdout);
    free(pixels);
    free(depth);
    return status;
}
