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
    QPushButton *record_button;
    QMenu *option_menu;
    QListView *username_view;
    QLineEdit *search_user_line;
    QPushButton *user_button;
    QMenuBar *menu;
    database_handler *data_handler;
    StringList *stringList;
    std::string username;
    QStandardItemModel standard_model;
    void append_sent(const QString& message);
    void append_received(const QString& username, const QString& message);
    void receive_picture(unsigned char* rec_image, std::size_t size);
    QString get_recipient() const;

public slots:
    bool connection();
    void sendMessage();
    void onReadyRead();
    void erase_all_messages();
    void add_user() const;
    void erase_user_messages();
    void send_picture();
    void logout();
    void show_context_menu(const QPoint& pos) const;
    void set_recipient();
    static unsigned int random_char();
    static std::string generate_hex(unsigned int len);


private slots:
    void copy_data() const;
    void save_image() const;

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
};
#endif // MAINWINDOW_H
