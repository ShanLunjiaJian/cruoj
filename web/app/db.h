#pragma once

#include<mariadb/conncpp.hpp>
#include<string>

std::string to_string(const sql::SQLString &s);

struct DB
{
    std::unique_ptr<sql::Connection> db;
    DB();
    std::shared_ptr<sql::ResultSet> query(const std::string &s);
    void execute(const std::string &s);
    std::shared_ptr<sql::ResultSet> query_user(const std::string &username);
    std::shared_ptr<sql::ResultSet> query_contest(const int contest_id);
    int query_zan_val(const std::string &type,const int id,const std::string &username);
};