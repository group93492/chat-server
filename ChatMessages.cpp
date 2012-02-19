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

AuthorizationAnswer::AuthorizationAnswer()
{
    messageType = (quint8) cmtAuthorizationAnswer;
}

InformationalMessage::InformationalMessage()
{
    messageType = (quint8) cmtInformationalMessage;
}

ChatMessageHeader::ChatMessageHeader()
{
}

Serializable::Serializable()
{
}

ChatMessageBody::ChatMessageBody()
{
}

AuthorizationRequest::AuthorizationRequest()
{
    messageType = (quint8) cmtAuthorizationRequest;
}
