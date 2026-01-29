#pragma once
#include <cmath>
#include <vector>
#include <algorithm>

struct float3 { float x, y, z; };
struct float2 { float x, y; };

inline void generateMeshFromHeightmapCPU(
    const float* heightmap,
    float3* vertices,
    float3* normals,
    float2* texcoords,
    int width,
    int height,
    float scaleX,
    float scaleY,
    float heightScale
) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = y * width + x;
            float h = heightmap[idx];

            // vertex
            vertices[idx].x = x * scaleX;
            vertices[idx].y = h * heightScale;
            vertices[idx].z = y * scaleY;

            // texcoord
            texcoords[idx].x = (width > 1)  ? float(x) / float(width - 1)  : 0.0f;
            texcoords[idx].y = (height > 1) ? float(y) / float(height - 1) : 0.0f;

            // finite differences with clamping
            int xm = std::max(x - 1, 0);
            int xp = std::min(x + 1, width - 1);
            int ym = std::max(y - 1, 0);
            int yp = std::min(y + 1, height - 1);

            float hL = heightmap[y * width + xm];
            float hR = heightmap[y * width + xp];
            float hD = heightmap[ym * width + x];
            float hU = heightmap[yp * width + x];

            float dx = (hR - hL) * 0.5f * heightScale / ((scaleX != 0.0f) ? scaleX : 1.0f);
            float dz = (hU - hD) * 0.5f * heightScale / ((scaleY != 0.0f) ? scaleY : 1.0f);

            float3 n;
            n.x = -dx;
            n.y = 1.0f;
            n.z = -dz;

            float len = std::sqrt(n.x*n.x + n.y*n.y + n.z*n.z);
            if (len > 1e-8f) {
                n.x /= len; n.y /= len; n.z /= len;
            } else {
                n.x = 0.0f; n.y = 1.0f; n.z = 0.0f;
            }

            normals[idx] = n;
        }
    }
}

