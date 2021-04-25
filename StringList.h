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

        QString username;
        if (!stringList().empty() && stringList().size() >= index.row())
            username = stringList().at(index.row());

        return username;
    }

    bool username_in_view(const QString& username) {
        for(int i = 0; i < this->rowCount(); i++) {
            if (stringList().at(i) == username) {
                return true;
            }
        }
        return false;
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
