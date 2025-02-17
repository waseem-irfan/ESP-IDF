#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "protocol_examples_common.h"
#include "esp_ota_ops.h"

#define TAG "OTA"

SemaphoreHandle_t ota_semaphore;

const int software_version = 82;
#define OTA_URL "http://172.19.144.253:8070/OTA_Updates.bin"

esp_err_t client_event_handler(esp_http_client_event_handle_t *evt) {
    return ESP_OK;
}

void run_ota(void *params) {
    while (1) {
        xSemaphoreTake(ota_semaphore, portMAX_DELAY);
        ESP_LOGI(TAG, "OTA Invoking");

        ESP_ERROR_CHECK(nvs_flash_init());
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        ESP_ERROR_CHECK(example_connect());

        esp_http_client_config_t config = {
            .url = OTA_URL,
            .event_handler = client_event_handler,
            .timeout_ms = 10000, 
        };

        esp_http_client_handle_t client = esp_http_client_init(&config);
        if (client == NULL) {
            ESP_LOGE(TAG, "Failed to initialize HTTP client");
            vTaskDelete(NULL);
        }

        if (esp_http_client_open(client, 0) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open HTTP connection");
            esp_http_client_cleanup(client);
            vTaskDelete(NULL);
        }

        esp_ota_handle_t ota_handle;
        const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);

        if (!update_partition) {
            ESP_LOGE(TAG, "No OTA partition found!");
            esp_http_client_cleanup(client);
            vTaskDelete(NULL);
        }

        ESP_ERROR_CHECK(esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle));

        char buffer[1024];
        int total_bytes_written = 0;
        int read_bytes;

        while ((read_bytes = esp_http_client_read(client, buffer, sizeof(buffer))) > 0) {
            if (esp_ota_write(ota_handle, buffer, read_bytes) != ESP_OK) {
                ESP_LOGE(TAG, "Error writing to OTA partition");
                esp_http_client_cleanup(client);
                esp_ota_end(ota_handle);
                vTaskDelete(NULL);
            }
            total_bytes_written += read_bytes;
        }

        if (read_bytes < 0) {
            ESP_LOGE(TAG, "Error reading OTA file");
            esp_http_client_cleanup(client);
            esp_ota_end(ota_handle);
            vTaskDelete(NULL);
        }

        ESP_LOGI(TAG, "OTA update downloaded (%d bytes), finalizing...", total_bytes_written);

        if (esp_ota_end(ota_handle) == ESP_OK && esp_ota_set_boot_partition(update_partition) == ESP_OK) {
            ESP_LOGI(TAG, "OTA update successful! Restarting...");
            vTaskDelay(pdMS_TO_TICKS(2000));
            esp_restart();
        } else {
            ESP_LOGE(TAG, "OTA update failed!");
        }

        esp_http_client_cleanup(client);
        vTaskDelete(NULL);
    }
}

void on_button_pushed(void *params) {
    xSemaphoreGiveFromISR(ota_semaphore, pdFALSE);
}

void app_main(void) {
    ESP_LOGI(TAG, "ESP32 New Feature added");
    ESP_LOGI("SOFTWARE_VERSION", "we are running %d", software_version);

    gpio_config_t gpioConfig = {
        .pin_bit_mask = 1ULL << GPIO_NUM_0,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&gpioConfig);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_0, on_button_pushed, NULL);

    ota_semaphore = xSemaphoreCreateBinary();
    xTaskCreate(run_ota, "run_ota", 1024 * 8, NULL, 2, NULL);
}
