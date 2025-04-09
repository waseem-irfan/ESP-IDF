idf_component_register(SRCS "wifi_connect.c" "my_wifi.c"
                    INCLUDE_DIRS "inc"
                    REQUIRES esp_wifi esp_netif log)