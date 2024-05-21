#include <stdio.h> //for basic printf commands
#include <string.h> //for handling strings
#include "freertos/FreeRTOS.h" //for delay,mutexs,semphrs rtos operations
#include "esp_system.h" //esp_init funtions esp_err_t 
#include "esp_wifi.h" //esp_wifi_init functions and wifi operations
#include "esp_log.h" //for showing logs
#include "esp_event.h" //for wifi event
#include "nvs_flash.h" //non volatile storage
#include "lwip/err.h" //light weight ip packets error handling
#include "lwip/sys.h" //system applications for light weight ip apps

const char *ssid = "Nalim";
const char *pass = "0123456789";
int retry_num=0;
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,void *event_data){
if(event_id == WIFI_EVENT_STA_START)
{
  printf("WIFI CONNECTING....\n");
}
else if (event_id == WIFI_EVENT_STA_CONNECTED)
{
  printf("WiFi CONNECTED\n");
}
else if (event_id == WIFI_EVENT_STA_DISCONNECTED)
{
  printf("WiFi lost connection\n");
  if(retry_num<5){esp_wifi_connect();retry_num++;printf("Retrying to Connect...\n");}
}
else if (event_id == IP_EVENT_STA_GOT_IP)
{
  printf("Wifi got IP...\n\n");
}
else if(event_id == WIFI_EVENT_AP_START){
    printf("WIFI AP is started...\n\n");
}
else if(event_id == WIFI_EVENT_AP_STOP){
    printf("WIFI AP is stopped...\n\n");
}
}

void wifi_connection()
{
     //                          s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_netif_init();
    esp_event_loop_create_default();     // event loop                    s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station                      s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); //     
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_sta_configuration = {
        .sta = {
            .ssid = "Nalim",
            .password = "0123456789",
            
           }
        };
    wifi_config_t wifi_ap_configuration = {
        .ap = {
            .ssid = "ESP32",
            .password = "waseem8888",
            .ssid_len = strlen("ESP32"),
            .max_connection = 3,
            .channel = 6,
            .authmode  =WIFI_AUTH_WPA2_WPA3_PSK,
        }
        };
    strcpy((char*)wifi_sta_configuration.sta.ssid, ssid);
    strcpy((char*)wifi_sta_configuration.sta.password, pass);    
    //esp_log_write(ESP_LOG_INFO, "Kconfig", "SSID=%s, PASS=%s", ssid, pass);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_configuration);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_configuration);
    vTaskDelay(1000/portTICK_PERIOD_MS);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    esp_wifi_set_mode(WIFI_MODE_APSTA);
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();
    printf( "wifi_init_softap finished. SSID:%s  password:%s",ssid,pass);
    
}

void app_main(void)
{
nvs_flash_init();
wifi_connection();
    
}