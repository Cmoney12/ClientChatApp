#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "chat_message.hpp"
#include "ListViewDelegate.h"
#include <QWidget>
#include <QSplitter>
#include <QFormLayout>
#include <list>


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

    //************Username Components*****************

    username_view = new QListView;
    stringList = new StringList;
    username_view->setModel(stringList);
    left_widget->setLayout(leftlayout);
    rightwidget->setLayout(rightlayout);

    //******************************************

    connect_button = new QPushButton("Connect");
    //*******************Message_View***************************
    message_view = new QListView;
    message_view->setResizeMode(QListView::Adjust);
    message_view->setWordWrap(true);
    message_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    message_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    message_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    message_view->setModel(&standard_model);
    message_view->setMinimumSize(300,350);
    message_view->setItemDelegate(new ListViewDelegate());

    //*******************Message_View***************************
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

    //************************************************************

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

    username = data_handler->get_username();
    receiver = "";

}

void MainWindow::connection() {
    std::string messages = data_handler->load_messages();
    //QString message = QString::fromUtf8(messages.c_str());
    std::list<std::string> message_list = simple_tokenizer(messages);
    for(const auto& msg: message_list) {
        append_sent(QString::fromUtf8(msg.c_str()));
    }
    //message_view->append(message)

    //Connect to host
    socket->connectToHost("127.0.0.1", 1234);

    //send username to server
    QString username_message = QString::fromUtf8(username.c_str()) + "\n";
    socket->write(QString(username_message).toUtf8());
}

std::list<std::string> MainWindow::simple_tokenizer(const std::string& messages)
{
    std::stringstream ss(messages);
    std::string message;
    std::list<std::string> stringlist;
    if (!messages.empty())
    {
        while(std::getline(ss, message,'\n')) {
            stringlist.push_back(message);
        }
    }
    return stringlist;
}

void MainWindow::sendMessage() {

    chat_message msg;

    std::string body = (message_line->text()).toStdString();
    msg.create_tree(username, receiver.toStdString(), body);
    std::string json = msg.write_json();
    msg.body_length(json.length());
    std::memcpy(msg.body(), json.c_str(), msg.body_length());
    msg.encode_header();

    if(std::strlen(msg.data()) == 0) {
        return;
    } else {
        //socket->write(QString(message).toUtf8());
        socket->write((char *)msg.data(), msg.length());
        data_handler->insert_message(username, receiver.toStdString(), body);
    }

    append_sent(message_line->text());
    message_line->clear();
}

void MainWindow::append_sent(const QString& message) {
    //QString message = message_line->text();
    auto *item1 = new QStandardItem(message);
    item1->setData("Outgoing", Qt::UserRole + 1);
    standard_model.appendRow(item1);
}

void MainWindow::append_received(const QString& message) {
    auto *received_message = new QStandardItem(message);
    received_message->setData("Incoming", Qt::UserRole + 1);
    standard_model.appendRow(received_message);
}

void MainWindow::onReadyRead() {

    QString line;
    line = QString::fromUtf8(socket->readAll());
    line.remove(0,chat_message::HEADER_SIZE);
    //message_view->append(line);
    data_handler->insert_message(receiver.toStdString(), username, line.toStdString());
    append_received(line);
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
    //message_view->clear();
}

void MainWindow::add_user() const {
    stringList->append(search_user_line->text());
}

void MainWindow::set_recipient(QModelIndex index) {
    receiver = stringList->set_recipient(index);
    std::cout << receiver.toStdString() << std::endl;
    //std::cout << receiver << std::endl;
}


MainWindow::~MainWindow()
{
    delete stringList;
    delete data_handler;
    delete ui;
}

