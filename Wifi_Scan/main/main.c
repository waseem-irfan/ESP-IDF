#include <stdio.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define MAX_APs 20
void Wifi_Connection(){
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t wifiConfig = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifiConfig)); 
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}
static char *getAuthModeName(wifi_auth_mode_t auth_mode)
{
  char *names[] = {"OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "MAX"};
  return names[auth_mode];
}
void app_main(void)
{
    Wifi_Connection();

    wifi_scan_config_t wifiScan = {
        .ssid = 0,  
        .bssid = 0,
        .channel = 0,
        .show_hidden = true
    };
    ESP_ERROR_CHECK(esp_wifi_scan_start(&wifiScan,true));
    wifi_ap_record_t wifi_records[MAX_APs];
    uint16_t maxRecords = MAX_APs; 
    esp_wifi_scan_get_ap_records(&maxRecords,wifi_records);

    //Displaying the wifi Records
    printf("Found %d access points:\n", maxRecords);
    printf("\n");
    printf("               SSID              | Channel | RSSI |   Auth Mode \n");
    printf("----------------------------------------------------------------\n");
    for (int i = 0; i < maxRecords; i++)
    printf("%32s | %7d | %4d | %12s\n", (char *)wifi_records[i].ssid, wifi_records[i].primary, wifi_records[i].rssi, getAuthModeName(wifi_records[i].authmode));
    printf("----------------------------------------------------------------\n");

}
