#include <stdio.h>
#include "esp_err.h"
#include "system_manager.h"

/************************* 
Purpose: 
Serves as the entry point for the application. It initializes and starts the system manager,
which in turn initializes and starts various system services and components.
*************************/

// Main Application Entry Point
void app_main(void)
{
    ESP_ERROR_CHECK(system_manager_init());     // Initialize the system manager and its services/components
    ESP_ERROR_CHECK(system_manager_start());    // Start the system manager operations (includes starting services/components)
    ESP_ERROR_CHECK(system_manager_test());     // Perform a one-shot test of the system manager (can be commented out if not needed)
}