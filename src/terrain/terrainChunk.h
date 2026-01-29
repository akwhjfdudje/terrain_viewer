#pragma once
#include <vector>
#include <glad/gl.h>
#include <glm/glm.hpp>

struct ChunkCoord {
    int x, z;
    bool operator==(const ChunkCoord& o) const {
        return x == o.x && z == o.z;
    }
};

struct ChunkCoordHash {
    size_t operator()(const ChunkCoord& c) const {
        return (std::hash<int>()(c.x) << 1) ^ std::hash<int>()(c.z);
    }
};

class TerrainChunk {
public:
    ChunkCoord coord;

    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    int indexCount = 0;

    std::vector<float> heightmap;

    TerrainChunk(ChunkCoord c);
    ~TerrainChunk();

    void generateHeightmap(int seed);
    void buildMesh(float heightScale);
    void draw() const;
};

