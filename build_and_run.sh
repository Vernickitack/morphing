#!/bin/sh

rm -rf build
mkdir build
cd build || exit 1

QT5_PATH="C:\Qt\5.15.2\msvc2019_64"
Qt5_DIR="$QT5_PATH/lib/cmake/Qt5"

cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config release

cd release
"$QT5_PATH/bin/windeployqt" morphing.exe
./morphing.exe

sleep 5s