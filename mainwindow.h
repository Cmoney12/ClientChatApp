#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    QPushButton *connect_button;
    QTextEdit *message_view;
    QPushButton *send_button;
    QLineEdit *message_line;

public slots:
    void messageRecieved(const QString &message);
    void sendMessage();
    //void dicsonnectFromServer();
    void connection();
    void onReadyRead();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
};
#endif // MAINWINDOW_H
