
#include "airPurifier.hpp"

bool AirPurifier::set(const Json::Value &request, Json::Value &response)
{
    bool ret;

    Json::Value params = request["params"];

    if(params["operation"].isNull())
    {
        cerr << "Error: AirPurifier::set invalid params." << endl;
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
        cerr << "Error: AirPurifier not support operation: " << operation << endl;
        Json::Value error;

        error["code"] = JSON_RPC_AIRPURIFIER_UNPPORT_OPERATION;
        error["message"] = "AirPurifier not support operation";
        response["error"] = error;

        return false;
    }

    IRCode = m_IRCodeTable[operation];
    ret = m_IRcontroller->send(IRCode);
    if (!ret)
    {
        Json::Value error;

        error["code"] = JSON_RPC_AIRPURIFIER_IR_SEND_ERROR;
        error["message"] = "IR send error";
        response["code"] = error;
        
        return false;
    }

    response["result"] = true;
    return true;
}

bool AirPurifier::registerHandler(AirPurifier & obj)
{
    assert(m_jsonRpcHandler);
    cout << "Info: Airbox::registerHandler pointer:" << m_jsonRpcHandler << endl;

    string name;
    string id = obj.getDeviceId();

    name.clear();
    name = name + "AirPurifier" + "." + "set" + "." + id;
    m_jsonRpcHandlerNameList.push_back(name);
    m_jsonRpcHandler->addHander(new RpcMethod<AirPurifier>(obj, &AirPurifier::set, name));

    return true;
}

bool AirPurifier::unregisterHandler(AirPurifier & obj)
{
    assert(m_jsonRpcHandler);

    for(vector<string>::iterator it = m_jsonRpcHandlerNameList.begin(); it != m_jsonRpcHandlerNameList.end(); ++it)
    {
        m_jsonRpcHandler->deleteHandler(*it);
    }
    m_jsonRpcHandlerNameList.clear();

    return true;
}