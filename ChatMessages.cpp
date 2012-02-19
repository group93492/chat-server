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
}

AuthorizationRequest::AuthorizationRequest()
{
}

InformationalMessage::InformationalMessage()
{
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
