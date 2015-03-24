#ifndef _JSON_RPC_HANDLER_
#define _JSON_RPC_HANDLER_

#include <map>
#include "json/json.h"

using namespace std;
using namespace Json;

typedef enum
{
    JSON_RPC_PARSE_ERROR = -32700,
    JSON_RPC_INVALID_REQUEST = -32600,
    JSON_RPC_METHOD_NOT_FOUND = -32601,
    JSON_RPC_INVALID_PARAMS = -32602,
    JSON_RPC_INTERNAL_ERROR = -32603,

    /* smart home internel error */
    JSON_RPC_SMART_CONFIG_PASSWD_ERROR = -32000,
    JSON_RPC_UOT_FIND_DEVICE = -32001,
    JSON_RPC_NOT_SUPPORT_DEVICE = -32003,

    /* airbox internal error */
    JSON_RPC_AIRBOX_INTERNEL_ERROR = -32100,
    JSON_RPC_AIRBOX_UNSPPORT_DEVICE = -32101,
    JSON_RPC_AIRBOX_UOT_FIND_DEVICE = -32102,
    JSON_RPC_AIRBOX_HAS_ADDED = -32103,

    /* airpurifier internal error */
    JSON_RPC_AIRPURIFIER_NOT_SPPORT_OPERATION = -32200,
    JSON_RPC_AIRPURIFIER_IR_SEND_ERROR = -32201,

    /* fan internal error */
    JSON_RPC_FAN_NOT_SUPPORT_OPERATION = -32301,
    JSON_RPC_FAN_IR_SERNDER_ERROR = -32302,


}JsonRpcErrorCode;

class CallBackMethod
{
public:
    virtual  ~CallBackMethod(){};
    
    virtual bool call(const Json::Value &request, Json::Value &response) = 0;
    virtual string getName(void) const = 0;
};

template<typename T>
class RpcMethod: public CallBackMethod
{
public:

    typedef bool (T::*Method)(const Json::Value& request, Json::Value& response);
    
 
    RpcMethod(T &_obj, Method _method, const string &_name)
    :obj(&_obj), method(_method), name(_name)
    {}

    virtual bool call(const Json::Value &request, Json::Value &response)
    {
        return (obj->*method)(request, response);
    }
    
    virtual string getName(void) const
    {
        return name;
    }

private:
    T *obj;
    string name;
    Method method;    
};

class JsonRpcHandler
{
public:

    void addHander(CallBackMethod *method);
    void deleteHandler(const string &name);

    bool process(const Json::Value &request, Json::Value &response);
    bool process(const string msg, Json::Value &response);
    bool check(const Json::Value &root, Json::Value &error);

private:
    CallBackMethod* lookup(const string &name);

private:
     vector<CallBackMethod*> methods;
};

#endif
