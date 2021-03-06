#-------------------------------------------------
#
# Project created by QtCreator 2011-05-11T11:13:39
# Tomasz 'sierdzio' Siekierda
# sierdzio@gmail.com
#-------------------------------------------------
include(../buildInfo.pri)

TARGET   = QWebService

TEMPLATE = lib

CONFIG   += dll

DEFINES  += QWEBSERVICE_LIBRARY

VERSION  = 0.6.0

DESTDIR  = ../lib
OBJECTS_DIR = $${BUILD_DIRECTORY}/QWebService
MOC_DIR = $${BUILD_DIRECTORY}/QWebService

SOURCES  += \
    sources/qwebmethod.cpp \
    sources/qwebservicemethod.cpp \
    sources/qwsdl.cpp \
    sources/qwebservice.cpp \

HEADERS  += headers/QWebService_global.h \
    headers/QWebService \
    headers/qwebmethod.h \
    headers/qwebservicemethod.h \
    headers/qwsdl.h \
    headers/qwebservice.h \
    headers/qwebmethod_p.h \
    headers/qwebservicemethod_p.h \
    headers/qwebservice_p.h \
    headers/qwsdl_p.h \
    headers/QtWebServiceQml.h

symbian {
    #Symbian specific definitions
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE45E3457
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = QWebService.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
    }
    INSTALLS += target
}









