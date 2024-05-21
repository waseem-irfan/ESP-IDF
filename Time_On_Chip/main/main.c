#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "esp_sntp.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "protocol_examples_common.h"

#define TAG "NTP TIME"

void on_got_time(struct timeval *tv){
    printf("secs: %lld\n",tv->tv_sec);
    struct tm *timeInfo = localtime(&tv->tv_sec);
    setenv("TZ","ULAT-8ULAST,M3.5.0/2,M9.5.0/2",1); // setenv tz knowledge
    tzset();
    char buffer[50];
    strftime(buffer,sizeof(buffer),"%c",timeInfo); // strftime .. use any source
    ESP_LOGI(TAG,"Time is %s",buffer);
    example_disconnect();
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();
    sntp_set_time_sync_notification_cb(on_got_time);

}
