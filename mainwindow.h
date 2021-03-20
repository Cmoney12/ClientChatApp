#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QMenu>
#include <iostream>
#include <QListView>
#include <QListWidget>
#include <QStandardItemModel>
#include "chat_message.hpp"
#include "StringList.h"
#include "database_handler.h"
#include "ListViewDelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public:
    QPushButton *connect_button;
    QListView *message_view;
    QPushButton *send_button;
    QPushButton *picture_button;
    QLineEdit *message_line;
    QMenu *option_menu;
    QMenu *erase_messages;
    QListView *username_view;
    QLineEdit *search_user_line;
    QPushButton *user_button;
    QMenuBar *menu;
    database_handler *data_handler;
    StringList *stringList;
    std::string username;
    QStandardItemModel standard_model;
    void append_sent(const QString& message);
    void append_received(const QString& message);


public slots:
    void connection();
    void sendMessage();
    void onReadyRead();
    void erase_all_messages() const;
    void add_user() const;
    void set_recipient(QModelIndex index) const;

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
};
#endif // MAINWINDOW_H
