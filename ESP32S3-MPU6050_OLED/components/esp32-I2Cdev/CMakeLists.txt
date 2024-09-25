set(COMPONENT_SRCS "src/I2Cdev.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_REQUIRES
    driver
)

register_component()
# idf_component_register(SRCS "I2Cdev.cpp"
#                     INCLUDE_DIRS "include"
#                     REQUIRES driver)
