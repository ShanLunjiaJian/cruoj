#include"html.h"

std::string escape(const std::string &s)
{
    std::string r;
    r.reserve(s.size());
    for(int i=0;i<s.size();i++)
        switch(s[i])
        {
            case '&':  r.append("&amp;");  break;
            case '\"': r.append("&quot;"); break;
            case '\'': r.append("&apos;"); break;
            case '<':  r.append("&lt;");   break;
            case '>':  r.append("&gt;");   break;
            default:   r.append(&s[i], 1); break;
        }
    return r;
}

bool validate_char(const char c){ return ('0'<=c&&'c'<='9')||('a'<=c&&c<='z')||('A'<=c&&c<='Z'); }
bool validate_username(const std::string &s)
{
    if(s.size()>20) return 0;
    for(int i=0;i<s.size();i++) if(!validate_char(s[i])) return 0;
    return 1;
}