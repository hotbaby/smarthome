#ifndef _SMART_HOME_FAN_H
#define _SMART_HOME_FAN_H

#include <cassert>
#include <iostream>
#include <map>
#include "IRDevice.hpp"
#include "jsonrpcHandler.hpp"

using namespace std;

class Fan : public SmartDevice
{
public:
    Fan(string type, string vender, string id, IRSender *sender)
    :SmartDevice(type, vender, id), m_IRcontroller(sender)
    {
        registerHandler(*this);
    }

    ~Fan()
    {
        unregisterHandler(*this);
    }

    bool set(const Json::Value &request, Json::Value &response);

private:
    bool registerHandler(Fan &obj);
    bool unregisterHandler(Fan &obj);

private:
    IRSender *m_IRcontroller;
    map<string, string> m_IRCodeTable = {
        {"power", "01010001000100F83390348E13AF358E348E12B113AF13B013B013AF11B23482A2348E348E12B2348E338F13B013AF11B213AF13AF14AF3482A4328F348F13AF348F338F12B014AF13AF14AF13AF13AF3582A4328F339012B0338F358E12B012B112B012B014AF12B03582A4328F338F13B0338F358E12B012B013B012B113B011B13282A6328F338F14AF338F358F12B011B112B112B014AF11B13382A5348E348E13B0348E348E14AF13AF14AF13AF13AF14AF3482A4348E348E12B1348E348E14AF13AF12B113AF13AF14AF3482A4348E348E13AF358E348E14AF13AF13AF14B012B013B03482A3338E348E14AF348F348F13AF12B112B013AF13B013AF34DB"},
        {"speed", "01010001000100C6348F338F13AF358E348E11B213AF13AF14AF13AF358E1382C4338F348E13B0348E348E14AF13AF14AF13AF13AF358E1382C5338E358E13AF358E348E13AF14AF13AF14AF13AF348E1482C5338E348F13AF348E358E13AF12B113AF13AF14AF348E1482C5338E348E14AF348E358E12B013AF14AF13AF14AF348E1382C6338E338F14AF338F358E12B012B112B111B113B0338F1282C53390329013B03290338F13B012B014AF12B012B0348F1282C6328F358E12B03390338F12B015AE12B012B112B0338F141F"},
        {"swing", "0101000100010143358E338F14AE348F338F12B114AF329012B114AE12B11482C23390329012B13290338F13B014AE348F12B014AF14AE1482C4328F348F12B0358E338F12B015AE338F13B114AE13B01382C43290329114AE338F348F12B012B1338F12B015AE14AF1182C7328F339114AE338F348F12B014AF338F12B013B014AE1482C5328F338F15AF3290348F12B014AF338F12B013B014AE1482C5328F338F13B0338F339012B015AD349011B114AF14AE1482C43290338F13B0338F339012B014AE358E12B014AF14AF1382C4338F338F12B23290338F13B011B1339012B012B013B01282C6348E338F12B1338F338F13B012B0339111B111B112B11182C63290338F12B03490329011B212B0329012B112B012B11282C53291329011B1348F329012B112B0338F13B012B012B11282C5338F338F14AF338F338F13B012B0358E12B012B013B01225"},
        {"timer", "01010001000100F8358E348E14AE358E348E14AF14AE13B0348E13AF14AF1382C5348D348E14AF348E339013AF14AE14B0348E13B013AF1382C43390338F12B0358E348E13B013AF13AF358E13AF13B01382C5338E358E13AF348E358E13AF14AF13AF349012B012B01482C4338E339112B0338F348F13AF12B113AF348E14AF13AF1282C7338E339013AF348E358E13AF13B013AF348E14B013AF1282C6338E348E14AF348F329113AF12B013B0348E12B113AF1382C6328F348E13B0348E339013AF13AF14AF348E12B113AF1382C5348E348E12B1348E348E14AF13AF12B1348F12B013B01282C5338E348F13AF348E358E13AF13B013AF348E14B012B013BB"},
        };

    static JsonRpcHandler *m_jsonRpcHandler;
    vector<string> m_jsonRpcHandlerNameList;
};
#endif
