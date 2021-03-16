#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "database_handler.h"

class login : public QMainWindow
{
    Q_OBJECT
public:
    explicit login(QWidget *parent = nullptr);
    ~login() override;
public:
    QPushButton *login_button;
    QLineEdit *username_line;
    QPushButton *register_button;
    QLineEdit *password_line;
    QLabel *username_label;
    QLabel *password_label;
    database_handler *data_handler;
public slots:
    void on_login();
    void register_user();

};

#endif // LOGIN_H
