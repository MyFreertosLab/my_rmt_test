/*
 * ppm8.h
 *
 *  Created on: 23 feb 2021
 *      Author: andrea
 */

#ifndef COMPONENTS_RC_INCLUDE_RC_H_
#define COMPONENTS_RC_INCLUDE_RC_H_
#include <stdint.h>
#include <esp_err.h>


#define RC_MAX_CHANNELS 8

typedef enum {
	RC_THROTTLE = 0,
	RC_ROLL,
	RC_PITCH,
	RC_YAW,
	RC_AUX1,
	RC_AUX2,
	RC_AUX3,
	RC_AUX4
} RC_STICK;

typedef union {
	uint8_t array[RC_MAX_CHANNELS];
	struct {
		uint8_t throttle;
		uint8_t roll;
		uint8_t pitch;
		uint8_t yaw;
		uint8_t aux1;
		uint8_t aux2;
		uint8_t aux3;
		uint8_t aux4;
	} fields;
} rc_stick_to_channel_t;

typedef struct {
	uint16_t min;
	uint16_t max;
	uint16_t center;
	uint16_t value;
} rc_stick_range_t;

typedef struct {
    uint8_t channel;
    uint8_t gpio_num;
    uint8_t clock_div;
    uint8_t mem_block_num;
    uint8_t ticks_thresh;
    uint16_t idle_threshold;
    rc_stick_to_channel_t stick_to_channel;
	rc_stick_range_t rc_stick_ranges[RC_MAX_CHANNELS];
} rc_t;
typedef rc_t* rc_handle_t;

esp_err_t rc_init(rc_handle_t rc_handle);
esp_err_t rc_start(rc_handle_t rc_handle);
esp_err_t rc_stop(rc_handle_t rc_handle);

#endif /* COMPONENTS_RC_INCLUDE_RC_H_ */
