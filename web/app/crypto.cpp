#include"crypto.h"
#include"libs/SHA256.h"
#include<random>
#include<time.h>

std::string sha256(const std::string &s)
{
    SHA256 hash;
    hash.update(s);
    uint8_t *p=hash.digest();
    std::string res=hash.toString(p);
    delete p;
    return res;
}

std::string generate_token()
{
    std::string r;
    const int L=64;
    r.resize(L);
    const unsigned int salt1=1234817821,salt2=4018923418;
    static std::mt19937 randi(time(0)*salt1^salt2);
    unsigned x;
    auto f=[](int x){ return x<10?x+'0':x-10+'a'; };
    for(int i=0;i<(L>>3);i++)
        x=randi(),
        r[i<<3]=f(x&15),
        r[i<<3|1]=f(x>>4&15),
        r[i<<3|2]=f(x>>8&15),
        r[i<<3|3]=f(x>>12&15),
        r[i<<3|4]=f(x>>16&15),
        r[i<<3|5]=f(x>>20&15),
        r[i<<3|6]=f(x>>24&15),
        r[i<<3|7]=f(x>>28&15);
    return r;
}