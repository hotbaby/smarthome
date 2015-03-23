/*************************************************************************
> File Name: airbox.cpp
> Author: yy
> Mail: mengyy_linux@163.com
 ************************************************************************/

#include "airbox.hpp"

using namespace std;

void* sysListenerCb(void *arg, context_t *ctx);
void* devListListnerCb(void *arg, context_t *ctx);
void* alarmListenerCb(void *arg, context_t *ctx);
void* attrListenerCb(void *arg, context_t *ctx);

void  find (Json::Value &root, const string &id, Json::Value **out)
{
    if(root.isNull())
    {
        return ;
    }

    if (root.isObject())
    {
        if (root["id"].asString() == id)
        {
            *out = &root;
            return ;
        }

         find(root["child"], id, out);
    }
    else if (root.isArray())
    {
        for(Json::Value::iterator it = root.begin(); it != root.end(); ++it)
        {
            if ((*it)["id"].asString() == id)
            {
                *out  = &(*it);
                return ;
            }

             find((*it)["child"], id, out);
        }
    }
    else
    {
        return ;
    }
}

bool Airbox::bind(const Json::Value &request, Json::Value &response )
{
    bool ret;
    Json::Value method;
    Json::Value params;
    Json::Value target;

    params = request["params"];
    target = params["target"];

    if (target.isNull() || target["type"].isNull() || target["vender"].isNull())
    {
        Json::Value error;

        error["code"] = JSON_RPC_INVALID_PARAMS;
        error["message"] = "Invalid params";

        response["error"] = error;
        return false;
    }

    Json::Value deviceJson = target;
    if (target["type"] == "AirPurifier")
    {
        string type = deviceJson["type"].asString();
        string vender = deviceJson["vender"].asString();
        string id = uuid();
        
        deviceJson["id"] = id;

        SmartDevice *device = new AirPurifier(type, vender, id, (IRSender*)this);
        m_list->push_back(device);
    }
    else
    {
        Json::Value error;

        error["code"] = JSON_RPC_AIRBOX_UNSPPORT_DEVICE;
        error["message"] = "Airbox unpport device";

        response["error"] = error;
        return false;
    }

    /* Add a new device to json list */
    Json::Value devices;
    Json::Value *airbox = 0;
    string id = this->getDeviceId();

    devices = m_deviceListJson->load();
    Json::Value &root = devices["root"];

    //TODO check whether added.
    cout << "Info: airbox id: " << id << endl;
    find(root, id, &airbox);
    if (airbox == 0)
    {
        cerr << "Error: can not find airbox device." << endl;
        assert(0);
    }

    Json::Value &child = (*airbox)["child"];
    child.append(deviceJson);

    m_deviceListJson->save(devices);

    response["result"] = true;
    return true;
}

bool Airbox::unbind(const Json::Value &request, Json::Value &response)
{
    bool ret;
    Json::Value params;
    Json::Value target;

    params= request["params"];
    target = params["target"];

    if (target.isNull() || target["id"].isNull() || target["type"].isNull() || target["vender"].isNull())
    {
        cerr << "Error: Airbox::unbind invalid params." << endl;

        Json::Value error;

        error["code"] = JSON_RPC_INVALID_PARAMS;
        error["message"] = "Invalid params";

        response["error"] = error;
        return false;
    }

    bool found = false;
    for(vector<SmartDevice*>::iterator it = m_list->begin(); it != m_list->end(); ++it)
    {
        if((*it)->getDeviceId() == target["id"].asString())
        {
            SmartDevice *tmp = *it;
            delete tmp;
            m_list->erase(it);

            found = true;
            break;
        }
    }

    if (!found)
    {
        Json::Value error;

        error["code"] = JSON_RPC_AIRBOX_UOT_FIND_DEVICE;
        error["message"] = "Airbox not find device";

        response["error"] = error;
        return false;
    }

    Json::Value *airbox = 0;
    Json::Value devices = m_deviceListJson->load();
    Json::Value &root = devices["root"];

    find(root, this->getDeviceId(), &airbox);
    if(airbox == 0)
    {
        cerr  << "Error: can not find airbox device." << endl;
        assert(0);
    }

    Json::Value &child = (*airbox)["child"];
    Json::Value modifiedChild;

    found = false;
    for(Json::Value::iterator it = child.begin(); it != child.end(); ++it)
    {
        if ((*it)["id"].asString() == target["id"].asString())
        {
            found = true;
        }
        else
        {
            modifiedChild.append(*it);
        }
    }

    if (!found)
    {
        assert(0);
    }
    (*airbox)["child"] = modifiedChild;

    m_deviceListJson->save(devices);
    
    response["result"] = true;
    return true;
}


bool Airbox::registerHandler(Airbox & obj)
{
    assert(m_jsonRpcHandler);
    cout << "Info: Airbox::registerHandler pointer:" << m_jsonRpcHandler << endl;

    string name;
    string id = obj.getDeviceId();

    name.clear();
    name = name + "Airbox" + "." + "bind" + "." + id;
    m_jsonRpcHandlerNameList.push_back(name);
    m_jsonRpcHandler->addHander(new RpcMethod<Airbox>(obj, &Airbox::bind, name));

    name.clear();
    name = name + "Airbox" + "." + "unbind" + "." + id;
    m_jsonRpcHandlerNameList.push_back(name);
    m_jsonRpcHandler->addHander(new RpcMethod<Airbox>(obj, &Airbox::unbind, name));

    name.clear();
    name = name + "Airbox" + "." + "getAttrInfo" + "." + id;
    m_jsonRpcHandlerNameList.push_back(name);
    m_jsonRpcHandler->addHander(new RpcMethod<Airbox>(obj, &Airbox::getAttrInfo, name));

    name.clear();
    name = name + "Airbox" + "." + "getDeviceList" + "." + id;
    m_jsonRpcHandlerNameList.push_back(name);
    m_jsonRpcHandler->addHander(new RpcMethod<Airbox>(obj, &Airbox::getDeviceList, name));

    return true;
}

bool Airbox::unregisterHandler(Airbox & obj)
{
    for(vector<string>::iterator it = m_jsonRpcHandlerNameList.begin(); it != m_jsonRpcHandlerNameList.end(); ++it)
    {
        m_jsonRpcHandler->deleteHandler(*it);
    }
    m_jsonRpcHandlerNameList.clear();
}

bool Airbox::getAttrInfo(const  Json::Value & request, Json::Value & response)
{
    bool ret;
    Json::Value params = request["params"];

    ret = getAllAttrInfo();
    if (!ret)
    {
        Json::Value error;

        error["code"] = JSON_RPC_AIRBOX_INTERNEL_ERROR;
        error["message"]  = "Airbox internel error";
        response["error"] = error;

        return false;
    }

    response["result"]["temperature"] = m_attrs.temperature;
    response["result"]["humidity"] = m_attrs.humidity;
    response["result"]["PM25Value"] = m_attrs.PM2Value;
    response["result"]["PM25Status"] = m_attrs.PM2Status;
    response["result"]["VOCValue"] = m_attrs.VOCValue;
    response["result"]["VOCStatus"] = m_attrs.VOCStatus;

    //response["result"] = result;
    return true;
}

bool Airbox::getDeviceList(const Json::Value &request, Json::Value &response)
{
    Json::Value params = request["params"];

    if (params["id"].isNull() || params["vender"].isNull() || params["type"].isNull())
    {
        Json::Value error;

        error["code"] = JSON_RPC_INVALID_PARAMS;
        error["message"] = "Invalid params";
        response["error"] = error;

        return false;
    }

    Json::Value devices = m_deviceListJson->load();
    Json::Value &root = devices["root"];
    Json::Value *airbox = 0;

    find(root, this->getDeviceId(), &airbox);
    if (airbox == 0)
    {
        cerr << "Error: can not find airbox." << endl;
        assert(0);
    }

    Json::Value child = (*airbox)["child"];

    Json::Value result(arrayValue);
    if (child.isNull())
    {
        response["result"] = result;
    }
    else
    {
        for(Json::Value::iterator it = child.begin(); it != child.end(); ++it)
        {
            result.append(*it);
        }
        response["result"] = result;
    }

    return true;
}

bool Airbox::send(string code)
{
    cout << "Info: call Airbox::send code: " << code << endl;
    int ret;
    context_t *devCtx = 0;
    
    if (!deviceReady())
    {
        cerr << "Error: can not find airbox." << endl;
        return false;
    }

    devCtx = ugw_new_dev_context((char*)m_id.c_str());
    if (devCtx == 0)
    {
        cerr << "Error: ugw_new_dev_context error." << endl;
        return false;
    }

    ret = ugw_dev_exec(devCtx, "60w0ZT", code.c_str());
    if (ret !=0)
    {
        cerr << "Error: ugw_dev_exec error. ret:" << ret << endl;
        ugw_free_context(devCtx);
        return false;
    }
    
    ugw_free_context(devCtx);
    return true;  
}
bool Airbox::getAllAttrInfo(void)
{
    int ret;
    string id;
    context_t *devCtx = 0;

    if (!deviceReady())
    {
        cerr << "Error: can not find airbox." << endl;
        return false;
    }

    devCtx = ugw_new_dev_context((char*)m_id.c_str());
    if (devCtx == 0)
    {
        cerr << "Error: ugw_new_dev_context error." << endl;
        return false;
    }

    ret = ugw_dev_exec(devCtx, "2000ZZ", "");
    if (ret != 0)
    {
        cerr << "Error: ugw_dev_exec error. ret: " << ret << endl;
        ugw_free_context(devCtx);
        return false;
    }
    
    string attr, value;
    for (int i = 0; i < devCtx->pair_count; i++)
    {
        //attr = devCtx->pairs[i].name;
        //value = devCtx->pairs[i].value;
        attr.assign(devCtx->pairs[i].name);
        value.assign(devCtx->pairs[i].value);

        if (!attr.compare("60w001")) //temperature
        {
            m_attrs.temperature = atoi(value.c_str()) / 10 - 30;
        }
        else if (!attr.compare("60w002")) //humidity
        {
            m_attrs.humidity = atoi(value.c_str()) / 10;
        }
        else if (!attr.compare("60w004")) //VOC status
        {
            if (!value.compare("30w001"))
            {
                m_attrs.VOCStatus = 1;
            }
            else if (!value.compare("30w002"))
            {
                m_attrs.VOCStatus = 2;
            }
            else if (!value.compare("30w003"))
            {
                m_attrs.VOCStatus = 3;
            }
            else if (!value.compare("30w004"))
            {
                m_attrs.VOCStatus = 4;
            }
            else
            {
                cerr << "Error: VOC status not identify value." << endl;
            }
        }
        else if (!attr.compare("60w007")) // PM2.5 status
        {
            if (!value.compare("30w001"))
            {
                m_attrs.PM2Status = 1;
            }
            else if (!value.compare("30w002"))
            {
                m_attrs.PM2Status = 2;
            }
            else if (!value.compare("30w003"))
            {
                m_attrs.PM2Status = 3;
            }
            else if (!value.compare("30w004"))
            {
                m_attrs.PM2Status = 4;
            }
            else
            {
                cerr << "Error:  PM2.5 status not identify value." << endl;
            }
        }
        else if (!attr.compare("60w00g")) //IR status
        {
            if (!value.compare("30w0S1"))
            {
                m_attrs.IRStatus = 1;
            }
            else if (!value.compare("30wS2"))
            {
                m_attrs.IRStatus = 2;
            }
            else
            {
                cerr << "Error: IR status not identify value." << endl;
            }
        }
        else if (!attr.compare("60w00j")) // VOC value
        {
            m_attrs.VOCValue = atoi(value.c_str());
        }
        else if (!attr.compare("60w00k")) // PM2.5 value
        {
            m_attrs.PM2Value = atoi(value.c_str());
        }
        else if (!attr.compare("60w00l")) // IR exit
        {
            if (!value.compare("30w002"))
            {
                m_attrs.IRExit = true;
            }
            else if (!value.compare("30w001"))
            {
                m_attrs.IRExit = false;
            }
            else
            {
                cerr << "Error: IR exit not identify value: " << value << endl;
            }
        }
        else if (!attr.compare("60w00m")) // formaldenhyde
        {
            if (!value.compare("30w002"))
            {
                m_attrs.formaldehydeExit = true;
            }
            else if (!value.compare("30w001"))
            {
                m_attrs.formaldehydeExit = false;
            }
            else
            {
                cerr << "Error: formaldenhyde exit not identify value." << endl;
            }
        }
        else if (!attr.compare("60w00n")) //VOC exit
        {
            if (!value.compare("30w002"))
            {
                m_attrs.VOCExit = true;
            }
            else if (!value.compare("30w001"))
            {
                m_attrs.VOCExit = false;
            }
            else
            {
                cerr << "Error: VOC exit not identify value."<< endl;
            }
        }
        else if (!attr.compare("60w00o")) // PM2.5 exit
        {
            if (!value.compare("30w002"))
            {
                m_attrs.PM2Exit = true;
            }
            else if (!value.compare("30w001"))
            {
                m_attrs.PM2Exit = false;
            }
            else
            {
                cerr << "Error:  PM2.5 exit not identify value."<< endl;
            }
        }
        else if (!attr.compare("60w00p")) // humidity exit
        {
            if (!value.compare("30w002"))
            {
                m_attrs.humidityExit = true;
            }
            else if (!value.compare("30w001"))
            {
                m_attrs.humidityExit = false;
            }
            else
            {
                cerr << "Error: humidity exit not identify value." << endl;
            }
        }
        else if (!attr.compare("60w00q")) // temperature exit
        {
            if (!value.compare("30w002"))
            {
                m_attrs.temperatureExit = true;
            }
            else if (!value.compare("30w001"))
            {
                m_attrs.temperatureExit = false;
            }
            else
            {
                cerr << "Error: temperature exit not identify value." << endl;
            }
        }
        else if (!attr.compare("60w00r")) // formaldehyde
        {
            m_attrs.formaldehyde = atoi(value.c_str());
        }
        else if (!attr.compare("60w00s")) // PM2.5 value
        {
            m_attrs.PM2Value = atoi(value.c_str());
        }
        else if (!attr.compare("60w00t")) // PM2.5 referance value
        {
            m_attrs.PM2RV = atoi(value.c_str());
        }
        else if (!attr.compare("60w0ZS")) // transparent data status
        {
            if (!value.compare("30w002"))
            {
                m_attrs.transparentDataStatus = 2;
            }
            else if (!value.compare("30w001"))
            {
                m_attrs.transparentDataStatus = 1;
            }
            else
            {
                cerr << "Error: transparent data status not identify value." << endl;
            }
        }
        else if (!attr.compare("60w0ZU"))
        {
            //do nothing.
        }
        else if (!attr.compare("60w0ZV")) //force user unbind
        {
            if (!value.compare("30w002"))
            {
                m_attrs.inForceUserUnbind = 1;
            }
            else if (!value.compare("30w001"))
            {
                m_attrs.inForceUserUnbind = 2;
            }
            else
            {
                //cerr << "Error: force user unbind not identify value: " <<  value << endl;
            }
        }
        else
        {
            // do nothing.
            //cerr << "Error: not identify value." << endl;
        }
            
    }

    ugw_free_context(devCtx);
    return true;
}

void Airbox::getAllAlarmInfo(void)
{}

void Airbox::stopAlarmUpload(void)
{}

void Airbox::startLearning(void)
{}

void Airbox::stopLearning(void)
{}

void Airbox::IRControl(void)
{}

void Airbox::info(void) const
{
    cout << "airbox info:" << endl;
    cout << "\t temperature: " << m_attrs.temperature << endl;;
    cout << "\t humidity: " << m_attrs.humidity << endl;
    cout << "\t VOCStatus: " << m_attrs.VOCStatus << endl;
}

bool Airbox::deviceReady(void)
{
    string id = getId();
    return !id.empty();
}

bool Airbox::initUGWListener(void)
{
    int ret;

    m_listener.alarm_arg = this;
    m_listener.attr_arg = this;
    m_listener.dev_list_arg = this;

    m_listener.sys_listener = sysListenerCb;
    m_listener.dev_list_listener = devListListnerCb;
    m_listener.alarm_listener = alarmListenerCb;
    m_listener.attr_listener = attrListenerCb;
    
    ret = ugw_init(&m_listener, (char*)"/root/");
    if (ret != 0)
    {
        cerr <<"Erro: ugw_init ret:" << ret << endl;
        return false;
    }

    return true;
}

void* sysListenerCb(void * arg, context_t * ctx)
{
    Airbox *device = (Airbox*)arg;

    cout << "call sysListenerCb." << endl;
}

void* devListListnerCb(void * arg, context_t * ctx)
{

    Airbox *device = (Airbox*)arg;

    for(int i = 0; i < ctx->dev_count; i++) 
    {
        cout << "\t mac: " << ctx->devs[i].device_id << endl;
        cout << "\t status: " << ctx->devs[i].connect_status << endl;
        cout << "\t type id: " << ctx->devs[i].type_id << endl;

        if(strcmp(ctx->devs[i].type_id, "101c120024000810140d00118003940000000000000000000000000000000000") == 0)
        {
            string id;
            if(ctx->devs[i].connect_status == 0)
            {
                id = "";
                device->setId(id);
            }
            else if(ctx->devs[i].connect_status == 1)
            {
                id = ctx->devs[i].device_id;
                device->setId(id);
            }
            break;
        }
    }

}

void* alarmListenerCb(void * arg, context_t * ctx)
{
    Airbox *device = (Airbox*)arg;

    cout << "alarm info:" << endl;
    cout << "\t device id: " << ctx->device_id << endl;
    for(int i = 0; i < ctx->dev_count; i++)
    {    
        cout << "\t name: " << ctx->pairs[i].name << endl;
        cout << "\t value: " << ctx->pairs[i].value << endl;
    }
}

void* attrListenerCb(void * arg, context_t * ctx)
{
    Airbox *device = (Airbox*)arg;

    cout << "attr info:" << endl;
    cout << "\t device id: " << ctx->device_id << endl;

    for(int i = 0; i < ctx->dev_count; i++)
    {
        cout << "\t name: " << ctx->pairs[i].name << endl;
        cout << "\t value: " << ctx->pairs[i].value << endl;
    }
}

/* unit test */
#if 0
int main(int argc, char **argv)
{
    Airbox airbox;

    while(1)
    {
        getchar();
        airbox.getAllAttrInfo();
        airbox.info();
    }

    return 0;
}
#endif
