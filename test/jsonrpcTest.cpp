/*************************************************************************
> File Name: jsonrpcTest.cpp
> Author: yy
> Mail: mengyy_linux@163.com
 ************************************************************************/

#include "json/json.h"
#include "jsonrpcHandler.hpp"
#include <iostream>

using namespace std;

class Test
{
public:

    bool info(const Json::Value &request, Json::Value &response)
    {
        cout << "Info: call Test::info function." << endl;
        response["result"] = "success";
    }

    bool print(const Json::Value &request, Json::Value &response)
    {
        cout << "Info: call Test::print function." << endl;
        response["result"] = "success";
    }
};

int main(int argc, char **argv)
{

    Json::Reader reader;
    Json::Value request, response;

    JsonRpcHandler *handler = new JsonRpcHandler;
    Test test;

    handler->addHander(new RpcMethod<Test>(test, &Test::print, "test.print.111"));
    handler->addHander(new RpcMethod<Test>(test, &Test::print, "test.print.111"));

#if 1
    string s = "{\"method\":\"test.print\", \"params\":{\"id\":\"111\"}}";

    if (reader.parse(s, request))
    {
        handler->process(request, response);
        cout << response.toStyledString() << endl;
    }
    else
    {
        cerr << "Error: parse error." << endl;
    }
#else
        string s = "{\"method\":\"test.print\", \"params\":{}}";
        handler->process(s, response);
        cout << response.toStyledString() << endl;
#endif

    return 0;
}

