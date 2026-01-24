//
// Created by veren on 18.01.2026.
//

#include <QApplication>
#include <QSurfaceFormat>
#include "Window.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    Window window;
    window.show();

    return QApplication::exec();
}