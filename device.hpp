#ifndef _SMART_HOME_DEVICE_H
#define _SMART_HOME_DEVICE_H

#include <iostream>

using namespace std;

class SmartDevice
{
public:
    SmartDevice(string &type, string &vender, string &id)
    :m_deviceType(type), m_deviceVender(vender), m_deviceId(id)
    {}

    virtual ~SmartDevice()  {}

    void deviceInfo(void) const
    {
        cout << "device type:" << m_deviceType<< endl; 
        cout << "device vender:" << m_deviceVender<< endl;
        cout << "device id:" << m_deviceId<< endl;
    }

    string getDeviceVender(void) const
    {
        return m_deviceVender;
    }
   
    string getDeviceId(void) const 
    {    
        return m_deviceId;
    }

    string getDeviceType(void) const
    {
        return m_deviceType;
    }
    
private:
    string m_deviceType;
    string m_deviceVender;
    string m_deviceId;

};

#endif
