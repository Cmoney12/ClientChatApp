#include "login.h"
#include <QWidget>
#include <QFormLayout>
#include <QMessageBox>
#include <QDir>
#include <QCoreApplication>

#include "mainwindow.h"


login::login(QWidget *parent): QMainWindow(parent)
{
    auto *widget = new QWidget;
    auto *layout = new QFormLayout;

    pic_label = new QLabel;
    QPixmap pixmap("/home/corey/CLionProjects/ClientChatApp/vega3.png");
    pic_label->setPixmap(pixmap);
    //pic_label->setMask(pixmap.mask());

    username_line = new QLineEdit;
    password_line = new QLineEdit;
    password_line->setEchoMode(QLineEdit::Password);
    username_label = new QLabel("Username: ");
    username_label->setStyleSheet("QLabel {color: rgb(8, 50, 201); }");
    password_label = new QLabel("Password: ");
    password_label->setStyleSheet("QLabel {color : rgb(8, 50, 201); }");
    login_button = new QPushButton("Login");
    register_button = new QPushButton("Register");

    widget->setLayout(layout);

    layout->addWidget(pic_label);
    layout->addRow(username_label, username_line);
    layout->addRow(password_label, password_line);
    layout->addWidget(login_button);
    layout->addWidget(register_button);
    setCentralWidget(widget);

    std::string current_directory = "/home/corey/CLionProjects/ClientChatApp";
    data_handler = new database_handler(current_directory);
    connect(login_button, &QPushButton::clicked, this, &login::on_login);
    connect(register_button, &QPushButton::clicked, this, &login::register_user);

}

void login::on_login() {
    QString username = username_line->text();
    QString password = password_line->text();
    bool login_result = data_handler->login(username.toStdString(), password.toStdString());

    if (login_result) {
        auto mainwindow = new MainWindow();
        //mainwindow->setBaseSize(500,500);
        mainwindow->show();
        this->close();
    } else {
        QMessageBox::warning(this, "Login Error", "Username/Password does not exist");
    }
}

void login::register_user() {
    std::string username = (username_line->text()).toStdString();
    std::string password = (password_line->text()).toStdString();
    bool registration = data_handler->register_user(username, password);
    if (registration) {
        QMessageBox::warning(this, "Registered Successfully", "Please login to continue");
    }
    else {
        QMessageBox::warning(this, "Registry error","Error when registering user");
    }
}

login::~login()
{
    delete data_handler;
}