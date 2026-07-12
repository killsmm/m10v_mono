#ifndef _UAC_GADGET_COMMON_H_
#define _UAC_GADGET_COMMON_H_
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include <alsa/asoundlib.h>

#include "uac1_mic.h"
#include "cif_stream.h"
#include "cmfwk_mm.h"

/* --------------------------------------------------------------------------- */
//#define DEBUG_PRINT
#ifdef DEBUG_PRINT
#define debug_p(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug_p(fmt, ...)
#endif /* DEBUG_PRINT */

/* ---------------------------------------------------------------------------
 * IPCU and UAC device instances
 */

struct ipcu_device {
	int ipcu_fd;
	bool is_streaming;
	uint8_t *buffer;
	uint32_t buffer_size;
	uint32_t buffer_size_max;
	pthread_mutex_t lock;

	uint32_t sampling_rate;
	uint32_t bit_rate;
	uint32_t channel_type;

	struct uac_device *udev;
};

struct uac_device {
	int uac_fd;
	bool is_streaming;

	snd_pcm_t *pcm_hnd;
	struct pollfd pcm_pfd;
	snd_pcm_hw_params_t *pcm_hw_params;
	snd_pcm_sw_params_t *pcm_sw_params;
	unsigned int pcm_rate;
	unsigned int pcm_channels;
	snd_pcm_sframes_t pcm_period;
	snd_pcm_uframes_t pcm_start_threshold;
	ssize_t pcm_period_bytes;

	unsigned int pcm_rate_current;
//	unsigned int pcm_channels_current;
	snd_pcm_sframes_t pcm_period_current;

	struct ipcu_device *idev;
};

int uac_camera_if_command(struct camera_if_cmd *cmd);

int
ipcu_open_sub();
int
ipcu_start_capturing_sub(struct ipcu_device *dev);
int
ipcu_stop_capturing_sub(struct ipcu_device *dev);

#endif /* _UAC_GADGET_COMMON_H_ */
