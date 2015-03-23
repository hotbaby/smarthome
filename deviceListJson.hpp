#ifndef _DEVICE_LIST_JSON_H
#define _DEVICE_LIST_JSON_H

#include <iostream>
#include <fstream>
#include <cassert>
#include <json/json.h>

using namespace std;

class DeviceListJson
{
public:
    DeviceListJson(const string &path = "./devicelist.json")
    :m_path(path)
    {
        load();
    }

    Json::Value load(void)
    {
        cout << "Info: call DeviceListJson::load function." << endl;

        if (!m_root.isNull())
        {
            return m_root;
        }

        ifstream input(m_path);
        assert(input.good());

        if (m_reader.parse(input, m_root))
        {
        }
        else
        {
            cerr << "Error: Json::Reader parse error. " << endl;
            assert(0);
        }
        input.close();

        return m_root;
    }

    void save(void)
    {
        ofstream output(m_path);
        string json;

        json = m_root.toStyledString();
        output.write(json.c_str(), json.size());        
        output.close();
    }

    void save(Json::Value root)
    {
        m_root = root;
        save();
    }

    void info() const
    {
        cout << "Device list info:" << endl;
        cout << "\t file path: " << m_path << endl;
        cout << "\t devices list: " << m_root.toStyledString() << endl;
    }

private:
    string m_path;    
    Json::Reader m_reader;
    Json::Value m_root;
};

#endif
