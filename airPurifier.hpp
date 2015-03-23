
#ifndef _SMART_HOME_AIR_PURIFIER_H
#define _SMART_HOME_AIR_PURIFIER_H

#include <cassert>
#include <iostream>
#include <map>
#include "IRDevice.hpp"
#include "jsonrpcHandler.hpp"

class AirPurifier:public SmartDevice
{
public:

    AirPurifier(string type, string vender, string id, IRSender *_IRDevice)
    :SmartDevice(type, vender, id), m_IRcontroller(_IRDevice) 
    {
        registerHandler(*this);
    }

    ~AirPurifier()
    {
        unregisterHandler(*this);
    }

    bool set(const Json::Value &request, Json::Value &reponse);

    void powerOn(void) {}
    void powerOff(void) {}

    void childLockOn(void) {}
    void childLockOff(void) {}

    void changeWindSpeed(void) {}
    void changeMode() {}
    
    void changeTimer(void) {}

    /* load IR codes from database */
    void load(void) {}

    /* sync data to database */
    void save(void){}

private:
    bool registerHandler(AirPurifier &obj);
    bool unregisterHandler(AirPurifier &obj);

private:
    void init(void)
    {
        /*init air purifier attributes */
        windSpeed = -1;
        runningMode = MODE_INVALID;
        powerStatus = STATUS_INVALID;
        childLockStatus = STATUS_INVALID;
    }

    typedef enum
    {
        MODE_INVALID,
        MODE_MANUAL, 
        MODE_ATUO,
        MODE_SLEEPING,
        MODE_MAX,
    }RunningMode;

    typedef enum
    {
        STATUS_INVALID,
        STATUS_ON, 
        STATUS_OFF,
        STATUS_MAX,
    }Status;

private:
    string m_DBPath;
    map<string, string> m_IRCodeTable = {
        {"power", "010100010001004B026F81B3199419941A941A9419941A941A941895199519C2199519C2199519C11AC219C21AC11A941A9419941A94199519C219C2199519C219C318C21AC11AC219941A941A8DDC026ED818D7"},
        {"speed", "0101000100010051026D81B5179519951A9418951995199517961896199517C4199517C4199517C419C119C319C21A94189519951A941895199519C2199519C219C319C219C219C318C21A94198DDD026CD9189EC1026CD9189A"},
        {"auto", "0101000100010045026E81B417951995199518951995199419941A941A9419C21A9419C21A9419C21AC11AC219941A94199518951995199518C319C11AC219C21AC11AC219C219C2199519951806"},
        {"sleep", "010100010001004B026F81B31994199518951995199518951995199518941AC219941AC219941AC219C21AC11A941A9419941A941A9419941A9419C21AC219C219C21AC219C21AC11AC219941AB9B80221D91894"},
        {"timer", "0101000100010051026E81B417961995179618961995179618951A94189519C3189519C3189519C318C31AC1199519C21995199517961896199517C419C2189619C119C318C31AC119C31895198DDF026DD9199EC3026ED71AA2"}
    };

    IRSender *m_IRcontroller;

    /* air purifier attributes */
    int windSpeed;
    RunningMode runningMode;
    Status powerStatus;
    Status childLockStatus;
    
    static JsonRpcHandler *m_jsonRpcHandler;
    vector<string> m_jsonRpcHandlerNameList;

};

#endif
