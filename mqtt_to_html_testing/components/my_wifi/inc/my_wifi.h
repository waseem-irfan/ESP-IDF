#ifndef MY_WIFI_H
#define MY_WIFI_H
#include <stdint.h>
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_log.h"

#define WIFI_TAG "WIFI"

void wifi_init(void);
void event_handler(void* arg, esp_event_base_t event_base,
    int32_t event_id, void* event_data);
#endif