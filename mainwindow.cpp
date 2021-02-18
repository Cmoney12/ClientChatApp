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
#include <iostream>
#include <QSplitter>
#include "database_handler.h"
//<widget class="QWidget" name="centralwidget"/>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket)
{
    ui->setupUi(this);
    auto *splitter = new QSplitter(Qt::Horizontal);

    auto *rightwidget = new QWidget;
    auto *left_widget = new QWidget;
    auto *rightlayout = new QFormLayout;
    auto *leftlayout = new QFormLayout;

    menu = new QMenuBar;
    search_user_line = new QLineEdit;
    user_button = new QPushButton("Add");
    option_menu = new QMenu("Options");
    erase_messages = new QMenu("Erase Messages");
    option_menu->addAction(tr("Erase Messages"));
    menu->addMenu(option_menu);
    username_view = new QListView;
    left_widget->setLayout(leftlayout);
    rightwidget->setLayout(rightlayout);
    connect_button = new QPushButton("connect");
    message_view = new QTextEdit;
    send_button = new QPushButton("Send");
    message_line = new QLineEdit;
    leftlayout->addRow(user_button, search_user_line);
    leftlayout->addRow(username_view);
    rightlayout->addRow(connect_button);
    rightlayout->addRow(message_view);
    rightlayout->addRow(send_button, message_line);
    splitter->addWidget(left_widget);
    splitter->addWidget(rightwidget);
    setCentralWidget(splitter);
    setMenuBar(menu);
    //left_widget->setBaseSize(400,400);
    //splitter->setSizes(QList<int>() << 10 << 70);

    connect(option_menu, SIGNAL(triggered(QAction*)), SLOT(erase_all_messages()));
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
    while(socket->canReadLine()) {
        database_handler data_handler;
        // Here's the line the of text the server sent us (we use UTF-8 so
        // that non-English speakers can chat in their native language)
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        data_handler.insert_message(line.toStdString());
        message_view->append(line);
    }
}

void MainWindow::erase_all_messages() {
    std::cout << "clicked";
    database_handler data_handler;
    data_handler.clear_messages();
    message_view->clear();
}


MainWindow::~MainWindow()
{
    delete ui;
}

