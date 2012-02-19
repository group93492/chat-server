#ifndef CHATMESSAGEPROCESSOR_H
#define CHATMESSAGEPROCESSOR_H

#include <QObject>
#include <ChatMessages.h>
#include <QDataStream>

class ChatMessageProcessor : public QObject
{
    Q_OBJECT
private:
    ChatMessageHeader &getMessageHeader(QDataStream &msg);
public:
    explicit ChatMessageProcessor(QObject *parent = 0);
    
signals:
    void messageProcessed(ChatMessage &msg);
public slots:
    void processMessage(QDataStream &messageStream);
};

#endif // CHATMESSAGEPROCESSOR_H
