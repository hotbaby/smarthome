#ifndef _SMART_HOME_ROUTER_H
#define _SMART_HOME_ROUTER_H

#include <vector>
#include <uuid/uuid.h>
#include <unistd.h>
#include <cstring>

#include "json/json.h"
#include "device.hpp"
#include "deviceListJson.hpp"
#include "jsonrpcHandler.hpp"
#include "common.hpp"
#include "uGWclient.h"
#include "airbox.hpp"

using namespace std;

class Router: public SmartDevice
{
public:

    Router(string type, string vender, string id)
    : SmartDevice(type, vender, id)
    {
        /* Note, id can not be null */
        m_jsonRpcHandlerNameList.clear();
        registerHandler(*this);
    }

    ~Router()
    {
        unregisterHandler(*this);
    }

    bool bind(const Json::Value &request, Json::Value &response);

    bool unbind(const Json::Value &request, Json::Value &response);

    bool getAirboxList(const Json::Value &request, Json::Value &response);

    bool smartConfig(const Json::Value &requet, Json::Value &response);
    bool smartConfig(const string &ssid, const string &password, vector<devinfo_t> &macs);

    static vector<SmartDevice*>* getList(void)
    {
        return m_list;
    }

    static DeviceListJson* getListJson(void)
    {
        return m_deviceListJson;
    }

    static JsonRpcHandler* getJsonRpcHander(void)
    {
        return m_jsonRpcHandler;
    }

private:
    bool registerHandler(Router &obj);
    bool unregisterHandler(Router &obj);

private:
    static vector<SmartDevice*> *m_list;
    static DeviceListJson *m_deviceListJson;
    static JsonRpcHandler *m_jsonRpcHandler;

    vector<string> m_jsonRpcHandlerNameList;

};

#endif
