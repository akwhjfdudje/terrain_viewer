#pragma once

#include <glad/gl.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

class TerrainMesh {
public:
    TerrainMesh(int width, int depth);
    ~TerrainMesh();

    void draw() const;
    void regenerate();

    void generateGrid(int width, int depth);

    std::vector<float> m_heightmap;
    float m_scale = 100.0f;
    int m_seed = 12348970;
    float m_mix = 0.6f;
    int m_width, m_depth;
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ebo = 0;
    size_t m_indexCount = 0;
};

