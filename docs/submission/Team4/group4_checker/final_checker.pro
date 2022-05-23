QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ask_join_room.cpp \
    ask_server_or_client.cpp \
    board.cpp \
    coord.cpp \
    cp-networkdata.cpp \
    cp-networkserver.cpp \
    cp-networksocket.cpp \
    hello.cpp \
    main.cpp \
    qwhtimimg.cpp \
    room.cpp \
    showinfo.cpp \
    user.cpp

HEADERS += \
    ask_join_room.h \
    ask_server_or_client.h \
    board.h \
    coord.h \
    cp-networkdata.h \
    cp-networkserver.h \
    cp-networksocket.h \
    hello.h \
    predefine.h \
    qwhtimimg.h \
    room.h \
    showinfo.h \
    user.h

FORMS += \
    ask_join_room.ui \
    ask_server_or_client.ui \
    hello.ui \
    showinfo.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
