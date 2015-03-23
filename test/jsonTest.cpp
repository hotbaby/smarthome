/*************************************************************************
> File Name: jsonTest.cpp
> Author: yy
> Mail: mengyy_linux@163.com
 ************************************************************************/

#include "json/json.h"

#include <list>
#include <fstream>
#include <iostream>
#include <cassert>

using namespace std;
using namespace Json;

class Test
{
public:
    void print(const Json::Value &root)
    {
        cout << "Info: call Test::print." << endl;
    }

private:
};

void func(Json::Value &response)
{
    Json::Value error;

    error["error"] = -32600;
    error["message"] = "Invalid request";
    response["error"] = error;
}

typedef void (*cb)(void);

template<typename T>
void func(const Value &t, list<string> &l)
{
    l.push_back(t["id"].asString());
}

void traverse(const Value &t, list<string> &l)
{
    if (t.isNull())
    {
        return;
    }

    if (t.isObject())
    {
        l.push_back(t["id"].asString());
        traverse(t["child"], l);
    }
    else
    {
        for(Value::iterator it = t.begin(); it != t.end(); ++it)
        {
            l.push_back((*it)["id"].asString());
            traverse((*it)["child"], l);
        }
    }
}

void find(Value &root, const string &id, Value **target)
{
    if (root.isNull())
    {
        cout << "null" << endl;
        return ;
    }

    if (root.isObject())
    {
        cout << "object" << endl;
        cout << root["id"].asString() << endl;
        if (root["id"].asString() == id)
        {
            *target = &root;
            return ;
        }

        find(root["child"], id, target);
    }
    else if (root.isArray())
    {
        cout << "array" << endl;
        for(Value::iterator it = root.begin(); it != root.end(); ++it)
        {
            cout << (*it)["id"].asString() << endl;
            if ((*it)["id"].asString() == id)
            {
                *target = &(*it);
                return ;
            }
            cout << (*it)["id"].asString() << endl;

            find((*it)["child"], id, target);
        }
    }
    else
    {
        cout << "null" << endl;
        return ;
    }
}

int main(int, char**)
{

    {
        const char *buffer =
        "{                 \
        \"jsonrpc\":\"2.0\",                \
        \"method\":\"airbox.smartconfig\",  \
        \"params\":{                        \
        \"id\": \"\",                   \
        \"vender\":\"haier\",           \
        \"ssid\":\"YiWangWuJi\",        \
        \"password\":\"2wsx1qaz\"       \
        },                                   \
        \"id\":\"100\"                      \
        }";

        Reader reader;
        Value request;

        /* request message */
        if (reader.parse(buffer, request))
        {
            if (request.isMember("method"))
            {
                cout << "method exit." << endl;
            }
            else
            {
                cout << "method is not exit." << endl;
            }

            string jsonrpcVersion  = request["jsonrpc"].asString();
            string method = request["method"].asString();
            Value params = request["parmas"];
            string id = request["id"].asString();

            {
                cout << "request info:" << endl;
                cout << "\t jsonrpc vertsion: " << jsonrpcVersion << endl;
                cout << "\t method: " << method << endl;
                cout << "\t id: " << id << endl;
            }
        }
        else
        {
            cerr << "Error: json parse error." << endl;
        }

        /* response message */
        Value response;
        Value result;

        response["jsonrpc"] = request["jsonrpc"];
        response["id"] = request["id"];

        result["temperature"] = 26;
        result["humidity"] = 16;
        response["result"] = result;

        cout << "response info:" << endl;
        cout << response.toStyledString() << endl;
    }

    {
        string path = "./json.txt";
        ifstream input(path);
        Reader reader;
        Value root;

        if (reader.parse(input, root))
        {
            cout << root.toStyledString() << endl;
        }
        else
        {
            cerr << "Error: parse error." <<endl;
        }
        input.close();

        ofstream output(path);

        root["test"] = "jsontest";
        string buffer = root.toStyledString();
        output.write(buffer.c_str(), buffer.size());

        output.close();

        Value defaultValue, result;
        result = root.get("test", defaultValue);
        cout << "default value: " << defaultValue.toStyledString() << endl;
        cout << "result: " << result.toStyledString() << endl;
    }

    {
        const char *buffer =
        "{                 \
        \"jsonrpc\":\"2.0\",                \
        \"method\":\"airbox.smartconfig\",  \
        \"params\":{                        \
        \"id\": \"\",                   \
        \"vender\":\"haier\",           \
        \"ssid\":\"YiWangWuJi\",        \
        \"password\":\"2wsx1qaz\"       \
        },                                   \
        \"id\":\"100\"                      \
        }";   

        Reader reader;
        Value root;
        string ssid, password;

        reader.parse(buffer, root);
        ssid = root["params"]["ssid"].asString();
        cout << ssid << endl;

        password = root["params"]["passwd"].asString();
        if (password.empty())
        {
            cout << "empty" << endl;
        }
        else
        {
            cout << "not empty" << endl;
        }
    }

    {
        cout << "@@@" << endl;
        string path = "./json.txt";
        ifstream input(path);
        Reader reader;
        Value json;

        if (reader.parse(input, json))
        {
            cout << json.toStyledString() << endl;
        }
        else
        {
            cerr << "Error: parse error." <<endl;
        }

        input.close();

        Value root, child, item;
        Value defaultValue;

        root = json["root"];
        child = root["child"];

        for(int i = 0; i < child.size(); i++)
        {
            //if (child[i]["id"] == "333333")
            Value tmp;
            if (child[i]["id"] == "333333")
            {
                //cout << child[i].toStyledString() << endl;
                //child[i] = tmp;
            }
        }

        {
            Value del;
            Value target;
            for(Value::iterator it = child.begin(); it != child.end(); ++it)
            {
                if ((*it)["id"] != "333333")
                {
                    target.append(*it);
                }
                else
                {
                    del.append(*it);
                }
            }

            cout << "target size: " << target.size() << endl;
            for(Value::iterator it = target.begin(); it != target.end(); ++it)
            {
                cout << (*it).toStyledString() << endl;
            }

            cout << "delete size: " << del.size() << endl;
            for(Value::iterator it = del.begin(); it != del.end(); ++it)
            {
                cout << (*it).toStyledString() << endl;
            }
        }

        for (Value::iterator it = child.begin(); it != child.end(); ++it)
        {
            //cout << (*it).toStyledString() << endl;
            //(*it).clear();
        }

        //cout << "array size: " << child.size() << endl;
        for (Value::iterator it = child.begin(); it != child.end(); ++it)
        {
            //cout << (*it).toStyledString() << endl;
        }
        cout << "@@@" << endl;
    }

    // traverse the tree
    {
        Reader reader;
        Value json;
        ifstream input("./json.txt");

        if (reader.parse(input,json))
        {
            cout << json.toStyledString() << endl;

            //traverse tree
            Value root = json["root"];
            list<string> l;
            traverse(root, l);

            for(list<string>::iterator it = l.begin(); it != l.end(); ++it)
            {
                cout << *it << ' ';
            }
            cout << endl;
        }        
        else
        {
            assert(0);
        }
    }

    {
        const char *buffer =
        "{                 \
        \"jsonrpc\":\"2.0\",                \
        \"method\":\"airbox.smartconfig\",  \
        \"params\":{                        \
        \"id\": \"\",                   \
        \"vender\":\"haier\",           \
        \"ssid\":\"YiWangWuJi\",        \
        \"password\":\"2wsx1qaz\"       \
        },                                   \
        \"id\":\"100\"                      \
        }";

        Reader reader;
        Value request;

        /* request message */
        if (reader.parse(buffer, request))
        {
            if (request.isMember("method"))
            {
                cout << "method exit." << endl;
            }
            else
            {
                cout << "method is not exit." << endl;
            }

            string jsonrpcVersion  = request["jsonrpc"].asString();
            Value method = request["method"];

            if (method.isObject())
            {
                cout << "true" << endl;
            }
            else
            {
                cout << "false" << endl;
            }
        }
        else
        {
            cerr << "Error: json parse error." << endl;
        }
    }

    {
        const char *buffer =
        "{                 \
        \"jsonrpc\":\"2.0\",                \
        \"method\":\"airbox.smartconfig\",  \
        \"child\":{                        \
        \"id\": \"200\",                   \
        \"vender\":\"haier\",           \
        \"ssid\":\"YiWangWuJi\",        \
        \"password\":\"2wsx1qaz\"       \
                },                                   \
        \"id\":\"100\"                      \
        }";

        Json::Value json;
        Json::Reader reader;

        if (reader.parse(buffer, json))
        {
            Json::Value test = json["test"];
            cout << "test:" << test.toStyledString() << endl;

            string id = "200";
            Value *target = NULL;
            bool ret;
            find(json, id, &target);

            cout << "result: " << ret << endl;
            if (target)
            {
                cout << (*target).toStyledString() << endl;
                (*target)["id"] = "300";
            }
            else
            {
                cout << "not found" << endl;
            }
            cout << json.toStyledString() << endl;
        }
    }

    {
        cout << "start" << endl;
        Value json, *out = 0;
        Reader reader;
        string id = "0178ee78a61e4d99a2a93c35e267d5dc";

        ifstream input("../devicelist.json");
        assert(input.good());

        if (reader.parse(input, json))
        {
            cout << json.toStyledString() << endl;
            find(json["root"], id, &out);
            cout << out << endl;
        }
        else
        {
            cerr << "Error: Json::Reader parse error. " << endl;
            assert(0);
        }
        input.close();
        cout << "end" << endl;    
    }

    return 0;
}
