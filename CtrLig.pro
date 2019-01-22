QT       += core gui serialport
QT += multimedia
TEMPLATE = app
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
RESOURCES += \
    ctrlig.qrc

DISTFILES +=
# 软件版本
VERSION = 0.0.2

# 图标
RC_ICONS = icons/user5.ico

# 公司名称
QMAKE_TARGET_COMPANY = "JC"
QMAKE_TARGET_PRODUCT = "Qt Creator"
QMAKE_TARGET_DESCRIPTION = "Qt Creator based on Qt 5.7.0 (MSVC 2013, 32 bit)"

QMAKE_TARGET_COPYRIGHT = "Copyright 2008-2016 The Qt Company Ltd. All rights reserved."
RC_LANG = 0x0004

FORMS += \
    ctrlig.ui \
    seriaset.ui
#添加串口插件
DEPENDPATH += .
include(./src/qextserialport.pri)

SUBDIRS += \
    CtrLig.pro

HEADERS += \
    myhelper.h \
    seriaset.h \
    version.h \
    ctrlig.h \
    plot/qcustomplot.h

SOURCES += \
    plot/qcustomplot.cpp \
    ctrlig.cpp \
    main.cpp \
    seriaset.cpp
