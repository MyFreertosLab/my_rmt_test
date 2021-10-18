#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_system.h>
#include <nvs_flash.h>
#include <nvs.h>
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



esp_err_t my_rc_init_flash(){
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
	return err;
}
esp_err_t my_rc_write_flash(rc_handle_t rc_handle){
    nvs_handle_t my_handle;
    ESP_ERROR_CHECK(nvs_open("RC_CAL", NVS_READWRITE, &my_handle));
    ESP_ERROR_CHECK(nvs_set_u8(my_handle, "FLASHED", 1));

    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MIN_1", rc_handle->rc_stick_ranges[0].min));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MIN_2", rc_handle->rc_stick_ranges[1].min));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MIN_3", rc_handle->rc_stick_ranges[2].min));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MIN_4", rc_handle->rc_stick_ranges[3].min));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MIN_5", rc_handle->rc_stick_ranges[4].min));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MIN_6", rc_handle->rc_stick_ranges[5].min));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MIN_7", rc_handle->rc_stick_ranges[6].min));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MIN_8", rc_handle->rc_stick_ranges[7].min));

    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MAX_1", rc_handle->rc_stick_ranges[0].max));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MAX_2", rc_handle->rc_stick_ranges[1].max));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MAX_3", rc_handle->rc_stick_ranges[2].max));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MAX_4", rc_handle->rc_stick_ranges[3].max));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MAX_5", rc_handle->rc_stick_ranges[4].max));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MAX_6", rc_handle->rc_stick_ranges[5].max));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MAX_7", rc_handle->rc_stick_ranges[6].max));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_MAX_8", rc_handle->rc_stick_ranges[7].max));

    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_CNT_1", rc_handle->rc_stick_ranges[0].center));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_CNT_2", rc_handle->rc_stick_ranges[1].center));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_CNT_3", rc_handle->rc_stick_ranges[2].center));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_CNT_4", rc_handle->rc_stick_ranges[3].center));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_CNT_5", rc_handle->rc_stick_ranges[4].center));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_CNT_6", rc_handle->rc_stick_ranges[5].center));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_CNT_7", rc_handle->rc_stick_ranges[6].center));
    ESP_ERROR_CHECK(nvs_set_u16(my_handle, "RC_CNT_8", rc_handle->rc_stick_ranges[7].center));

    printf("RC: Committing min/max/center into NVS ... \n");
    ESP_ERROR_CHECK(nvs_commit(my_handle));

    // Close
    nvs_close(my_handle);
	return ESP_OK;
}
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
	printf("Read Values\n");
	ESP_ERROR_CHECK(rc_start(rc_handle));
	ESP_ERROR_CHECK(rc_stop(rc_handle));

        vTaskDelay(pdMS_TO_TICKS(20000));
	printf("Register Min/Max Values\n");
	ESP_ERROR_CHECK(rc_start(rc_handle));
	ESP_ERROR_CHECK(rc_stop(rc_handle));

        vTaskDelay(pdMS_TO_TICKS(20000));
	printf("Register Center Values\n");
	ESP_ERROR_CHECK(rc_start(rc_handle));
	ESP_ERROR_CHECK(rc_stop(rc_handle));
	printf("Min/Center/Max values for Sticks\n");
	for(uint8_t i = 0; i < RC_MAX_CHANNELS; i++) {
		rc_handle->rc_stick_ranges[i].center = rc_handle->rc_stick_ranges[i].value;
		printf("Channel [%d] range: [%d][%d][%d]\n", i, rc_handle->rc_stick_ranges[i].min, rc_handle->rc_stick_ranges[i].center, rc_handle->rc_stick_ranges[i].max);
	}
	ESP_ERROR_CHECK(my_rc_init_flash());
	ESP_ERROR_CHECK(my_rc_write_flash(rc_handle));

	printf("Bye, bye ...\n");
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(ppm_rx_task, "ppm_rx_task", 6144, NULL, 10, NULL);

}


