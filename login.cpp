#include "login.h"
#include <QWidget>
#include <QFormLayout>

#include "mainwindow.h"


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
    auto mainwindow = new MainWindow();
    mainwindow->show();
    this->hide();
}
