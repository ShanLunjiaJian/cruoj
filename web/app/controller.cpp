#include<crow.h>
#include<crow/middlewares/session.h>
#include"db.h"
#include"view.h"
#include"html.h"
#include"crypto.h"
#include<string>
#include<iostream>
#include<unordered_map>
#include<time.h>

extern crow::App<crow::CookieParser> app;
extern DB db;
extern std::unordered_map<std::string,crow::mustache::context> default_ctx;

crow::response redirect(std::string s){ crow::response x; x.redirect(s); return x; }

crow::mustache::context generate_ctx(const crow::CookieParser::context cookie)
{
    crow::mustache::context ctx=default_ctx[cookie.get_cookie("locale")==""?"en":cookie.get_cookie("locale")];
    ctx["username"]=cookie.get_cookie("username");
    ctx["now_time"]=time(0);
    return ctx;
}

crow::response message_page(const crow::CookieParser::context cookie,std::string message1,std::string message2,int status=200)
{
    auto c=generate_ctx(cookie);
    c["message1"]=message1,c["message2"]=message2;
    return {status,crow::mustache::load("message_page.mustache").render(c)};
}
crow::response not_found(const crow::CookieParser::context cookie){ return message_page(cookie,"404","Not found",404); }

void route_login()
{
    CROW_ROUTE(app,"/login").methods("GET"_method)([](const crow::request &request)->crow::response{
        auto &cookie=app.get_context<crow::CookieParser>(request);
        if(cookie.get_cookie("token")!="") return redirect("/");
        return crow::mustache::load("login.mustache").render(default_ctx);
    });
    CROW_ROUTE(app,"/login").methods("POST"_method)([](const crow::request &request)->crow::response{
        auto &cookie=app.get_context<crow::CookieParser>(request);
        if(cookie.get_cookie("token")!="") return redirect("/");

        auto params=request.get_body_params();
        std::string username,password;
        if(params.get("username")==nullptr||params.get("password")==nullptr) return std::string("incomplete params");
        username=params.get("username"),password=params.get("password");
        if(!validate_username(username)) return std::string("illegal username");

        auto r=db.query_user(username);
        if(!r->next()) return std::string("user not found");
        std::string _password=to_string(r->getString("password"));
        std::cerr<<username<<' '<<password<<' '<<sha256(username+"|"+password)<<' '<<_password<<std::endl;
        if(_password!=sha256(username+"|"+password)) return std::string("wrong password");

        std::string token=generate_token();
        cookie.set_cookie("token",token).max_age(30*24*60*60);
        cookie.set_cookie("username",username).max_age(30*24*60*60);
        db.execute("update user_info set remember_token='"+token+"'where username='"+username+"'");
        return std::string("ok");
    });
}

void route_index()
{
    CROW_ROUTE(app,"/")([](const crow::request &request){
        auto &cookie=app.get_context<crow::CookieParser>(request);
        auto ctx=generate_ctx(cookie);
        
        ctx["title"]="index";

        auto r=db.query("select blogs.id, title, poster, post_time, unix_timestamp(post_time) from important_blogs, blogs where is_hidden = 0 and important_blogs.blog_id = blogs.id order by level desc, important_blogs.blog_id desc limit 5");
        int cnt=0;
        std::string important_blogs;
        for(std::time_t post_time;r->next();)
            post_time=r->getInt("unix_timestamp(post_time)"),
            important_blogs+=std::string(
                "<tr><td><a href=\"/blogs/")
                    +to_string(r->getString("id"))
                +"\">"
                    +to_string(r->getString("title"))
                +"</a>"
                    +((time(0)-post_time)/3600/24<=7?"<sup style=\"color:red\">&nbsp;new</sup>":"")
                +"</td><td>by "
                    +get_user_link(to_string(r->getString("poster")))
                +"</td><td><small>"
                    +to_string(r->getString("post_time"))
                +"</small></td></tr>";
        ctx["important_blogs"]=important_blogs;

        return crow::mustache::load("index.mustache").render(ctx);
    });
}

void route_user_info()
{
    CROW_ROUTE(app,"/user/profile")([](const crow::request &request)->crow::response{
        auto &cookie=app.get_context<crow::CookieParser>(request);
        return not_found(cookie);
    });
    CROW_ROUTE(app,"/user/profile/<string>")([](const crow::request &request,const std::string &username)->crow::response{
        auto &cookie=app.get_context<crow::CookieParser>(request);
        auto ctx=generate_ctx(cookie);
        
        auto user=db.query_user(username);
        if(!user->next()) return std::string("no such user");
        ctx["title"]="user_info";
        ctx["rating"]=user->getInt("rating");
        ctx["username"]=username;
        ctx["gender"]=to_string(user->getString("gender"));
        ctx["email"]=to_string(user->getString("email"));
        ctx["qq"]=user->getInt("qq");
        ctx["motto"]=to_string(user->getString("motto"));

        //draw rating changes
        {
            auto reg=db.query("select contest_id,rank,user_rating from contests_registrants where username='"+username+"' and has_participated=1 order by contest_id");
            std::vector<int> a,b,c,e,f;
            std::vector<std::string> d;
            int rating_min=1500,rating_max=1500;
            a.push_back(user->getInt("unix_timestamp(register_time)")),
            c.push_back(0),
            d.push_back("Initial rating"),
            e.push_back(0),
            f.push_back(0);
            for(int rating_last=1500,rating_now;reg->next();)
            {
                auto contest=db.query_contest(reg->getInt("contest_id"));
                if(!contest->next()){ std::cerr<<"? broken contest list."<<std::endl;continue; }
                rating_now=reg->getInt("user_rating"),
                rating_min=std::min(rating_min,rating_now),rating_max=std::max(rating_max,rating_now),
                a.push_back(contest->getInt("unix_timestamp(start_time)")),
                b.push_back(rating_now),
                c.push_back(reg->getInt("contest_id")),
                d.push_back(to_string(contest->getString("name"))),
                e.push_back(reg->getInt("rank")),
                f.push_back(rating_now-rating_last),
                rating_last=rating_now;
            }
            b.push_back(user->getInt("rating"));
            std::string res;
            for(int i=0;i<a.size();i++) res+="["+std::to_string(a[i])+","+std::to_string(b[i])+","+std::to_string(c[i])+",'"+d[i]+"',"+std::to_string(e[i])+","+std::to_string(f[i])+"],";
            ctx["rating_data"]=res;
            ctx["rating_min"]=rating_min-400;
            ctx["rating_max"]=rating_max+400;
        }

        return crow::mustache::load("user_info.mustache").render(ctx);
    });
}

void route_problem_set()
{
    CROW_ROUTE(app,"/problems")([](const crow::request &request)->crow::response{
        auto &cookie=app.get_context<crow::CookieParser>(request);
        auto ctx=generate_ctx(cookie);

        std::string username=cookie.get_cookie("username");

        int page;
        if(request.url_params.get("page")!=nullptr) page=std::atoi(request.url_params.get("page"));
        else page=1;

        auto p=db.query("select problems.id,title,zan,ac_num,submit_num,click_zans.val from problems left join click_zans on problems.id=click_zans.target_id and click_zans.type='P' and click_zans.username='"+username+"' order by problems.id limit "+std::to_string((page-1)*50)+",50");
        std::vector<crow::mustache::context> v;
        bool show_submit_mode=(cookie.get_cookie("show_submit_mode")!="");
        while(p->next())
        {
            crow::mustache::context temp;
            temp["id"]=p->getInt("id"),
            temp["title"]=to_string(p->getString("title")),
            temp["click_zan_block"]=get_click_zan_block("P",p->getInt("id"),p->getInt("zan"),p->getInt("val"));
            if(show_submit_mode)
                temp["ac_num"]=p->getInt("ac_num"),
                temp["submit_num"]=p->getInt("submit_num"),
                temp["ac_perc"]=(double)p->getInt("ac_num")*100/p->getInt("submit_num");
            v.push_back(temp);
        }
        ctx["problems"]=crow::json::wvalue(v);

        if(cookie.get_cookie("show_submit_mode")!="") ctx["show_submit_mode"]="on";

        return crow::mustache::load("problem_set.mustache").render(ctx);
    });
}

void route_problem()
{
    CROW_ROUTE(app,"/problem/<int>")([](const crow::request &request)->crow::response{
}

void route_all()
{
    route_index();
    route_login();
    route_user_info();
    route_problem_set();
}