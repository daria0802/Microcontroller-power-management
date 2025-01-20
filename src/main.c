/**
 * IMP
 * @brief Demonstrates transitions between power modes (Run, Light Sleep, Deep Sleep) on ESP32.
 * @author Kinash Daria xkinas00
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "esp_timer.h"  

#define LED_PIN GPIO_NUM_2  // Built-in LED on GPIO 2

/**
 * @brief Blinks the LED.
 * @param times Number of times to blink the LED.
 * @param delayOn Duration (ms) the LED stays ON.
 * @param delayOff Duration (ms) the LED stays OFF.
 */
void blinkLED(int times, int delayOn, int delayOff) {
    for (int i = 0; i < times; i++) {
        gpio_set_level(LED_PIN, 1); // Turn LED on
        vTaskDelay(pdMS_TO_TICKS(delayOn));
        gpio_set_level(LED_PIN, 0); // Turn LED off
        vTaskDelay(pdMS_TO_TICKS(delayOff));
    }
}

/**
 * @brief Configures wake-up sources for Light Sleep mode.
 *        Wake-up source: RTC Timer (10 seconds).
 */
void configure_wakeup_sources_light_sleep() {
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL); 
    esp_sleep_enable_timer_wakeup(10 * 1000000);           
}

/**
 * @brief Configures wake-up sources for Deep Sleep mode.
 *        Wake-up source: RTC Timer (5 seconds).
 */
void configure_wakeup_sources_deep_sleep() {
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL); 
    esp_sleep_enable_timer_wakeup(5 * 1000000);           
}

/**
 * @brief Simulates CPU workload in Run Mode to demonstrate CPU activity.
 */
void simulate_cpu_workload() {
    printf("Simulating CPU workload...\n");
    uint64_t start_time = esp_timer_get_time(); 

    // Perform a dummy CPU-intensive task
    volatile int result = 0;
    for (int i = 0; i < 1000000; i++) {
        result += i * 2;
    }

    uint64_t end_time = esp_timer_get_time(); 
    printf("CPU workload completed. Time taken: %llu microseconds. Result: %d\n",
           (end_time - start_time), result);
}

/**
 * @brief Logs CPU utilization of all tasks.
 */
void log_cpu_utilization() {
    printf("Logging CPU utilization...\n");

    char *runtime_stats_buffer = malloc(1024);
    if (runtime_stats_buffer == NULL) {
        printf("Error: Failed to allocate memory for runtime stats.\n");
        return;
    }

    // Retrieve and print task runtime statistics
    vTaskGetRunTimeStats(runtime_stats_buffer);
    printf("Task Runtime Statistics:\n%s\n", runtime_stats_buffer);

    free(runtime_stats_buffer); 
}

/**
 * @brief Executes Run Mode tasks.
 */
void run_mode_activity() {
    printf("Entering Run Mode...\n");

    blinkLED(1, 300, 300); // Indicate Run Mode with a single blink
    simulate_cpu_workload(); 
    log_cpu_utilization();  

    vTaskDelay(pdMS_TO_TICKS(2000)); // Delay for 2 seconds
}

/**
 * @brief Transitions to Light Sleep mode.
 */
void enter_light_sleep() {
    printf("Entering Light Sleep Mode...\n");

    blinkLED(5, 200, 200); // Indicate transition to Light Sleep

    configure_wakeup_sources_light_sleep(); 

    esp_light_sleep_start(); // Enter Light Sleep mode

    printf("Woke up from Light Sleep.\n");
    log_cpu_utilization();   // Log CPU utilization after wake-up

    blinkLED(2, 500, 500); // Indicate wake-up with two slow blinks
}

/**
 * @brief Transitions to Deep Sleep mode.
 */
void enter_deep_sleep() {
    printf("Entering Deep Sleep Mode...\n");

    blinkLED(3, 500, 500); // Indicate transition to Deep Sleep

    configure_wakeup_sources_deep_sleep();

    vTaskDelay(pdMS_TO_TICKS(500)); 

    esp_deep_sleep_start(); // Enter Deep Sleep mode
    log_cpu_utilization();  // Note: This will not execute due to CPU power-down
}

void app_main() {
    // Initialize GPIO for LED
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    printf("System booted. Configuring wake-up sources...\n");

    // Active Mode
    run_mode_activity();    
    // Light Sleep mode
    enter_light_sleep();   
    // Deep Sleep mode
    enter_deep_sleep();     
}
