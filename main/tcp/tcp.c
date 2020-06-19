#include "tcp.h"

#include <stdio.h>
#include "string.h"
//ESP and FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
//LWIP
#include "lwip/ip4_addr.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/err.h"
#include "lwip/netdb.h"

static char *TAG = __FILE__;
static int tcpsock = 0;

void tcpInit(void)
{
	int ret = 0;
	struct sockaddr_in server_addr={0};
	//struct sockaddr_in client_addr;
	
	tcpsock = socket(AF_INET,SOCK_STREAM, IPPROTO_IP);//初始化套接字 IP类型:IPV4 (TCP)流格式套接字/面向连接的套接字
	if(tcpsock < 0){
		ESP_LOGE(TAG,"socket error: %d",tcpsock);
		return;
	}
	else
		ESP_LOGI(TAG,"socket created: %d",tcpsock);

	//配置服务端口
	server_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);//IP地址
	server_addr.sin_family = AF_INET;//IPV4
	server_addr.sin_port = htons(HOST_PORT);//端口
	//配置本地端口
	/*client_addr.sin_family = AF_INET;//IPV4
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
	client_addr.sin_port = htons(0);//端口

	ret = bind(tcpsock,(struct sockaddr *)&client_addr,sizeof(client_addr));//绑定
	if(ret != 0){
		ESP_LOGE(TAG,"bind error:%d",ret);
		return;
	}*/
	ESP_LOGI(TAG,"connect to %s:%u",HOST_IP_ADDR,HOST_PORT);
	ret = connect(tcpsock,(const struct sockaddr *)&server_addr,sizeof(server_addr));//连接TCP服务
	if(ret != 0){
		ESP_LOGE(TAG,"connect error: %d",ret);
		return;
	}
	else
		ESP_LOGI(TAG,"connect success.");
	xTaskCreate(tcpTask,"tcp task",4096,NULL,configMAX_PRIORITIES,NULL);
}


void tcpTask(void *param)
{
	param = param;
	char *test_data="ESP32 TCP client test.\r\n";
	char recv_buff[1024];
	int len;
	send(tcpsock,test_data,strlen(test_data),0);
	while(1)
	{
		len = recv(tcpsock,recv_buff,sizeof(recv_buff),0);
		if(len > 0)
		{
			send(tcpsock,recv_buff,len,0);
			recv_buff[len] = '\0';
			ESP_LOGI(TAG,"recv: %s",recv_buff);
			ESP_LOGI(TAG,"send: %s",recv_buff);
		}
		else if(len < 0)
		{
			ESP_LOGE(TAG,"recv error: %d",len);
			close(tcpsock);//关闭套接字
			vTaskDelete(NULL);//删除本任务
		}
	}
}


