/*
 * es705_escore.c  --  Audience eS705 ALSA SoC Audio driver
 *
 * Copyright 2011 Audience, Inc.
 *
 * Author:
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _ES705_ESCORE_H
#define _ES705_ESCORE_H

#include <linux/cdev.h>
#include <linux/mutex.h>
#include <sound/soc.h>
#include <linux/time.h>
#include "escore.h"
#include "es705-uart.h"

#ifdef CONFIG_SND_SOC_ES_SPI_SENSOR_HUB
#include "es705-sensorhub-demo.h"
#endif

/* TODO: condition of kernel version or commit code to specific kernels */
#ifdef CONFIG_SLIMBUS_MSM_NGD
#define ES705_DAI_ID_BASE	0
#else
#define ES705_DAI_ID_BASE	1
#endif

#define PARSE_BUFFER_SIZE (PAGE_SIZE * 2)

#define ES705_READ_VE_OFFSET		0x0804
#define ES705_READ_VE_WIDTH		4
#define ES705_WRITE_VE_OFFSET		0x0800
#define ES705_WRITE_VE_WIDTH		4

#define ES705_MCLK_DIV			0x0000
#define ES705_CLASSD_CLK_DIV		0x0001
#define ES705_CP_CLK_DIV		0x0002

#define ES705_BOOT_CMD			0x0001
#define ES705_BOOT_ACK			0x01010101

#define ES705_WDB_CMD			0x802f
#define ES705_RDB_CMD			0x802e

#ifdef CONFIG_SND_SOC_ES804_ESCORE
#define ES705_WDB_MAX_SIZE		4096
#else
#define ES705_WDB_MAX_SIZE		512
#endif

#define ES705_SYNC_CMD			0x8000
#define ES705_SYNC_POLLING		0x0000
#define ES705_SYNC_INTR_ACITVE_LOW	0x0001
#define ES705_SYNC_INTR_ACITVE_HIGH	0x0002
#define ES705_SYNC_INTR_FALLING_EDGE	0x0003
#define ES705_SYNC_INTR_RISING_EDGE	0x0004
#define ES705_SYNC_ACK			0x80000000
#define ES705_SBL_ACK			0x8000FFFF
#define ES705_ROUTE_STATE               0x804f0000

#define ES705_RESET_CMD			0x8002
#define ES705_RESET_IMMED		0x0000
#define ES705_RESET_DELAYED		0x0001

#define ES705_GET_POWER_STATE		0x800F
#define ES705_SET_POWER_STATE		0x9010
#define ES705_SET_POWER_STATE_SLEEP	0x0001
#define ES705_SET_POWER_STATE_MP_SLEEP	0x0002
#define ES705_SET_POWER_STATE_MP_CMD	0x0003
#define ES705_SET_POWER_STATE_NORMAL	0x0004
#define ES705_SET_POWER_STATE_VS_OVERLAY	0x0005
#define ES705_SET_POWER_STATE_VS_LOWPWR	0x0006

#define ES705_STREAM_UART_ON		0x90250101
#define ES705_STREAM_UART_OFF		0x90250100

#define ES705_SET_SMOOTH			0x904E
#define ES705_SET_SMOOTH_RATE		0x0000

#define ES705_SET_POWER_LEVEL		0x8011
#define ES705_POWER_LEVEL_5			0x0005
#define ES705_GET_POWER_LEVEL		0x8012
#define ES705_DMIC0_VS_ROUTE_PREST	0x1F40
#define ES705_DMIC0_VS_TUNING_ROUTE_PREST 0x2008
#define ES705_DMIC0_CVS_PREST		0x270F
#ifdef CONFIG_SND_SOC_ES_AVOID_REPEAT_FW_DOWNLOAD
#define ES705_DMIC0_CVS_PREST_SINGLE_DOWNLOAD	0x270F
#endif
#define ES80X_SDE_PARAMETER		0x8042
/*
 * bit15 - reserved
 * bit[14:12] - access type
 * bit11 - commit = 0, staged = 1
 * bit[10:0] - psuedo address
 */
#define ES705_ACCESS_MASK	(7 << 12)
#define ES705_ALGO_ACCESS	(0 << 12)
#define ES705_DEV_ACCESS	(1 << 12)
#define ES705_CMD_ACCESS	(2 << 12)
#define ES705_OTHER_ACCESS	(3 << 12)

#define ES705_CMD_MASK		(1 << 11)
#define ES705_STAGED_CMD	(1 << 11)
#define ES705_COMMIT_CMD	(0 << 11)

#define ES705_ADDR_MASK		0x7ff

#define ES705_STAGED_MSG_BIT	(1 << 13)
/*
 * Device parameter command codes
 */
#define ES705_DEV_PARAM_OFFSET		0x2000
#define ES705_GET_DEV_PARAM		0x800b
#define ES705_SET_DEV_PARAM_ID		0x900c
#define ES705_SET_DEV_PARAM		0x900d

/*
 * Algoithm parameter command codes
 */
#define ES705_ALGO_PARAM_OFFSET		0x0000
#define ES705_GET_ALGO_PARAM		0x8016
#define ES705_SET_ALGO_PARAM_ID		0x9017
#define ES705_SET_ALGO_PARAM		0x9018
#define ES_GET_EVENT			0x806D
#define ES705_FW_DOWNLOAD_MAX_RETRY	5
/*
 * addresses
 */
enum {
	ES705_MIC_CONFIG,
	ES705_AEC_MODE,
	ES705_VEQ_ENABLE,
	ES705_DEREVERB_ENABLE,
	ES705_DEREVERB_GAIN,
	ES705_BWE_ENABLE,
	ES705_BWE_HIGH_BAND_GAIN,
	ES705_BWE_POST_EQ_ENABLE,
	ES705_SLIMBUS_LINK_MULTI_CHANNEL,
	ES705_POWER_STATE,
	ES705_RUNTIME_PM,
	ES705_FE_STREAMING,
	ES705_PRESET,
	ES705_ALGO_PROCESSING,
	ES705_CHANGE_STATUS,
	ES705_FW_FIRST_CHAR,
	ES705_FW_NEXT_CHAR,
	ES705_EVENT_RESPONSE,
	ES705_VOICE_SENSE_ENABLE,
	ES705_VOICE_SENSE_SET_KEYWORD,
	ES705_VOICE_SENSE_EVENT,
	ES705_VOICE_SENSE_TRAINING_MODE,
	ES705_VOICE_SENSE_DETECTION_SENSITIVITY,
	ES705_VOICE_ACTIVITY_DETECTION_SENSITIVITY,
	ES705_VOICE_SENSE_TRAINING_RECORD,
	ES705_VOICE_SENSE_TRAINING_STATUS,
	ES705_VOICE_SENSE_TRAINING_MODEL_LENGTH,
	ES705_VOICE_SENSE_DEMO_ENABLE,
	ES705_VS_STORED_KEYWORD,
	ES705_VS_INT_OSC_MEASURE_START,
	ES705_VS_INT_OSC_MEASURE_STATUS,
	ES705_CVS_PRESET,
	ES705_RX_ENABLE,
	ES80X_AF_DUO_MODE,
	ES80X_ENABLE_SDE_MODE,
	ES80X_SDE_BEARING_VALUE,
	ES80X_SDE_SALIENCE_VALUE,
	ES80X_SDE_RMS_ESTIMATE_VALUE,
	ES705_API_ADDR_MAX,
};

/*
 * addresses
 * Ensure digital addresses do not overlap with analog
 */
enum {
	ES_CHANGE_STATUS = 55,
	ES_FW_FIRST_CHAR,
	ES_FW_NEXT_CHAR,
	ES_GET_SYS_INTERRUPT,
	ES_ACCDET_CONFIG,
	ES_GET_ACCDET_STATUS,
	ES_BUTTON_DETECTION_ENABLE,
	ES_BUTTON_SERIAL_CONFIG,
	ES_BUTTON_PARALLEL_CONFIG,
	ES_BUTTON_DETECTION_RATE,
	ES_BUTTON_PRESS_SETTLING_TIME,
	ES_BUTTON_BOUNCE_TIME,
	ES_BUTTON_DETECTION_LONG_PRESS_TIME,
	ES_ALGO_SAMPLE_RATE,
	ES_ALGORITHM,
	ES_BUTTON_CTRL1,
	ES_BUTTON_CTRL2,
	ES_BUTTON_CTRL3,
	ES_BUTTON_CTRL4,

	ES_PRIMARY_MUX,
	ES_SECONDARY_MUX,
	ES_TERTIARY_MUX,
	ES_AECREF1_MUX,
	ES_FEIN_MUX,
	ES_UITONE1_MUX,
	ES_UITONE2_MUX,
	ES_UITONE3_MUX,
	ES_UITONE4_MUX,

	ES_AUDIN1_MUX,
	ES_AUDIN2_MUX,
	ES_MMUITONE1_MUX,
	ES_MMUITONE2_MUX,
	ES_PASSIN1_MUX,
	ES_PASSIN2_MUX,
	ES_PASSIN3_MUX,
	ES_PASSIN4_MUX,

	ES_DAC0_0_MUX,
	ES_DAC0_1_MUX,
	ES_DAC1_0_MUX,
	ES_DAC1_1_MUX,

	ES_PCM0_0_MUX,
	ES_PCM0_1_MUX,
	ES_PCM0_2_MUX,
	ES_PCM0_3_MUX,
	ES_PCM1_0_MUX,
	ES_PCM1_1_MUX,
	ES_PCM1_2_MUX,
	ES_PCM1_3_MUX,
	ES_PCM2_0_MUX,
	ES_PCM2_1_MUX,
	ES_PCM2_2_MUX,
	ES_PCM2_3_MUX,

	ES_SBUSTX0_MUX,
	ES_SBUSTX1_MUX,
	ES_SBUSTX2_MUX,
	ES_SBUSTX3_MUX,
	ES_SBUSTX4_MUX,
	ES_SBUSTX5_MUX,

	ES_CODEC_VALUE,
	ES_POWER_STATE,
	ES_VS_INT_OSC_MEASURE_START,
	ES_VS_INT_OSC_MEASURE_STATUS,
	ES_EVENT_RESPONSE,
	ES_VOICE_SENSE_SET_KEYWORD,
	ES_VOICE_SENSE_EVENT,
	ES_VOICE_SENSE_TRAINING_MODE,
	ES_VOICE_SENSE_DETECTION_SENSITIVITY,
	ES_VOICE_ACTIVITY_DETECTION_SENSITIVITY,
	ES_VS_STORED_KEYWORD,
	ES_VOICE_SENSE_TRAINING_RECORD,
	ES_VOICE_SENSE_TRAINING_STATUS,
	ES_CVS_PRESET,
	ES_STEREO_WIDTH,
	ES_API_ADDR_MAX,
};

#define ES_SLIM_1_PB_MAX_CHANS	2
#define ES_SLIM_1_CAP_MAX_CHANS	2
#define ES_SLIM_2_PB_MAX_CHANS	2
#define ES_SLIM_2_CAP_MAX_CHANS	2
#define ES_SLIM_3_PB_MAX_CHANS	2
#define ES_SLIM_3_CAP_MAX_CHANS	2

#define ES_SLIM_1_PB_OFFSET	0
#define ES_SLIM_2_PB_OFFSET	2
#define ES_SLIM_3_PB_OFFSET	4
#define ES_SLIM_1_CAP_OFFSET	0
#define ES_SLIM_2_CAP_OFFSET	2
#define ES_SLIM_3_CAP_OFFSET	4


#define ES_SLIM_CH_RX_OFFSET		152
#define ES_SLIM_CH_TX_OFFSET		156
/* #define ES_SLIM_RX_PORTS		10 */
#define ES_SLIM_RX_PORTS		6
#define ES_SLIM_TX_PORTS		6

#define ES_I2S_PORTA		7
#define ES_I2S_PORTB		8
#define ES_I2S_PORTC		9

#define ES_SLIM_RX_PORTS		6
#define ES_SLIM_TX_PORTS		6

#if defined(CONFIG_SND_SOC_ES_SLIM)
#define ES_NUM_CODEC_SLIM_DAIS       6
#define ES_NUM_CODEC_I2S_DAIS	0
#else
#define ES_NUM_CODEC_SLIM_DAIS       0
#define ES_NUM_CODEC_I2S_DAIS	2
#endif

#define ES_NUM_CODEC_DAIS	(ES_NUM_CODEC_SLIM_DAIS + \
		ES_NUM_CODEC_I2S_DAIS)

/* Delay values required for different stages */
#define ES705_DELAY_WAKEUP_TO_NORMAL	30 /* ms */
#define ES705_DELAY_WAKEUP_TO_VS	20 /* ms */
#define ES705_DELAY_VS_TO_NORMAL	50 /* ms */


enum {
	ES_SLIM_1_PB = ES_DAI_ID_BASE,
	ES_SLIM_1_CAP,
	ES_SLIM_2_PB,
	ES_SLIM_2_CAP,
	ES_SLIM_3_PB,
	ES_SLIM_3_CAP,
};

enum {
	ES_PM_SLEEP = 0x1,
	ES_PM_MP_SLEEP,
	ES_PM_MP_CMD,
	ES_PM_ACTIVE,
};

#define ES705_SLIM_CH_RX_OFFSET		152
#define ES705_SLIM_CH_TX_OFFSET		156
/* #define ES705_SLIM_RX_PORTS		10 */
#define ES705_SLIM_RX_PORTS		6
#define ES705_SLIM_TX_PORTS		6

#define ES705_NUM_CODEC_SLIM_DAIS	6

#define ES705_NUM_CODEC_I2S_DAIS	4

#define ES705_I2S_PORTA		7
#define ES705_I2S_PORTB		8
#define ES705_I2S_PORTC		9
#define ES705_I2S_PORTD		10

#define ES705_NS_ON_PRESET		969
#define ES705_NS_OFF_PRESET		624
#define ES705_SW_ON_PRESET		702
#define ES705_SW_OFF_PRESET		703
#define ES705_STS_ON_PRESET		984
#define ES705_STS_OFF_PRESET	985
#define ES705_RX_NS_ON_PRESET	996
#define ES705_RX_NS_OFF_PRESET	997
#define ES705_WNF_ON_PRESET		994
#define ES705_WNF_OFF_PRESET	995
#define ES705_BWE_ON_PRESET		622
#define ES705_BWE_OFF_PRESET	623
#define ES705_AVALON_WN_ON_PRESET	704
#define ES705_AVALON_WN_OFF_PRESET	705
#define ES705_VBB_ON_PRESET		706
#define ES705_VBB_OFF_PRESET	707

#define ES705_VS_PRESET		1382

#define ES705_AUD_FOCUS_PRESET			1355
#define ES705_AUD_FOCUS_NARRATOR_PRESET	756
#define ES705_AUD_FOCUS_SCENE_PRESET		757
#define ES705_AUD_FOCUS_NARRATION_PRESET	758


#define ES705_NUM_CODEC_DAIS	(ES705_NUM_CODEC_SLIM_DAIS +\
		ES705_NUM_CODEC_I2S_DAIS)

enum {
	ES705_SLIM_1_PB = ES705_DAI_ID_BASE,
	ES705_SLIM_1_CAP,
	ES705_SLIM_2_PB,
	ES705_SLIM_2_CAP,
	ES705_SLIM_3_PB,
	ES705_SLIM_3_CAP,
};


enum {
	NO_EVENT,
	CODEC_EVENT,
	KW_DETECTED,
};

enum {
	POWER_LEVEL_0 = 0,
	POWER_LEVEL_1,
	POWER_LEVEL_2,
	POWER_LEVEL_3,
	POWER_LEVEL_4,
	POWER_LEVEL_5,
	POWER_LEVEL_6,
	POWER_LEVEL_MAX
};
#define ES705_SLIM_INTF		0
#define ES705_I2C_INTF		1
#define ES705_SPI_INTF         2
#define ES705_UART_INTF		3

struct es705_slim_dai_data {
	unsigned int rate;
	unsigned int *ch_num;
	unsigned int ch_act;
	unsigned int ch_tot;
};

struct es705_slim_ch {
	u32	sph;
	u32	ch_num;
	u16	ch_h;
	u16	grph;
};

enum {
	ES705_PM_ACTIVE,
	ES705_PM_SUSPENDING,
	ES705_PM_SUSPENDED,
	ES705_PM_SUSPENDING_TO_VS,
	ES705_PM_SUSPENDED_VS
};

enum {
	ES705_AUD_FOCUS_DISABLED,
	ES705_AUD_FOCUS_NARRATOR,
	ES705_AUD_FOCUS_SCENE,
	ES705_AUD_FOCUS_NARRATION
};

/* Maximum size of keyword parameter block in bytes. */
#define ES705_VS_KEYWORD_PARAM_MAX 512

/* Base name used by character devices. */
#define ES705_CDEV_NAME "adnc"


extern struct snd_soc_codec_driver soc_codec_dev_es705;
extern struct snd_soc_dai_driver es705_dai[];

extern int es705_core_probe(struct device *dev);

extern int es705_bootup(struct escore_priv *es705);

extern int remote_esxxx_startup(struct snd_pcm_substream *substream,
		struct snd_soc_dai *dai);

extern int remote_esxxx_shutdown(struct snd_pcm_substream *substream,
		struct snd_soc_dai *dai);
int es705_set_streaming(struct escore_priv *escore, int value);


#define es705_resp(obj) ((obj)->last_response)
int es705_cmd(struct escore_priv *es705, u32 cmd);
int es705_cmd_without_sleep(struct escore_priv *es705, u32 cmd);

int es705_bus_init(struct escore_priv *es705);
extern int fw_download(void *arg);

#define ES705_STREAM_DISABLE	0
#define ES705_STREAM_ENABLE	1
#define ES705_VS_FW_CHUNK	512
/* #define ES705_WAIT_TIMEOUT	2*/
#define MAX_RETRY_TO_SWITCH_TO_LOW_POWER_MODE	5

#endif /* _ES705_ESCORE_H */
