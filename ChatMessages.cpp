#include "ChatMessages.h"
#include <QDataStream>
#include <QMetaObject>
#include <QMetaProperty>

bool Serializable::pack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    for(int propIndex = this->metaObject()->propertyOffset(); propIndex < this->metaObject()->propertyCount(); ++propIndex)
    {
        QMetaProperty prop = this->metaObject()->property(propIndex);
        stream << this->property(prop.name());
    }
    return true;
}

bool Serializable::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    for(int propIndex = this->metaObject()->propertyOffset(); propIndex < this->metaObject()->propertyCount(); ++propIndex)
    {
        QMetaProperty prop = this->metaObject()->property(propIndex);
        QVariant value;
        stream >> value;
        this->setProperty(prop.name(), value);
    }
    return true;
}

Serializable::Serializable()
{
}

quint8 ChatMessageHeader::getMessageType()
{
    return m_messageType;
}

void ChatMessageHeader::setMessageType(const quint8 type)
{
    m_messageType = type;
}

quint32 ChatMessageHeader::getMessageSize()
{
    return m_messageSize;
}

void ChatMessageHeader::setMessageSize(const quint32 size)
{
    m_messageSize = size;
}

ChatMessageHeader::ChatMessageHeader()
{
}

quint8 ChatMessageBody::getMessageType()
{
    return m_messageType;
}

ChatMessageBody::ChatMessageBody()
{
}

bool AuthorizationAnswer::getAuthorizationResult()
{
    return m_authorizationResult;
}

void AuthorizationAnswer::setAuthorizationResult(const bool result)
{
    m_authorizationResult = result;
}

QString &AuthorizationAnswer::getDenialReason()
{
    return m_denialReason;
}

void AuthorizationAnswer::setDenialReason(const QString &reason)
{
    m_denialReason = reason;
}

AuthorizationAnswer::AuthorizationAnswer()
{
    m_messageType = cmtAuthorizationAnswer;
}

AuthorizationRequest::AuthorizationRequest()
{
    m_messageType = cmtAuthorizationRequest;
}

QString &AuthorizationRequest::getUsername()
{
    return m_username;
}

void AuthorizationRequest::setUsername(const QString &un)
{
    m_username = un;
}

QString &AuthorizationRequest::getPassword()
{
    return m_password;
}

void AuthorizationRequest::setPassword(const QString &pw)
{
    m_password = pw;
}

ChannelMessage::ChannelMessage()
{
    m_messageType = cmtChannelMessage;
}

QString &ChannelMessage::getSender()
{
    return m_sender;
}

void ChannelMessage::setSender(const QString &sender)
{
    m_sender = sender;
}

QString &ChannelMessage::getReceiver()
{
    return m_receiver;
}

void ChannelMessage::setReceiver(const QString &receiver)
{
    m_receiver = receiver;
}

QString &ChannelMessage::getMessageText()
{
    return m_messageText;
}

void ChannelMessage::setMessageText(const QString &text)
{
    m_messageText = text;
}


