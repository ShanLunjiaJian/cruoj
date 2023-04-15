#include"config.h"

crow::mustache::context generate_config_ctx()
{
    crow::mustache::context ctx;
    ctx["OJ_NAME_SHORT"]="cruoj";
    return ctx;
}