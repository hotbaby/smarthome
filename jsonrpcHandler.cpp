/*************************************************************************
> File Name: jsonrpcHandler.cpp
> Author: yy
> Mail: mengyy_linux@163.com
 ************************************************************************/

#include "jsonrpcHandler.hpp"

map<JsonRpcErrorCode, string> jsonrpcErrorCodeTable =
{
    {JSON_RPC_PARSE_ERROR, "Parse error"},
    {JSON_RPC_INVALID_REQUEST, "Invalid Request"},
    {JSON_RPC_METHOD_NOT_FOUND, "Method not found"},
    {JSON_RPC_INVALID_PARAMS, "Invalid params"},
    {JSON_RPC_INTERNAL_ERROR, "Internal error"},
};

void JsonRpcHandler::addHander(CallBackMethod *method)
{
    methods.push_back(method);
}

void JsonRpcHandler::deleteHandler(const string &name)
{
    
    for(vector<CallBackMethod*>::iterator  it = methods.begin(); it != methods.end(); ++it)
    {
        if ((*it)->getName() ==  name)
        {
            delete (*it);
            methods.erase(it);
            break;
        }
    }
}


bool JsonRpcHandler::process(const Json::Value &request, Json::Value &response)
{
    Json::Value error;

    response["id"] = request["id"];
    response["jsonrpc"] = request["jsonrpc"];
      
    if (!check(request, error))
    {
        response["error"] = error;
        return false;
    }

    Json::Value params = request["params"];
    string method = request["method"].asString();

    if (method.compare(0, 6, "Router"))
    {
        method = method + '.' + params["id"].asString();
    }
    else
    {
        // Nothing to do.
    }

    CallBackMethod *cb = lookup(method);
    if (cb)
    {
        cb->call(request, response);
    }
    else
    {
        cerr << "Error: can not find method: " << method << endl;
        error["code"] =  JSON_RPC_METHOD_NOT_FOUND;
        error["message"] = jsonrpcErrorCodeTable[JSON_RPC_METHOD_NOT_FOUND];
        response["error"] = error;
        return false;
    }

     return true;
}

bool JsonRpcHandler::process(const string msg,  Value & response)
{
    Json::Reader reader;
    Json::Value request;

    if (reader.parse(msg, request))
    {
        return process(request, response);
    }
    else
    {
        response["jsonrpc"] = "2.0";

        Json::Value error;
        error["code"] = JSON_RPC_PARSE_ERROR;
        error["message"] = jsonrpcErrorCodeTable[JSON_RPC_PARSE_ERROR];
        response["error"] = error;

        return false;
    }

    return true;
}

bool JsonRpcHandler::check(const Json::Value &root, Json::Value &error)
{
    if (!root.isMember("method") || !root.isMember("params"))
    {
        error["code"] = JSON_RPC_INVALID_REQUEST;
        error["message"] = jsonrpcErrorCodeTable[JSON_RPC_INVALID_REQUEST];

        return false;
    }

    Json::Value params = root["params"];
    Json::Value method = root["method"];

    //if (!params.isObject() || !params.isMember("id") || params["id"].isNull())
    if (!params.isObject()
        || (method.asString().compare(0, 6, "Router") && (!params.isMember("id") || params["id"].isNull())))
    {
        error["code"] = JSON_RPC_INVALID_PARAMS;
        error["message"] = jsonrpcErrorCodeTable[JSON_RPC_INVALID_PARAMS];

        return false;
    }

    return true;
}

CallBackMethod* JsonRpcHandler::lookup(const string &name)
{
    for(vector<CallBackMethod*>::const_iterator  it = methods.cbegin(); it != methods.cend(); ++it)
    {
        if ((*it)->getName() == name)
        {
            return *it;
        }
    }

    return NULL;
}

/* JSON-RPC Hander unit test */
#if 0
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
    JsonRpcHandler *handler = new JsonRpcHandler;
    Test testObj;

    handler->addHander(new RpcMethod<Test>(testObj, &Test::print, "test.print"));
    handler->addHander(new RpcMethod<Test>(testObj, &Test::info, "test.info"));


    Json::Reader reader;
    Json::Value request, response;

#if 0
    string s = "{\"method\":\"test.print\", \"params\":\"obj\"}";
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
    string s = "{\"method\":\"test.print\", \"params\":\"obj\"}";
    handler->process(s, response);
    cout << response.toStyledString() << endl;
#endif

    return 0;
}

#endif