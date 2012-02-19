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
    		ChatMessageProcessor.cpp \
    		ChatMessages.cpp

HEADERS +=	statswindow.h \
    		ChatMessageProcessor.h \
    		ChatMessages.h

FORMS    += statswindow.ui
