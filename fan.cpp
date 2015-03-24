/*************************************************************************
> File Name: fan.cpp
> Author: yy
> Mail: mengyy_linux@163.com
 ************************************************************************/

#include "fan.hpp"

bool Fan::registerHandler(Fan & obj)
{
    assert(m_jsonRpcHandler);
    cout << "Info: Fan::registerHandler pointer:" << m_jsonRpcHandler << endl;

    string name;
    string id = obj.getDeviceId();

    name.clear();
    name = name + "Fan" + "." + "set" + "." + id;
    m_jsonRpcHandlerNameList.push_back(name);
    m_jsonRpcHandler->addHander(new RpcMethod<Fan>(obj, &Fan::set, name));

    return true;
}


bool Fan::unregisterHandler(Fan & obj)
{
    assert(m_jsonRpcHandler);

    for(vector<string>::iterator it = m_jsonRpcHandlerNameList.begin(); it != m_jsonRpcHandlerNameList.end(); ++it)
    {
        m_jsonRpcHandler->deleteHandler(*it);
    }
    m_jsonRpcHandlerNameList.clear();

    return true;
}


bool Fan::set(const  Json::Value &request, Json::Value & response)
{
    bool ret;

    Json::Value params = request["params"];

    if(params["operation"].isNull())
    {
        cerr << "Error: Fan::set invalid params." << endl;
        Json::Value error;

        error["code"] = JSON_RPC_INVALID_PARAMS;
        error["message"] = "Invalid params";

        response["error"] = error;

        return false;
    }

    string operation = params["operation"].asString();
    string IRCode;
    
    if (!m_IRCodeTable[operation].size())
    {
        cerr << "Error: Fan not support operation: " << operation << endl;
        Json::Value error;

        error["code"] = JSON_RPC_FAN_NOT_SUPPORT_OPERATION;
        error["message"] = "Fan not support operation";
        response["error"] = error;

        return false;
    }

    IRCode = m_IRCodeTable[operation];
    ret = m_IRcontroller->send(IRCode);
    if (!ret)
    {
        Json::Value error;

        error["code"] = JSON_RPC_FAN_IR_SERNDER_ERROR;
        error["message"] = "Fan IR send error";
        response["code"] = error;
        
        return false;
    }

    response["result"] = true;
    return true;
}


