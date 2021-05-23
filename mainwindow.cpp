#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "chat_message.hpp"
#include "ListViewDelegate.h"
#include "login.h"
#include <QWidget>
#include <QSplitter>
#include <QFormLayout>
#include <list>
#include <QFileDialog>
#include <QAction>
#include <QClipboard>
#include <random>
#include <string>
#include <sstream>

typedef unsigned char uint8_t;


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

    std::string dir = QDir::currentPath().toStdString();

    std::size_t size = dir.find_last_of("/\\");
    dir = dir.substr(0,size);

    menu = new QMenuBar;
    search_user_line = new QLineEdit;
    user_button = new QPushButton("+");
    erase_user = new QPushButton("-");
    option_menu = new QMenu("Options");
    //connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));
    QAction *logout_ = option_menu->addAction("Logout");
    QAction *erase_messages_ = option_menu->addAction("Erase Messages");
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
    message_view->setMinimumSize(400,450);
    message_view->setItemDelegate(new ListViewDelegate());

    //*******************Message_View***************************

    send_button = new QPushButton("Send");
    message_line = new QLineEdit;

    std::string logo_path = dir + "/resources/picture_button.png";

    QPixmap pixmap(QString::fromUtf8(logo_path.c_str()));
    QIcon ButtonIcon(pixmap);
    picture_button = new QPushButton();
    picture_button->setIcon(ButtonIcon);

    std::string picture_logo = dir + "/resources/record_button.png";
    QPixmap record_map(QString::fromUtf8(picture_logo.c_str()));
    QIcon record_icon(record_map);
    record_button = new QPushButton;
    record_button->setIcon(record_icon);

    leftlayout->addRow(search_user_line, user_button);
    leftlayout->addWidget(erase_user);
    leftlayout->addRow(username_view);
    rightlayout->addRow(connect_button);
    rightlayout->addRow(message_view);

    //QString path = QCoreApplication::applicationDirPath();

    //************************************************************

    auto *gridlayout = new QGridLayout;

    gridlayout->addWidget(picture_button,0,0,1,1);
    gridlayout->addWidget(record_button, 0,1,1,1);
    auto *media_buttons_container = new QWidget;
    media_buttons_container->setLayout(gridlayout);

    rightlayout->addRow(send_button, message_line);
    rightlayout->addRow(media_buttons_container);
    splitter->addWidget(left_widget);
    splitter->addWidget(rightwidget);
    setCentralWidget(splitter);
    setMenuBar(menu);

    //auto current_directory = QCoreApplication::applicationDirPath();
    std::string db_directory = dir + "/resources/messanger_db.sqlite";

    data_handler = new database_handler(db_directory);

    message_view->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(message_view, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(show_context_menu(const QPoint &)));

    connect(connect_button, &QPushButton::clicked, this, &MainWindow::connection);
    connect(logout_, SIGNAL(triggered()), this, SLOT(logout()));
    connect(erase_messages_, SIGNAL(triggered()), this, SLOT(erase_all_messages()));
    connect(send_button, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(message_line, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(user_button, &QPushButton::clicked, this, &MainWindow::add_user);
    connect(username_view, SIGNAL(clicked(QModelIndex)), this, SLOT(set_recipient()));
    connect(erase_user, &QPushButton::clicked, this, &MainWindow::erase_user_messages);
    connect(picture_button, &QPushButton::clicked, this, &MainWindow::send_picture);

    username = data_handler->get_username();

}

void MainWindow::show_context_menu(const QPoint& pos) const {

    QPoint globalPos = message_view->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Copy", this, SLOT(copy_data()));
    myMenu.addAction("Save_img", this, SLOT(save_image()));



    // Show context menu at handling position
    myMenu.exec(globalPos);

}

void MainWindow::save_image() {
    QModelIndex index = message_view->currentIndex();
    standard_model.data(index);

    QStandardItem *item = standard_model.itemFromIndex(index);

    if (index.data(Qt::UserRole + 1) == "Picture") {

        //Load base 64 string from index
        QByteArray pic_array;
        pic_array.append(item->text());

        QPixmap image;
        image.loadFromData(QByteArray::fromBase64(pic_array));

        QString uuid_filename = QString::fromUtf8(generate_hex(7).c_str()) + ".png";
        std::cout << uuid_filename.toStdString() << std::endl;

        QFile file(uuid_filename);
        file.open(QIODevice::WriteOnly);
        image.save(&file, "PNG");
    }
}

unsigned int MainWindow::random_char() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return dis(gen);
}

std::string MainWindow::generate_hex(unsigned int len) {
    std::stringstream ss;
    for (auto i = 0; i < len; i++) {
        const auto rc = random_char();
        std::stringstream hexstream;
        hexstream << std::hex << rc;
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}


void MainWindow::copy_data() const {

    QModelIndex index = message_view->currentIndex();
    standard_model.data(index);
    QClipboard *clipboard = QApplication::clipboard();

    QStandardItem *item = standard_model.itemFromIndex(index);

    if (index.data(Qt::UserRole + 1) == "Picture") {

        QByteArray pic_array;
        pic_array.append(item->text());

        QPixmap image;
        image.loadFromData(QByteArray::fromBase64(pic_array));

        clipboard->setPixmap(image, QClipboard::Clipboard);
    }

    else {
        clipboard->setText(item->text());
    }

}

void MainWindow::connection() {
    //retrieve and parse messages from db
    std::string messages = data_handler->load_messages();
    /**std::vector<std::pair<std::string, std::string>> message_list = simple_tokenizer(messages);
    for(const auto& msg: message_list) {
        if (msg.first == "user")
            append_received(QString::fromUtf8(msg.first.c_str()), QString::fromUtf8(msg.second.c_str()));
        else
            append_sent(QString::fromUtf8(msg.second.c_str()));
    }**/

    //Connect to host
    socket->connectToHost("127.0.0.1", 1234);

    //Send Username to Server
    QString username_message = QString::fromUtf8(username.c_str()) + "\n";
    socket->write(QString(username_message).toUtf8());
}


void MainWindow::send_picture() {

    QString fileName = QFileDialog::getOpenFileName(
            this,tr("Open Image"), "/home/", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty()) {

        auto *message = new chat_message;

        QByteArray ba = fileName.toLocal8Bit();
        const char *file_name = ba.data();
        message->read_file(file_name);

        // in order to copy and paste image
        // convert to base64 when passing to delegate
        // so we can save it in original size

        QImage image;
        image.loadFromData(message->file_buffer, message->file_size);
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        image.save(&buffer, "PNG");
        QString base_64 = QString::fromLatin1(byteArray.toBase64().data());
        auto *item = new QStandardItem(base_64);
        item->setData("Picture", Qt::UserRole + 1);
        standard_model.appendRow(item);

        char content_type[] = "Picture";

        QString receiver = get_recipient();

        QByteArray receiver_array = receiver.toLocal8Bit();
        char* rec = receiver_array.data();

        char* user = &username[0];

        message->create_bson(rec, user, content_type);
        message->set_size(message->length());
        std::memcpy(message->body(), message->bson, message->body_length());
        message->encode_header();

        if (!message->body()) {
            delete message;
            return;
        }
        else {
            socket->write((char*)message->data(), message->length());
            delete message;
        }
    }
}

void MainWindow::sendMessage() {


    // instantiate a new chat_message and create header
    auto *msg = new chat_message;

    std::string body = (message_line->text()).toStdString();
    QString receiver = get_recipient();

    char type[] = "Text";
    QByteArray receiver_array = receiver.toLocal8Bit();
    char* rec = receiver_array.data();

    char* user = &username[0];
    char* bod = &body[0];

    msg->create_bson(rec, user, type, bod);
    msg->set_size(msg->body_length());
    std::memcpy(msg->body(), msg->bson, msg->body_length());
    msg->encode_header();

    if(!msg->data()) {
        delete msg;
        return;
    } else {
        //socket->write(QString(message).toUtf8());
        socket->write((char *)msg->data(), msg->length());
        data_handler->insert_message(user, rec, type, bod);
    }

    append_sent(message_line->text());
    message_line->clear();
    delete msg;

}

void MainWindow::append_sent(const QString& message) {

    //append a sent message to the message view
    auto *item1 = new QStandardItem(message);
    item1->setData("Outgoing", Qt::UserRole + 1);
    standard_model.appendRow(item1);

}

void MainWindow::append_received(const QString& user_name, const QString& message) {
    // append a received message to the message_view
    QString receiver = get_recipient();
    std::cout << receiver.toStdString() << std::endl;
    if (receiver == user_name) {

        auto *received_message = new QStandardItem(message);
        received_message->setData("Incoming", Qt::UserRole + 1);
        standard_model.appendRow(received_message);

    }

    else if (!stringList->username_in_view(user_name)) {
        stringList->append(user_name);
    }

}

void MainWindow::receive_picture(const QString& user_name, const QString& img_data) {

    auto *item = new QStandardItem(img_data);
    item->setData("Picture", Qt::UserRole + 1);
    standard_model.appendRow(item);

}

void MainWindow::onReadyRead() {

    // read message received, insert contents into db,
    // append to the appropriate view
    auto *message = new chat_message;

    if (socket->bytesAvailable() > message->HEADER_LENGTH) {

        socket->read((char*)message->header, 4);

        if (message->decode_header() && socket->bytesAvailable() >= message->body_length_) {

            socket->read((char *) message->body(), message->body_length());

            message->parse_bson(message->body(), message->body_length());

            data_handler->insert_message(message->Deliverer, message->Receiver, message->Content_Type,
                                         message->Text_Message);
            append_received(QString::fromUtf8(message->Receiver),
                            QString::fromUtf8(message->Text_Message));
        }
        delete message;
    }
}

void MainWindow::erase_user_messages() {
    stringList->delete_user(username_view->currentIndex());
    standard_model.clear();

}

void MainWindow::erase_all_messages() {
    data_handler->clear_messages();
    standard_model.clear();
}

void MainWindow::add_user() const {
    stringList->append(search_user_line->text());
}

void MainWindow::logout() {
    this->close();
    auto *log = new login();
    log->resize(500,500);
    log->show();
}

QString MainWindow::get_recipient() const {
    QModelIndex index = username_view->currentIndex();
    QString recipient = stringList->set_recipient(index);

    return recipient;
}

void MainWindow::set_recipient() {
    //sets recipient of the message and changes messages
    QString receiver = get_recipient();
    std::list<std::tuple<std::string, std::string, std::string>> message_data = data_handler->
            get_messages(receiver.toStdString());

    for (auto i: message_data) {
        if (std::get<0>(i) != username) {
            append_received(QString::fromUtf8(std::get<0>(i).c_str()),
                            QString::fromUtf8(std::get<2>(i).c_str()));
        }
        else {
            append_sent(QString::fromUtf8(std::get<2>(i).c_str()));
        }
    }
}


MainWindow::~MainWindow()
{
    delete data_handler;
    delete ui;
}
