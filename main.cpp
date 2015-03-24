/*************************************************************************
> File Name: main.cpp
> Author: yy
> Mail: mengyy_linux@163.com
 ************************************************************************/

#include <cassert>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <cstdio>

#include "common.hpp"
#include "gearmanWorker.hpp"
#include "jsonrpcHandler.hpp"
#include "deviceListJson.hpp"
#include "router.hpp"
#include "device.hpp"
#include "airbox.hpp"

using namespace std;

const char *path = "/etc/smarthome/devicelist.json";
static vector<SmartDevice*> g_deviceList;
static JsonRpcHandler g_jsonRpcHandler;
static DeviceListJson g_deviceListJson;

vector<SmartDevice*>* Router::m_list = &g_deviceList;
JsonRpcHandler* Router::m_jsonRpcHandler = &g_jsonRpcHandler;
DeviceListJson* Router::m_deviceListJson = &g_deviceListJson;

vector<SmartDevice*>* Airbox::m_list = &g_deviceList;
JsonRpcHandler* Airbox::m_jsonRpcHandler = &g_jsonRpcHandler;
DeviceListJson* Airbox::m_deviceListJson = &g_deviceListJson;

JsonRpcHandler* AirPurifier::m_jsonRpcHandler = &g_jsonRpcHandler;

void* dispatch(gearman_job_st *job, void *context, size_t *resultSize, gearman_return_t *retPtr)
{
    assert(context != NULL);

    size_t reqMsgSize;
    char * reqMsg, *resMsg;

    reqMsgSize= gearman_job_workload_size(job);
    reqMsg= (char*)gearman_job_workload(job);

    bool ret;
    string msg = reqMsg;
    Json::Value response;
    JsonRpcHandler *jsonrpcHandler = (JsonRpcHandler*)context;

#define GEARMAN_DEBUG
#ifdef GEARMAN_DEBUG
    cout << "request:" << msg << endl;
#endif

    ret = jsonrpcHandler->process(msg, response);
    msg = response.toStyledString();

#ifdef GEARMAN_DEBUG
    cout << "response:" << msg << endl;
#endif
    resMsg = strdup(msg.c_str()); 
    *resultSize = msg.size();
    *retPtr = GEARMAN_SUCCESS;

    return (void*)resMsg;
}

void help(void)
{
    cout << "usage, common options:" << endl;
    cout << "\t -f, module name" << endl;
    cout << "\t -h, host" << endl;
    cout << "\t -p, port" << endl;
    cout << endl;
}

SmartDevice* instance(string type, string vender, string id)
{
    SmartDevice *device = 0;
    static IRSender *sender = 0; // TODO, temporary program.

    cout << "Info: instance type:" << type << endl;
    if (type == "Router")
    {
        device = new Router(type, vender, id);
    }
    else if(type == "Airbox")
    {
        sender = new Airbox(type, vender, id);
        device = sender;
    }
    else if (type == "AirPurifier")
    {
        //TODO, IRSender not sure.
        device = new AirPurifier(type, vender, id, sender);
    }
    else
    {
        cerr << "Error: not support devices." << type <<endl; 
        assert(0);
    }

    return device;
}

static void traverse(const Json::Value &json, vector<SmartDevice*> &deviceList)
{
    if (json.isNull())
    {
        return;
    }

    SmartDevice *device = 0;
    if (json.isObject())
    {
        device = instance(json["type"].asString(), json["vender"].asString(), json["id"].asString());
        deviceList.push_back(device);
        traverse(json["child"], deviceList);
    }
    else if (json.isArray())
    {
        for(Json::Value::iterator it = json.begin(); it != json.end(); ++it)
        {
            device = instance((*it)["type"].asString(), (*it)["vender"].asString(), (*it)["id"].asString());
            deviceList.push_back(device);
            traverse((*it)["child"], deviceList);
        }
    }
    else
    {
        return;
    }
}

/*
  * The entry of Smart Home routine.
  * Before running the routine, you must run the ugwcore routine.
*/
int main(int argc, char **argv)
{

    const char *moduleName = "smarthome";
    const char *host = "127.0.0.1";
    in_port_t port = 4730;
    int c;

    /* Parse params */
    while(1)
    {
        c = getopt(argc, argv, "f:h:p:");
        if (c < 0)
        {
            break;
        }

        switch(c)
        {
            case 'f':
                moduleName = optarg;
                break;

            case 'h':
                host = optarg;                
                break;

            case 'p':
                port = atoi(optarg);
                break;

            default:
                help();
                return -1;                
        }
    }

    /* debug params */
    {
        cout << "params info:" << endl;
        cout << "\t module name: " << moduleName << endl;
        cout << "\t host: " << host << endl;
        cout << "\t port: " << port << endl;
        cout << endl;
    }

    Json::Value devices, root;
    
    /* Load device list from json file */
    devices = g_deviceListJson.load();
    root = devices["root"];
    assert(!root.isNull());

    traverse(root, g_deviceList);

#define DEVICE_LIST_DEBUG
#ifdef DEVICE_LIST_DEBUG
    for (vector<SmartDevice*>::iterator it = g_deviceList.begin(); it != g_deviceList.end(); ++it)
    {
        (*it)->deviceInfo();
    }
#endif

    GearmanWorker *worker = new GearmanWorker(moduleName, host, port);
    
    worker->addCb(dispatch, &g_jsonRpcHandler);
    worker->run();

    /* recycling resource */
    delete worker;

    return 0;
}
