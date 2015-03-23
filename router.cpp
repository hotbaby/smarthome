
#include "router.hpp"

bool Router::registerHandler(Router &obj)
{
    assert(m_jsonRpcHandler);
    cout << "Info: Router::registerHandler pointer:" << m_jsonRpcHandler<< endl;

    string name;
    string id = obj.getDeviceId();

    name.clear();
    name = name + "Router" + "." + "bind";
    m_jsonRpcHandlerNameList.push_back(name);
    m_jsonRpcHandler->addHander(new RpcMethod<Router>(obj, &Router::bind, name));

    name.clear();
    name = name + "Router" + "." + "unbind";
    m_jsonRpcHandlerNameList.push_back(name);
    m_jsonRpcHandler->addHander(new RpcMethod<Router>(obj, &Router::unbind, name));

    name.clear();
    name = name + "Router" + "." + "smartConfig";
    m_jsonRpcHandlerNameList.push_back(name);
    m_jsonRpcHandler->addHander(new RpcMethod<Router>(obj, &Router::smartConfig, name));

    name.clear();
    name = name + "Router" + "." + "getAirboxList";
    m_jsonRpcHandlerNameList.push_back(name);
    m_jsonRpcHandler->addHander(new RpcMethod<Router>(obj, &Router::getAirboxList, name));

    return true;
}

bool Router::unregisterHandler(Router &obj)
{
    for(vector<string>::iterator it = m_jsonRpcHandlerNameList.begin(); it != m_jsonRpcHandlerNameList.end(); ++it)
    {
        m_jsonRpcHandler->deleteHandler(*it);
    }
    m_jsonRpcHandlerNameList.clear();
}

bool Router::bind(const Json::Value &request, Json::Value &response)
{
    Json::Value target;
    Json::Value method, params;
    bool ret;

    //TODO
    method = request["method"];
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

    if (target["type"].isNull() || target["mac"].isNull())
    {
        cerr << "Error:  Router::bind params error." << endl;

        Json::Value error;

        error["code"] = JSON_RPC_INVALID_PARAMS;
        error["message"]= "Invalid params";
        response["error"] = error;
        return false;
    }

    /* Check Whether device added */
    Json::Value devices = m_deviceListJson->load();
    Json::Value &root = devices["root"];
    Json::Value &child = root["child"];

    for(Json::Value::iterator it = child.begin(); it != child.end(); ++it)
    {
        if ((*it)["mac"] == target["mac"])
        {
            cerr << "Error: device has added." << endl;

            Json::Value error;

            error["code"] = JSON_RPC_AIRBOX_HAS_ADDED;
            error["message"] = "Airbox has added";
            response["error"] = error;
            return false;
        }
    }

    /**
      * TODO,  Only support Airbox.
      * Add new device to list.
      */
    if (target["type"] == "Airbox")
    {
        string type = target["type"].asString();
        string vender = target["vender"].asString();
        string id = uuid();
        cout << "Info: uuid generate unique id: " << id << endl;

        target["id"] = id;

        SmartDevice *device = new Airbox(type,vender, id);
        
        m_list->push_back(device);

        cout << "Info: create Airbox object." << endl;
        device->deviceInfo();
    }
    else
    {
        cerr << "Error: unspport type." << endl;

        Json::Value error;

        error["code"] = JSON_RPC_NOT_SUPPORT_DEVICE;
        error["message"] = "Not support device";
        response["error"] = error;
        return false;
    }

    /* Add a new device to json list */
    child.append(target);
    m_deviceListJson->save(devices);

    response["result"] = true;
    return true;
}

bool Router::smartConfig(const  Json::Value & request, Json::Value & response)
{

    Json::Value method, params;
    string ssid, password;
    vector<devinfo_t> devices;
    bool ret;

    method = request["method"];
    params = request["params"];


    ssid = params["ssid"].asString();
    password = params["password"].asString();

    if (ssid.empty() || password.empty())
    {
        Json::Value error;

        error["code"] = JSON_RPC_INVALID_PARAMS;
        error["message"] = "Invalid params";
        response["error"] = error;

        return false;
    }

    ret = smartConfig(ssid, password, devices);
    if (!ret)
    {
        Json::Value error;

        error["code"] = JSON_RPC_SMART_CONFIG_PASSWD_ERROR;
        error["message"] = "Smart config password error.";
        response["error"]  = error;

        return false;
    }

    if (devices.size() > 0)
    {
        for(vector<devinfo_t>::iterator it = devices.begin(); it != devices.end(); ++it)
        {
            /**
              * TODO
              * Just support Airbox for the moment.
              */
            Json::Value *item = new Json::Value;
            (*item)["mac"] = it->device_id;
            (*item)["ip"] = it->ip;
            //(*item)["type"] = it->type_id;
            (*item)["type"] = "Airbox";
            response["result"].append(*item);
        }
    }
    else
    {
        response["result"] = "";
    }

    return true;
}

bool Router::smartConfig(const string &ssid, const string &password, vector<devinfo_t> &devices)
{
    int ret;
    ugw_listener_t listener;

    memset(&listener, 0, sizeof(listener));
    ret=ugw_init(&listener, (char *)"/root/");
    if (ret)
    {
        cerr << "Error: ugw_init error." << endl;
        return false;
    }

    ret = ugw_smartconfig((char*)ssid.c_str(), (char*)password.c_str());
    if (ret)
    {
        cerr << "Error: ugw_smartconfig error." << endl;
        ugw_destroy();
        return false;
    }

    context_t *context = ugw_new_context();
    if (!context)
    {
        ugw_destroy();
        return false;
    }

    int num = 10;
    do
    {
        ret=ugw_get_dev_list(context);
        if (context->dev_count==0)
        {
            continue;
        }
        else
        {
            for(int i = 0; i < context->dev_count; ++i)
            {
                
                devices.push_back( context->devs[i]);
                cout << "Info: devie list:" << endl;
                cout << "\t id: " << context->devs[i].device_id << endl;
                cout << "\t ip: " << context->devs[i].ip << endl;
                cout << "\t connect status: " << context->devs[i].connect_status<< endl;
                cout << "\t type id: " << context->devs[i].type_id << endl;
                cout << "\t connect type: " << context->devs[i].connect_type << endl;
            }
            break;
        }
    
        sleep(1);
    }
    while(num > 0);

    ugw_free_context(context);
    ugw_destroy();

    if (num > 0)
    {
        return true;
    }
    else
    {
        cerr << "Error: get device list timeout." << endl;
        return false;
    }
}

void traverse(const Json::Value &json, vector<string> &v)
{
    if (json.isNull())
    {
        return;
    }

    if (json.isObject())
    {
        v.push_back(json["id"].asString());
        traverse(json["child"], v);
    }
    else if (json.isArray())
    {
        for(Value::iterator it = json.begin(); it != json.end(); ++it)
        {
            v.push_back((*it)["id"].asString());
            traverse((*it)["child"], v);
        }
    }
    else
    {
        return;
    }
}

bool Router::unbind(const  Value & request, Json :: Value & response)
{
    Json::Value params;
    Json::Value source, target;

    params= request["params"];
    source = params["source"];
    target = params["target"];

    if (target.isNull() || target["id"].isNull() || target["type"].isNull() || target["vender"].isNull())
    {
        Json::Value error;

        error["code"] = JSON_RPC_INVALID_PARAMS;
        error["message"] = "Invalid params";

        response["error"] = error;
        return false;
    }

    /* Delete device from json list */
    Json::Value devices;
    Json::Value deleteDvices, targetDevices;

    devices = m_deviceListJson->load();
    Json::Value &root = devices["root"];
    Json::Value &child = root["child"];

    bool found = false;
    for (Json::Value::iterator it = child.begin(); it != child.end(); ++it)
    {
        if ((*it)["id"] == target["id"])
        {
            found = true;
            deleteDvices.append(*it);
        }
        else
        {
            targetDevices.append(*it);
        }
    }

    if (!found)
    {
        Json::Value error;

        error["code"] = JSON_RPC_UOT_FIND_DEVICE;
        error["message"] = "Not find device";
        response["error"] = error;
        return false;
    }

    root["child"] = targetDevices;
    m_deviceListJson->save(devices);

    /* Delete device from the list */
    vector<string> deleteList;

    traverse(deleteDvices, deleteList);

#define ROUTER_DEBUG
#ifdef ROUTER_DEBUG
    cout << "Debug: associated device list:" << endl;
    for (vector<SmartDevice*>::iterator it = m_list->begin(); it != m_list->end(); ++it)
    {
        (*it)->deviceInfo();
        cout << "pointer address: " << *it << endl;
        cout << endl;
    }
#endif

    for (vector<string>::iterator it1 = deleteList.begin(); it1 !=deleteList.end(); ++it1)
    {
        for(vector<SmartDevice*>::iterator it2 = m_list->begin(); it2 != m_list->end(); ++it2)
        {
            if (*it1 == (*it2)->getDeviceId())
            {
                SmartDevice *tmp = *it2;
                delete tmp;
                m_list->erase(it2);
                break;
            }
        }
    }

    response["result"] = true;
    return true;
}

bool Router::getAirboxList(const  Json::Value & request, Json::Value & response)
{
    Json::Value devices = m_deviceListJson->load();
    Json::Value &root = devices["root"];
    Json::Value &child = root["child"];

    if (child.isNull())
    {
        cout << "Info: device list is null." << endl;
        Json::Value result(arrayValue);
        response["result"] = result;
    }
    else
    {
        Json::Value result(arrayValue);

        for (Json::Value::iterator it = child.begin(); it != child.end(); ++it)
        {
            Json::Value item;

            item["id"] = (*it)["id"];
            item["vender"] = (*it)["vender"];
            item["type"] = (*it)["type"];
            item["mac"] = (*it)["mac"];

            result.append(item);
        }
        response["result"] = result;
    }

    return true;
}

