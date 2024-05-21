#include <stdio.h>
#include <string.h>
#include "protocol_examples_common.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_err.h"
static const char *TAG = "REST";
    // extern const uint8_t cert[] asm("_binary_amazon_crt_start");
typedef struct chunck_payload_t{
    uint8_t *buffer;
    int buffer_index;

}chunck_payload_t;
esp_err_t on_client_data(esp_http_client_event_t * evt)
{
    switch(evt->event_id)
    {
        case HTTP_EVENT_ON_DATA:
        {
            // ESP_LOGI(TAG,"Length=%d",evt->data_len);
            // printf("%.*s\n",evt->data_len, (char *)evt->data);
            chunck_payload_t *chunck_payload = evt->user_data;
            chunck_payload ->buffer = realloc(chunck_payload->buffer,chunck_payload->buffer_index + evt->data_len + 1);
            memcpy(&chunck_payload->buffer[chunck_payload->buffer_index],(uint8_t*) evt->data, evt->data_len);
            chunck_payload->buffer_index = chunck_payload->buffer_index + evt->data_len;
            chunck_payload->buffer[chunck_payload->buffer_index] = 0;

            // printf("buffer******** %s\n",chunck_payload->buffer);
        }
        break;

        default:
        break;
    }
    return ESP_OK;
}

void fetch_quote()
{
    chunck_payload_t chunck_payload = {0};
    esp_http_client_config_t esp_http_client_config = {
        .url = "https://open-weather13.p.rapidapi.com/city/Islamabad",
        .method = HTTP_METHOD_GET,
        .event_handler = on_client_data,
        .user_data = &chunck_payload,
        // .cert_pem = (char *)cert
        };
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "X-RapidAPI-Key", "a563267425mshe11e61a3be4a76dp132763jsn8e1c82d9aa2a");
    esp_http_client_set_header(client, "X-RapidAPI-Host", "open-weather13.p.rapidapi.com");
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET status = %d, result = %s\n",
                 esp_http_client_get_status_code(client),  chunck_payload.buffer);
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    if(chunck_payload.buffer != NULL){
       free(chunck_payload.buffer); 
    }
    esp_http_client_cleanup(client);
    esp_wifi_stop();
}

void app_main(void)
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    example_connect();
    fetch_quote();
}
