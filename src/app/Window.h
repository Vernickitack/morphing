//
// Created by veren on 18.01.2026.
//

#ifndef MORPHING_WINDOW_H
#define MORPHING_WINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLayout>
#include <QSlider>
#include <QtWidgets/QFileDialog>
#include "../base/Camera.h"
#include "../base/GLWidget.h"

class Window : public QMainWindow {
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window() override;

    void updateMorphingLabel(int value);

private:
    QSlider *morphingSlider;
    QLabel *morphingLabel;
    GLWidget *m_glWidget;
    Camera *m_camera;
};

#endif //MORPHING_WINDOW_H
