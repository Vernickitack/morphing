//
// Created by veren on 18.01.2026.
//

#include "GLWidget.h"

GLWidget::GLWidget(QWidget *parent)
        : QOpenGLWidget(parent),
          m_camera(nullptr),
          m_wireframe(false),
          m_deltaTime(0.0f),
          m_lastFrame(0.0f) {

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    for (int i = 0; i < 1024; ++i)
        m_keys[i] = false;
}

GLWidget::~GLWidget() {
    makeCurrent();
    glDeleteProgram(m_shaderProgram);
    doneCurrent();
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    createShaders();
    loadTargets();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        float currentFrame = (float) QTime::currentTime().msecsSinceStartOfDay() / 1000.0f;
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        processInput(m_deltaTime);
        update();
    });
    m_timer->start(16);
}

QByteArray GLWidget::loadShaderFromResource(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QByteArray();
    }
    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    return content.toLocal8Bit();
}

void GLWidget::createShaders() {
    QByteArray vertexSource = loadShaderFromResource(":/shaders/vertex.glsl");
    QByteArray fragmentSource = loadShaderFromResource(":/shaders/fragment.glsl");

    if (vertexSource.isEmpty() || fragmentSource.isEmpty()) {
        qCritical() << "Didnt load shaders";
        return;
    }

    const char *vertexShaderSource = vertexSource.constData();
    const char *fragmentShaderSource = fragmentSource.constData();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void GLWidget::loadTargets() {
    makeCurrent();

    m_target1.cleanup();

    bool loadSuccess = m_target1.load("targets/AnimatedMorphSphere.gltf");
    if (loadSuccess) {
        m_target1.initializeGL(this);
    }
    doneCurrent();
    update();
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL() {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_camera) return;

    if (m_target1.isLoaded()) {
        glUseProgram(m_shaderProgram);

        glm::mat4 projection = m_camera->getProjectionMatrix((float) width() / (float) height());
        glm::mat4 view = m_camera->getViewMatrix();
        auto model = glm::mat4(1.0f);

        int projectionLoc = glGetUniformLocation(m_shaderProgram, "projection");
        int viewLoc = glGetUniformLocation(m_shaderProgram, "view");
        int modelLoc = glGetUniformLocation(m_shaderProgram, "model");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


        glUniform3f(glGetUniformLocation(m_shaderProgram, "material.ambient"),
                    m_materialAmbient.r, m_materialAmbient.g, m_materialAmbient.b);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "material.diffuse"),
                    m_materialDiffuse.r, m_materialDiffuse.g, m_materialDiffuse.b);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "material.specular"),
                    m_materialSpecular.r, m_materialSpecular.g, m_materialSpecular.b);
        glUniform1f(glGetUniformLocation(m_shaderProgram, "material.shininess"),
                    m_materialShininess);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "dirLight.direction"),
                    m_dirLight.direction.x, m_dirLight.direction.y, m_dirLight.direction.z);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "dirLight.ambient"),
                    m_dirLight.ambient.r, m_dirLight.ambient.g, m_dirLight.ambient.b);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "dirLight.diffuse"),
                    m_dirLight.diffuse.r, m_dirLight.diffuse.g, m_dirLight.diffuse.b);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "dirLight.specular"),
                    m_dirLight.specular.r, m_dirLight.specular.g, m_dirLight.specular.b);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "spotLight.position"),
                    m_spotLight.position.x, m_spotLight.position.y, m_spotLight.position.z);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "spotLight.direction"),
                    m_spotLight.direction.x, m_spotLight.direction.y, m_spotLight.direction.z);
        glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.cutOff"), m_spotLight.cutOff);
        glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.outerCutOff"), m_spotLight.outerCutOff);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "spotLight.ambient"),
                    m_spotLight.ambient.r, m_spotLight.ambient.g, m_spotLight.ambient.b);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "spotLight.diffuse"),
                    m_spotLight.diffuse.r, m_spotLight.diffuse.g, m_spotLight.diffuse.b);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "spotLight.specular"),
                    m_spotLight.specular.r, m_spotLight.specular.g, m_spotLight.specular.b);
        glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.constant"), m_spotLight.constant);
        glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.linear"), m_spotLight.linear);
        glUniform1f(glGetUniformLocation(m_shaderProgram, "spotLight.quadratic"), m_spotLight.quadratic);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "viewPos"),
                    m_camera->getPosition().x, m_camera->getPosition().y, m_camera->getPosition().z);

        m_target1.render(this, m_shaderProgram, m_morphFactor);
    }
}

void GLWidget::processInput(float deltaTime) {
    if (!m_camera) return;

    if (m_keys[Qt::Key_W]) m_camera->processKeyboard(Camera::FORWARD, deltaTime);
    if (m_keys[Qt::Key_S]) m_camera->processKeyboard(Camera::BACKWARD, deltaTime);
    if (m_keys[Qt::Key_A]) m_camera->processKeyboard(Camera::LEFT, deltaTime);
    if (m_keys[Qt::Key_D]) m_camera->processKeyboard(Camera::RIGHT, deltaTime);
    if (m_keys[Qt::Key_Space]) m_camera->processKeyboard(Camera::UP, deltaTime);
    if (m_keys[Qt::Key_Shift]) m_camera->processKeyboard(Camera::DOWN, deltaTime);
    if (m_keys[Qt::Key_Escape]) unlockCursor();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    m_keys[event->key()] = true;
}

void GLWidget::keyReleaseEvent(QKeyEvent *event) {
    m_keys[event->key()] = false;
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        if (!m_cursorLocked) {
            lockCursor();
        } else {
            unlockCursor();
        }
    }
    if (event->button() == Qt::LeftButton && !m_cursorLocked) {
        lockCursor();
    }
}

void GLWidget::lockCursor() {
    if (m_cursorLocked) return;
    QRect geo = geometry();
    m_centerPos = mapToGlobal(QPoint(geo.width() / 2, geo.height() / 2));
    setCursor(Qt::BlankCursor);
    QCursor::setPos(m_centerPos);
    grabMouse();
    m_cursorLocked = true;
}

void GLWidget::unlockCursor() {
    if (!m_cursorLocked) return;

    releaseMouse();
    setCursor(Qt::ArrowCursor);
    m_cursorLocked = false;
}

void GLWidget::updateCursorLock() {
    if (m_cursorLocked) {
        QCursor::setPos(m_centerPos);
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!m_camera) return;

    if (m_cursorLocked) {
        QPoint globalPos = event->globalPos();
        QPoint delta = globalPos - m_centerPos;

        if (delta.x() != 0 || delta.y() != 0) {
            m_camera->processMouseMovement((float) delta.x(), -(float)delta.y());
            updateCursorLock();
        }
    }
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    if (!m_camera) return;
    m_camera->processMouseScroll((float)event->angleDelta().y() / 120.0f);
}

void GLWidget::setWireframe(bool enabled) {
    m_wireframe = enabled;
    makeCurrent();
    glPolygonMode(GL_FRONT_AND_BACK, enabled ? GL_LINE : GL_FILL);
    doneCurrent();
    update();
}

void GLWidget::setMorphing(int value, int maximum) {
    m_morphFactor = (float) value / (float) maximum;
}

void GLWidget::setDirLightAngle(int angle) {
    float rad = glm::radians((float)angle);
    m_dirLight.direction = glm::vec3(-cos(rad), -1.0f, -sin(rad));
    update();
}

void GLWidget::setSpotlightIntensity(int percent) {
    float intensity = (float) percent / 100.0f;
    m_spotLight.diffuse = glm::vec3(intensity);
    m_spotLight.specular = glm::vec3(intensity);
    update();
}

void GLWidget::setShininess(int value) {
    m_materialShininess = (float) value;
    update();
}

void GLWidget::setAmbientLevel(int percent) {
    float level = (float) percent / 100.0f;
    m_dirLight.ambient = glm::vec3(level);
    m_spotLight.ambient = glm::vec3(level * 0.5f);
    update();
}
