#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QMenu>
#include <QListView>

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
    QLineEdit *message_line;
    QMenu *option_menu;
    QMenu *erase_messages;
    QListView *username_view;
    QLineEdit *search_user_line;
    QPushButton *user_button;
    QMenuBar *menu;

public slots:
    void sendMessage();
    //void dicsonnectFromServer();
    void connection();
    void onReadyRead();
    void erase_all_messages();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
};
#endif // MAINWINDOW_H
