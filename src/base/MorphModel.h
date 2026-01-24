//
// Created by veren on 18.01.2026.
//

#ifndef MORPHING_MORPHMODEL_H
#define MORPHING_MORPHMODEL_H

#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#define TINYGLTF_NO_STB_IMAGE_WRITE
//#define TINYGLTF_NO_STB_IMAGE

#include "tiny_gltf.h"

struct MorphMesh {
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int morphVBO = 0;
    unsigned int normalVBO = 0;
    unsigned int morphNormalVBO = 0;
    unsigned int indexCount = 0;
    unsigned int vertexCount = 0;
    bool hasIndices = false;
    bool isInitialized = false;
};

class MorphModel {
public:
    MorphModel();

    ~MorphModel();

    bool load(const std::string &filename);

    void initializeGL(QOpenGLExtraFunctions *gl);

    void cleanup();

    void render(QOpenGLExtraFunctions *gl, unsigned int shaderProgram, float factor);

    [[nodiscard]] bool isLoaded() const { return m_loaded; }

    std::vector<MorphMesh> m_meshes;

private:
    bool createMesh(QOpenGLExtraFunctions *gl, const tinygltf::Model &model,
                    const tinygltf::Primitive &primitive, MorphMesh &gltfMesh);

    void processNode(QOpenGLExtraFunctions *gl, const tinygltf::Model &model,
                     const tinygltf::Node &node, const glm::mat4 &parentTransform);

    void processScene(QOpenGLExtraFunctions *gl);

    tinygltf::Model m_model;
    bool m_loaded = false;
    bool m_glInitialized = false;
};


#endif //MORPHING_MORPHMODEL_H
