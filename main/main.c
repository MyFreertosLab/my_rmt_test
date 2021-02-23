#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "driver/gpio.h"
#include "rc.h"

#define RC_CHANNEL            0                  /* Channel input (0-7) for receiver */
#define RC_GPIO_NUM           23                 /* GPIO number for receiver */
#define RC_CLK_DIV            8                  /* Clock divider */
#define RC_TICK_US            (80/RC_CLK_DIV)    /* Number of Ticks for us */
#define RC_PPM_TIMEOUT_US     3500               /* min PPM silence (us) */
#define RC_BUFF_BLOCK_NUM     4                  /* Memory Blocks */
#define RC_TICK_TRASH         100                /* Interference */

/**
 * @brief PPM receiver task
 *
 */
static void ppm_rx_task(void *pvParameter)
{
	rc_t rc;
	rc_handle_t rc_handle = &rc;

	rc.gpio_num = RC_GPIO_NUM;
	rc.channel = RC_CHANNEL;
	rc.gpio_num = RC_GPIO_NUM;
	rc.clock_div = RC_CLK_DIV;
	rc.mem_block_num = RC_BUFF_BLOCK_NUM;
	rc.ticks_thresh = RC_TICK_TRASH;
	rc.idle_threshold = RC_PPM_TIMEOUT_US * (RC_TICK_US);

	ESP_ERROR_CHECK(rc_init(rc_handle));
	ESP_ERROR_CHECK(rc_start(rc_handle));
	ESP_ERROR_CHECK(rc_stop(rc_handle));
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(ppm_rx_task, "ppm_rx_task", 2048, NULL, 10, NULL);

}


