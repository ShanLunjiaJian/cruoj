#include<crow.h>
#include<crow/middlewares/session.h>
#include<mariadb/conncpp.hpp>
#include"config.h"
#include"locale.h"
#include"./app/controller.h"
#include"./app/db.h"
#include<iostream>
#include<stdlib.h>
#include<unordered_map>

crow::App<crow::CookieParser> app;
std::unordered_map<std::string,crow::mustache::context> default_ctx;
crow::mustache::context config_ctx;

DB db;

int main()
{
    //define your endpoint at the root directory
    CROW_ROUTE(app, "/hello")([](){
        return "hello crow";
    });

    config_ctx=generate_config_ctx();
    default_ctx["en"]=en_locale(config_ctx);

    route_all();

    app.port(80).multithreaded().run();
}