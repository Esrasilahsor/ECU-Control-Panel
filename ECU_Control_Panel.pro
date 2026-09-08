QT += quick quickcontrols2

CONFIG += c++14

INCLUDEPATH += $$PWD/Headers
INCLUDEPATH += $$PWD/thirdparty/QXlsx/QXlsx/header

SOURCES += \
    Sources/main.cpp \
    Sources/connector.cpp \
    Sources/dispatcher.cpp \
    Sources/excelparser.cpp

HEADERS += \
    Headers/connector.h \
    Headers/dispatcher.h \
    Headers/excelparser.h

RESOURCES += \
    Resources/qml.qrc

include($$PWD/thirdparty/QXlsx/QXlsx/QXlsx.pri)