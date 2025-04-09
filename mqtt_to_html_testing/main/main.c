#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "my_mqtt.h"
#include "wifi_connect.h"
// #include "my_wifi.h"

void app_main(void){
    // NVS init
    nvs_flash_init();

    wifi_connect_init();
    esp_err_t err = wifi_connect_sta("Signum Signal", "ntgl5273", 10000);
    // wifi_init();

    // mqtt_start should run the wifi_API
    mqtt_start();
    // Creating Task for Sending Localization Data
    xTaskCreate(test_send_messages, "test_send_messages", 1024 * 2, NULL, 5, NULL);
}
