#ifndef _SMART_HOME_IR_DEVICE_H
#define _SMART_HOME_IR_DEVICE_H

#include <iostream>
#include "device.hpp"

using namespace std;

class IRSender:public SmartDevice
{
public:

    IRSender(string &type, string &vender, string &id)
    :SmartDevice(type, vender, id) {}

    virtual ~IRSender() {}

    /* send IR code */
    virtual bool send(string code) = 0;

};

#endif
