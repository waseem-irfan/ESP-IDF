set(COMPONENT_SRCS "src/MPU6050.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_REQUIRES
    esp32-I2Cdev
)

register_component()
