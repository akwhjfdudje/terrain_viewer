#pragma once

#include <glad/gl.h>
#include <vector>

class TerrainMesh {
public:
    TerrainMesh(int width, int depth);
    ~TerrainMesh();

    void draw() const;
    void regenerate();

private:
    void generateGrid(int width, int depth);

private:
    std::vector<float> m_heightmap;
    float m_scale = 10.0f;
    int m_seed = 1337;
    float m_mix = 0.3f;
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ebo = 0;
    size_t m_indexCount = 0;
};

