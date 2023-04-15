#include"db.h"
#include"../config.h"

#include<iostream>

std::string to_string(const sql::SQLString &s){ return std::string((const char*)s,s.length()); }

DB::DB()
{
    try
    {
        sql::Driver *driver=sql::mariadb::get_driver_instance();
        db.reset(driver->connect("tcp://127.0.0.1:3306",db_user,db_password));
        db->setSchema("app_uoj233");
    }
    catch(sql::SQLException &e){ std::cerr<<"db connection failed: "<<e.what(),exit(0); }
}

std::shared_ptr<sql::ResultSet> DB::query(const std::string &s)
{
    std::cerr<<"db_query: "<<s<<std::endl;
    try
    {
        std::unique_ptr<sql::Statement> x(db->createStatement());
        std::shared_ptr<sql::ResultSet> y(x->executeQuery(s.c_str()));
        return y;
    }
    catch(sql::SQLException &e){ std::cerr<<"db_query failed: "<<e.what()<<std::endl; return std::shared_ptr<sql::ResultSet>(0); }
}

void DB::execute(const std::string &s)
{
    std::cerr<<"db_execute: "<<s<<std::endl;
    try
    {
        std::unique_ptr<sql::Statement> x(db->createStatement());
        x->executeQuery(s.c_str());
    }
    catch(sql::SQLException &e){ std::cerr<<"db_execute failed: "<<e.what()<<std::endl; }
}

std::shared_ptr<sql::ResultSet> DB::query_user(const std::string &username){ return query("select *,unix_timestamp(register_time) from user_info where username='"+username+"'"); }
std::shared_ptr<sql::ResultSet> DB::query_contest(const int contest_id){ return query("select *,unix_timestamp(start_time) from contests where id="+std::to_string(contest_id)); }

int DB::query_zan_val(const std::string &type,const int id,const std::string &username)
{
    if(username=="") return 0;
    auto r=query("select val from click_zans where type='"+type+"' and target_id="+std::to_string(id)+" and username='"+username+"'");
    if(!r->next()) return 0;
    return r->getInt("val");
}