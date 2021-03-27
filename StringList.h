//
// Created by corey on 3/15/21.
//

#ifndef CLIENTCHATAPP_STRINGLIST_H
#define CLIENTCHATAPP_STRINGLIST_H

#include <QStringListModel>

class StringList : public QStringListModel
{
public:
    void append (const QString& string){
        insertRows(rowCount(), 1);
        setData(index(rowCount()-1), string);
    }

    QString set_recipient(QModelIndex index) {
        QString string = stringList().at(index.row());
        return string;
    }

    void delete_user(const QModelIndex& index) {
        if (!index.isValid() || index.row() >= stringList().size())
            return;
        removeRows(index.row(), 1);
    }

    StringList& operator<<(const QString& string){
        append(string);
        return *this;
    }
};

#endif //CLIENTCHATAPP_STRINGLIST_H
