#ifndef RELATION_H
#define RELATION_H

#include <qmap.h>
#include <QVector>
#include <QList>

#include <QtCore/QString>

#include "defs.h"
#include "classes.h"

/*!
\brief This class contains information about a class' relation.

Stores the information of a relation to a class. Mainly stores the name
of the relation and a pointer to a function that allows the creation of
an object of the related class type.

The constructor is only called from the addObject function in ClassInfo which is called in the OBJECT macro in object.h

\see ClassInfo
*/
class RelationInfo
{
public:
    RelationInfo( const ClassInfo *classInfo, const QString& name, CreateObjectFunction function );
    const QString& name() const;
    bool isOneToOne() const;
    const ClassInfo* relatedClassInfo() const;
    const ClassInfo* parentClassInfo() const;
    bool browsable() const;
    bool inherited() const;

private:
    // This allows Classes to call setBrowsable & setOneToOne
    friend class Classes;

    void setOneToOne( bool oneToOne );
    void setBrowsable( bool browsable );
    void setInherited( bool inherited );

    QString m_name;
    bool m_oneToOne;
    bool m_browsable;
    bool m_inherited;

    // Pointer to the parent ClassInfo
    const ClassInfo *m_parentClassInfo;

    // Contains the cached ClassInfo of the related class
    const ClassInfo *m_relatedClassInfo;
};

#endif // RELATION_H
