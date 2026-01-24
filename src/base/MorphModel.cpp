//
// Created by veren on 18.01.2026.
//

#include "MorphModel.h"

MorphModel::MorphModel() = default;

MorphModel::~MorphModel() {
    cleanup();
}

bool MorphModel::load(const std::string &filename) {
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    m_loaded = loader.LoadASCIIFromFile(&m_model, &err, &warn, filename);
    return m_loaded;
}

void MorphModel::initializeGL(QOpenGLExtraFunctions *gl) {
    if (!m_loaded || m_glInitialized) {
        return;
    }
    processScene(gl);
    m_glInitialized = true;
}

void MorphModel::processScene(QOpenGLExtraFunctions *gl) {
    if (m_model.scenes.empty()) {
        return;
    }
    const auto &scene = m_model.scenes[m_model.defaultScene >= 0 ? m_model.defaultScene : 0];
    for (int nodeId : scene.nodes) {
        if (nodeId >= 0 && nodeId < m_model.nodes.size()) {
            processNode(gl, m_model, m_model.nodes[nodeId], glm::mat4(1.0f));
        } else {
            continue;
        }
    }
}

bool MorphModel::createMesh(QOpenGLExtraFunctions *gl, const tinygltf::Model &model,
                            const tinygltf::Primitive &primitive, MorphMesh &gltfMesh) {
    if (primitive.mode != TINYGLTF_MODE_TRIANGLES) {
        return false;
    }
    auto posIt = primitive.attributes.find("POSITION");
    if (posIt == primitive.attributes.end()) {
        return false;
    }
    const auto &posAccessor = model.accessors[posIt->second];
    if (posAccessor.type != TINYGLTF_TYPE_VEC3) {
        return false;
    }
    if (posAccessor.bufferView < 0 || posAccessor.bufferView >= model.bufferViews.size()) {
        return false;
    }
    const auto &posView = model.bufferViews[posAccessor.bufferView];
    const auto &posBuffer = model.buffers[posView.buffer];
    gl->glGenVertexArrays(1, &gltfMesh.VAO);
    gl->glGenBuffers(1, &gltfMesh.VBO);
    gl->glBindVertexArray(gltfMesh.VAO);
    gl->glBindBuffer(GL_ARRAY_BUFFER, gltfMesh.VBO);
    size_t dataOffset = posView.byteOffset + posAccessor.byteOffset;
    size_t dataSize = posAccessor.count * 3 * sizeof(float);
    if (dataOffset + dataSize > posBuffer.data.size()) {
        gl->glDeleteVertexArrays(1, &gltfMesh.VAO);
        gl->glDeleteBuffers(1, &gltfMesh.VBO);
        return false;
    }
    gl->glBufferData(GL_ARRAY_BUFFER, dataSize,
                     posBuffer.data.data() + dataOffset,
                     GL_STATIC_DRAW);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) nullptr);
    gl->glEnableVertexAttribArray(0);

    if (!primitive.targets.empty()) {
        const auto &target = primitive.targets[0];
        auto targetPosIt = target.find("POSITION");
        if (targetPosIt != target.end()) {
            const auto &targetPosAccessor = model.accessors[targetPosIt->second];
            const auto &targetPosView = model.bufferViews[targetPosAccessor.bufferView];
            const auto &targetPosBuffer = model.buffers[targetPosView.buffer];

            gl->glGenBuffers(1, &gltfMesh.morphVBO);
            gl->glBindBuffer(GL_ARRAY_BUFFER, gltfMesh.morphVBO);

            size_t targetPosOffset = targetPosView.byteOffset + targetPosAccessor.byteOffset;
            gl->glBufferData(GL_ARRAY_BUFFER, dataSize,
                             targetPosBuffer.data.data() + targetPosOffset,
                             GL_STATIC_DRAW);

            gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) nullptr);
            gl->glEnableVertexAttribArray(1);
        }
    }

    auto normIt = primitive.attributes.find("NORMAL");
    if (normIt != primitive.attributes.end()) {
        const auto &normAccessor = model.accessors[normIt->second];
        if (normAccessor.type == TINYGLTF_TYPE_VEC3 && normAccessor.bufferView >= 0) {
            const auto &normView = model.bufferViews[normAccessor.bufferView];
            const auto &normBuffer = model.buffers[normView.buffer];

            gl->glGenBuffers(1, &gltfMesh.normalVBO);
            gl->glBindBuffer(GL_ARRAY_BUFFER, gltfMesh.normalVBO);

            size_t normOffset = normView.byteOffset + normAccessor.byteOffset;
            size_t normSize = normAccessor.count * 3 * sizeof(float);
            gl->glBufferData(GL_ARRAY_BUFFER, normSize,
                             normBuffer.data.data() + normOffset,
                             GL_STATIC_DRAW);

            gl->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *) nullptr);
            gl->glEnableVertexAttribArray(2);
        }
    }

    if (!primitive.targets.empty()) {
        const auto &target = primitive.targets[0];
        auto targetNormIt = target.find("NORMAL");
        if (targetNormIt != target.end()) {
            const auto &targetNormAccessor = model.accessors[targetNormIt->second];
            if (targetNormAccessor.type == TINYGLTF_TYPE_VEC3 && targetNormAccessor.bufferView >= 0) {
                const auto &targetNormView = model.bufferViews[targetNormAccessor.bufferView];
                const auto &targetNormBuffer = model.buffers[targetNormView.buffer];

                gl->glGenBuffers(1, &gltfMesh.morphNormalVBO);
                gl->glBindBuffer(GL_ARRAY_BUFFER, gltfMesh.morphNormalVBO);

                size_t targetNormOffset = targetNormView.byteOffset + targetNormAccessor.byteOffset;
                size_t normSize = targetNormAccessor.count * 3 * sizeof(float);
                gl->glBufferData(GL_ARRAY_BUFFER, normSize,
                                 targetNormBuffer.data.data() + targetNormOffset,
                                 GL_STATIC_DRAW);

                gl->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *) nullptr);
                gl->glEnableVertexAttribArray(3);
            }
        }
    }

    if (primitive.indices >= 0) {
        const auto &indexAccessor = model.accessors[primitive.indices];
        if (indexAccessor.bufferView >= 0 && indexAccessor.bufferView < model.bufferViews.size()) {
            const auto &indexView = model.bufferViews[indexAccessor.bufferView];
            const auto &indexBuffer = model.buffers[indexView.buffer];
            gl->glGenBuffers(1, &gltfMesh.EBO);
            gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gltfMesh.EBO);
            size_t indexOffset = indexView.byteOffset + indexAccessor.byteOffset;
            if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                std::vector<unsigned short> shortIndices(indexAccessor.count);
                memcpy(shortIndices.data(), indexBuffer.data.data() + indexOffset,
                       indexAccessor.count * sizeof(unsigned short));
                std::vector<unsigned int> intIndices(indexAccessor.count);
                for (size_t i = 0; i < indexAccessor.count; ++i) {
                    intIndices[i] = static_cast<unsigned int>(shortIndices[i]);
                }
                gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, intIndices.size() * sizeof(unsigned int),
                                 intIndices.data(), GL_STATIC_DRAW);
            } else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                size_t indexSize = indexAccessor.count * sizeof(unsigned int);
                gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize,
                                 indexBuffer.data.data() + indexOffset, GL_STATIC_DRAW);
            }
            gltfMesh.hasIndices = true;
            gltfMesh.indexCount = indexAccessor.count;
        }
    }
    gltfMesh.vertexCount = posAccessor.count;
    gltfMesh.isInitialized = true;
    gl->glBindVertexArray(0);
    return true;
}

void MorphModel::processNode(QOpenGLExtraFunctions *gl, const tinygltf::Model &model,
                             const tinygltf::Node &node, const glm::mat4 &parentTransform) {
    glm::mat4 transform = parentTransform;
    if (!node.matrix.empty()) {
        std::vector<float> matrixFloat;
        for (double d : node.matrix) {
            matrixFloat.push_back((d));
        }
        transform *= glm::make_mat4(matrixFloat.data());
    } else {
        if (!node.translation.empty()) {
            glm::vec3 translation(
                    node.translation[0],
                    node.translation[1],
                    node.translation[2]
            );
            transform *= glm::translate(glm::mat4(1.0f), translation);
        }
        if (!node.rotation.empty()) {
            glm::quat q(
                    node.rotation[3], node.rotation[0],
                    node.rotation[1], node.rotation[2]);
            transform *= glm::mat4_cast(q);
        }
        if (!node.scale.empty()) {
            glm::vec3 scale(
                    node.scale[0],
                    node.scale[1],
                    node.scale[2]
            );
            transform *= glm::scale(glm::mat4(1.0f), scale);
        }
    }
    if (node.mesh >= 0) {
        const auto &mesh = model.meshes[node.mesh];
        for (const auto &primitive : mesh.primitives) {
            MorphMesh morphMesh;
            if (createMesh(gl, model, primitive, morphMesh)) {
                m_meshes.push_back(morphMesh);
            }
        }
    }
    for (int childId : node.children) {
        processNode(gl, model, model.nodes[childId], transform);
    }
}

void MorphModel::render(QOpenGLExtraFunctions *gl, unsigned int shaderProgram, float morphFactor) {
    if (!m_loaded || !m_glInitialized) return;
    if (m_meshes.empty()) return;
    int morphLoc = gl->glGetUniformLocation(shaderProgram, "morphFactor");
    if (morphLoc != -1) {
        gl->glUniform1f(morphLoc, morphFactor);
    }
    glm::mat4 modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(30.0f));
    int modelLoc = gl->glGetUniformLocation(shaderProgram, "model");
    if (modelLoc != -1) {
        gl->glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    }
    for (const auto &mesh : m_meshes) {
        if (!mesh.isInitialized || mesh.VAO == 0) continue;
        gl->glBindVertexArray(mesh.VAO);
        if (mesh.hasIndices) {
            gl->glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
        } else {
            gl->glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
        }
    }
    gl->glBindVertexArray(0);
}

void MorphModel::cleanup() {
    m_meshes.clear();
    m_loaded = false;
}

