#include"view.h"
#include"html.h"
#include"db.h"

#include<assert.h>
#include<iostream>

extern DB db;

std::string get_user_link(std::string username,int rating)
{
    if(!validate_username(username)) return std::cerr<<username<<" is invalid."<<std::endl,"";
    auto r=db.query_user(username);
    if(!r->next()) return "<span>"+username+"</span>";
    if(rating==-114514) rating=r->getInt("rating");
    return "<span class=\"uoj-username\" data-rating=\""+std::to_string(rating)+"\">"+username+"</span>";
}

std::string get_click_zan_block(std::string type,int id,int cnt,int val)
{
	return "<div class=\"uoj-click-zan-block\" data-id=\""+std::to_string(id)+"\" data-type=\""+type+"\" data-val=\""+std::to_string(val)+"\" data-cnt=\""+std::to_string(cnt)+"\"></div>";
}
