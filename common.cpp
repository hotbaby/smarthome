/*************************************************************************
> File Name: common.cpp
> Author: yy
> Mail: mengyy_linux@163.com
 ************************************************************************/

#include "common.hpp"

string uuid(void)
{
    uuid_t uu;
    char s[128] = {0};
    string uuid;

    uuid_generate(uu);
    uuid_unparse(uu, s);

    for (int i = 0; i < 128; i++)
    {
        if (s[i] != '-' && s[i]!= '\0')
        {
            uuid = uuid + s[i];
        }
    }

    return uuid;
}

