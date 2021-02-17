#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFormLayout>
#include <QTcpSocket>
#include <QWidget>
#include <QHostAddress>
#include <QString>
#include <QThread>
#include <QStandardItemModel>
#include <QStringLiteral>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket)
{
    ui->setupUi(this);
    QWidget *widget = new QWidget;
    QFormLayout *layout = new QFormLayout;
    widget->setLayout(layout);
    connect_button = new QPushButton("connect");
    message_view = new QTextEdit;
    send_button = new QPushButton("Send");
    message_line = new QLineEdit;
    layout->addWidget(connect_button);
    layout->addWidget(message_view);
    layout->addRow(send_button, message_line);
    setCentralWidget(widget);
    connect(connect_button, &QPushButton::clicked, this, &MainWindow::connection);
    connect(send_button, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(message_line, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}



void MainWindow::connection() {

    socket->connectToHost("127.0.0.1", 1234);
}

void MainWindow::sendMessage() {
    QString message = message_line->text() + "\n";
    if(message.isEmpty()) {
        return;
    } else {
        QDataStream clientStream(socket);
        clientStream << message;
        message_line->clear();
    }
}

void MainWindow::onReadyRead()
{
    // We'll loop over every (complete) line of text that the server has sent us:
    while(socket->canReadLine())
    {
        // Here's the line the of text the server sent us (we use UTF-8 so
        // that non-English speakers can chat in their native language)
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        message_view->append(line);
    }
}


void MainWindow::messageRecieved(const QString &message) {
    message_view->append(message);

}


MainWindow::~MainWindow()
{
    delete ui;
}

