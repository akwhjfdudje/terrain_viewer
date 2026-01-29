#pragma once
#include "terrainChunk.h"
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

class TerrainManager {
public:
    int viewRadius = 4;
    int m_seed = 1337;
    float m_scale = 100.0f;

    std::unordered_map<ChunkCoord,
        std::unique_ptr<TerrainChunk>,
        ChunkCoordHash> chunks;

    void update(const glm::vec3& cameraPos);
    void draw() const;
};

