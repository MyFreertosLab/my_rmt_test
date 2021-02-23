/*
 * ppm8.c
 *
 *  Created on: 23 feb 2021
 *      Author: andrea
 */

#include <rc.h>
#include <driver/gpio.h>
#include "driver/rmt.h"

/************************************************************************
 ****************** A P I  I M P L E M E N T A T I O N ******************
 ************************************************************************/
esp_err_t rc_init(rc_handle_t rc_handle) {
	printf("PPM RX initialization started ... \n");
	printf("Gpio Pin [%d] ... \n", rc_handle->gpio_num);
    gpio_pulldown_en(rc_handle->gpio_num);
    gpio_set_direction(rc_handle->gpio_num, GPIO_MODE_INPUT);

	printf("Configuring RMT module ... \n");
    rmt_config_t rmt_rx;
    rmt_rx.channel = rc_handle->channel;
    rmt_rx.gpio_num = rc_handle->gpio_num;
    rmt_rx.clk_div = rc_handle->clock_div;
    rmt_rx.mem_block_num = rc_handle->mem_block_num;
    rmt_rx.rmt_mode = RMT_MODE_RX;
    rmt_rx.rx_config.filter_en = true;
    rmt_rx.rx_config.filter_ticks_thresh = rc_handle->ticks_thresh;
    rmt_rx.rx_config.idle_threshold = rc_handle->idle_threshold;
    rmt_config(&rmt_rx);

	printf("Installing RMT module ... \n");
    rmt_driver_install(rmt_rx.channel, 1000, 0);
    printf("PPM RX initialized\n");
	return ESP_OK;
}

esp_err_t rc_start(rc_handle_t rc_handle) {
    int channel = rc_handle->channel;
    uint16_t rmt_tick_microseconds = (80/rc_handle->clock_div);   /*!< RMT counter value for 10 us.(Source clock is APB clock) */

	printf("Starting RC on channel [%d] ... \n", channel);

    RingbufHandle_t rb = NULL;
    rmt_get_ringbuf_handle(channel, &rb);
    rmt_rx_start(channel, 1);

	printf("RC Started ... \n");
    while(rb) {
        size_t rx_size = 0;
        int channels;
        rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 1000);
        if(item) {
                channels = (rx_size / 4) - 1;
                for (int i = 0; i < channels; i++) {
                    printf("%d-%04d ",i, ((item+i)->duration1 + (item+i)->duration0) / rmt_tick_microseconds);
                }
                printf("\n");
            vRingbufferReturnItem(rb, (void*) item);
        } else {
        	printf("Data NOT received\n");
        }
    }
	return ESP_OK;
}

esp_err_t rc_stop(rc_handle_t rc_handle) {
	return rmt_rx_stop(rc_handle->channel);
}
