#include <QApplication>

#include <QListView>
#include <QStringListModel>

#include <collectionmodel.h>

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    QListView * lv = new QListView();
    CollectionModel * cmodel = new CollectionModel(lv);

    lv->setModel(cmodel);

    lv->show();

    return app.exec();
}
