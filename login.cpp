#include "login.h"
#include <QWidget>
#include <QFormLayout>
#include <QMessageBox>

#include "mainwindow.h"
#include "database_handler.h"


login::login(QWidget *parent) : QMainWindow(parent)
{
    QWidget *widget = new QWidget;
    auto *layout = new QFormLayout;
    username_line = new QLineEdit;
    password_line = new QLineEdit;
    username_label = new QLabel("Username: ");
    password_label = new QLabel("Password: ");
    login_button = new QPushButton("Login");
    register_button = new QPushButton("Register");
    widget->setLayout(layout);
    layout->addRow(username_label, username_line);
    layout->addRow(password_label, password_line);
    layout->addWidget(login_button);
    layout->addWidget(register_button);
    setCentralWidget(widget);
    connect(login_button, &QPushButton::clicked, this, &login::on_login);

}

void login::on_login() {
    database_handler sqlite_handler;
    QString username = username_label->text();
    QString password = password_label->text();
    bool login_result = sqlite_handler.login(username.toStdString(), password.toStdString());
    if (login_result) {
        auto mainwindow = new MainWindow();
        mainwindow->show();
        this->hide();
    } else {
        QMessageBox::warning(this, "Login Error", "Username/Password does not exist");
    }
}