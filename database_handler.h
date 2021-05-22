//
// Created by coreylovette on 2/17/21.
//

#include <sqlite3.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>

#ifndef CLIENTCHATAPP_DATABASE_HANDLER_H
#define CLIENTCHATAPP_DATABASE_HANDLER_H

class database_handler {
public:

    explicit database_handler(const std::string& current_directory) {
        directory = current_directory;
    }

    bool connect() {
        if(!is_open && sqlite3_open(directory.c_str(), &db) == SQLITE_OK) {
            is_open = true;

        }
        return is_open;
    }

    void disconnect() {
        if(is_open) {
            sqlite3_close(db);
            is_open = false;
        }
    }


    bool login(const std::string &user_name, const std::string &pass_word) {
        bool found = false;
        if (connect()) {
            std::string sql = "SELECT * FROM Login WHERE username='" +
                              user_name + "' and password='" + pass_word + "'";

            struct sqlite3_stmt *selectstmt;
            int result = sqlite3_prepare_v2(db, sql.c_str(), -1, &selectstmt, nullptr);
            if (result == SQLITE_OK) {
                if (sqlite3_step(selectstmt) == SQLITE_ROW) {
                    // record found
                    sqlite3_finalize(selectstmt);
                    found = true;
                }
            }
        }
        disconnect();
        return found;
    }

    bool register_user(const std::string &user_name, const std::string &pass_word) {
        bool response;
        check_tables();
        if (connect()) {
            std::string sql = "INSERT INTO Login VALUES('" + user_name + "','" + pass_word + "')";
            int exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
            sqlite3_free(zErrMsg);
            if (exit == SQLITE_OK) {
                disconnect();
                response = true;
            }
        }

        return response;
    }

    bool check_tables() {
        if (!valid_login_table()) {
            create_login_table();
        }

        if(!valid_message_table()) {
            create_message_table();
        }

        if(valid_login_table() && valid_message_table()) {
            return true;
        }

        else {
            return false;
        }
    }

    bool valid_message_table() {
        bool selected = false;
        if (connect()) {
            char sql[] = "SELECT * FROM Messages";
            struct sqlite3_stmt *selectstmt;
            int result = sqlite3_prepare_v2(db, sql, -1, &selectstmt, nullptr);
            if (result == SQLITE_OK) {
                sqlite3_free(selectstmt);
                selected = true;
            }
        }
        disconnect();
        return selected;
    }

    bool valid_login_table() {
        bool exists = false;
        if (connect()) {
            char sql[] = "SELECT * FROM Login";
            struct sqlite3_stmt *selectstmt;
            int result = sqlite3_prepare_v2(db, sql, -1, &selectstmt, nullptr);
            if (result == SQLITE_OK) {
                exists = true;
            }
        }
        disconnect();
        return exists;
    }


    void create_message_table() {
        if (connect()) {
            char create_table[] = "CREATE TABLE Messages ( deliverer TEXT, recipient TEXT, message TEXT, datetime  TEXT);";
            rc = sqlite3_exec(db, create_table, nullptr, nullptr, &zErrMsg);
        }
        sqlite3_close(db);
        sqlite3_free(zErrMsg);
    }

    void create_login_table() {
        if (connect()) {
            char create_table[] = "CREATE TABLE Login (username TEXT, password TEXT)";
            rc = sqlite3_exec(db, create_table, nullptr, nullptr, &zErrMsg);
        }
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
    }

    bool insert_message(const char *deliverer, const char *recipient, const char *type, const char *message) {
        bool success;
        if (connect()) {
            sqlite3_stmt *insert_stmt = nullptr;
            const char* sql = "INSERT INTO Messages VALUES(?,?,?,?)";

            if (sqlite3_prepare_v2(db, sql, -1, &insert_stmt, nullptr) == SQLITE_OK) {

                sqlite3_bind_text(insert_stmt, 1, deliverer, -1, nullptr);
                sqlite3_bind_text(insert_stmt, 2, recipient, -1, nullptr);
                sqlite3_bind_text(insert_stmt, 3, message, -1, nullptr);
                sqlite3_bind_text(insert_stmt, 4, get_current_datetime(), -1, nullptr);
                rc = sqlite3_step(insert_stmt);
                sqlite3_exec(db, "COMMIT TRANSACTION", nullptr, nullptr, &zErrMsg);
                sqlite3_finalize(insert_stmt);
                sqlite3_free(zErrMsg);
                if (rc == SQLITE_OK) {
                    success = true;
                }
            }
        }
        disconnect();
        return success;
    }

    std::string load_messages() {
        sqlite3_stmt *selectStmt;
        char query[] = "select recipient, message from Messages";
        //std::string query = "select message from Messages";
        std::string messages;
        if (connect()) {
            if (sqlite3_prepare_v2(db, query, -1, &selectStmt, nullptr) == SQLITE_OK) {
                int ctotal = sqlite3_column_count(selectStmt); // Count the Number of Columns in the Table
                int res = 0;
                while (true) {
                    res = sqlite3_step(selectStmt); // Execute SQL Statement.
                    if (res == SQLITE_ROW) {

                        for (int i = 0; i < ctotal; i++)  // Loop times the number of columns in the table
                        {
                            std::string s = (char *) sqlite3_column_text(selectStmt,
                                                                         i);  // Read each Column in the row.
                            // print or format the output as you want
                            if (i % 2 == 0) {
                                messages += s + ":";
                            } else {
                                messages += s + "\n";
                            }
                        }
                    }

                    if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                        break;
                    }
                }
            }
        }
        sqlite3_finalize(selectStmt);
        disconnect();
        return messages;
    }

    static const char* get_current_datetime() {
        time_t t = time(nullptr);
        struct tm *tm = localtime(&t);
        const char* datetime = asctime(tm);
        return datetime;

    }

    void clear_messages() {
        if (connect()) {
            std::string sql = "DELETE FROM Messages";
            rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
        }
        sqlite3_free(zErrMsg);
        disconnect();
    }

    std::string get_messages(const std::string& username) {
        sqlite3_stmt *selectStmt;
        char sql[] = "SELECT recipient, message from Messages WHERE recipient = ? or deliverer = ?";
        //std::string sql = "select recipient, message from Messages where recipient = '" +
        //        username + "' or deliverer = '" + username + "'";
        std::string messages;
        if (connect()) {
            if (sqlite3_prepare_v2(db, sql, -1, &selectStmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_text(selectStmt, 1, username.c_str(), -1, nullptr);
                sqlite3_bind_text(selectStmt, 2, username.c_str(), -1, nullptr);
                int ctotal = sqlite3_column_count(selectStmt); // Count the Number of Columns in the Table
                int res = 0;
                while (true) {
                    res = sqlite3_step(selectStmt); // Execute SQL Statement.
                    if (res == SQLITE_ROW) {
                        //sqlite3_finalize(selectStmt);
                        for (int i = 0; i < ctotal; i++)  // Loop times the number of columns in the table
                        {
                            std::string s = (char *) sqlite3_column_text(selectStmt,
                                                                         i);  // Read each Column in the row.
                            // print or format the output as you want
                            if (i % 2 == 0) {
                                messages += s + "\n";
                            } else {
                                messages += s + ":";
                            }
                        }
                    }

                    if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                        break;
                    }
                }
            }
        }
        sqlite3_finalize(selectStmt);
        disconnect();
        return messages;
    }

    std::string get_username() {
        std::string user_name;

        char query[] = "SELECT username FROM Login";
        struct sqlite3_stmt *selectStmt;
        if (connect()) {
            if (sqlite3_prepare(db, query, -1, &selectStmt, nullptr) == SQLITE_OK) {
                int ctotal = sqlite3_column_count(selectStmt); // Count the Number of Columns in the Table
                int res;
                while (true) {
                    res = sqlite3_step(selectStmt); // Execute SQL Statement.
                    if (res == SQLITE_ROW) {
                        //sqlite3_finalize(selectStmt);
                        for (int i = 0; i < ctotal; i++)  // Loop times the number of columns in the table
                        {
                            std::string s = (char *) sqlite3_column_text(selectStmt,
                                                                         i);  // Read each Column in the row.
                            // print or format the output as you want
                            user_name += s;
                        }
                    }

                    if (res == SQLITE_DONE || res == SQLITE_ERROR) {
                        break;
                    }
                }
            }
        }
        sqlite3_finalize(selectStmt);
        disconnect();
        return user_name;
    }

private:
    bool is_open{};
    std::string directory;
    int rc{};
    sqlite3 *db{};
    char *zErrMsg = nullptr;
};

#endif //CLIENTCHATAPP_DATABASE_HANDLER_H
