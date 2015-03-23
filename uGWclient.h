#ifndef __UGW_CLIENT__
#define __UGW_CLIENT__


#ifdef __cplusplus
extern "C" {
#endif

//设备结构体，包含设备基本信息和状态
typedef struct{
    char    device_id[18];//设备ID
    char    ip[16];//设备IP
    int     connect_status;//0:离线，1:上线
    char    type_id[65];//
	int		connect_type;//连接类型 详见接口文档
}devinfo_t;

//属性/报警信息，一个长度不限制的键值对
typedef struct{
	char *name;
	char *value;
}pair_t;

//上下文结构体，用于传入传出参数
typedef struct{
	char device_id[18];	//设备ID
	int subd_type;		//子设备类型
	int subd_no;		//子设备号
	int group_name;	//组命令的名字
	int group_count;	//组命令包含键值的个数
	int pair_count;	//属性/报警个数
	int dev_count;		//设备个数
    pair_t *pairs;		//属性/报警列表
	devinfo_t *devs;		//设备列表
}context_t; 
//listener函数定义
typedef void * (*listener_handle_f)(void *arg, context_t *context);

//ugw监听器
typedef struct{
	listener_handle_f  sys_listener;
	listener_handle_f  dev_list_listener;//变化设备列表在context->devs中返回
	listener_handle_f  alarm_listener;//报警在context->pairs中返回
	listener_handle_f  attr_listener;//属性在context->pairs中返回
	void              *dev_list_arg;
	void              *attr_arg;
	void              *alarm_arg;
}ugw_listener_t;

//启动UGW API
int ugw_init(ugw_listener_t *ugw_listener, char *config_path);
//释放ugw资源
void ugw_destroy(void);

//上下文API，主要用于传出参数，辅助传入参数，并负责释放传出参数
context_t* ugw_new_context();
context_t* ugw_new_dev_context(char *device_id);
//跟设备相关的组命令上下文
context_t* ugw_new_dev_group_context(char *device_id, int group_name,int group_count);
//跟子设备相关的组命令上下文
context_t* ugw_new_subdev_group_context(char *device_id, int subdev_type,int subdev_no,int group_name,int group_count);

//释放使用后的context，每一个context只能使用一次
void ugw_free_context(context_t *context);
//获取设备列表API
int ugw_get_dev_list(context_t *context);//设备列表在context->devs中返回
//配置设备入网
int ugw_smartconfig(char *ssid, char *pwd);
//执行命令API
int ugw_dev_exec(context_t *context, ...);//所有键值对通过变参传入

//获取版本号
int ugw_version();

#ifdef __cplusplus
}
#endif

#endif
