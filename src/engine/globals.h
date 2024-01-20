
#pragma once

extern int LOG_LEVEL;

static int __WIN_H = 720;
static int __WIN_W = 1280;

[[nodiscard]] inline int WIN_W() { return __WIN_W; }
[[nodiscard]] inline float WIN_WF() { return static_cast<float>(__WIN_W); }

[[nodiscard]] inline int WIN_H() { return __WIN_H; }
[[nodiscard]] inline float WIN_HF() { return static_cast<float>(__WIN_H); }
