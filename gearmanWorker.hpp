
#ifndef _GEARMAN_WORKER_H
#define _GEARMAN_WORKER_H

#include <iostream>
#include <libgearman/gearman.h>

using namespace std;

class GearmanWorker
{
public:
    GearmanWorker(string _name = "smarthome");
    GearmanWorker(string _name, string _host, in_port_t _port);
    GearmanWorker(string _name, string _host, in_port_t _port, int _timeout, bool ssl);
    ~GearmanWorker();

    void addCb(gearman_worker_fn *cb, void *ctx = NULL);
    void info(void);
    void run(void);

private:
    bool init(void);

private:
    gearman_worker_st *context;
    gearman_worker_fn *workerCb;

    string name;
    string host;
    in_port_t port;
    int timeout;
    bool ssl;
};

#endif
