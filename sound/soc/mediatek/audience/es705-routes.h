/*
 * es705-routes.h  --  Audience eS705 ALSA SoC Audio driver
 *
 * Copyright 2013 Audience, Inc.
 *
 * Author: Greg Clemson <gclemson@audience.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _ES705_ROUTES_H
#define _ES705_ROUTES_H

struct esxxx_route_config {
	const u32 *route;
	const u32 *nb;
	const u32 *wb;
	const u32 *swb;
	const u32 *fb;
};

enum {
	ROUTE_OFF,						/* 0 */
	ROUTE_CS_VOICE_1MIC_CT,			/* 1 */
	ROUTE_CS_VOICE_2MIC_CT,			/* 2 */
	ROUTE_CS_VOICE_3MIC_CT,			/* 3 */
	ROUTE_CS_VOICE_1MIC_FT,			/* 4 */
	ROUTE_CS_VOICE_2MIC_FT,			/* 5 */
	ROUTE_CS_VOICE_3MIC_FT,			/* 6 */
	ROUTE_CS_VOICE_HEADSET,			/* 7 */
	ROUTE_CS_VOICE_1MIC_HEADPHONE,	/* 8 */
	ROUTE_VOIP_1MIC_CT,				/* 9 */
	ROUTE_VOIP_2MIC_CT,				/* 10 */
	ROUTE_VOIP_3MIC_CT,				/* 11 */
	ROUTE_VOIP_1MIC_FT,				/* 12 */
	ROUTE_VOIP_2MIC_FT,				/* 13 */
	ROUTE_VOIP_3MIC_FT,				/* 14 */
	ROUTE_VOIP_HEADSET,				/* 15 */
	ROUTE_VOIP_1MIC_HEADPHONE,		/* 16 */
	ROUTE_VOICE_ASR_1MIC,			/* 17 */
	ROUTE_VOICE_ASR_2MIC,			/* 18 */
	ROUTE_VOICE_ASR_3MIC,			/* 19 */
	ROUTE_VOICESENSE_SBUSRX4,		/* 20 */
	ROUTE_VOICESENSE_SBUSRX0,		/* 21 */
	ROUTE_VOICESENSE_PDM,			/* 22 */
	ROUTE_1CHAN_PLAYBACK,			/* 23 */
	ROUTE_2CHAN_PLAYBACK,			/* 24 */
	ROUTE_1CHAN_CAPTURE,			/* 25 */
	ROUTE_2CHAN_CAPTURE,			/* 26 */
	ROUTE_AUDIOFOCUS_2MIC,			/* 27 */
	ROUTE_AUDIOFOCUS_3MIC,			/* 28 */
	ROUTE_2MIC_NS_CT_ANALOG,		/* 29 */
	ROUTE_2MIC_NS_FT_ANALOG,		/* 30 */
	ROUTE_2MIC_NS_FO_ANALOG,		/* 31 */
	ROUTE_3MIC_NS_CT_ANALOG,		/* 32 */
	ROUTE_3MIC_NS_FT_ANALOG,		/* 33 */
	ROUTE_3MIC_NS_FO_ANALOG,		/* 34 */
	ROUTE_AEC7_2MIC_NS_FT,			/* 35 */
	ROUTE_AEC7_3MIC_NS_FT,			/* 36 */
	ROUTE_ASR_2MIC_NS_AF,			/* 37 */
	ROUTE_ASR_3MIC_NS_AF,			/* 38 */
	ROUTE_AZV_2MIC,					/* 39 */
	ROUTE_AZV_3MIC,					/* 40 */
	ROUTE_STEREO_RECORD_48K,		/* 41 */
	ROUTE_STEREO_RECORD_96K,		/* 42 */
	ROUTE_STEREO_RECORD_192K,		/* 43 */
	ROUTE_AVALON_PLAY,				/* 44 */
	ROUTE_AVALON_CAPTURE,			/* 45 */
	ROUTE_ASR_1MIC_NS_AF_WB,		/* 46 */
	ROUTE_ASR_2MIC_NS_AF_WB,		/* 47 */
	ROUTE_VOICESENSE_PDM_16K,		/* 48 */
	ROUTE_TRAINING_KEYWORD_8K,		/* 49 */
	ROUTE_TRAINING_KEYWORD_16K,		/* 50 */
	ROUTE_ASR_3MIC_SLIM_SDE_24K,		/* 51 */
	ROUTE_ASR_3MIC_SLIM_SDE_48K,		/* 52 */
	ROUTE_MAX						/* 53 */
};

enum {
	RATE_NB,
	RATE_WB,
	RATE_SWB,
	RATE_FB,
	RATE_MAX
};

static const u32 route_off[] = {
	0xffffffff,
};

static const u32 pxx_default_power_preset[] = {
	0x90311388, /* 5000 - Def */
	0xffffffff	/* terminate */
};
static const u32 route_cs_voice_1mic_ct[] = {
	0x903103e9,
	0xffffffff	/* terminate */
};
static const u32 nb_cs_voice_1mic_ct[] = {
	0x90310227, /* 551 */
	0xffffffff	/* terminate */
};
static const u32 wb_cs_voice_1mic_ct[] = {
	0x9031024f, /* 591 */
	0xffffffff	/* terminate */
};
static const u32 swb_cs_voice_1mic_ct[] = {
	0x90310250, /* 592 */
	0xffffffff	/* terminate */
};

static const u32 route_cs_voice_2mic_ct[] = {
	0x9031041b,	/* without PDM  */
/*	0x90310577,	 1399 with PDM*/
	0xffffffff	/* terminate */
};
static const u32 nb_cs_voice_2mic_ct[] = {
	0x9031022a, /* 554 */
	0xffffffff	/* terminate */
};
static const u32 wb_cs_voice_2mic_ct[] = {
	0x9031022b, /* 555 */
	0xffffffff	/* terminate */
};
static const u32 swb_cs_voice_2mic_ct[] = {
	0x9031022c, /* 556 */
	0xffffffff	/* terminate */
};

static const u32 route_cs_voice_3mic_ct[] = {
	0x903157d,	/* 1405, 3 mic, 1PDM-2Analog */
	0xffffffff	/* terminate */
};
static const u32 nb_cs_voice_3mic_ct[] = {
	0xffffffff	/* terminate */
};
static const u32 wb_cs_voice_3mic_ct[] = {
	0xffffffff	/* terminate */
};
static const u32 swb_cs_voice_3mic_ct[] = {
	0xffffffff	/* terminate */
};

static const u32 route_cs_voice_1mic_ft[] = {
	0x903103e9,
	0xffffffff	/* terminate */
};
static const u32 nb_cs_voice_1mic_ft[] = {
	0x90310230, /* 560 */
	0xffffffff	/* terminate */
};
static const u32 wb_cs_voice_1mic_ft[] = {
	0x9031024f, /* 591 */
	0xffffffff	/* terminate */
};
static const u32 swb_cs_voice_1mic_ft[] = {
	0x90310250, /* 592 */
	0xffffffff	/* terminate */
};

static const u32 route_cs_voice_2mic_ft[] = {
	0x9031041b,	/* Analog Mic */
/*	0x90310577,	1399, 2mic, PDM mic */
	0xffffffff	/* terminate */
};
static const u32 nb_cs_voice_2mic_ft[] = {
	0x9031023c, /* 572 */
	0xffffffff	/* terminate */
};
static const u32 wb_cs_voice_2mic_ft[] = {
	0x9031023a, /* 570 */
	0xffffffff	/* terminate */
};
static const u32 swb_cs_voice_2mic_ft[] = {
	0x9031023b, /*571 */
	0xffffffff	/* terminate */
};

static const u32 route_cs_voice_3mic_ft[] = {
	0xffffffff	/* terminate */
};
static const u32 nb_cs_voice_3mic_ft[] = {
	0xffffffff	/* terminate */
};
static const u32 wb_cs_voice_3mic_ft[] = {
	0xffffffff	/* terminate */
};
static const u32 swb_cs_voice_3mic_ft[] = {
	0xffffffff	/* terminate */
};

static const u32 route_cs_voice_headset[] = {
	0x903103ed,
	0xffffffff	/* terminate */
};
static const u32 nb_cs_voice_headset[] = {
	0x9031024e,
	0xffffffff	/* terminate */
};
static const u32 wb_cs_voice_headset[] = {
	0x9031024f,
	0xffffffff	/* terminate */
};
static const u32 swb_cs_voice_headset[] = {
	0xffffffff	/* terminate */
};
static const u32 route_cs_voice_1mic_headphone[] = {
	0x903103ed,
	0xffffffff	/* terminate */
};
static const u32 nb_cs_voice_1mic_headphone[] = {
	0xffffffff	/* terminate */
};
static const u32 wb_cs_voice_1mic_headphone[] = {
	0xffffffff	/* terminate */
};
static const u32 swb_cs_voice_1mic_headphone[] = {
	0xffffffff	/* terminate */
};

static const u32 route_voip_1mic_ct[] = {
	0x903103e9,
	0xffffffff	/* terminate */
};
static const u32 nb_voip_1mic_ct[] = {
	0x9031024e,
	0xffffffff	/* terminate */
};
static const u32 wb_voip_1mic_ct[] = {
	0x9031024f,
	0xffffffff	/* terminate */
};
static const u32 swb_voip_1mic_ct[] = {
	0x90310250,
	0xffffffff	/* terminate */
};

static const u32 route_voip_2mic_ct[] = {
	/*	0x9031041b,     without PDM  */
	0x90310577,	/* 1399 with PDM*/
	0xffffffff	/* terminate */
};
static const u32 nb_voip_2mic_ct[] = {
	0x9031022a, /* 554 */
	0xffffffff	/* terminate */
};
static const u32 wb_voip_2mic_ct[] = {
	0x9031022b, /* 555 */
	0xffffffff	/* terminate */
};
static const u32 swb_voip_2mic_ct[] = {
	0x9031022c,
	0xffffffff	/* terminate */
};
static const u32 fb_voip_2mic_ct[] = {
	0x90310272, /* 626 */
	0xffffffff	/* terminate */
};

static const u32 route_voip_3mic_ct[] = {
	0x9031057d,	/* 1405, 3 mic, 1PDM-2Analog */
	0xffffffff	/* terminate */
};
static const u32 nb_voip_3mic_ct[] = {
	0x9031022d, /* 557 */
	0xffffffff	/* terminate */
};
static const u32 wb_voip_3mic_ct[] = {
	0x9031022e, /* 558 */
	0xffffffff	/* terminate */
};
static const u32 swb_voip_3mic_ct[] = {
	0x9031022f, /* 559 */
	0xffffffff	/* terminate */
};
static const u32 fb_voip_3mic_ct[] = {
	0x90310273, /* 627 */
	0xffffffff	/* terminate */
};

static const u32 route_voip_1mic_ft[] = {
	0x903103e9,
	0xffffffff	/* terminate */
};
static const u32 nb_voip_1mic_ft[] = {
	0x9031024e,
	0xffffffff	/* terminate */
};
static const u32 wb_voip_1mic_ft[] = {
	0x9031024f,
	0xffffffff	/* terminate */
};
static const u32 swb_voip_1mic_ft[] = {
	0x90310250,
	0xffffffff	/* terminate */
};

static const u32 route_voip_2mic_ft[] = {
/*	0x9031041b, Analog Mic */
	0x90310577, /* 1399, 2mic, PDM mic */
	0xffffffff	/* terminate */
};
static const u32 nb_voip_2mic_ft[] = {
	0x90310239, /* 570 */
	0xffffffff	/* terminate */
};
static const u32 wb_voip_2mic_ft[] = {
	0x9031023a, /* 571 */
	0xffffffff	/* terminate */
};
static const u32 swb_voip_2mic_ft[] = {
	0x9031023b, /* 572 */
	0xffffffff	/* terminate */
};
static const u32 fb_voip_2mic_ft[] = {
	0x90310277, /* 631 */
	0xffffffff	/* terminate */
};

static const u32 route_voip_3mic_ft[] = {
	0x90310595,	/* 1429, 3 mic, 1PDM-2Analog, Ter is PDM Mic */
	0xffffffff	/* terminate */
};
static const u32 nb_voip_3mic_ft[] = {
	0x90310242, /* 578 */
	0xffffffff	/* terminate */
};
static const u32 wb_voip_3mic_ft[] = {
	0x90310243, /* 579 */
	0xffffffff	/* terminate */
};
static const u32 swb_voip_3mic_ft[] = {
	0x90310244, /* 580 */
	0xffffffff	/* terminate */
};
static const u32 fb_voip_3mic_ft[] = {
	0x9031027a, /* 634 */
	0xffffffff	/* terminate */
};
static const u32 route_voip_headset[] = {
	0x903103ed,
	0xffffffff	/* terminate */
};
static const u32 nb_voip_headset[] = {
	0x9031024e, /* 590 */
	0xffffffff	/* terminate */
};
static const u32 wb_voip_headset[] = {
	0x9031024f, /* 591 */
	0xffffffff	/* terminate */
};
static const u32 swb_voip_headset[] = {
	0xffffffff	/* terminate */
};

static const u32 route_voip_1mic_headphone[] = {
	0x903103ed,
	0xffffffff	/* terminate */
};
static const u32 nb_voip_1mic_headphone[] = {
	0xffffffff	/* terminate */
};
static const u32 wb_voip_1mic_headphone[] = {
	0xffffffff	/* terminate */
};
static const u32 swb_voip_1mic_headphone[] = {
	0xffffffff	/* terminate */
};

static const u32 route_voice_asr_1mic[] = {
	0x903104b1,
	0xffffffff	/* terminate */
};
static const u32 nb_voice_asr_1mic[] = {
	0xffffffff	/* terminate */
};
static const u32 wb_voice_asr_1mic[] = {
	0xffffffff	/* terminate */
};
static const u32 swb_voice_asr_1mic[] = {
	0xffffffff	/* terminate */
};

static const u32 route_voice_asr_2mic[] = {
	0x903104ca,
	0xffffffff	/* terminate */
};
static const u32 nb_voice_asr_2mic[] = {
	0xffffffff	/* terminate */
};
static const u32 wb_voice_asr_2mic[] = {
	0xffffffff	/* terminate */
};
static const u32 swb_voice_asr_2mic[] = {
	0xffffffff	/* terminate */
};

static const u32 route_voice_asr_3mic[] = {
	0xffffffff	/* terminate */
};
static const u32 nb_voice_asr_3mic[] = {
	0xffffffff	/* terminate */
};
static const u32 wb_voice_asr_3mic[] = {
	0xffffffff	/* terminate */
};
static const u32 swb_voice_asr_3mic[] = {
	0xffffffff	/* terminate */
};

static const u32 route_voicesense_sbusrx4[] = {
	0xb05a0a40,	/* SBUS.Rx4 -> RxChMgr0*/
	0xb05b0000,	/* Set PathId  RxChMgr0 PATH_ID_PRI */
	0xb0640038,	/* connect RxChMgr0.o0 */
	0xb0640190,	/* connect senseVoice.i0 */
	0xb0630003,	/* set rate RxChMgr0 8k*/
	0xb0630019,	/* set rate senseVoice 8k*/
	0xb0680300,	/* set group RxChMgr0 0*/
	0xb0681900,	/* set group sensevoice 0*/
	0xb00c0900,	/* setDeviceParamID set Multichannel Link Bits*/
	0x900d0000,	/* setDeviceParam Slimbus Linked*/
	0xffffffff	/* terminate */
};

static const u32 route_voicesense_pdm[] = {
	0x90311f40,	 /* 8k route */
	0x90312008,
	0xffffffff      /* terminate */
};

static const u32 route_voicesense_sbusrx0[] = {
	0xb05a0a00,	/* SBUS.Rx0 -> RxChMgr0*/
	0xb05b0000,	/* Set PathId  RxChMgr0 PATH_ID_PRI */
	0xb0640038,	/* connect RxChMgr0.o0 */
	0xb0640190,	/* connect senseVoice.i0 */
	0xb0630003,	/* set rate RxChMgr0 8k*/
	0xb0630019,	/* set rate senseVoice 8k*/
	0xb0680300,	/* set group RxChMgr0 0*/
	0xb0681900,	/* set group sensevoice 0*/
	0xb00c0900,	/* setDeviceParamID set Multichannel Link Bits*/
	0x900d0000,	/* setDeviceParam Slimbus Linked*/
	0xffffffff	/* terminate */
};

static const u32 route_1chan_playback[] = {
	0x9031047f,
	0xffffffff	/* terminate */
};

static const u32 route_2chan_playback[] = {
	0x90310483,
	0xffffffff	/* terminate */
};

static const u32 route_1chan_capture[] = {
	0x90310480,
	0xffffffff	/* terminate */
};

static const u32 route_2chan_capture[] = {
	0x90310484,
	0xffffffff	/* terminate */
};

static const u32 route_audiofocus_2mic[] = {
	0x9031054b,
	0xffffffff	/* terminate */
};

static const u32 route_audiofocus_3mic[] = {
	0x9031054f,
	0xffffffff	/* terminate */
};

static const u32 route_2mic_ns_ct_analog[] = {
	0x9031041b, /* 1051 */
	0x9031022A, /* 554 */
	0xffffffff	/* terminate */
};

static const u32 route_2mic_ns_ft_analog[] = {
	0x9031041b, /* 1051 */
	0x90310239, /* 569 */
	0xffffffff	/* terminate */
};

static const u32 route_2mic_ns_fo_analog[] = {
	0x9031041b, /* 1051 */
	0x90310391, /* 913 */
	0xffffffff	/* terminate */
};

static const u32 route_3mic_ns_ct_analog[] = {
	0x9031044d, /* 1101 */
	0x9031022D, /* 557 */
	0xffffffff	/* terminate */
};

static const u32 route_3mic_ns_ft_analog[] = {
	0x9031044d, /* 1101 */
	0x90310242, /* 578 */
	0xffffffff	/* terminate */
};

static const u32 route_3mic_ns_fo_analog[] = {
	0x9031044d, /* 1101 */
	0x9031039d, /* 925 */
	0xffffffff	/* terminate */
};

static const u32 route_aec7_2mic_ns_ft[] = {
	0x9031041b, /* 1051 */
	0x90310239, /* 569 */
	0xffffffff	/* terminate */
};

static const u32 route_aec7_3mic_ns_ft[] = {
	0xffffffff	/* terminate */
};

static const u32 route_asr_2mic_ns_af[] = {
	0x903104ca, /* 1226 */
	0x903103b1, /* 945 */
	0xffffffff	/* terminate */
};

static const u32 route_asr_3mic_ns_af[] = {
	0x903104e3, /* 1251 */
	0x903103b9, /* 953 */
	0xffffffff	/* terminate */
};

static const u32 route_azv_2mic[] = {
	0xffffffff	/* terminate */
};

static const u32 route_azv_3mic[] = {
	0xffffffff	/* terminate */
};

static const u32 route_stereo_record_48k[] = {
	0x9031049a, /* 1178 */
	0x9031028d, /* 653 */
	0xffffffff	/* terminate */
};

static const u32 route_stereo_record_96k[] = {
	0x9031049a, /* 1178 */
	0x9031029f, /* 671 */
	0xffffffff	/* terminate */
};

static const u32 route_stereo_record_192k[] = {
	0x9031049a, /* 1178 */
	0x903102ae, /* 686 */
	0xffffffff	/* terminate */
};

static const u32 route_avalon_play[] = {
	0x90310497, /* 1175 */
	0xffffffff	/* terminate */
};
static const u32 route_avalon_capture[] = {
	0x9031049a, /* 1178 */
	0xffffffff	/* terminate */
};

static const u32 route_asr_1mic_ns_af_wb[] = {
	0x903104b1, /* 1201 */
	0x9031025e, /* 606 */
	0xffffffff	/* terminate */
};

static const u32 route_asr_2mic_ns_af_wb[] = {
	0x903104ca, /* 1226 */
	0x90310261, /* 609 */
	0xffffffff	/* terminate */
};

static const u32 route_voicesense_pdm_16k[] = {
	0x90311f40,	/* 16k route */
	0x90312008,
	0xffffffff      /* terminate */
};

static const u32 route_training_keyword_8k[] = {
	0x903105a3, /* 16K Route for recording the trained keyword PCR18 */
	0xffffffff      /* terminate */
};

static const u32 route_training_keyword_16k[] = {
	0x903105a4, /* 16K Route for recording the trained keyword PCR18 */
	0xffffffff      /* terminate */
};

static const u32 route_asr_3mic_slim_sde_24k[] = {
	0x9031054e, /* 1358 - 24K Route for 3 mic ASR SDE */
	0xffffffff      /* terminate */
};

static const u32 route_asr_3mic_slim_sde_48k[] = {
	0x903105a3, /* 1443 - 48K Route for 3 mic ASR SDE */
	0xffffffff      /* terminate */
};

static const u32 route_call[] = {
	0xB00C0A00,// ;
	0xB00D000F,// ;
	0xB00C0A09,// ;
	0xB00D0010,// ;
	0xB05A0C00,//	; Set Path	pdm0.rx 		RxChMgr0
	0xB05A0001,//	; Set Path	pcm0.rx0		RxChMgr1
	0xB05A000A,//	; Set Path	pcm0.tx0		TxChMgr0
	0xB05A001B,//	; Set Path	pcm0.tx1		TxChMgr1
	0xB05B0000,//	; Set PathId	RxChMgr0		PATH_ID_PRI
	0xB05B0103,//	; Set PathId	RxChMgr1		PATH_ID_FEI
	0xB05B0A20,//	; Set PathId	TxChMgr0		PATH_ID_CSO
	0xB05B0B22,//	; Set PathId	TxChMgr1		PATH_ID_FO1
	0xB0640038,//	; Connect	RxChMgr0.o0 	(Connect # 0 Output)
	0xB0640131,//	; Connect	voice.i1		(Connect # 0 Input)
	0xB0640048,//	; Connect	RxChMgr1.o0 	(Connect # 1 Output)
	0xB0640130,//	; Connect	voice.i0		(Connect # 1 Input)
	0xB0640138,//	; Connect	voice.o0		(Connect # 2 Output)
	0xB06400E0,//	; Connect	TxChMgr1.i0 	(Connect # 2 Input)
	0xB0640139,//	; Connect	voice.o1		(Connect # 3 Output)
	0xB06400D0,//	; Connect	TxChMgr0.i0 	(Connect # 3 Input)
	0xB0450005,//	; Set Max External Port Rate	192k
	0xB0630003,//	; Set Rate	RxChMgr0		  8k
	0xB0630004,//	; Set Rate	RxChMgr1		  8k
	0xB017004F,//	; A300_msg_SetAlgorithmParmID	BPID_USE_BWE		 
	0xB0180000,//	; A300_msg_SetAlgorithmParm 	No					 
	0xB0630013,//	; Set Rate	voice			  8k
	0xB063000D,//	; Set Rate	TxChMgr0		  8k
	0xB063000E,//	; Set Rate	TxChMgr1		  8k
	0xB0680300,//	; Set Group RxChMgr0		  0
	0xB0680400,//	; Set Group RxChMgr1		  0
	0xB0681300,//	; Set Group voice			  0
	0xB0680D00,//	; Set Group TxChMgr0		  0
	0xB0680E00,//	; Set Group TxChMgr1		  0
	0x80000000,//	; Sync to commit changes
	0xffffffff      /* terminate */
};

static const u32 route_PT_mic0[] = {
	0xB00C0A00,//;
	0xB00D000F,//;
	0xB00C0A09,//;
	0xB00D0010,//;
//!Preset id: 1152 ; Passthrough-1 Ch PT Type2
	0xB05A0C00,// ; Set Path pdm0.rx RxChMgr0
	0xB05A000A,// ; Set Path pcm0.tx0 TxChMgr0
	0xB05B000D,// ; Set PathId RxChMgr0 PATH_ID_PI1
	0xB05B0A2C,// ; Set PathId TxChMgr0 PATH_ID_PO1
	0xB0640038,// ; Connect RxChMgr0.o0 (Connect # 0 Output)
	0xB0640160,// ; Connect passthru.i0 (Connect # 0 Input)
	0xB0640168,// ; Connect passthru.o0 (Connect # 1 Output)
	0xB06400D0,// ; Connect TxChMgr0.i0 (Connect # 1 Input)
	0xB0450005,// ; Set Max External Port Rate 192k
	0xB0630103,// ; Set Rate RxChMgr0 16k
	0xB0630116,// ; Set Rate passthru 16k
	0xB063010D,// ; Set Rate TxChMgr0 16k
	0xB0680300,// ; Set Group RxChMgr0 0
	0xB0681600,// ; Set Group passthru 0
	0xB0680D00,// ; Set Group TxChMgr0 0
	//0xB00C0900,// ; A300_msg_SetDeviceParmID Set Multichannel Link bits
	//0xB00D0000,// ; A300_msg_SetDeviceParm SLIMbus Linked: <no linked, so clear bits>
	0xB069000D,// ; set End Point of PI1
	0xB015000C,// ; set digital Gain PI1 12dB
	0x80000000,// ; Sync to commit changes
	0xffffffff      /* terminate */
};

static const u32 route_PT_mic2[] = {
	0xB00C0A00,//;
	0xB00D000F,//;
	0xB00C0A09,//;
	0xB00D0010,//;
//!Preset id: 1152 ; Passthrough-1 Ch PT Type2
	0xB05A1000,// ; Set Path pdm2.rx RxChMgr0
	0xB05A000A,// ; Set Path pcm0.tx0 TxChMgr0
	0xB05B000D,// ; Set PathId RxChMgr0 PATH_ID_PI1
	0xB05B0A2C,// ; Set PathId TxChMgr0 PATH_ID_PO1
	0xB0640038,// ; Connect RxChMgr0.o0 (Connect # 0 Output)
	0xB0640160,// ; Connect passthru.i0 (Connect # 0 Input)
	0xB0640168,// ; Connect passthru.o0 (Connect # 1 Output)
	0xB06400D0,// ; Connect TxChMgr0.i0 (Connect # 1 Input)
	0xB0450005,// ; Set Max External Port Rate 192k
	0xB0630103,// ; Set Rate RxChMgr0 16k
	0xB0630116,// ; Set Rate passthru 16k
	0xB063010D,// ; Set Rate TxChMgr0 16k
	0xB0680300,// ; Set Group RxChMgr0 0
	0xB0681600,// ; Set Group passthru 0
	0xB0680D00,// ; Set Group TxChMgr0 0
	//0xB00C0900,// ; A300_msg_SetDeviceParmID Set Multichannel Link bits
	//0xB00D0000,// ; A300_msg_SetDeviceParm SLIMbus Linked: <no linked, so clear bits>
	0xB069000D,// ; set End Point of PI1
	0xB015000C,// ; set digital Gain PI1 12dB
	0x80000000,// ; Sync to commit changes
	0xffffffff      /* terminate */
};

static const u32 route_PT_mic3[] = {
	0xB00C0A00,//;
	0xB00D000F,//;
	0xB00C0A09,//;
	0xB00D0010,//;
//!Preset id: 1152 ; Passthrough-1 Ch PT Type2
	0xB05A1200,// ; Set Path pdm3.rx RxChMgr0
	0xB05A000A,// ; Set Path pcm0.tx0 TxChMgr0
	0xB05B000D,// ; Set PathId RxChMgr0 PATH_ID_PI1
	0xB05B0A2C,// ; Set PathId TxChMgr0 PATH_ID_PO1
	0xB0640038,// ; Connect RxChMgr0.o0 (Connect # 0 Output)
	0xB0640160,// ; Connect passthru.i0 (Connect # 0 Input)
	0xB0640168,// ; Connect passthru.o0 (Connect # 1 Output)
	0xB06400D0,// ; Connect TxChMgr0.i0 (Connect # 1 Input)
	0xB0450005,// ; Set Max External Port Rate 192k
	0xB0630103,// ; Set Rate RxChMgr0 16k
	0xB0630116,// ; Set Rate passthru 16k
	0xB063010D,// ; Set Rate TxChMgr0 16k
	0xB0680300,// ; Set Group RxChMgr0 0
	0xB0681600,// ; Set Group passthru 0
	0xB0680D00,// ; Set Group TxChMgr0 0
	//0xB00C0900,// ; A300_msg_SetDeviceParmID Set Multichannel Link bits
	//0xB00D0000,// ; A300_msg_SetDeviceParm SLIMbus Linked: <no linked, so clear bits>
	0xB069000D,// ; set End Point of PI1
	0xB015000C,// ; set digital Gain PI1 12dB
	0x80000000,// ; Sync to commit changes
	0xffffffff      /* terminate */
};


static const u32 route_voicecall_3mic_noref[] = {
	0xB00C0A00,//;
	0xB00D000F,//;
	0xB00C0A09,//;
	0xB00D0010,//;
	//!Preset id: 1101 ;	, Base Band 1, 3 MIC, 1 FEIN, 1 CSOUT, 1 FEOUT
	0xB05A0C00,//	; Set Path	pdm0.rx 		RxChMgr0
	0xB05A1001,//	; Set Path	pdm2.rx 		RxChMgr1
	0xB05A1202,//	; Set Path	pdm3.rx 		RxChMgr2
	0xB05A0003,//	; Set Path	pcm0.rx0		RxChMgr3
	0xB05A000A,//	; Set Path	pcm0.tx0		TxChMgr0
	0xB05A140B,//	; Set Path	pdm0.tx 		TxChMgr1
	0xB05B0000,//	; Set PathId	RxChMgr0		PATH_ID_PRI
	0xB05B0101,//	; Set PathId	RxChMgr1		PATH_ID_SEC
	0xB05B0202,//	; Set PathId	RxChMgr2		PATH_ID_TER
	0xB05B0303,//	; Set PathId	RxChMgr3		PATH_ID_FEI
	0xB05B0A20,//	; Set PathId	TxChMgr0		PATH_ID_CSO
	0xB05B0B22,//	; Set PathId	TxChMgr1		PATH_ID_FO1
	0xB0640038,//	; Connect	RxChMgr0.o0 	(Connect # 0 Output)
	0xB0640131,//	; Connect	voice.i1		(Connect # 0 Input)
	0xB0640048,//	; Connect	RxChMgr1.o0 	(Connect # 1 Output)
	0xB0640132,//	; Connect	voice.i2		(Connect # 1 Input)
	0xB0640058,//	; Connect	RxChMgr2.o0 	(Connect # 2 Output)
	0xB0640133,//	; Connect	voice.i3		(Connect # 2 Input)
	0xB0640068,//	; Connect	RxChMgr3.o0 	(Connect # 3 Output)
	0xB0640130,//	; Connect	voice.i0		(Connect # 3 Input)
	0xB0640138,//	; Connect	voice.o0		(Connect # 4 Output)
	0xB06400E0,//	; Connect	TxChMgr1.i0 	(Connect # 4 Input)
	0xB0640139,//	; Connect	voice.o1		(Connect # 5 Output)
	0xB06400D0,//	; Connect	TxChMgr0.i0 	(Connect # 5 Input)
	0xB0450005,//	; Set Max External Port Rate	192k
	0xB0630003,//	; Set Rate	RxChMgr0		  8k
	0xB0630004,//	; Set Rate	RxChMgr1		  8k
	0xB0630005,//	; Set Rate	RxChMgr2		  8k
	0xB0630006,//	; Set Rate	RxChMgr3		  8k
	0xB017004F,//	; A300_msg_SetAlgorithmParmID	BPID_USE_BWE		 
	0xB0180000,//	; A300_msg_SetAlgorithmParm 	No					 
	0xB0630013,//	; Set Rate	voice			  8k
	0xB063000D,//	; Set Rate	TxChMgr0		  8k
	0xB063000E,//	; Set Rate	TxChMgr1		  8k
	0xB0680300,//	; Set Group RxChMgr0		  0
	0xB0680400,//	; Set Group RxChMgr1		  0
	0xB0680500,//	; Set Group RxChMgr2		  0
	0xB0680600,//	; Set Group RxChMgr3		  0
	0xB0681300,//	; Set Group voice			  0
	0xB0680D00,//	; Set Group TxChMgr0		  0
	0xB0680E00,//	; Set Group TxChMgr1		  0
	0x80000000,//	; Sync to commit changes
	0xffffffff      /* terminate */
};

static const u32 route_voicecall_3mic_ref_nofein[] = {
	0xB00C0A00,//;
	0xB00D000F,//;
	0xB00C0A09,//;
	0xB00D0010,//;
	0xB00C0B00,//;
	0xB00D000F,//;
	0xB00C0B09,//;
	0xB00D0010,//;
	//!Preset id: 1251 ;  , Automatic Speech Recognition ASR,3 MIC, 1 CSOUT
	0xB05A0C00,//	; Set Path	pdm0.rx     	RxChMgr0
	0xB05A1001,//	; Set Path	pdm2.rx     	RxChMgr1
	0xB05A1202,//	; Set Path	pdm3.rx     	RxChMgr2
	0xB05A0203,//	; Set Path	pcm1.rx0    	RxChMgr3
	0xB05A000A,//	; Set Path	pcm0.tx0    	TxChMgr0
	0xB05B0000,//	; Set PathId	RxChMgr0    	PATH_ID_PRI
	0xB05B0101,//	; Set PathId	RxChMgr1    	PATH_ID_SEC
	0xB05B0202,//	; Set PathId	RxChMgr2    	PATH_ID_TER
	0xB05B0304,//	; Set PathId	RxChMgr3    	PATH_ID_AEC
	0xB05B0A20,//	; Set PathId	TxChMgr0    	PATH_ID_CSO
	0xB0640038,//	; Connect	RxChMgr0.o0 	(Connect # 0 Output)
	0xB0640131,//	; Connect	voice.i1    	(Connect # 0 Input)
	0xB0640048,//	; Connect	RxChMgr1.o0 	(Connect # 1 Output)
	0xB0640132,//	; Connect	voice.i2    	(Connect # 1 Input)
	0xB0640058,//	; Connect	RxChMgr2.o0 	(Connect # 2 Output)
	0xB0640133,//	; Connect	voice.i3    	(Connect # 2 Input)
	0xB0640068,//	; Connect	RxChMgr3.o0 	(Connect # 3 Output)
	0xB0640134,//	; Connect	voice.i4    	(Connect # 3 Input)
	0xB0640139,//	; Connect	voice.o1    	(Connect # 4 Output)
	0xB06400D0,//	; Connect	TxChMgr0.i0 	(Connect # 4 Input)
	0xB0450005,//	; Set Max External Port Rate	192k
	0xB0630003,//	; Set Rate	RxChMgr0    	  8k
	0xB0630004,//	; Set Rate	RxChMgr1    	  8k
	0xB0630005,//	; Set Rate	RxChMgr2    	  8k
	0xB0630006,//	; Set Rate	RxChMgr3    	  8k
	0xB017004F,//	; A300_msg_SetAlgorithmParmID	BPID_USE_BWE         
	0xB0180000,//	; A300_msg_SetAlgorithmParm  	No                   
	0xB0630013,//	; Set Rate	voice       	  8k
	0xB063000D,//	; Set Rate	TxChMgr0    	  8k
	0xB0680300,//	; Set Group	RxChMgr0     	  0
	0xB0680400,//	; Set Group	RxChMgr1     	  0
	0xB0680500,//	; Set Group	RxChMgr2     	  0
	0xB0680600,//	; Set Group	RxChMgr3     	  0
	0xB0681300,//	; Set Group	voice        	  0
	0xB0680D00,//	; Set Group	TxChMgr0     	  0
	0x80000000,//	; Sync to commit changes
	0xffffffff      /* terminate */
};

static const u32 route_rec_mono[] = {
	0xB00C0A00,//;
	0xB00D001F,//;
	0xB00C0A09,//;
	0xB00D0030,//;
//!Preset id: 1152 ; Passthrough-1 Ch PT Type2
	0xB05A0C00,// ; Set Path pdm0.rx RxChMgr0
	0xB05A000A,// ; Set Path sbus.tx0 TxChMgr0
	0xB05B000D,// ; Set PathId RxChMgr0 PATH_ID_PI1
	0xB05B0A2C,// ; Set PathId TxChMgr0 PATH_ID_PO1
	0xB0640038,// ; Connect RxChMgr0.o0 (Connect # 0 Output)
	0xB0640160,// ; Connect passthru.i0 (Connect # 0 Input)
	0xB0640168,// ; Connect passthru.o0 (Connect # 1 Output)
	0xB06400D0,// ; Connect TxChMgr0.i0 (Connect # 1 Input)
	0xB0450005,// ; Set Max External Port Rate 192k
	0xB0630303,// ; Set Rate RxChMgr0 48k
	0xB0630316,// ; Set Rate passthru 48k
	0xB063030D,// ; Set Rate TxChMgr0 48k
	0xB0680300,// ; Set Group RxChMgr0 0
	0xB0681600,// ; Set Group passthru 0
	0xB0680D00,// ; Set Group TxChMgr0 0
	//0xB00C0900,// ; A300_msg_SetDeviceParmID Set Multichannel Link bits
	//0xB00D0000,// ; A300_msg_SetDeviceParm SLIMbus Linked: <no linked, so clear bits>
	0xB069000D,// ; set End Point of PI1
	0xB015000C,// ; set digital Gain PI1 12dB
	0x80000000,// ; Sync to commit changes
	0xffffffff      /* terminate */
};

//MIC1-MIC2:L-R
static const u32 route_rec_stereo[] = {
	0xB00C0A00,//;
	0xB00D001F,//;
	0xB00C0A09,//;
	0xB00D002C,//;2C-44100,30-48000,modify rec sample 44100
	//!Preset id: 1155 ;	, 2 Channel Pass Through Type 1, 2 Audio
	0xB05A0C00,//	; Set Path	pdm0.rx 		RxChMgr0
	0xB05A1001,//	; Set Path	pdm2.rx 		RxChMgr1
	0xB05A000A,//	; Set Path	pcm0.tx0		TxChMgr0
	0xB05A001B,//	; Set Path	pcm0.tx1		TxChMgr1
	0xB05B000D,//	; Set PathId	RxChMgr0		PATH_ID_PI1
	0xB05B010E,//	; Set PathId	RxChMgr1		PATH_ID_PI2
	0xB05B0A2C,//	; Set PathId	TxChMgr0		PATH_ID_PO1
	0xB05B0B2D,//	; Set PathId	TxChMgr1		PATH_ID_PO2
	0xB0640038,//	; Connect	RxChMgr0.o0 	(Connect # 0 Output)
	0xB0640160,//	; Connect	passthru.i0 	(Connect # 0 Input)
	0xB0640048,//	; Connect	RxChMgr1.o0 	(Connect # 1 Output)
	0xB0640161,//	; Connect	passthru.i1 	(Connect # 1 Input)
	0xB0640168,//	; Connect	passthru.o0 	(Connect # 2 Output)
	0xB06400D0,//	; Connect	TxChMgr0.i0 	(Connect # 2 Input)
	0xB0640169,//	; Connect	passthru.o1 	(Connect # 3 Output)
	0xB06400E0,//	; Connect	TxChMgr1.i0 	(Connect # 3 Input)
	0xB0450005,//	; Set Max External Port Rate	192k
	0xB0630003,//	; Set Rate	RxChMgr0		  8k
	0xB0630004,//	; Set Rate	RxChMgr1		  8k
	0xB0630016,//	; Set Rate	passthru		  8k
	0xB063000D,//	; Set Rate	TxChMgr0		  8k
	0xB063000E,//	; Set Rate	TxChMgr1		  8k
	0xB0680300,//	; Set Group RxChMgr0		  0
	0xB0680400,//	; Set Group RxChMgr1		  0
	0xB0681600,//	; Set Group passthru		  0
	0xB0680D00,//	; Set Group TxChMgr0		  0
	0xB0680E00,//	; Set Group TxChMgr1		  0
	0x80000000,//	; Sync to commit changes
	0xffffffff      /* terminate */
};

//MIC1:L-R
static const u32 route_rec_stereo_pri_pri[] = {
	0xB00C0A00,//;
	0xB00D001F,//;
	0xB00C0A09,//;
	0xB00D0030,//;
	//!Preset id: 1155 ;	, 2 Channel Pass Through Type 1, 2 Audio
	0xB05A0C00,//	; Set Path	pdm0.rx 		RxChMgr0
	0xB05A0C01,//	; Set Path	pdm0.rx 		RxChMgr1
	0xB05A000A,//	; Set Path	pcm0.tx0		TxChMgr0
	0xB05A001B,//	; Set Path	pcm0.tx1		TxChMgr1
	0xB05B000D,//	; Set PathId	RxChMgr0		PATH_ID_PI1
	0xB05B010E,//	; Set PathId	RxChMgr1		PATH_ID_PI2
	0xB05B0A2C,//	; Set PathId	TxChMgr0		PATH_ID_PO1
	0xB05B0B2D,//	; Set PathId	TxChMgr1		PATH_ID_PO2
	0xB0640038,//	; Connect	RxChMgr0.o0 	(Connect # 0 Output)
	0xB0640160,//	; Connect	passthru.i0 	(Connect # 0 Input)
	0xB0640048,//	; Connect	RxChMgr1.o0 	(Connect # 1 Output)
	0xB0640161,//	; Connect	passthru.i1 	(Connect # 1 Input)
	0xB0640168,//	; Connect	passthru.o0 	(Connect # 2 Output)
	0xB06400D0,//	; Connect	TxChMgr0.i0 	(Connect # 2 Input)
	0xB0640169,//	; Connect	passthru.o1 	(Connect # 3 Output)
	0xB06400E0,//	; Connect	TxChMgr1.i0 	(Connect # 3 Input)
	0xB0450005,//	; Set Max External Port Rate	192k
	0xB0630003,//	; Set Rate	RxChMgr0		  8k
	0xB0630004,//	; Set Rate	RxChMgr1		  8k
	0xB0630016,//	; Set Rate	passthru		  8k
	0xB063000D,//	; Set Rate	TxChMgr0		  8k
	0xB063000E,//	; Set Rate	TxChMgr1		  8k
	0xB0680300,//	; Set Group RxChMgr0		  0
	0xB0680400,//	; Set Group RxChMgr1		  0
	0xB0681600,//	; Set Group passthru		  0
	0xB0680D00,//	; Set Group TxChMgr0		  0
	0xB0680E00,//	; Set Group TxChMgr1		  0
	0x80000000,//	; Sync to commit changes
	0xffffffff      /* terminate */
};

//MIC2-MIC3:L-R
static const u32 route_rec_stereo_sec_ter[] = {
	0xB00C0A00,//;
	0xB00D001F,//;
	0xB00C0A09,//;
	0xB00D0030,//;
	//!Preset id: 1155 ;	, 2 Channel Pass Through Type 1, 2 Audio
	0xB05A1200,//	; Set Path	pdm0.rx 		RxChMgr0
	0xB05A1001,//	; Set Path	pdm2.rx 		RxChMgr1
	0xB05A000A,//	; Set Path	pcm0.tx0		TxChMgr0
	0xB05A001B,//	; Set Path	pcm0.tx1		TxChMgr1
	0xB05B000D,//	; Set PathId	RxChMgr0		PATH_ID_PI1
	0xB05B010E,//	; Set PathId	RxChMgr1		PATH_ID_PI2
	0xB05B0A2C,//	; Set PathId	TxChMgr0		PATH_ID_PO1
	0xB05B0B2D,//	; Set PathId	TxChMgr1		PATH_ID_PO2
	0xB0640038,//	; Connect	RxChMgr0.o0 	(Connect # 0 Output)
	0xB0640160,//	; Connect	passthru.i0 	(Connect # 0 Input)
	0xB0640048,//	; Connect	RxChMgr1.o0 	(Connect # 1 Output)
	0xB0640161,//	; Connect	passthru.i1 	(Connect # 1 Input)
	0xB0640168,//	; Connect	passthru.o0 	(Connect # 2 Output)
	0xB06400D0,//	; Connect	TxChMgr0.i0 	(Connect # 2 Input)
	0xB0640169,//	; Connect	passthru.o1 	(Connect # 3 Output)
	0xB06400E0,//	; Connect	TxChMgr1.i0 	(Connect # 3 Input)
	0xB0450005,//	; Set Max External Port Rate	192k
	0xB0630003,//	; Set Rate	RxChMgr0		  8k
	0xB0630004,//	; Set Rate	RxChMgr1		  8k
	0xB0630016,//	; Set Rate	passthru		  8k
	0xB063000D,//	; Set Rate	TxChMgr0		  8k
	0xB063000E,//	; Set Rate	TxChMgr1		  8k
	0xB0680300,//	; Set Group RxChMgr0		  0
	0xB0680400,//	; Set Group RxChMgr1		  0
	0xB0681600,//	; Set Group passthru		  0
	0xB0680D00,//	; Set Group TxChMgr0		  0
	0xB0680E00,//	; Set Group TxChMgr1		  0
	0x80000000,//	; Sync to commit changes
	0xffffffff      /* terminate */
};

static const struct esxxx_route_config es705_route_config[ROUTE_MAX] = {
	[ROUTE_OFF] = {
		.route = route_off,
	},
#if 0
	[ROUTE_CS_VOICE_1MIC_CT] = {
		.route = route_cs_voice_1mic_ct,
		.nb = nb_cs_voice_1mic_ct,
		.wb = wb_cs_voice_1mic_ct,
		.swb = swb_cs_voice_1mic_ct,
	},
	[ROUTE_CS_VOICE_2MIC_CT] = {
		.route = route_cs_voice_2mic_ct,
		.nb = nb_cs_voice_2mic_ct,
		.wb = wb_cs_voice_2mic_ct,
		.swb = swb_cs_voice_2mic_ct,
	},
	[ROUTE_CS_VOICE_3MIC_CT] = {
		.route = route_cs_voice_3mic_ct,
		.nb = nb_cs_voice_3mic_ct,
		.wb = wb_cs_voice_3mic_ct,
		.swb = swb_cs_voice_3mic_ct,
	},
	[ROUTE_CS_VOICE_1MIC_FT] = {
		.route = route_cs_voice_1mic_ft,
		.nb = nb_cs_voice_1mic_ft,
		.wb = wb_cs_voice_1mic_ft,
		.swb = swb_cs_voice_1mic_ft,
	},
	[ROUTE_CS_VOICE_2MIC_FT] = {
	.route = route_cs_voice_2mic_ft,
	.nb = nb_cs_voice_2mic_ft,
	.wb = wb_cs_voice_2mic_ft,
	.swb = swb_cs_voice_2mic_ft,
	},
	[ROUTE_CS_VOICE_3MIC_FT] = {
		.route = route_cs_voice_3mic_ft,
		.nb = nb_cs_voice_3mic_ft,
		.wb = wb_cs_voice_3mic_ft,
		.swb = swb_cs_voice_3mic_ft,
	},
	[ROUTE_CS_VOICE_HEADSET] = {
		.route = route_cs_voice_headset,
		.nb = nb_cs_voice_headset,
		.wb = wb_cs_voice_headset,
		.swb = swb_cs_voice_headset,
	},
#else
	[ROUTE_CS_VOICE_1MIC_CT] = {
		.route = route_PT_mic0,
	},
	[ROUTE_CS_VOICE_2MIC_CT] = {
		.route = route_PT_mic2,
	},
	[ROUTE_CS_VOICE_3MIC_CT] = {
		.route = route_PT_mic3,
	},
	[ROUTE_CS_VOICE_1MIC_FT] = {
		.route = route_voicecall_3mic_noref,
	},
	[ROUTE_CS_VOICE_2MIC_FT] = {
		.route = route_voicecall_3mic_ref_nofein,
	},
	[ROUTE_CS_VOICE_3MIC_FT] = {
		.route = route_rec_mono,
	},
	[ROUTE_CS_VOICE_HEADSET] = {
		.route = route_rec_stereo,
	},
#endif

	[ROUTE_CS_VOICE_1MIC_HEADPHONE] = {
		.route = route_rec_stereo_pri_pri
	},
	[ROUTE_VOIP_1MIC_CT] = {
		.route = route_rec_stereo_sec_ter,
	},
	[ROUTE_VOIP_2MIC_CT] = {
		.route = route_voip_2mic_ct,
		.nb = nb_voip_2mic_ct,
		.wb = wb_voip_2mic_ct,
		.swb = swb_voip_2mic_ct,
		.fb = fb_voip_2mic_ct,
	},
	[ROUTE_VOIP_3MIC_CT] = {
		.route = route_voip_3mic_ct,
		.nb = nb_voip_3mic_ct,
		.wb = wb_voip_3mic_ct,
		.swb = swb_voip_3mic_ct,
		.fb = fb_voip_3mic_ct,
	},
	[ROUTE_VOIP_1MIC_FT] = {
		.route = route_voip_1mic_ft,
		.nb = nb_voip_1mic_ft,
		.wb = wb_voip_1mic_ft,
		.swb = swb_voip_1mic_ft,
	},
	[ROUTE_VOIP_2MIC_FT] = {
		.route = route_voip_2mic_ft,
		.nb = nb_voip_2mic_ft,
		.wb = wb_voip_2mic_ft,
		.swb = swb_voip_2mic_ft,
		.fb = fb_voip_2mic_ft,
	},
	[ROUTE_VOIP_3MIC_FT] = {
		.route = route_voip_3mic_ft,
		.nb = nb_voip_3mic_ft,
		.wb = wb_voip_3mic_ft,
		.swb = swb_voip_3mic_ft,
		.fb = fb_voip_3mic_ft,
	},
	[ROUTE_VOIP_HEADSET] = {
		.route = route_voip_headset,
		.nb = nb_voip_headset,
		.wb = wb_voip_headset,
		.swb = swb_voip_headset,
	},
	[ROUTE_VOIP_1MIC_HEADPHONE] = {
		.route = route_voip_1mic_headphone,
		.nb = nb_voip_1mic_headphone,
		.wb = wb_voip_1mic_headphone,
		.swb = swb_voip_1mic_headphone,
	},
	[ROUTE_VOICE_ASR_1MIC] = {
		.route = route_voice_asr_1mic,
		.nb = nb_voice_asr_1mic,
		.wb = wb_voice_asr_1mic,
		.swb = swb_voice_asr_1mic,
	},
	[ROUTE_VOICE_ASR_2MIC] = {
		.route = route_voice_asr_2mic,
		.nb = nb_voice_asr_2mic,
		.wb = wb_voice_asr_2mic,
		.swb = swb_voice_asr_2mic,
	},
	[ROUTE_VOICE_ASR_3MIC] = {
		.route = route_voice_asr_3mic,
		.nb = nb_voice_asr_3mic,
		.wb = wb_voice_asr_3mic,
		.swb = swb_voice_asr_3mic,
	},
	[ROUTE_VOICESENSE_SBUSRX4] = {
		.route = route_voicesense_sbusrx4,
	},
	[ROUTE_VOICESENSE_SBUSRX0] = {
		.route = route_voicesense_sbusrx0,
	},
	[ROUTE_VOICESENSE_PDM] = {
		.route = route_voicesense_pdm,
	},
	[ROUTE_1CHAN_PLAYBACK] = {
		.route = route_1chan_playback,
	},
	[ROUTE_2CHAN_PLAYBACK] = {
		.route = route_2chan_playback,
	},
	[ROUTE_1CHAN_CAPTURE] = {
		.route = route_1chan_capture,
	},
	[ROUTE_2CHAN_CAPTURE] = {
		.route = route_2chan_capture,
	},
	[ROUTE_AUDIOFOCUS_2MIC] = {
		.route = route_audiofocus_2mic,
	},
	[ROUTE_AUDIOFOCUS_3MIC] = {
		.route = route_audiofocus_3mic,
	},
	[ROUTE_2MIC_NS_CT_ANALOG] = {
		.route = route_2mic_ns_ct_analog,
	},
	[ROUTE_2MIC_NS_FT_ANALOG] = {
		.route = route_2mic_ns_ft_analog,
	},
	[ROUTE_2MIC_NS_FO_ANALOG] = {
		.route = route_2mic_ns_fo_analog,
	},
	[ROUTE_3MIC_NS_CT_ANALOG] = {
		.route = route_3mic_ns_ct_analog,
	},
	[ROUTE_3MIC_NS_FT_ANALOG] = {
		.route = route_3mic_ns_ft_analog,
	},
	[ROUTE_3MIC_NS_FO_ANALOG] = {
		.route = route_3mic_ns_fo_analog,
	},
	[ROUTE_AEC7_2MIC_NS_FT] = {
	.route = route_aec7_2mic_ns_ft,
	},
	[ROUTE_AEC7_3MIC_NS_FT] = {
	.route = route_aec7_3mic_ns_ft,
	},
	[ROUTE_ASR_2MIC_NS_AF] = {
	.route = route_asr_2mic_ns_af,
	},
	[ROUTE_ASR_3MIC_NS_AF] = {
		.route = route_asr_3mic_ns_af,
	},
	[ROUTE_AZV_2MIC] = {
		.route = route_azv_2mic,
	},
	[ROUTE_AZV_3MIC] = {
		.route = route_azv_3mic,
	},
	[ROUTE_STEREO_RECORD_48K] = {
		.route = route_stereo_record_48k,
	},
	[ROUTE_STEREO_RECORD_96K] = {
		.route = route_stereo_record_96k,
	},
	[ROUTE_STEREO_RECORD_192K] = {
		.route = route_stereo_record_192k,
	},
	[ROUTE_AVALON_PLAY] = {
		.route = route_avalon_play,
	},
	[ROUTE_AVALON_CAPTURE] = {
		.route = route_avalon_capture,
	},
	[ROUTE_ASR_1MIC_NS_AF_WB] = {
		.route = route_asr_1mic_ns_af_wb,
	},
	[ROUTE_ASR_2MIC_NS_AF_WB] = {
		.route = route_asr_2mic_ns_af_wb,
	},
	[ROUTE_VOICESENSE_PDM_16K] = {
		.route = route_voicesense_pdm_16k,
	},
	[ROUTE_TRAINING_KEYWORD_8K] = {
		.route = route_training_keyword_8k,
	},
	[ROUTE_TRAINING_KEYWORD_16K] = {
		.route = route_training_keyword_16k,
	},
	[ROUTE_ASR_3MIC_SLIM_SDE_24K] = {
		.route = route_asr_3mic_slim_sde_24k,
	},
	[ROUTE_ASR_3MIC_SLIM_SDE_48K] = {
		.route = route_asr_3mic_slim_sde_48k,
	},
};

#endif
