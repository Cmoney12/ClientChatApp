#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "chat_message.hpp"
#include <QFormLayout>
#include <QTcpSocket>
#include <QWidget>
#include <QHostAddress>
#include <QString>
#include <QThread>
#include <QStringLiteral>
#include <QSplitter>
#include <QPixmap>
#include <QIcon>
#include <QHBoxLayout>
#include <QStringListModel>


//you can use model->setData() method
//
//Or you can use model->setStringList(listePays) after listePays.append() method



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
    stringList = new StringList;
    username_view->setModel(stringList);

    left_widget->setLayout(leftlayout);
    rightwidget->setLayout(rightlayout);

    connect_button = new QPushButton("Connect");
    message_view = new QTextEdit;
    send_button = new QPushButton("Send");
    message_line = new QLineEdit;

    QPixmap pixmap("/home/corey/CLionProjects/ClientChatApp/picture_button.png");
    QIcon ButtonIcon(pixmap);
    picture_button = new QPushButton();
    picture_button->setIcon(ButtonIcon);
    leftlayout->addRow(user_button, search_user_line);
    leftlayout->addRow(username_view);
    rightlayout->addRow(connect_button);
    rightlayout->addRow(message_view);

    //**************************

    auto *hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(send_button);
    hBoxLayout->addWidget(picture_button);
    auto *container = new QWidget;
    container->setLayout(hBoxLayout);

    rightlayout->addRow(container, message_line);
    splitter->addWidget(left_widget);
    splitter->addWidget(rightwidget);
    setCentralWidget(splitter);
    setMenuBar(menu);

    auto current_directory = QCoreApplication::applicationDirPath();
    data_handler = new database_handler(current_directory.toStdString());

    connect(connect_button, &QPushButton::clicked, this, &MainWindow::connection);
    connect(option_menu, SIGNAL(triggered(QAction*)), SLOT(erase_all_messages()));
    connect(send_button, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(message_line, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(user_button, &QPushButton::clicked, this, &MainWindow::add_user);
    connect(username_view, SIGNAL(clicked(QModelIndex)), this, SLOT(set_recipient(QModelIndex)));

    QString header;

}

void MainWindow::connection() {
    std::string messages = data_handler->load_messages();
    QString message = QString::fromUtf8(messages.c_str());
    message_view->append(message);
    socket->connectToHost("127.0.0.1", 1234);
}

void MainWindow::sendMessage() {

    chat_message msg;

    std::string message = (message_line->text()).toStdString();
    std::size_t len = message.length();
    msg.body_length(len);
    std::memcpy(msg.body(), message.c_str(), msg.body_length());
    msg.encode_header();
    if(message.empty()) {
        return;
    } else {
        //socket->write(QString(message).toUtf8());
        socket->write((char *)msg.data(), msg.length());
    }
    message_line->clear();
}


void MainWindow::onReadyRead()
{
    chat_message msg;
    // We'll loop over every (complete) line of text that the server has sent us:
    QString line;
    line = QString::fromUtf8(socket->readAll());
    line.remove(0,4);
    message_view->append(line);
    data_handler->insert_message(line.toStdString());
    //while(socket->canReadLine()) {
        // Here's the line the of text the server sent us (we use UTF-8 so
        //QString line = QString::fromUtf8(socket->readLine()).trimmed();
        //line += QString::fromUtf8(socket->readLine());
        //std::cout << line.toStdString();

    //}
    //line.remove(0,3);

}

void MainWindow::erase_all_messages() const {
    data_handler->clear_messages();
    message_view->clear();
}

void MainWindow::add_user() {
    stringList->append(search_user_line->text());
}

void MainWindow::set_recipient(QModelIndex index) {
    receiver = stringList->set_recipient(index);
    std::cout << receiver.toStdString() << std::endl;
}


MainWindow::~MainWindow()
{
    delete stringList;
    delete data_handler;
    delete ui;
}

