#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_system.h>
#include <nvs_flash.h>
#include <nvs.h>
#include "driver/rmt.h"
#include "driver/gpio.h"
#if CONFIG_ESP_RC_PROTOCOL_IBUS
#include <rc_ibus.h>
#include <driver/uart.h>
#elif  CONFIG_ESP_RC_PROTOCOL_PPM
#include <rc_ppm.h>
#endif
#include "rc_task.h"

static rc_t rc = {
#ifdef RC_PROTOCOL_IBUS
	.init = (esp_err_t (*)(void *))rc_ibus_init,
	.start = (esp_err_t (*)(void *))rc_ibus_start,
	.stop = (esp_err_t (*)(void *))rc_ibus_stop
#else
#ifdef RC_PROTOCOL_PPM
	.init = (esp_err_t (*)(void *))rc_ppm_init,
	.start = (esp_err_t (*)(void *))rc_ppm_start,
	.stop = (esp_err_t (*)(void *))rc_ppm_stop,
#endif
#endif
};
void app_main(void)
{
	rc_handle_t rc_handle = &rc;
    xTaskCreate(rc_rx_task, "rc_rx_task", 6144, (void*)rc_handle, 10, NULL);
}


