#include "my_wifi.h"
uint8_t retry_num = 0;

void event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data)
{
    if(event_id == WIFI_EVENT_STA_START){
        ESP_LOGI(WIFI_TAG,"WIFI CONNECTING....");
    }
    else if (event_id == WIFI_EVENT_STA_CONNECTED){
        ESP_LOGI(WIFI_TAG,"WIFI CONNECTED...");
    }
    else if (event_id == WIFI_EVENT_STA_DISCONNECTED){
        ESP_LOGE(WIFI_TAG,"WIFI lost connection....");
        if(retry_num<5){esp_wifi_connect();retry_num++;printf("Retrying to Connect...\n");}
    }
    else if (event_id == IP_EVENT_STA_GOT_IP){
        ESP_LOGI(WIFI_TAG,"WIFI GOT IP...");
    }
}

void wifi_init(void){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t Config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&Config));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        NULL,
        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        NULL,
        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "Signum Signal",
            .password = "ntgl5273"
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}