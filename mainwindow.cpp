#include "mainwindow.h"
#include "./ui_mainwindow.h"
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
#include <QFileDialog>
#include <fstream>
#include <cstring>


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


    connect_button = new QPushButton("Connect");
    message_view = new QTextEdit;
    send_button = new QPushButton("Send");
    message_line = new QLineEdit;

    QPixmap pixmap("/home/coreylovette/CLionProjects/ClientChatApp/picture_button.png");
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
    data_handler = new database_handler;

    connect(connect_button, &QPushButton::clicked, this, &MainWindow::connection);
    connect(option_menu, SIGNAL(triggered(QAction*)), SLOT(erase_all_messages()));
    connect(send_button, &QPushButton::clicked, this, &MainWindow::sendMessage);
    connect(message_line, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(picture_button, &QPushButton::clicked, this, &MainWindow::send_picture);

    QString header;

}

void MainWindow::connection() {
    std::string messages = data_handler->load_messages();
    QString message = QString::fromUtf8(messages.c_str());
    message_view->append(message);
    socket->connectToHost("127.0.0.1", 1234);
}

void MainWindow::sendMessage() {
    QString message = "text:"+ message_line->text() + "\n";
    if(message.isEmpty()) {
        return;
    } else {
        socket->write(QString(message + "\n").toUtf8());
    }
    message_line->clear();
}


void MainWindow::onReadyRead()
{
    // We'll loop over every (complete) line of text that the server has sent us:
    while(socket->canReadLine()) {
        // Here's the line the of text the server sent us (we use UTF-8 so
        //QString line = QString::fromUtf8(socket->readLine()).trimmed();
        QString line = socket->readLine();
        std::cout << line.toStdString();
        //message_view->append(line);
        //data_handler->insert_message(line.toStdString());
    }
}

void MainWindow::send_picture() {
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),
                                                    "/home/jana", tr("Image Files (*.png *.jpg)"));
    std::string file_name = fileName.toStdString();
    std::ifstream stream;
    std::ifstream file(file_name);
    std::istream_iterator<char> begin(file), end;
    std::vector<char> buffer(begin, end);
    //std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(std::cout, ","));

    //******************************************************
    enum {HEADER_SIZE = 6};
    std::ifstream DataFile(file_name, std::ios::binary);
    if(!DataFile.good())

    DataFile.seekg(0, std::ios::end);
    size_t filesize = (int)DataFile.tellg();
    DataFile.seekg(0);

    char output[filesize + HEADER_SIZE];
    //or std::vector
    //or unsigned char *output = new unsigned char[filesize];
    if(DataFile.read((char*)filesize, filesize))
    {
        std::ofstream fout(file_name, std::ios::binary);
        if(!fout.good())
        fout.write((char*)output, filesize);
    }

    char header[HEADER_SIZE + 1] = "";
    std::sprintf(header, "%4d", static_cast<int>(filesize));
    std::memcpy(output, header, HEADER_SIZE);

    if (!fileName.isEmpty()) {
        socket->write(output);
    }

}

void MainWindow::erase_all_messages() const {
    data_handler->clear_messages();
    message_view->clear();
}

MainWindow::~MainWindow()
{
    delete data_handler;
    delete ui;
}

