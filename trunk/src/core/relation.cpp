#include "relation.h"

#include <assert.h>
#include "object.h"


/* RelationInfo */

/*!
RelationInfo constructor. Note that the relation is browsable by default.
@param classInfo A pointer to the ClassInfo object the collection is in.
@param name The name of the relation (not the class of the related object)
@param function The function which creates an object of the type of the related one
*/
RelationInfo::RelationInfo( const ClassInfo *classInfo, const QString& name, CreateObjectFunction function )
{
    assert( classInfo );
    assert( function );
    m_parentClassInfo = classInfo;
    m_name = name;
    m_browsable = true;
    Object *obj = function();
    assert( obj );
    m_relatedClassInfo = obj->classInfo();
    // TODO: We don't check if the type of the collection is of the same type of our class. Maybe this check could be added when compiled with the DEBUG flag.
    if ( m_relatedClassInfo->containsObject( m_name ) ) {
        m_oneToOne = true;
    } else {
        m_oneToOne = false;
    }
    delete obj;
}

/*!
Get the name of the relation
@return The name of the relation
*/
const QString& RelationInfo::name() const
{
    return m_name;
}

/*!
Get whether the relation is One to One or One to N.
@return True if the relation is One to One, false if it's One to N.
*/
bool RelationInfo::isOneToOne() const
{
    return m_oneToOne;
}

/*!
Gets the ClassInfo of the class of the objects related.
@return The ClassInfo pointer of the class.
*/
const ClassInfo* RelationInfo::relatedClassInfo() const
{
    return m_relatedClassInfo;
}

/*!
Gets the ClassInfo of the parent class
@return The ClassInfo pointer of the parent class
*/
const ClassInfo* RelationInfo::parentClassInfo() const
{
    return m_parentClassInfo;
}

/*!
Obtains whether the relation was designed to be browsable. That is, there is a
declaration in the the definition of the class parentClassInfo() that points to
relatedClassInfo(). Otherwise, the relation exists only in the declaration from
relatedClassInfo().
@return True if it was designed to be browsable, false otherwise.
*/
bool RelationInfo::browsable() const
{
    return m_browsable;
}

/*!
Establishes whether the relation is 1-1 or 1-N.
@param oneToOne True if the relation is 1-1, false if it's 1-N
*/
void RelationInfo::setOneToOne( bool oneToOne )
{
    m_oneToOne = oneToOne;
}

/*!
Establishes whether the relation is browsable or not.
@param browsable True if the relation is browsable, false otherwise.
*/
void RelationInfo::setBrowsable( bool browsable )
{
    m_browsable = browsable;
}

/*!
Obtains whether the relation has been inherited or not.
@return True if the relation has been inherited, false otherwise.
*/
bool RelationInfo::inherited() const
{
    return m_inherited;
}

/*!
Establishes whether the relation has been inherited or not.
@param inherited True if the relation has been inherited, false otherwise.
*/
void RelationInfo::setInherited( bool inherited )
{
    m_inherited = inherited;
}
 
