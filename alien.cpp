#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

namespace {
constexpr int width = 79;
constexpr int height = 23;
constexpr float pi = 3.14159265f;
volatile std::sig_atomic_t running = 1;

void stop(int) {
    running = 0;
}

struct Point {
    float x;
    float y;
    float z;
};

struct Rotation {
    float cy;
    float sy;
    float cp;
    float sp;

    Rotation(float yaw, float pitch)
        : cy(std::cos(yaw)), sy(std::sin(yaw)),
          cp(std::cos(pitch)), sp(std::sin(pitch)) {}

    Point apply(Point p) const {
        const float x = p.x * cy + p.z * sy;
        const float z = p.z * cy - p.x * sy;
        return {x, p.y * cp - z * sp, p.y * sp + z * cp};
    }
};

char faceDetail(Point p) {
    if (p.z < 0.25f) {
        return '\0';
    }

    // Olhos grandes e inclinados, como no rosto clássico de um alienígena.
    for (const float side : {-1.0f, 1.0f}) {
        const float center = side * (0.46f - 0.15f * p.y);
        const float dx = (p.x - center) / 0.29f;
        const float dy = (p.y - 0.13f) / 0.36f;
        if (dx * dx + dy * dy < 1.0f) {
            return ' ';
        }
    }

    if (std::abs(p.y + 0.41f) < 0.055f &&
        (std::abs(p.x - 0.09f) < 0.035f ||
         std::abs(p.x + 0.09f) < 0.035f)) {
        return '.';
    }
    if (std::abs(p.y + 0.59f) < 0.04f && std::abs(p.x) < 0.20f) {
        return '-';
    }
    return '\0';
}

std::string render(float yaw, float pitch) {
    const Rotation rotation(yaw, pitch);
    std::array<char, width * height> pixels;
    std::array<float, width * height> depth;
    pixels.fill(' ');
    depth.fill(std::numeric_limits<float>::infinity());

    constexpr char shades[] = ".:-=+*#%@";
    constexpr int shadeCount = sizeof(shades) - 2;

    // Amostra a superfície da cabeça, projeta cada ponto e guarda o mais próximo.
    for (float latitude = -pi / 2; latitude <= pi / 2; latitude += 0.018f) {
        const float vertical = std::sin(latitude);
        const float ring = std::cos(latitude) * (1.0f + 0.30f * vertical);
        for (float longitude = -pi; longitude < pi; longitude += 0.018f) {
            const Point p{1.16f * ring * std::sin(longitude),
                          1.24f * vertical,
                          0.83f * std::cos(latitude) * std::cos(longitude)};
            const Point turned = rotation.apply(p);
            const float distance = 4.2f - turned.z;
            const int x = static_cast<int>(std::lround(39.0f + 67.0f * turned.x / distance));
            const int y = static_cast<int>(std::lround(11.0f - 32.0f * turned.y / distance));
            if (x < 0 || x >= width || y < 0 || y >= height) {
                continue;
            }

            const int index = y * width + x;
            if (distance >= depth[index]) {
                continue;
            }
            depth[index] = distance;

            // A normal aproximada fornece a luz, como no sombreado do donut.
            const Point normal = rotation.apply({p.x / 1.35f, p.y / 1.54f,
                                                 p.z / 0.69f});
            const float light = std::clamp(0.35f - 0.30f * normal.x +
                                           0.25f * normal.y + 0.65f * normal.z,
                                           0.0f, 1.0f);
            const int shade = static_cast<int>(light * shadeCount);
            const char detail = faceDetail(p);
            pixels[index] = detail == '\0' ? shades[shade] : detail;
        }
    }

    std::string frame;
    frame.reserve((width + 1) * height);
    for (int y = 0; y < height; ++y) {
        frame.append(pixels.data() + y * width, width);
        frame += '\n';
    }
    return frame;
}
} // namespace

int main(int argc, char* argv[]) {
    if (argc > 1 && std::strcmp(argv[1], "--once") == 0) {
        std::cout << render(0.0f, 0.0f);
        return 0;
    }

    std::signal(SIGINT, stop);
    std::signal(SIGTERM, stop);
    std::cout << "\x1b[2J\x1b[?25l";
    float yaw = 0.0f;
    float pitch = 0.0f;
    while (running) {
        std::cout << "\x1b[H" << render(yaw, pitch) << std::flush;
        yaw += 0.045f;
        pitch = 0.16f * std::sin(yaw * 0.65f);
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
    std::cout << "\x1b[?25h\x1b[0m\n";
}
