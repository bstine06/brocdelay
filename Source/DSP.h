/*
  ==============================================================================

    DSP.h
    Created: 16 May 2025 8:53:03pm
    Author:  Brett

  ==============================================================================
*/

#pragma once

#include <cmath>

// Applies equal power gain to two channels L and R based on a panning value between {-1, 1}
inline void panningEqualPower(float panning, float& left, float& right)
{
    float x = 0.25f * static_cast<float>(M_PI) * (panning + 1.0f);
    left = std::cos(x);
    right = std::sin(x);
}

// Applies equal power gain to a signal based on a value from [0, 1]
inline void equalPower(float& signal, float mix)
{
    float x = 0.5f * static_cast<float>(M_PI) * mix;
    signal *= std::cos(x);
}

inline float lerp(float a, float b, float c) {
    return a * (1 - c) + b * c;
}
