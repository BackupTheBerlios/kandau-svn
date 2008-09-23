#include "property.h"

#include <QStringList>
#include <kdebug.h>

#include "object.h"

/* PropertyInfo */

PropertyInfo::PropertyInfo()
{
    m_name = QString::null;
    m_type = QVariant::String;
    m_readOnly = false;
    m_inherited = false;
    m_enumType = false;
    m_setType = false;
}

PropertyInfo::PropertyInfo( const QString& name, QVariant::Type type, bool readOnly, bool inherited )
{
    m_name = name;
    m_type = type;
    m_readOnly = readOnly;
    m_inherited = inherited;
}

bool PropertyInfo::isSetType() const
{
    return m_setType;
}

bool PropertyInfo::isEnumType() const
{
    return m_enumType;
}

void PropertyInfo::setName( const QString& name )
{
    m_name = name;
}

void PropertyInfo::setReadOnly( bool value )
{
    m_readOnly = value;
}

void PropertyInfo::setInherited( bool value )
{
    m_inherited = value;
}

void PropertyInfo::setSetType( bool value )
{
    m_setType = value;
}

void PropertyInfo::setEnumType( bool value )
{
    m_enumType = value;
}

void PropertyInfo::setType( QVariant::Type type )
{
    m_type = type;
}

QVariant::Type PropertyInfo::type() const
{
    return m_type;
}

const QString& PropertyInfo::name() const
{
    return m_name;
}

bool PropertyInfo::readOnly() const
{
    return m_readOnly;
}

bool PropertyInfo::inherited() const
{
    return m_inherited;
}

void PropertyInfo::addKeyAndValue( const QString& key, int value )
{
    m_enums.append( KeyAndValue( key, value ) );
}

QStringList PropertyInfo::enumKeys() const
{
    QStringList ret;
    QVector<KeyAndValue>::const_iterator it( m_enums.constBegin() );
    QVector<KeyAndValue>::const_iterator end( m_enums.constEnd() );
    for ( ; it != end; ++it ) {
        ret.append( (*it).m_key );
    }
    return ret;
}

int PropertyInfo::keyToValue( const QString& key ) const
{
    QVector<KeyAndValue>::const_iterator it( m_enums.constBegin() );
    QVector<KeyAndValue>::const_iterator end( m_enums.constEnd() );
    for ( ; it != end; ++it ) {
        if ( (*it).m_key == key )
            return (*it).m_value;
    }
    return -1;
}

QString PropertyInfo::valueToKey( int value ) const
{
    QVector<KeyAndValue>::const_iterator it( m_enums.constBegin() );
    QVector<KeyAndValue>::const_iterator end( m_enums.constEnd() );
    for ( ; it != end; ++it ) {
        if ( (*it).m_value == value )
            return (*it).m_key;
    }
    return QString();
}

int PropertyInfo::keysToValue( const QStringList& keys ) const
{
    int ret = 0;
    QVector<KeyAndValue>::const_iterator it( m_enums.constBegin() );
    QVector<KeyAndValue>::const_iterator end( m_enums.constEnd() );
    for ( ; it != end; ++it ) {
        if ( keys.contains( (*it).m_key ) ) {
            kdDebug() << "Afegint: " << (*it).m_key << endl;
            ret |= (*it).m_value;
        }
    }

    return ret;
}

QStringList PropertyInfo::valueToKeys ( int value ) const
{
    QStringList ret;
    int i;
    int left;

    left = value;
    QVector<KeyAndValue>::const_iterator it( m_enums.constBegin() );
    QVector<KeyAndValue>::const_iterator end( m_enums.constEnd() );
    for ( ; it != end; ++it ) {
        i = (*it).m_value;
        if ( i != 0 &&  ( left & i ) == i || ( i == value ) ) {
            left = left & ~i;
            ret.append( (*it).m_key );
        }
    }
    return ret;
}

// Property

/*!
Constructor for Property. Typically used by Object only.
*/
Property::Property( Object *obj, const QString& name )
{
    m_object = obj;
    m_constObject = obj;
    m_name = name;
}

/*!
Constructor for Property. Typically used by const Object only.
*/
Property::Property( const Object *obj, const QString& name )
{
    m_object = 0;
    m_constObject = obj;
    m_name = name;
}

/*!
Obtains the data type of the property.
*/
QVariant::Type Property::type() const
{
    return m_constObject->classInfo()->property( m_name )->type();
}

/*!
Obtains the value of the property.
*/
QVariant Property::value() const
{
    return m_constObject->propertyValue( m_name.toAscii() );
}

/*!
Sets the value of the property.
*/
void Property::setValue( const QVariant& value )
{
    if ( m_object->setProperty( m_name.toAscii(), value ) )
        m_object->setModified( true );
}

/*!
Obtains the name of the property.
*/
const QString& Property::name() const
{
    return m_name;
}

/*!
Returns true if the property is read-only. False if it's read-write.
*/
bool Property::readOnly() const
{
    return m_constObject->classInfo()->property( m_name )->readOnly();
}

/*!
Returns a const pointer to the PropertyInfo associated with this property.
*/
const PropertyInfo* Property::propertyInfo() const
{
    return m_object->classInfo()->property( m_name );
}

// PropertiesIterator

/*!
Class constructor. Typically used Object class only.
\param object Object that holds the values of the properties to iterate.
\param it PropertiesInfoConstIterator that holds the properties info to iterate.
*/
PropertiesIterator::PropertiesIterator( Object *object, PropertiesInfoConstIterator it )
{
    m_object = object;
    m_it = it;
}

/*!
Obtains the property associated with the current iterator.
*/
Property PropertiesIterator::data()
{
    return Property( m_object, (*m_it)->name() );
}

/*!
Obtains the const property associated with the current iterator.
*/
const Property PropertiesIterator::data() const
{
    return Property( m_object, (*m_it)->name() );
}

/*!
Increase by one the current iterator position.
*/
PropertiesIterator& PropertiesIterator::operator++()
{
    m_it++;
    return *this;
}

/*!
Decrease by one the current iterator position.
*/
PropertiesIterator& PropertiesIterator::operator--()
{
    m_it--;
    return *this;
}

/*!
Increase by one the current iterator position.
*/
PropertiesIterator PropertiesIterator::operator++(int)
{
    PropertiesIterator tmp = *this;
    m_it++;
    return tmp;
}

/*!
Decrease by one the current iterator position.
*/
PropertiesIterator PropertiesIterator::operator--(int)
{
    PropertiesIterator tmp = *this;
    m_it--;
    return tmp;
}

/*!
Compares two PropertiesIterator. Returns true if they're the same, false otherwise.
*/
bool PropertiesIterator::operator==( const PropertiesIterator& it ) const
{
    return m_it == it.m_it && m_object == it.m_object;
}

/*!
Compares two PropertiesIterator. Returns true if they're different, false otherwise.
*/
bool PropertiesIterator::operator!=( const PropertiesIterator& it ) const
{
    return m_it != it.m_it || m_object != it.m_object;
}

/*!
Obtains the property of the current position of the iterator. If the
iterator is in an invalid position, the behaviour is undefined.
*/
Property PropertiesIterator::operator*()
{
    return Property( m_object, (*m_it)->name() );
}

/*!
Obtains the const property of the current position of the iterator. If the
iterator is in an invalid position, the behaviour is undefined.
*/
const Property PropertiesIterator::operator*() const
{
    return Property( m_object, (*m_it)->name() );
}

/*!
Establishes the position of the iterator to be the same as the it iterator.
*/
PropertiesIterator& PropertiesIterator::operator=(const PropertiesIterator& it)
{
    m_object = it.m_object;
    m_it = it.m_it;
    return *this;
}


// PropertiesConstIterator
/*!
Class constructor. Typically used Object class only.
\param object Object that holds the values of the properties to iterate.
\param it PropertiesInfoConstIterator that holds the properties info to iterate.
*/
PropertiesConstIterator::PropertiesConstIterator( const Object *object, PropertiesInfoConstIterator it )
{
    m_object = object;
    m_it = it;
}

/*!
Obtains the property associated with the current iterator.
*/
const Property PropertiesConstIterator::data() const
{
    return Property( m_object, (*m_it)->name() );
}

/*!
Increase by one the current iterator position.
*/
PropertiesConstIterator& PropertiesConstIterator::operator++()
{
    m_it++;
    return *this;
}

/*!
Decrease by one the current iterator position.
*/
PropertiesConstIterator& PropertiesConstIterator::operator--()
{
    m_it--;
    return *this;
}

/*!
Increase by one the current iterator position.
*/
PropertiesConstIterator PropertiesConstIterator::operator++(int)
{
    PropertiesConstIterator tmp = *this;
    m_it++;
    return tmp;
}

/*!
Decrease by one the current iterator position.
*/
PropertiesConstIterator PropertiesConstIterator::operator--(int)
{
    PropertiesConstIterator tmp = *this;
    m_it--;
    return tmp;
}

/*!
Compares two PropertiesIterator. Returns true if they're the same, false otherwise.
*/
bool PropertiesConstIterator::operator==( const PropertiesConstIterator& it ) const
{
    return m_it == it.m_it && m_object == it.m_object;
}

/*!
Compares two PropertiesIterator. Returns true if they're different, false otherwise.
*/
bool PropertiesConstIterator::operator!=( const PropertiesConstIterator& it ) const
{
    return m_it != it.m_it || m_object != it.m_object;
}

/*!
Obtains the property of the current position of the iterator. If the
iterator is in an invalid position, the behaviour is undefined.
*/
const Property PropertiesConstIterator::operator*() const
{
    return Property( m_object, (*m_it)->name() );
}

/*!
Establishes the position of the iterator to be the same as the it iterator.
*/
PropertiesConstIterator& PropertiesConstIterator::operator=(const PropertiesConstIterator& it)
{
    m_object = it.m_object;
    m_it = it.m_it;
    return *this;
}
 
