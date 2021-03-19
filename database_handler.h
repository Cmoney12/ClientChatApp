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
        directory = current_directory + "/messanger_db.sqlite";;

    }

    bool login(const std::string &user_name, const std::string &pass_word) {
        bool found = false;
        if (sqlite3_open(directory.c_str(), &db) == SQLITE_OK) {
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
        sqlite3_close(db);
        return found;
    }

    bool register_user(const std::string &user_name, const std::string &pass_word) {
        if (sqlite3_open(directory.c_str(), &db) == SQLITE_OK) {
            std::string sql = "INSERT INTO Login VALUES('" + user_name + "','" + pass_word + "')";
            int exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
            sqlite3_free(zErrMsg);
            if (exit != SQLITE_OK) {
                sqlite3_close(db);
                return true;
            }
        }
        else {
            sqlite3_close(db);
            return false;
        }
    }

    void check_tables() {
        if (!valid_login_table()) {
                create_login_table();
        }
        if(!valid_message_table()) {
            create_message_table();
        }
    }

    bool valid_message_table() {
        bool selected = false;
        if (sqlite3_open(directory.c_str(), &db) == SQLITE_OK) {
            char sql[] = "SELECT * FROM Messages";
            struct sqlite3_stmt *selectstmt;
            int result = sqlite3_prepare_v2(db, sql, -1, &selectstmt, nullptr);
            if (result == SQLITE_OK) {
                sqlite3_free(selectstmt);
                selected = true;
            }
        }
        sqlite3_close(db);
        return selected;
    }

    bool valid_login_table() {
        bool exists = false;
        if (sqlite3_open(directory.c_str(), &db) == SQLITE_OK) {
            char sql[] = "SELECT * FROM Login";
            struct sqlite3_stmt *selectstmt;
            int result = sqlite3_prepare_v2(db, sql, -1, &selectstmt, nullptr);
            if (result == SQLITE_OK) {
                exists = true;
            }
        }
        sqlite3_close(db);
        return exists;
    }


    void create_message_table() {
        if (sqlite3_open(directory.c_str(), &db) == SQLITE_OK) {
            char create_table[] = "CREATE TABLE Messages (message text, timesent text)";
            rc = sqlite3_exec(db, create_table, nullptr, nullptr, &zErrMsg);
        }
        sqlite3_close(db);
        sqlite3_free(zErrMsg);

    }

    void create_login_table() {
        char create_table[] = "CREATE TABLE Login (username TEXT, password TEXT)";

        rc = sqlite3_exec(db, create_table, nullptr, nullptr, &zErrMsg);
        sqlite3_free(zErrMsg);
    }

    int insert_message(const std::string& message) {
        if (rc != SQLITE_OK) {
            //fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
            return 0;
        }

        std::string sql = "INSERT INTO Messages VALUES('" + message + "','" + get_current_datetime() +"')";

        rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

        if (rc != SQLITE_OK ) {
            return 1;
        }
        sqlite3_free(zErrMsg);
    }
    std::string load_messages() {
        sqlite3_stmt *selectStmt;
        std::string query = "select message from Messages";
        std::string messages;
        rc = sqlite3_open("/home/corey/CLionProjects/ClientChatApp/messanger_db.sqlite", &db);
        if ( sqlite3_prepare(db, query.c_str(), -1, &selectStmt, 0 ) == SQLITE_OK )
        {
            int ctotal = sqlite3_column_count(selectStmt); // Count the Number of Columns in the Table
            int res = 0;
            while (true)
            {
                res = sqlite3_step(selectStmt); // Execute SQL Statement.
                if ( res == SQLITE_ROW )
                {
                    //sqlite3_finalize(selectStmt);
                    for ( int i = 0; i < ctotal; i++ )  // Loop times the number of columns in the table
                    {
                        std::string s = (char*)sqlite3_column_text(selectStmt, i);  // Read each Column in the row.
                        // print or format the output as you want
                        messages += s;
                    }
                    messages+="\n";
                }

                if ( res == SQLITE_DONE || res==SQLITE_ERROR)
                {
                    break;
                }
            }
        }
        sqlite3_finalize(selectStmt);
        sqlite3_free(db);
        return messages;
    }

    static std::string get_current_datetime() {
        time_t t = time(nullptr);
        struct tm *tm = localtime(&t);
        std::string datetime = asctime(tm);
        return datetime;

    }

    void clear_messages() {
        if (sqlite3_open("/home/corey/CLionProjects/ClientChatApp/messanger_db.sqlite", &db) == SQLITE_OK) {
            std::string sql = "DELETE FROM Messages";
            rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);
        }
        sqlite3_close(db);
        sqlite3_free(zErrMsg);
    }

private:
    std::string directory;
    int rc{};
    sqlite3 *db{};
    char *zErrMsg = nullptr;
};

#endif //CLIENTCHATAPP_DATABASE_HANDLER_H
