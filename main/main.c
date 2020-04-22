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
//My include
#include "st7789v.h"
#include "wifi.h"

#define HOST_IP_ADDR			"192.168.3.35"
#define HOST_PORT					1234

static char *TAG = "TCP";
static int tcpsock = 0;

void tcpInit(void)
{
	int ret = 0;
	struct sockaddr_in server_addr={0};
	struct sockaddr_in client_addr;
	
	tcpsock = socket(AF_INET,SOCK_STREAM, IPPROTO_IP);//初始化套接字 IP类型:IPV4 (TCP)流格式套接字/面向连接的套接字
	if(tcpsock < 0){
		ESP_LOGE(TAG,"socket error:%d",tcpsock);
		return;
	}
	else
		ESP_LOGI(TAG,"socket created:%d",tcpsock);

	//配置服务端口
	server_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);//IP地址
	server_addr.sin_family = AF_INET;//IPV4
	server_addr.sin_port = htons(HOST_PORT);//端口
	//配置本地端口
	client_addr.sin_family = AF_INET;//IPV4
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
	client_addr.sin_port = htons(0);//端口

	ret = bind(tcpsock,(struct sockaddr *)&client_addr,sizeof(client_addr));//绑定
	if(ret != 0){
		ESP_LOGE(TAG,"bind error:%d",ret);
		return;
	}
	ESP_LOGI(TAG,"connect to %s:%u",HOST_IP_ADDR,HOST_PORT);
	ret = connect(tcpsock,(const struct sockaddr *)&server_addr,sizeof(server_addr));//连接TCP服务
	if(ret != 0){
		ESP_LOGE(TAG,"connect error:%d",ret);
		return;
	}
	else
		ESP_LOGI(TAG,"connect success.");
	send(tcpsock,"tcp test",8,0);
}


void app_main(void)
{
	//Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  lcd_init();//初始化显示
	wifiSTAInit();//初始化WiFi连接
	vTaskDelay(5000 / portTICK_RATE_MS);//最笨的方法等待WiFi连接并获取IP地址
	tcpInit();//初始化TCP
	while (1)
	{
	  vTaskDelay(100);
	}
}
