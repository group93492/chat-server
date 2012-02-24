#include "ChatMessages.h"
#include <QDataStream>
#include <QMetaObject>
#include <QMetaProperty>

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

bool ChatMessageHeader::pack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream << m_messageType;
    return true;
}

bool ChatMessageHeader::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> m_messageType;
    return true;
}

quint8 ChatMessageBody::getMessageType()
{
    return m_messageType;
}

bool ChatMessageBody::pack(QDataStream &stream)
{
}

bool ChatMessageBody::unpack(QDataStream &stream)
{
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

bool AuthorizationAnswer::pack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    if(getAuthorizationResult())
        stream << m_authorizationResult;
    else
        stream << m_authorizationResult << m_denialReason;
    return true;
}

bool AuthorizationAnswer::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> m_authorizationResult;
    if(!m_authorizationResult)
        stream >> m_denialReason;
    return true;
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

bool AuthorizationRequest::pack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream << m_username << m_password;
    return true;
}

bool AuthorizationRequest::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> m_username >> m_password;
    return true;
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

bool ChannelMessage::pack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream << m_sender << m_receiver << m_messageText;
    return true;
}

bool ChannelMessage::unpack(QDataStream &stream)
{
    if (stream.status() != QDataStream::Ok)
        return false;
    stream >> m_sender >> m_receiver >> m_messageText;
    return true;
}
