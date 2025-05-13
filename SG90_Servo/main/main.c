#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "SERVO_MOTOR"

#define SERVO_PIN GPIO_NUM_25

uint32_t angle_to_duty_cycle(uint8_t angle){
    if(angle > 180) angle = 180;
    // Map angle (0 degree -> 0.5ms, 180 degree -> 2.5ms)
    float pulse_width = 0.5 + (angle / 180.0) *2;
    // Convert pulse width to duty cycle (12-bit resolution, 50Hz)
    uint32_t duty = (pulse_width/ 20.0) * 4096;

    return duty;
}

void Servo_Task(void *params){

    // Configure PWM Timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE, // Low-speed mode (better stability)
        .timer_num = LEDC_TIMER_0, // Timer 0 for PWM
        .duty_resolution = LEDC_TIMER_12_BIT, // 12-bit resolution -> 4096 levels
        .freq_hz = 50,  // 50 Hz for SG90
        .clk_cfg = LEDC_AUTO_CLK // Auto Select clock source
    };
    ledc_timer_config(&ledc_timer);

    // Configure PWM channel
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,      // start with 0 duty cycle
        .gpio_num = SERVO_PIN,  // GPIO for Output
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&ledc_channel);

    for(;;)
    {
        for(int angle = 0; angle <= 50; angle += 10){
            ESP_LOGI(TAG, "Moving to %d degree\n", angle);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, angle_to_duty_cycle(angle));
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        for(int angle = 40; angle >= 10; angle -= 10){
            ESP_LOGI(TAG, "Moving to %d degrees\n", angle);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, angle_to_duty_cycle(angle));
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }


}



void app_main(void)
{
    ESP_LOGI(TAG, "Control Servo Motor SG90 Example");
    xTaskCreate(Servo_Task, "Servo Task", 2048, NULL, 5, NULL);

}
