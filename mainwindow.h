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
#include <map>
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
    QPushButton *erase_user;
    QLineEdit *message_line;
    QMenu *option_menu;
    QListView *username_view;
    QLineEdit *search_user_line;
    QPushButton *user_button;
    QMenuBar *menu;
    database_handler *data_handler;
    StringList *stringList;
    std::string username;
    QStandardItemModel standard_model;
    QString receiver;
    void append_sent(const QString& message);
    void append_received(const QString& username, const QString& message);
    void receive_picture(const QString& user_name, const QString& img_data);
    static std::vector<std::pair<std::string, std::string>> simple_tokenizer(const std::string& s);


public slots:
    void connection();
    void sendMessage();
    void onReadyRead();
    void erase_all_messages();
    void add_user() const;
    void erase_user_messages();
    void send_picture();
    void logout();
    void ShowContextMenu(const QPoint& point) const;
    void set_recipient(QModelIndex index);

private slots:
    void copy_data() const;

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
};
#endif // MAINWINDOW_H
