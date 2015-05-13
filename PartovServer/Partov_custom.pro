# Custom.....

# Partov
TARGET = partov

INCLUDEPATH += src

CONFIG += debug warn_on thread console
QT += network xml xmlpatterns sql
QT -= gui

QMAKE_CXXFLAGS += -rdynamic -fPIC
QMAKE_LFLAGS += -rdynamic -fPIC

DESTDIR = deploy/bin

MOC_DIR = tmp_files
OBJECTS_DIR = tmp_files

unix {
  LIBS += -lpcap
}

win32 {
  DEPENDPATH += ../../win32-dep/include \
                ../../win32-dep/lib
  LIBS += ../../win32-dep/lib/libwpcap.a -lWs2_32
  INCLUDEPATH += ../../win32-dep/include
}

