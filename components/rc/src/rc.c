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
rmt_config_t rmt_rx;
esp_err_t rc_init(rc_handle_t rc_handle) {
	printf("PPM RX initialization started ... \n");
	printf("Gpio Pin [%d] ... \n", rc_handle->gpio_num);
	gpio_pulldown_en(rc_handle->gpio_num);
	gpio_set_direction(rc_handle->gpio_num, GPIO_MODE_INPUT);

	printf("Configuring RMT module ... \n");
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

	printf("Configure stick to channel map ... \n");
	rc_handle->stick_to_channel.fields.throttle = 2;
	rc_handle->stick_to_channel.fields.roll = 1;
	rc_handle->stick_to_channel.fields.pitch = 3;
	rc_handle->stick_to_channel.fields.yaw = 4;
	rc_handle->stick_to_channel.fields.aux1 = 6;
	rc_handle->stick_to_channel.fields.aux2 = 5;
	rc_handle->stick_to_channel.fields.aux3 = 7;
	rc_handle->stick_to_channel.fields.aux4 = 8;
	printf("Configured stick to channel map\n");
	printf("- Throttle: %d\n", rc_handle->stick_to_channel.array[RC_THROTTLE]);
	printf("- Roll ...: %d\n", rc_handle->stick_to_channel.array[RC_ROLL]);
	printf("- Pitch ..: %d\n", rc_handle->stick_to_channel.array[RC_PITCH]);
	printf("- Yaw ....: %d\n", rc_handle->stick_to_channel.array[RC_YAW]);
	printf("- Aux1 ...: %d\n", rc_handle->stick_to_channel.array[RC_AUX1]);
	printf("- Aux2 ...: %d\n", rc_handle->stick_to_channel.array[RC_AUX2]);
	printf("- Aux3 ...: %d\n", rc_handle->stick_to_channel.array[RC_AUX3]);
	printf("- Aux4 ...: %d\n", rc_handle->stick_to_channel.array[RC_AUX4]);
	for (uint8_t i = 0; i < RC_MAX_CHANNELS; i++) {
		rc_handle->rc_stick_ranges[i].min = 0;
		rc_handle->rc_stick_ranges[i].max = 0;
		rc_handle->rc_stick_ranges[i].center = 0;
	}

	printf("PPM RX initialized\n");

	return ESP_OK;
}

esp_err_t rc_start(rc_handle_t rc_handle) {
	int channel = rc_handle->channel;
	uint16_t rmt_tick_microseconds = (80 / rc_handle->clock_div); /*!< RMT counter value for 10 us.(Source clock is APB clock) */
	printf("Starting RC on channel [%d] ... \n", channel);

	RingbufHandle_t rb = NULL;
	rmt_get_ringbuf_handle(channel, &rb);
	rmt_rx_start(channel, 1);

	printf("RC Started ... \n");
	while (rb) {
		size_t rx_size = 0;
		int channels;
		rmt_item32_t *item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size,
				1000);
		if (item) {
			channels = (rx_size / 4) - 1;
			for (int i = 0; i < channels; i++) {
				rc_handle->rc_stick_ranges[i].value = (uint16_t)(((item + i)->duration1+ (item + i)->duration0) / rmt_tick_microseconds);
				if (rc_handle->rc_stick_ranges[i].min == 0
						|| rc_handle->rc_stick_ranges[i].min
								> rc_handle->rc_stick_ranges[i].value) {
					rc_handle->rc_stick_ranges[i].min =
							rc_handle->rc_stick_ranges[i].value;
				}
				if (rc_handle->rc_stick_ranges[i].max == 0
						|| rc_handle->rc_stick_ranges[i].max
								< rc_handle->rc_stick_ranges[i].value) {
					rc_handle->rc_stick_ranges[i].max =
							rc_handle->rc_stick_ranges[i].value;
				}
				printf("%d-%04d ", i, rc_handle->rc_stick_ranges[i].value);
			}
			printf("\n");
			vRingbufferReturnItem(rb, (void*) item);
		} else {
			printf("Data NOT received\n");
			return ESP_OK;
		}
	}
	return ESP_OK;
}

esp_err_t rc_stop(rc_handle_t rc_handle) {
	return rmt_rx_stop(rc_handle->channel);
}
