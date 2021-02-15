#pragma once
#include <cstdint>

namespace sensactcolors
{
    struct cRGB
    {
        uint8_t R; //0..255
        uint8_t G;
        uint8_t B;
    };

    struct cHSV
    {
        double h; // 0...360
        double s; // 0...100
        double v; // 0...100
    };

    constexpr cRGB Black = {0, 0, 0};
    constexpr cRGB White = {250, 250, 250};
    constexpr cRGB Red = {250, 0, 0};
    constexpr cRGB Lime = {0, 250, 0};
    constexpr cRGB Blue = {0, 0, 250};
    constexpr cRGB Yellow = {250, 250, 0};
    constexpr cRGB Cyan = {0, 250, 250};
    constexpr cRGB Magenta = {250, 0, 250};
    constexpr cRGB Silver = {188, 188, 188};
    constexpr cRGB Gray = {125, 125, 125};
    constexpr cRGB Maroon = {125, 0, 0};
    constexpr cRGB Olive = {125, 125, 0};
    constexpr cRGB Green = {0, 125, 0};
    constexpr cRGB Purple = {125, 0, 125};
    constexpr cRGB Teal = {0, 125, 125};
    constexpr cRGB Navy = {0, 0, 125};
    constexpr cRGB Palette16[] = {
        White,
        Red,
        Lime,
        Blue,
        Yellow,
        Cyan,
        Magenta,
        Silver,
        Gray,
        Maroon,
        Olive,
        Green,
        Purple,
        Teal,
        Navy,
        Black,
    };
} // namespace sensactcolors