#ifndef _TCP_H_
#define _TCP_H_

#define HOST_IP_ADDR			"192.168.3.2"
#define HOST_PORT					1234

void tcpInit(void);
void tcpTask(void *param);
#endif