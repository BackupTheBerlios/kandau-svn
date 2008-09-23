#ifndef PROPERTY_H
#define PROPERTY_H

#include <QVector>
#include <QtCore/QVariant>

class Object;

/*!
\brief This class contains information about a class' property.

The most obvious meta-information about a class' property are name and type. But it
also contains wheather it's read-only or read-write, and inherited or not.

\see ClassInfo
*/
class PropertyInfo
{
public:
    PropertyInfo();
    PropertyInfo( const QString& name, QVariant::Type type, bool readOnly, bool inherited );
    QVariant::Type type() const;
    const QString& name() const;
    bool readOnly() const;
    bool inherited() const;
    bool isSetType() const;
    bool isEnumType() const;
    QStringList enumKeys() const;

    void setType( QVariant::Type type );
    void setName( const QString& name );
    void setReadOnly( bool value );
    void setInherited( bool value );
    void setSetType( bool value );
    void setEnumType( bool value );
    void addKeyAndValue( const QString& key, int value );
    int keyToValue ( const QString& key ) const;
    QString valueToKey ( int value ) const;
    int keysToValue ( const QStringList& keys ) const;
    QStringList valueToKeys ( int value ) const;

private:
    class KeyAndValue
    {
    public:
        KeyAndValue() : m_value( 0 ) {}
        KeyAndValue( const QString& key, int value ) :
            m_key( key ),
            m_value( value )
            {}
        QString m_key;
        int m_value;
    };

    QString m_name;
    QVariant::Type m_type;
    bool m_readOnly;
    bool m_inherited;
    bool m_enumType;
    bool m_setType;
    QVector<KeyAndValue> m_enums;
};

/*!
\brief This class provides access to the value and data type of a property in an Object.

This class will be returned by the Object class and permits setting and obtaining
a property's value. You can also obtain the data type and weather it's read-only or
read-write.

To know about class properties without the need of creating an instance look at the
PropertyInfo class. Property is similar to PropertyInfo but also holds the value.

\see PropertyInfo, Object
*/
class Property
{
public:
    Property() {}
    Property( Object *obj, const QString& name );
    Property( const Object *obj, const QString& name );
    QVariant::Type type() const;
    QVariant value() const;
    void setValue( const QVariant& value );
    const QString& name() const;
    bool readOnly() const;
    const PropertyInfo* propertyInfo() const;

private:
    Object *m_object;
    const Object *m_constObject;
    QString m_name;
};

/*
typedef QMap<QString,PropertyInfo*> PropertiesInfo;
typedef QMapIterator<QString,PropertyInfo*> PropertiesInfoIterator;
typedef QMutableMapIterator<QString,PropertyInfo*> PropertiesInfoConstIterator;
*/

typedef QMap<QString,PropertyInfo*> PropertiesInfo;
typedef QMap<QString,PropertyInfo*>::iterator PropertiesInfoIterator;
typedef QMap<QString,PropertyInfo*>::const_iterator PropertiesInfoConstIterator;

/*!
\brief This class provides an iterator for the properties in an Object.

\see Property, Object
*/
class PropertiesIterator
{
public:
    PropertiesIterator( Object *object, PropertiesInfoConstIterator it );
    Property data();
    const Property data() const;
    PropertiesIterator& operator++();
    PropertiesIterator& operator--();
    PropertiesIterator operator++(int);
    PropertiesIterator operator--(int);
    bool operator==( const PropertiesIterator& it ) const;
    bool operator!=( const PropertiesIterator& it ) const;
    Property operator*();
    const Property operator*() const;
    PropertiesIterator& operator=(const PropertiesIterator& it);

private:
    Object *m_object;
    PropertiesInfoConstIterator m_it;
};

/*!
\brief This class provides a const iterator for the properties in an Object.

\see Property, Object
*/
class PropertiesConstIterator
{
public:
    PropertiesConstIterator( const Object *object, PropertiesInfoConstIterator it );
    const Property data() const;
    PropertiesConstIterator& operator++();
    PropertiesConstIterator& operator--();
    PropertiesConstIterator operator++(int);
    PropertiesConstIterator operator--(int);
    bool operator==( const PropertiesConstIterator& it ) const;
    bool operator!=( const PropertiesConstIterator& it ) const;
    const Property operator*() const;
    PropertiesConstIterator& operator=(const PropertiesConstIterator& it);

private:
    const Object *m_object;
    PropertiesInfoConstIterator m_it;
}; 

#endif // PROPERTY_H
