#include "collectionview.h"

#include <classes.h>
#include <collection.h>

CollectionView::CollectionView( const ClassInfo *classInfo, QWidget *parent )
    : QListView(parent)
{
    //setAlternateBackground( QColor(40, 40, 40) );
    setClassInfo( classInfo );
    m_manager = 0;
}

void CollectionView::fill()
{
    //clear();
    while (  columns() > 0 )
        removeColumn( 0 );

    if ( ! m_classInfo )
        return;

    PropertiesInfoConstIterator it( m_classInfo->propertiesBegin() );
    PropertiesInfoConstIterator end( m_classInfo->propertiesEnd() );
    addColumn( "oid" );
    for ( ; it != end; ++it ) {
        addColumn( it.data()->name() );
    }
    Collection col( m_classInfo->name() );
    CollectionIterator it2( col.begin() );
    CollectionIterator end2( col.end() );
    for (; it2 != end2; ++it2 ) {
        add( it2.data() );
    }
}

void CollectionView::add( const Object* object )
{
    QString val;
    QListViewItem *item = new QListViewItem( this );
    item->setDropEnabled( true );
    item->setDragEnabled( true );
    item->setText( 0, oidToString( object->oid() ) );
    for ( int i = 1; i < columns(); ++i ) {
        const PropertyInfo *info = object->classInfo()->property( columnText( i ) );
        if ( info->isEnumType() ) {
            if ( info->isSetType() ) {
                val = info->valueToKeys( object->property( columnText( i ) ).value().toInt() ).join( ", " );
            } else {
                val = info->valueToKey( object->property( columnText( i ) ).value().toInt() );
            }
        } else {
            val = object->property( columnText( i ) ).value().toString();
        }
        item->setText( i, val );
    }
}

void CollectionView::remove( const OidType& oid )
{
    delete findItem( oidToString( oid ), 0 );
}

void CollectionView::setClassInfo( const ClassInfo *classInfo )
{
    m_classInfo = classInfo;

    while (  columns() > 0 )
        removeColumn( 0 );

    if ( m_classInfo ) {
        PropertiesInfoConstIterator it( m_classInfo->propertiesBegin() );
        PropertiesInfoConstIterator end( m_classInfo->propertiesEnd() );
        addColumn( "oid" );
        for ( ; it != end; ++it ) {
            addColumn( it.data()->name() );
        }
    }
}

const ClassInfo* CollectionView::classInfo() const
{
    return m_classInfo;
}

/*!
Obtains a pointer to the Object refered by the currently selected item.
*/
Object * CollectionView::currentObject() const
{
    if ( ! currentItem() )
        return 0;

    if ( m_manager )
        return m_classInfo->create( stringToOid( currentItem()->text( 0 ) ), m_manager );
    else
        return m_classInfo->create( stringToOid( currentItem()->text( 0 ) ) );
}

/*!
Obtains the Oid of the currently selected item.
*/
OidType CollectionView::currentOid() const
{
    if ( currentItem() )
        return stringToOid( currentItem()->text( 0 ) );
    else
        return 0;
}

QDragObject * CollectionView::dragObject()
{
    if ( ! currentItem() )
        return 0;

    QTextDrag *text = new QTextDrag();
    text->setText( oidToString( currentOid() ) );
    return text;
}

#include "collectionview.moc"
