#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QMenu>
#include <QListView>
#include "database_handler.h"

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
    QTextEdit *message_view;
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

public slots:
    void connection();
    void sendMessage();
    void onReadyRead();
    void erase_all_messages() const;
    void send_picture();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
};
#endif // MAINWINDOW_H
