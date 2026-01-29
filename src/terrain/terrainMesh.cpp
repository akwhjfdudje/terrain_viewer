#include "terrainMesh.h"
#include "heightmap.h"
#include "mesh.h"

TerrainMesh::TerrainMesh(int width, int depth) {

    m_width = width;
    m_depth = depth;

    // generate a terrain heightmap
    m_heightmap.resize(width * depth);

    generateHeightmapCPU(
        m_heightmap.data(),
        width, depth,
        m_scale,
        m_seed,
        m_mix
    ); 

    // allocate CPU buffers
    std::vector<float3> vertices(width * depth);
    std::vector<float3> normals(width * depth);
    std::vector<float2> texcoords(width * depth);

    // generate mesh CPU version
    generateMeshFromHeightmapCPU(
        m_heightmap.data(),
        vertices.data(),
        normals.data(),
        texcoords.data(),
        width, depth,
        1.0f, 1.0f, m_scale // scaleX, scaleY, heightScale
    );

    // flatten vertices and normals into OpenGL buffer (interleaved or positions only)
    std::vector<float> vertexBuffer;
    vertexBuffer.reserve(width * depth * 6); // 3 pos + 3 normal

    for (int i = 0; i < vertices.size(); ++i) {
        const auto& v = vertices[i];
        const auto& n = normals[i];

        vertexBuffer.push_back(v.x);
        vertexBuffer.push_back(v.y);
        vertexBuffer.push_back(v.z);

        vertexBuffer.push_back(n.x);
        vertexBuffer.push_back(n.y);
        vertexBuffer.push_back(n.z);
    }

    // generate indices
    std::vector<unsigned int> indices;
    for (int y = 0; y < depth - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {
            int start = y * width + x;

            indices.push_back(start);
            indices.push_back(start + width);
            indices.push_back(start + 1);

            indices.push_back(start + 1);
            indices.push_back(start + width);
            indices.push_back(start + width + 1);
        }
    }
    m_indexCount = indices.size();

    // upload to OpenGL
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), vertexBuffer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));


    glBindVertexArray(0);
}

TerrainMesh::~TerrainMesh() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
}

void TerrainMesh::regenerate() {
    generateHeightmapCPU(
        m_heightmap.data(),
        m_width,
        m_depth,
        m_scale,
        m_seed,
        m_mix
    );

    std::vector<float3> vertices(m_width * m_depth);
    std::vector<float3> normals(m_width * m_depth);
    std::vector<float2> texcoords(m_width * m_depth);

    generateMeshFromHeightmapCPU(
        m_heightmap.data(),
        vertices.data(),
        normals.data(),
        texcoords.data(),
        m_width,
        m_depth,
        1.0f, 1.0f, m_scale
    );

    // update OpenGL buffer
    std::vector<float> vertexBuffer;
    vertexBuffer.reserve(m_width * m_depth * 6);
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertexBuffer.push_back(vertices[i].x);
        vertexBuffer.push_back(vertices[i].y);
        vertexBuffer.push_back(vertices[i].z);

        vertexBuffer.push_back(normals[i].x);
        vertexBuffer.push_back(normals[i].y);
        vertexBuffer.push_back(normals[i].z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), vertexBuffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TerrainMesh::generateGrid(int width, int depth) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices
    for (int z = 0; z <= depth; ++z) {
        for (int x = 0; x <= width; ++x) {
            vertices.push_back(float(x) / width - 0.5f); // x [-0.5,0.5]
            vertices.push_back(0.0f);                     // y height (flat for now)
            vertices.push_back(float(z) / depth - 0.5f); // z [-0.5,0.5]
        }
    }

    // Generate indices (two triangles per quad)
    for (int z = 0; z < depth; ++z) {
        for (int x = 0; x < width; ++x) {
            int start = z * (width + 1) + x;
            indices.push_back(start);
            indices.push_back(start + width + 1);
            indices.push_back(start + 1);

            indices.push_back(start + 1);
            indices.push_back(start + width + 1);
            indices.push_back(start + width + 2);
        }
    }

    m_indexCount = indices.size();

    // OpenGL buffers
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

void TerrainMesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

