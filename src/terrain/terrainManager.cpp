#include "terrainManager.h"
#include "const.h"
#include <cmath>

void TerrainManager::update(const glm::vec3& camPos) {
    int cx = (int)floor(camPos.x / (CHUNK_SIZE * CELL_SIZE));
    int cz = (int)floor(camPos.z / (CHUNK_SIZE * CELL_SIZE));

    for (int dz = -viewRadius; dz <= viewRadius; dz++) {
        for (int dx = -viewRadius; dx <= viewRadius; dx++) {
            ChunkCoord cc{ cx + dx, cz + dz };

            if (chunks.find(cc) == chunks.end()) {
                auto chunk = std::make_unique<TerrainChunk>(cc);
                chunk->generateHeightmap(m_seed);
                chunk->buildMesh(m_scale);
                chunks.emplace(cc, std::move(chunk));
            }
        }
    }

    for (auto it = chunks.begin(); it != chunks.end();) {
        int dx = it->first.x - cx;
        int dz = it->first.z - cz;
        if (abs(dx) > viewRadius || abs(dz) > viewRadius) {
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
}

void TerrainManager::draw() const {
    for (auto& [_, chunk] : chunks) {
        chunk->draw();
    }
}

