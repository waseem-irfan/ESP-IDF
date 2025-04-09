idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES my_wifi nvs_flash my_mqtt)
