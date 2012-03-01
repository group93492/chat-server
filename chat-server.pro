#-------------------------------------------------
#
# Project created by QtCreator 2012-02-18T02:08:40
#
#-------------------------------------------------

QT       += core gui network

TARGET = chat-server
TEMPLATE = app


SOURCES += 	main.cpp\
        	statswindow.cpp \
    		ChatMessages.cpp \
            ChatServer.cpp \
    ClientList.cpp

HEADERS +=	statswindow.h \
    		ChatMessages.h \
            ChatServer.h \
    ClientList.h

FORMS    += statswindow.ui
