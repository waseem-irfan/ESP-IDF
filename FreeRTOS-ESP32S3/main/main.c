#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t receiverHandler1 = NULL;
static TaskHandle_t receiverHandler2 = NULL;

void sender(){
    while(1){
    xTaskNotifyGive(receiverHandler1);
    xTaskNotifyGive(receiverHandler2);
    xTaskNotifyGive(receiverHandler1);
    xTaskNotifyGive(receiverHandler2);
    xTaskNotifyGive(receiverHandler1);
    xTaskNotifyGive(receiverHandler2);
    vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void receiver1(){
    while(1){
        int count = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        ESP_LOGI("Task Notification 1: ","Received Notification %d", count);
    }

}
void receiver2(){
    while(1){
        int count = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        ESP_LOGI("Task Notification 2: ","Received Notification %d", count);
    }

}

void app_main(void)
{
    xTaskCreate(&receiver1, "Receiver 1", 1024*2, NULL, 2, &receiverHandler1);
    xTaskCreate(&receiver2, "Receiver 2", 1024*2, NULL, 2, &receiverHandler2);
    xTaskCreate(&sender, "Sender", 1024*2, NULL, 2, NULL);


}
