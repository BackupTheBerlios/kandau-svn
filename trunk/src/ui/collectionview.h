#ifndef COLLECTIONLISTVIEW_H
#define COLLECTIONLISTVIEW_H

#include <QListView>

#include <object.h>

class ClassInfo;
class Manager;


class CollectionView : public QListView
{
    Q_OBJECT

    public:
        CollectionView(const ClassInfo *classInfo = 0, QWidget *parent = 0 );

        void setClassInfo( const ClassInfo *classInfo );
        const ClassInfo* classInfo() const;
        void fill();

        void setManager( Manager* manager );
        Manager* manager() const;

        void add( const Object* object );
        void remove( const OidType& oid );
        Object* currentObject() const;
        OidType currentOid() const;
        
    protected:
        //QDragObject* dragObject();

    private:
        const ClassInfo *m_classInfo;
        Manager *m_manager;
};

#endif
