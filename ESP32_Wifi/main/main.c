#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_http_client.h"
#include "data.h"

static void wifi_event_handler(void* event_handler_arg,esp_event_base_t event_base,int32_t event_id,void* event_data){
    switch(event_id){
        case WIFI_EVENT_STA_START:
        printf("Wifi Connecting....\n");
        break;
        case IP_EVENT_STA_GOT_IP:
        printf("Got IP.....\n");
        break;
        case WIFI_EVENT_STA_DISCONNECTED:
        printf("Wifi Disconnected ...\n");
        break;
        case WIFI_EVENT_STA_CONNECTED:
        printf("Wifi Connected...\n");
        break;
        default:
        break;
    }
}

// esp_err_t client_event_handler(esp_http_client_event_t *evt){
//     switch(evt->event_id){
//         case HTTP_EVENT_ON_DATA:
//         printf("HTTP_EVENT_ON_DATA: %.*s\n",evt->data_len,evt->data);
//         break;
//         default:
//         break;
//     }
// }

void app_main(void)
{
    nvs_flash_init();
// Phase 1: Init LwIP
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initialization = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initialization);
// Phase 2: WIFI Configuration
    esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID, wifi_event_handler,NULL);
    esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,wifi_event_handler, NULL);
     wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS
        }
     };
     esp_wifi_set_config(ESP_IF_WIFI_STA,&wifi_configuration);
// Phase 3: WFIF Start
    esp_wifi_start();
// Phase 4: WIFI Connect
    esp_wifi_connect();

    vTaskDelay(5000/portTICK_PERIOD_MS);

}
