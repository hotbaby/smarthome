#ifndef _SMART_HOME_AIRBOX_H
#define _SMART_HOME_AIRBOX_H

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "json/json.h"
#include "uGWclient.h"
#include "device.hpp"
#include "IRDevice.hpp"
#include "jsonrpcHandler.hpp"
#include "deviceListJson.hpp"
#include "common.hpp"
#include "airPurifier.hpp"
#include "fan.hpp"

class Airbox:public IRSender
{
public: 
    Airbox(string &type, string &vender, string &id)
    :IRSender(type, vender, id)
    {
        initUGWListener();
        registerHandler(*this);
    }

    ~Airbox()
    {
        unregisterHandler(*this);
        ugw_destroy();
    }

    bool bind(const Json::Value &request, Json::Value &response);
    bool unbind(const Json::Value &request, Json::Value &response);
    bool getAttrInfo(const Json::Value &request, Json::Value &response);
    bool getDeviceList(const Json::Value &request, Json::Value &response);

    virtual bool send(string code);

    bool getAllAttrInfo(void);
    void getAllAlarmInfo(void);
    void stopAlarmUpload(void);
    void startLearning(void);
    void stopLearning(void);
    void IRControl(void);

    void info(void) const;
    bool deviceReady(void);
    bool initUGWListener(void);

    string getId(void)
    {
        return m_id;
    }

    void setId(string &id)
    {
        m_id = id;
    }

    /* airbox callbacks */
    //friend void* sysListenerCb(void *arg, context_t *ctx);
    //friend void* devListListnerCb(void *arg, context_t *ctx);
    //friend void* alarmListenerCb(void *arg, context_t *ctx);
    //friend void* attrListenerCb(void *arg, context_t *ctx);
    
    /* airbox attributes */
    struct Attr
    {
        bool temperatureExit;      // true: exit, false: not exit.
        int temperature;    // range 0 ~ 900, equation = (temperature-300)/10.

        bool humidityExit;  // ture: exit, false: not exit.
        int humidity;       // range 0 ~ 1000, equation = humidity/10.

        bool VOCExit;       // true: exit, false: not exit.
        int VOCValue;       // range 0 ~ 1023.
        int VOCStatus;      // range 1 ~ 4.

        bool PM2Exit;       // true: exit, false: not exit.
        int PM2Value;       // range 0 ~ 500.
        int PM2Status;      // range 1 ~ 4.
        int PM2Sample;      //range: 0 ~ 1023.
        int PM2RV;          //PM2.5 reference value, range: 0 ~ 1023

        bool IRExit;        // true: exit, false: not exit.
        int IRStatus;       // 1: constrol status 2:learning status.

        bool formaldehydeExit; // true: exit, false: not exit.
        int formaldehyde;   // range: 1 ~ 14.

        int transparentDataStatus; //1: normal, 2: error.
        int inForceUserUnbind; //1: be force user unbind, 2: not be user unbind.    
    };
private:
    bool registerHandler(Airbox &obj);
    bool unregisterHandler(Airbox &obj);

private:

    /* ugw sdk context*/
    ugw_listener_t m_listener;
    string m_id;
    Attr m_attrs;

    static vector<SmartDevice*> *m_list;
    static DeviceListJson *m_deviceListJson;
    static JsonRpcHandler *m_jsonRpcHandler;

    vector<string> m_jsonRpcHandlerNameList;
};
#endif
