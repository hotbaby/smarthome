/*************************************************************************
> File Name: gearmanWorker.cpp
> Author: yy
> Mail: mengyy_linux@163.com
 ************************************************************************/

#include <cstring>

#include "gearmanWorker.hpp"

GearmanWorker::GearmanWorker(string _name)
:name(_name), host("127.0.0.1"), port(4730), timeout(0), ssl(false)
{
    context = new gearman_worker_st;
    init();
}

GearmanWorker:: GearmanWorker(string _name, string _host, in_port_t _port)
:name(_name), host(_host), port(_port), timeout(0), ssl(false)
{
    context = new gearman_worker_st;
    init();
}

GearmanWorker::GearmanWorker(string _name, string _host, in_port_t _port, int _timeout, bool _ssl)
:name(_name), host(_host), port(_port), timeout(_timeout), ssl(_ssl)
{
    context = new gearman_worker_st;
    init();
}

GearmanWorker::~GearmanWorker()
{
    gearman_worker_free(context);
    delete context;
}

bool GearmanWorker::init(void)
{

    gearman_worker_create(context);

    if (timeout > 0)
    {
        gearman_worker_set_timeout(context, timeout);
    }

    if (ssl)
    {
        gearman_worker_add_options(context, GEARMAN_WORKER_SSL);
    }

    gearman_worker_add_server(context, host.c_str(), port);
}

void GearmanWorker::addCb(gearman_worker_fn * cb, void *ctx)
{
    workerCb = cb;
    gearman_worker_add_function(context, name.c_str(), 0, workerCb, ctx);
}

void GearmanWorker::info(void)
{
    cout << "gearman worke info:" << endl;
    cout << "\t worker name: " <<  name << endl;
    cout << "\t host: " << host << endl;
    cout << "\t port " << port << endl;
    cout << "\t timeout: " << endl;
    cout << "\t ssl: " << ssl << endl;
}

void GearmanWorker::run(void)
{
    gearman_return_t ret;

    while(1)
    {
        ret = gearman_worker_work(context);
        if (gearman_failed(ret))
        {
            cerr << "Error: gearman worker work error." << endl;
        }
    }
}

/* gearman worker unit test */
#if 0

void* callback(gearman_job_st *job, void *context, size_t *resultSize, gearman_return_t *retPtr)
{
    const char *workload;
    size_t workloadSize;

    workloadSize = gearman_job_workload_size(job);
    workload = (const char*)gearman_job_workload(job);

    cout << "workload: " << workload << endl;
    cout << "workload size: " << workloadSize << endl;

    const char *tmp = "OK.";
    const char *outBuff = strdup(tmp);
    *resultSize = strlen(outBuff);
    *retPtr = GEARMAN_SUCCESS;

    return (void*)outBuff;
}

int main(int, char**)
{
    GearmanWorker worker;

    worker.addCb(callback);
    worker.run();
}
#endif

