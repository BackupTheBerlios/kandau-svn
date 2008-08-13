#ifndef COLLECTION_MODEL_H
#define COLLECTION_MODEL_H

#include <QAbstractListModel>

class Collection;

class CollectionModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        CollectionModel(QObject * parent = 0);
        ~CollectionModel();

        QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

        int rowCount (const QModelIndex & parent) const;

        bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
        bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

        set

    private:
        Collection * m_collection;
};

#endif // COLLECTION_MODEL_H
