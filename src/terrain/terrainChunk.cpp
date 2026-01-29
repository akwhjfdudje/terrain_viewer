#include "terrainChunk.h"
#include "const.h"
#include <cmath>

static inline float hash(int x, int y, int seed) {
    int n = x + y * 57 + seed * 131;
    n = (n << 13) ^ n;
    return 1.0f - ((n * (n * n * 15731 + 789221) + 1376312589)
        & 0x7fffffff) / 1073741824.0f;
}

static inline float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

static inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

static float perlin(float x, float y, int seed) {
    int x0 = (int)floorf(x);
    int y0 = (int)floorf(y);
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

//
TerrainChunk::TerrainChunk(ChunkCoord c) : coord(c) {}

TerrainChunk::~TerrainChunk() {
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

void TerrainChunk::generateHeightmap(int seed) {
    heightmap.resize(CHUNK_SIZE * CHUNK_SIZE);

    int wx0 = coord.x * CHUNK_SIZE;
    int wz0 = coord.z * CHUNK_SIZE;

    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            float wx = float(wx0 + x) * NOISE_SCALE;
            float wz = float(wz0 + z) * NOISE_SCALE;

            float h = 0.0f;
            float amp = 1.0f;
            float freq = 1.0f;

            for (int o = 0; o < 4; o++) {
                h += perlin(wx * freq, wz * freq, seed + o * 17) * amp;
                amp *= 0.5f;
                freq *= 2.0f;
            }

            heightmap[z * CHUNK_SIZE + x] = glm::clamp(h, -1.0f, 1.0f);
        }
    }
}

void TerrainChunk::buildMesh(float heightScale) {
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
    };

    std::vector<Vertex> vertices(CHUNK_SIZE * CHUNK_SIZE);
    std::vector<uint32_t> indices;

    int wx0 = coord.x * CHUNK_SIZE;
    int wz0 = coord.z * CHUNK_SIZE;

    auto H = [&](int x, int z) {
        x = glm::clamp(x, 0, CHUNK_SIZE - 1);
        z = glm::clamp(z, 0, CHUNK_SIZE - 1);
        return heightmap[z * CHUNK_SIZE + x] * heightScale;
    };

    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int i = z * CHUNK_SIZE + x;

            float wx = (wx0 + x) * CELL_SIZE;
            float wz = (wz0 + z) * CELL_SIZE;
            float wy = H(x, z);

            float dx = (H(x + 1, z) - H(x - 1, z)) * 0.5f;
            float dz = (H(x, z + 1) - H(x, z - 1)) * 0.5f;

            glm::vec3 n = glm::normalize(glm::vec3(-dx, 1.0f, -dz));

            vertices[i] = { {wx, wy, wz}, n };
        }
    }

    for (int z = 0; z < CHUNK_SIZE - 1; z++) {
        for (int x = 0; x < CHUNK_SIZE - 1; x++) {
            int i = z * CHUNK_SIZE + x;
            indices.push_back(i);
            indices.push_back(i + CHUNK_SIZE);
            indices.push_back(i + 1);

            indices.push_back(i + 1);
            indices.push_back(i + CHUNK_SIZE);
            indices.push_back(i + CHUNK_SIZE + 1);
        }
    }

    indexCount = (int)indices.size();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(uint32_t),
        indices.data(),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glBindVertexArray(0);
}

void TerrainChunk::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

