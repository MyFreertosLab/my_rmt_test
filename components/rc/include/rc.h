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

#if CONFIG_ESP_RC_PROTOCOL_IBUS
#define RC_PROTOCOL_IBUS
#elif  CONFIG_ESP_RC_PROTOCOL_PPM
#define RC_PROTOCOL_PPM
#endif

#define RC_MAX_CHANNELS 10

typedef enum {
	RC_THROTTLE = 0,
	RC_ROLL,
	RC_PITCH,
	RC_YAW,
	RC_VRA,
	RC_VRB,
	RC_SWA,
	RC_SWB,
	RC_SWC,
	RC_SWD
} RC_STICK;

typedef union {
	uint8_t array[RC_MAX_CHANNELS];
	struct {
		uint8_t throttle;
		uint8_t roll;
		uint8_t pitch;
		uint8_t yaw;
		uint8_t vra;
		uint8_t vrb;
		uint8_t swa;
		uint8_t swb;
		uint8_t swc;
		uint8_t swd;
	} fields;
} rc_stick_to_channel_t;

typedef struct {
	uint16_t min;
	uint16_t max;
	uint16_t center;
	uint16_t value;
} rc_stick_range_t;

typedef struct {
	void* dev_config;
    esp_err_t (*init)(void* rc_handle);
    esp_err_t (*start)(void* rc_handle);
    esp_err_t (*stop)(void* rc_handle);
    rc_stick_to_channel_t stick_to_channel;
    rc_stick_range_t rc_stick_ranges[RC_MAX_CHANNELS];
} rc_t;
typedef rc_t* rc_handle_t;


#endif /* COMPONENTS_RC_INCLUDE_RC_H_ */
