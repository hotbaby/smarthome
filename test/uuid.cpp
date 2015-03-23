/*************************************************************************
> File Name: uuid.cpp
> Author: yy
> Mail: mengyy_linux@163.com
 ************************************************************************/

#include <uuid/uuid.h>
#include <iostream>

using namespace std;

int main(int, char**)
{
    uuid_t id;
    char str[128] = {0};
    string s;

    uuid_generate(id);
    //uuid_unparse(id, str);
    //uuid_unparse_lower(id, str);
    uuid_unparse_lower(id, str);
    int i;
    for(i = 0; str[i] != '\0', i < 128; i++)
    {
        if (str[i] != '-')
        {
            s = s+ str[i];
        }
    }
    cout << i << endl;

    cout << s << endl;
    return 0;
}
