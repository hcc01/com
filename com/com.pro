QT       += core gui serialport axcontainer sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
 CONFIG += static
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    autofillconfirmdialog.cpp \
    dbmanager.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    manager.cpp \
    mytablemodel.cpp \
    mytableview.cpp \
    openfiledialog.cpp \
    qexcel.cpp \
    testmanager.cpp \
    usermanager.cpp \
    weighui.cpp \
    widget.cpp

HEADERS += \
    autofillconfirmdialog.h \
    dbmanager.h \
    logindialog.h \
    mainwindow.h \
    manager.h \
    mytablemodel.h \
    mytableview.h \
    openfiledialog.h \
    qexcel.h \
    testmanager.h \
    usermanager.h \
    weighui.h \
    widget.h

FORMS += \
    autofillconfirmdialog.ui \
    logindialog.ui \
    mainwindow.ui \
    manager.ui \
    openfiledialog.ui \
    testmanager.ui \
    usermanager.ui \
    weighui.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



INCLUDEPATH += E:/QT/A/QExcel
DEPENDPATH += E:/QT/A/QExcel
CONFIG(debug,debug|release){
    DLLDESTDIR = "../../debug"
}else{
    DLLDESTDIR = "../../bin"
}

RESOURCES += \
    image.qrc
RC_ICONS = logo.ico
