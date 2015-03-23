/*************************************************************************
> File Name: sqliteTest.cpp
> Author: yy
> Mail: mengyy_linux@163.com
 ************************************************************************/

#include <cassert>
#include <iostream>
#include <sqlite3.h>

using namespace std;

class SqliteTest
{
public:

    typedef int (*Callback) (void*, int, char**, char**);

    SqliteTest(string name)
    :m_name(name){}

    bool open()
    {
        m_error = sqlite3_open(m_name.c_str(), &m_db);
        if (m_error!= SQLITE_OK)
        {
            cerr << "Error: sqlite3_open error." << endl;
            cerr << sqlite3_errstr(m_error);
            assert(0);
        }

        return true;
    }

    void close()
    {
        m_error = sqlite3_close(m_db);
        if ( m_error != SQLITE_OK)
        {
            cerr << "Error: sqlite3_close error." << endl;
            cerr << sqlite3_errstr(m_error) << endl;
            assert(0);
        }
    }

    int exec(const string &sql, Callback cb = 0)
    {
        char *errmsg;

        m_error = sqlite3_exec(m_db, sql.c_str(), cb, 0, &errmsg);
        if (m_error != SQLITE_OK)
        {
            cerr << "Error: sqlite3_exec error." << endl;
            cerr << sqlite3_errstr(m_error) << endl;
            return false;
        }

        return true;
    }


private:
    const string m_name;
    sqlite3 *m_db;
    int m_error;
};

static int callback(void *data, int argc, char **argv, char **colName)
{
    for(int i = 0; i < argc; i++)
    {
        fprintf(stderr, "%s = %s ", colName[i], argv[i] ? argv[i] : "NULL");
    }

    return 0;
}

int main(int argc, char **argv)
{
    SqliteTest sqliteTest("./IRCode.db");

    sqliteTest.open();

   string sql = "SELECT * from AirPurifier WHERE Operation = 'power';";

    sqliteTest.exec(sql, callback);

    sqliteTest.close();

    return 0;
}

