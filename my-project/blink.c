// /**
//  * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
//  *
//  * SPDX-License-Identifier: BSD-3-Clause
//  */

// #include "pico/stdlib.h"
// #include "pico/cyw43_arch.h"

// int main() {
//     stdio_init_all();
//     if (cyw43_arch_init()) {
//         printf("Wi-Fi init failed");
//         return -1;
//     }
//     while (true) {
//         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
//         sleep_ms(250);
//         cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
//         sleep_ms(250);
//     }
// }

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/regs/clocks.h"
#include "hardware/pwm.h"

// Input
#define INFRARED_DIGITAL_PIN 3
#define SOUND_DIGITAL_PIN 4

// Output
#define SOUND_LED_PIN 28
#define INFRARED_LED_PIN 26
#define BUZZER_PIN 27
#define SERVO_PIN 16
#define SERVO_LED_PIN 17

// Function to set servo position
void setServoPosition(unsigned int angle) {
    unsigned int duty_cycle = (angle * 2000 / 180 + 500) * 65535 / 20000;
    pwm_set_gpio_level(SERVO_PIN, duty_cycle);
}

void setMillis(int servoPin, float millis) {
    pwm_set_gpio_level(servoPin, (millis/20000.0f) * 39062.0f);
}

void setServo(int servoPin, float startMillis) {
    gpio_set_function(servoPin, GPIO_FUNC_PWM);
    unsigned int slice_num = pwm_gpio_to_slice_num(servoPin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 64.0f);
    pwm_config_set_wrap(&config, 39062.0f);
    pwm_init(slice_num, &config, true);
    setMillis(servoPin, startMillis);
}

int main() {

    stdio_init_all();
    adc_init();

    // Initialize the pins 
    gpio_init(INFRARED_DIGITAL_PIN);
    gpio_init(SOUND_DIGITAL_PIN);
    gpio_init(SOUND_LED_PIN);
    gpio_init(INFRARED_LED_PIN);
    gpio_init(BUZZER_PIN);
    gpio_init(SERVO_LED_PIN);

    gpio_set_dir(INFRARED_DIGITAL_PIN, GPIO_IN);
    gpio_set_dir(SOUND_DIGITAL_PIN, GPIO_IN);
    gpio_set_dir(SOUND_LED_PIN, GPIO_OUT);
    gpio_set_dir(INFRARED_LED_PIN, GPIO_OUT);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_set_dir(SERVO_LED_PIN, GPIO_OUT);

    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    gpio_set_function(SERVO_LED_PIN, GPIO_FUNC_PWM);

    // Figure out which slice we just connected to the pin
    unsigned int slice_num = pwm_gpio_to_slice_num(SERVO_LED_PIN);

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();

    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f); // Why 4.f here? Is this a "resolution"-thing?
                                         
    pwm_init(slice_num, &config, true);

    slice_num = pwm_gpio_to_slice_num(SERVO_PIN); //Get new slice number for each pin.
    pwm_init(slice_num, &config, true);

    while (1) {
        unsigned int sound_sensor_value = gpio_get(SOUND_DIGITAL_PIN);
        unsigned int infrared_sensor_value = gpio_get(INFRARED_DIGITAL_PIN);

        // printf("Infrared Sensor Reading:%d\n", infrared_sensor_value);
        printf("Sound Sensor Reading:%d\n", sound_sensor_value);

        // Note: Sensor output 0 when it detects an object infront of it which in our case would be the hand
        if (sound_sensor_value == 0) {
            gpio_put(SOUND_LED_PIN, 1);  // Turn on the LED
            gpio_put(BUZZER_PIN, 1);  // Turn on the BUZZER
        } else {
            gpio_put(SOUND_LED_PIN, 0);  // Turn off the LED
            gpio_put(BUZZER_PIN, 0);  // Turn off the BUZZER
        }

        if (infrared_sensor_value == 0) {
            gpio_put(INFRARED_LED_PIN, 1);  // Turn off the LED
            // setServo(SERVO_PIN, 2400); // Rotate Servo Motor 180
            pwm_set_gpio_level(SERVO_PIN, 128 * 128);
            pwm_set_gpio_level(SERVO_LED_PIN, 255 * 255);
        }
        else {
            gpio_put(INFRARED_LED_PIN, 0);  // Turn off the LED
            // setServo(SERVO_PIN, 400); // Rotate Servo Motor Back 180
            pwm_set_gpio_level(SERVO_PIN, 255 * 255);
            pwm_set_gpio_level(SERVO_LED_PIN, 0 * 0);
        }

        sleep_ms(1);
    }

    return 0;
}
