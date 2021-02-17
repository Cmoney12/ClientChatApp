//
// Created by coreylovette on 2/17/21.
//

#include <sqlite3.h>
#include <cstdio>
#include <cstdlib>

#ifndef CLIENTCHATAPP_DATABASE_HANDLER_H
#define CLIENTCHATAPP_DATABASE_HANDLER_H

#include <iostream>
#include <sqlite3.h>
#include <cstdio>

class database_handler {
public:
    bool login(const std::string &user_name, const std::string &pass_word) {

        rc = sqlite3_open("/home/coreylovette/CLionProjects/ClientChatApp/messanger_db.sqlite", &db);
        //std::string sql  = "SELECT EXISTS (SELECT 1 FROM Login WHERE username = '" + user_name + "'password='" + pass_word + "');";
        std::string sql = "SELECT * FROM Login WHERE username = 'pp'";
        struct sqlite3_stmt *selectstmt;
        int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &selectstmt, nullptr);
        if(result == SQLITE_OK) {

            if (sqlite3_step(selectstmt) == SQLITE_ROW) {

                // record found
                return true;
            }
            else {

                return false;
            }
        }
        sqlite3_close(db);
    }

    void register_user(const std::string &user_name, const std::string &pass_word) {
        rc = sqlite3_open("/home/coreylovette/CLionProjects/ClientChatApp/messanger_db.sqlite", &db);
        std::string sql = "INSERT INTO Login VALUES('" + user_name + "','" + pass_word + "')";
        int exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
        if (exit != SQLITE_OK) {
            std::cerr << "Error Create Table" << std::endl;
            sqlite3_free(zErrMsg);
            sqlite3_close(db);
        }
        std::cout << sql << std::endl;
        if(exit == SQLITE_ERROR)
            std::cout << (sqlite3_sql(stmt), sqlite3_errmsg(db));
        if(exit == SQLITE_MISUSE)
            std::cout << (sqlite3_sql(stmt), sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    bool valid_database() {
        rc = sqlite3_open("messanger_db.sqlite", &db);
        if (!rc) {
            std::string create_table = "CREATE TABLE Login (username TEXT, password TEXT)";
            rc = sqlite3_exec(db, create_table.c_str(), nullptr, nullptr, &zErrMsg);
            if (rc != SQLITE_OK) {
                sqlite3_free(zErrMsg);
                return false;
            } else {
                return true;
            }
        } else {
            return true;
        }
    }

    int rc;
    sqlite3 *db{};
    char *zErrMsg = nullptr;
    sqlite3_stmt *stmt;
};



#endif //CLIENTCHATAPP_DATABASE_HANDLER_H
