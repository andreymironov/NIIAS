QT     += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
SOURCES += \
    Config.cpp \
    ErrorMessanger.cpp \
    FileReader.cpp \
    Model.cpp \
    Parser.cpp \
    Stage.cpp \
    View.cpp \
    Window.cpp \
    main.cpp
HEADERS += \
    Config.h \
    ErrorMessanger.h \
    FileReader.h \
    IConfig.h \
    IErrorMessanger.h \
    IFileReader.h \
    IModel.h \
    IParser.h \
    ISafe.h \
    IWindow.h \
    Model.h \
    Parser.h \
    Stage.h \
    Types.h \
    View.h \
    Window.h
FORMS += \
    Window.ui
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
SDIR = $$PWD
DDIR = $$OUT_PWD
QMAKE_POST_LINK += $$QMAKE_COPY $$shell_quote($$SDIR/config/sensors.conf) $$shell_quote($$DDIR)
