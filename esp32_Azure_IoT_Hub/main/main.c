#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_connect.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char *baltimore_root_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDdzCCAl+gAwIBAgIEbJc7szANBgkqhkiG9w0BAQsFADBoMQswCQYDVQQGEwJV\n" \
"UzETMBEGA1UEChMKQmFsdGltb3JlMRwwGgYDVQQLExNTZWN1cml0eSBTZXJ2aWNl\n" \
"czEkMCIGA1UEAxMbQmFsdGltb3JlIEN5YmVyVHJ1c3QgUm9vdCBDQTAeFw0wMTA1\n" \
"MjQwMDQwMDBaFw0zNjAxMjgwMDQwMDBaMGgxCzAJBgNVBAYTAlVTMRMwEQYDVQQK\n" \
"EwpCYWx0aW1vcmUxHDAaBgNVBAsTE1NlY3VyaXR5IFNlcnZpY2VzMSQwIgYDVQQD\n" \
"ExtCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290IENBMIIBIjANBgkqhkiG9w0BAQEF\n" \
"AAOCAQ8AMIIBCgKCAQEA7ahE5M2vMz1s3S7gu5a2cZ6DgRf6skLepAAbHXnZK7Cz\n" \
"Tt4A9fZ3sZgGSvAN+KrdAHMp8kIawfdcN5FwlvPtqMkPb4cTuByn8rwp5/u0WkYQ\n" \
"DF/fpznG1+q2n9kG0GYOYiXk+f5tqUIM5U8iJzBCUynUDifqNoyWrz1b6WcNRllr\n" \
"6klcGO6MKD+zK6nYfjfdZV/XY97wE7r7ypztfq0E7GzVvE/+Pmb0iJuZaystP7A1\n" \
"cQIbjUCzMcjG8rfvV0gyS3IymRlD9Wjh/eM+ZDxQK9O1cJciVQ9Uvms/N+DbmH6v\n" \
"yCDqzWmVtNfaTRKmrEl94uMlG2xrDlvf2ZZMEpO3RYsHTF4NnwIDAQABo0IwQDAP\n" \
"BgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTAephojYn7qwVkDBF9qn1luMrMTjAN\n" \
"BgkqhkiG9w0BAQsFAAOCAQEAw3T4A3WFhvzKk59L+lKed1qErzA/BQnoCNQcYafA\n" \
"7tNQY92czFLcBSfC8hJH/Jr+33me3D8mYQhP6pyI7VU7Iw8cpLLf3q7hEYoCWl1u\n" \
"lERmu4DnvJSjW5ZJ1KkxFZULyP3ePzdi1W+05msIWoZ5JEW8jj1xHDjFvjql0+fq\n" \
"6H48MwYTqbHckfKr3ZP27blC4fa23wdd3vTYJLKh9CVpS1ZTRHJF+5b2JvJMQN6X\n" \
"r9Ci2wrYgGMoCGxjbKmPRnGcRPpJ1EFh+a2FZDaWDi73lEoUks27uvhd1CDBkfHD\n" \
"yWjYwQpBFwZJbAA8eT6gzZjwhrxvEGyzoQ7PHMmqcPMkzA==\n" \
"-----END CERTIFICATE-----\n";



static char *TAG = "MQTT";
static void mqtt_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void test_send_messages(void *param);
static esp_mqtt_client_handle_t client;

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_connect_init();
    ESP_ERROR_CHECK(wifi_connect_sta("Signum Signal", "ntgl5273", 10000));
    esp_mqtt_client_config_t esp_mqtt_client_config = {
        .broker.address.uri = "mqtts://MSRR-IoT-Hub.azure-devices.net",
        .broker.address.port = 8883,
        .broker.verification.certificate = baltimore_root_cert, 
        .credentials.username = "MSRR-IoT-Hub.azure-devices.net/esp32_id",
        .credentials.client_id = "esp32_id",
        // .credentials.authentication.password = "njv+G1cl771WE8mZALLaor3FIf0exRcYHHbUKNp+NsA=" 
    };
    client = esp_mqtt_client_init(&esp_mqtt_client_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    xTaskCreate(test_send_messages, "test_send_messages", 1024 * 2, NULL, 5, NULL);
}

static void mqtt_event_handler(void *event_handler_arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "msrr_esp32", 1);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("topic: %.*s\n", event->topic_len, event->topic);
        printf("message: %.*s\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "ERROR %s", strerror(event->error_handle->esp_transport_sock_errno));
        break;
    default:
        break;
    }
}

int mqtt_send(const char *topic, const char *payload)
{
    return esp_mqtt_client_publish(client, topic, payload, strlen(payload), 1, 0);
}

static void test_send_messages(void *param)
{
    int count = 0;
    char message[50];
    while (true)
    {
        sprintf(message, "hello from ESP32 count %d", count++);
        mqtt_send("msrr_esp32", message);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}