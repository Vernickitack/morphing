//
// Created by veren on 18.01.2026.
//

#ifndef MORPHING_GLWIDGET_H
#define MORPHING_GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLExtraFunctions>
#include <QTimer>
#include <QKeyEvent>
#include <QFileDialog>
#include <QTime>
#include <gl/GL.h>
#include "Camera.h"
#include "MorphModel.h"
#include <map>

class GLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;

    void setWireframe(bool enabled);
    void setCamera(Camera* camera) { m_camera = camera; }
    void setMorphing(int value, int maximum);
    void setDirLightAngle(int angle);
    void setSpotlightIntensity(int percent);
    void setShininess(int value);
    void setAmbientLevel(int percent);
    void loadTargets();

    struct DirLight {
        glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        glm::vec3 ambient = glm::vec3(0.5f);
        glm::vec3 diffuse = glm::vec3(0.4f);
        glm::vec3 specular = glm::vec3(0.5f);
    } m_dirLight;

    struct SpotLight {
        glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f);
        glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
        float cutOff = glm::cos(glm::radians(12.5f));
        float outerCutOff = glm::cos(glm::radians(17.5f));
        glm::vec3 ambient = glm::vec3(0.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    } m_spotLight;

    glm::vec3 m_materialAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 m_materialDiffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    glm::vec3 m_materialSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_materialShininess = 32.0f;
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void lockCursor();
    void unlockCursor();
    void updateCursorLock();

    void processInput(float deltaTime);
    static QByteArray loadShaderFromResource(const QString& path);
    void createShaders();

    Camera* m_camera;
    QTimer* m_timer{};
    unsigned int m_shaderProgram{};

    MorphModel m_target1;
    float m_morphFactor = 0.0f;

    std::map<int, bool> m_keys;
    bool m_wireframe;

    float m_deltaTime;
    float m_lastFrame;

    bool m_cursorLocked = false;
    QPoint m_centerPos;
};

#endif //MORPHING_GLWIDGET_H
