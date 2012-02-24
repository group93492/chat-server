#-------------------------------------------------
#
# Project created by QtCreator 2012-02-18T02:08:40
#
#-------------------------------------------------

QT       += core gui network

TARGET = chat-server-polygon
TEMPLATE = app


SOURCES += 	main.cpp\
        	statswindow.cpp \
    		ChatMessages.cpp \
    ChatServer.cpp

HEADERS +=	statswindow.h \
    		ChatMessages.h \
    ChatServer.h \
    tempchatmessage.h

FORMS    += statswindow.ui
