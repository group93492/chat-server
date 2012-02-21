#ifndef CHATMESSAGEPROCESSOR_H
#define CHATMESSAGEPROCESSOR_H

#include <QObject>
#include <../chat-server-polygon/ChatMessages.h>
#include <QDataStream>

class ChatMessageSerializer: public QObject
{
    Q_OBJECT
private:

public:
    explicit ChatMessageSerializer(QObject *parent = 0);
    static ChatMessageBody *unpackMessage(QDataStream &msgStream);
    static void packMessage(QDataStream &msgStream, ChatMessageBody *msgBody);
signals:
//    void messageUnpacked(ChatMessageBody *msgBody);
//    void messagePacked(QDataStream &msgStream);
public slots:
//    void unpackMessage(QDataStream &messageStream);
//    void packMessage(ChatMessageBody *msgBody, ChatMessageType msgType);
};

#endif // CHATMESSAGEPROCESSOR_H
