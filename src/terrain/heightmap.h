#pragma once
#include <cmath>
#include <algorithm>

inline float hash(int x, int y, int seed = 1337) {
    int n = x + y * 57 + seed * 131;
    n = (n << 13) ^ n;
    return 1.0f - float((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f;
}

inline float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

inline float perlin(float x, float y, int seed) {
    int x0 = std::floor(x);
    int y0 = std::floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float sx = fade(x - x0);
    float sy = fade(y - y0);

    float n00 = hash(x0, y0, seed);
    float n10 = hash(x1, y0, seed);
    float n01 = hash(x0, y1, seed);
    float n11 = hash(x1, y1, seed);

    float ix0 = lerp(n00, n10, sx);
    float ix1 = lerp(n01, n11, sx);
    return lerp(ix0, ix1, sy);
}

inline float voronoi(float x, float y, int cell_count, int seed) {
    int xi = std::floor(x);
    int yi = std::floor(y);

    float minDist = 1e10f;
    for (int j = -1; j <= 1; ++j) {
        for (int i = -1; i <= 1; ++i) {
            int cx = xi + i;
            int cy = yi + j;

            float rx = hash(cx, cy, seed) * 0.5f + 0.5f;
            float ry = hash(cx, cy, seed + 42) * 0.5f + 0.5f;

            float dx = (cx + rx) - x;
            float dy = (cy + ry) - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            minDist = std::min(minDist, dist);
        }
    }
    return minDist;
}

inline void generateHeightmapCPU(
    float* heightmap,
    int width,
    int height,
    float scale,
    int seed,
    float mix_ratio
) {
    for (int idx = 0; idx < width * height; ++idx) {
        int x = idx % width;
        int y = idx / width;

        float fx = x / scale;
        float fy = y / scale;

        // Multi-octave Perlin noise
        float amp = 1.0f;
        float freq = 1.0f;
        float noise_sum = 0.0f;
        for (int o = 0; o < 4; ++o) {
            noise_sum += perlin(fx * freq, fy * freq, seed + o * 17) * amp;
            amp *= 0.5f;
            freq *= 2.0f;
        }

        // Voronoi distance field
        float v = voronoi(fx * 0.5f, fy * 0.5f, 8, seed + 999);

        // Blend Perlin and Voronoi
        float height_val = (1.0f - mix_ratio) * noise_sum + mix_ratio * (1.0f - v * 2.0f);

        // Clamp to [-1, 1]
        height_val = std::max(-1.0f, std::min(1.0f, height_val));
        heightmap[idx] = height_val;
    }
}

