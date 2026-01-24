//
// Created by veren on 18.01.2026.
//

#include "Window.h"


Window::Window(QWidget *parent)
        : QMainWindow(parent) {

    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(2);
    mainLayout->setContentsMargins(2, 2, 2, 2);

    m_camera = new Camera();
    m_glWidget = new GLWidget(this);
    m_glWidget->setCamera(m_camera);
    mainLayout->addWidget(m_glWidget, 1);

    auto *controlsLayout = new QHBoxLayout();
    controlsLayout->setSpacing(4);
    controlsLayout->setContentsMargins(4, 2, 4, 2);

    auto *wireframeCheck = new QCheckBox("Wireframe Mode", this);
    wireframeCheck->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(wireframeCheck, &QCheckBox::toggled, m_glWidget, &GLWidget::setWireframe);
    controlsLayout->addWidget(wireframeCheck);

    auto *controlsLabel = new QLabel(
            "WASD: Move, Space/Shift: up/down\n Click/Esc to focus/unfocus cursor\n Scroll for zoom",
            this
    );
    controlsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    controlsLabel->setWordWrap(false);
    controlsLayout->addWidget(controlsLabel);

    auto *morphingTitle = new QLabel("Morphing:");
    morphingSlider = new QSlider(Qt::Horizontal);
    morphingSlider->setRange(0, 100);
    morphingSlider->setValue(0);
    morphingLabel = new QLabel("0");

    controlsLayout->addWidget(morphingTitle);
    controlsLayout->addWidget(morphingSlider);
    controlsLayout->addWidget(morphingLabel);

    controlsLayout->addSpacing(20);

    connect(morphingSlider, &QSlider::valueChanged, m_glWidget,
            [=](int value) { m_glWidget->setMorphing(value, morphingSlider->maximum()); });
    connect(morphingSlider, &QSlider::valueChanged, this, &Window::updateMorphingLabel);

    controlsLayout->addSpacing(20);

    auto *dirLightLabel = new QLabel("Directional angle:");
    auto *dirLightSlider = new QSlider(Qt::Horizontal);
    dirLightSlider->setRange(0, 360);
    dirLightSlider->setValue(45);
    controlsLayout->addWidget(dirLightLabel);
    controlsLayout->addWidget(dirLightSlider);
    connect(dirLightSlider, &QSlider::valueChanged, m_glWidget, &GLWidget::setDirLightAngle);

    controlsLayout->addSpacing(10);

    auto *spotIntensityLabel = new QLabel("Spotlight:");
    auto *spotIntensitySlider = new QSlider(Qt::Horizontal);
    spotIntensitySlider->setRange(0, 200);
    spotIntensitySlider->setValue(100);
    controlsLayout->addWidget(spotIntensityLabel);
    controlsLayout->addWidget(spotIntensitySlider);
    connect(spotIntensitySlider, &QSlider::valueChanged, m_glWidget, &GLWidget::setSpotlightIntensity);

    controlsLayout->addSpacing(10);

    auto *shininessLabel = new QLabel("Shininess:");
    auto *shininessSlider = new QSlider(Qt::Horizontal);
    shininessSlider->setRange(1, 128);
    shininessSlider->setValue(32);
    controlsLayout->addWidget(shininessLabel);
    controlsLayout->addWidget(shininessSlider);
    connect(shininessSlider, &QSlider::valueChanged, m_glWidget, &GLWidget::setShininess);

    controlsLayout->addSpacing(10);

    auto *ambientLabel = new QLabel("Ambient:");
    auto *ambientSlider = new QSlider(Qt::Horizontal);
    ambientSlider->setRange(0, 100);
    ambientSlider->setValue(40);
    controlsLayout->addWidget(ambientLabel);
    controlsLayout->addWidget(ambientSlider);
    connect(ambientSlider, &QSlider::valueChanged, m_glWidget, &GLWidget::setAmbientLevel);

    mainLayout->addLayout(controlsLayout, 0);

    setWindowTitle("Super amazing morphing");
    resize(1280, 720);
}

void Window::updateMorphingLabel(int value) {
    morphingLabel->setText(QString::number(value));
}

Window::~Window() {
    delete m_camera;
}