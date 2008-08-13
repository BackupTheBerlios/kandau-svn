#include "collectionmodel.h"

#include <collection.h>

CollectionModel::CollectionModel(QObject * parent)
    : QAbstractListModel(parent)
{
    m_collection = new Collection();
}

CollectionModel::~CollectionModel()
{
    if (m_collection)
        delete m_collection;
}

QVariant CollectionModel::data(const QModelIndex & index, int role) const
{
    return QString("hola");
}

int CollectionModel::rowCount (const QModelIndex & parent) const
{
    return m_collection->count();
}

bool CollectionModel::insertRows(int row, int count, const QModelIndex & parent)
{
    //beginInsertRows(QModelIndex(), row, row + count - 1);
    beginInsertRows(QModelIndex(), row, row + 1 - 1); // NOTE TEMP only insert one item

    m_collection->add(new Object());

    endInsertRows();

    return true;
}

bool CollectionModel::removeRows(int row, int count, const QModelIndex & parent)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);

    //remove(row);
    
    endRemoveRows();

    return true;
}

#include "collectionmodel.moc"
