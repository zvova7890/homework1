TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        QueueManager.cpp \
        TaskObject.cpp \
        TaskWorker.cpp \
        WorkersPool.cpp \
        main.cpp

HEADERS += \
    QueueManager.h \
    TaskObject.h \
    TaskWorker.h \
    WorkersPool.h
