/* 
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
/*
 * Definitions for em3071 als/ps sensor chip.
 */
#ifndef __EM30918_H__
#define __EM30918_H__

#include <linux/ioctl.h>

#define EM30918_CMM_ID					0X00
#define EM30918_CMM_ENABLE 				0X01
#define EM30918_CMM_STATUS				0x02
#define EM30918_CMM_INT_PS_LB			0X03
#define EM30918_CMM_INT_PS_HB  			0X04
#define EM30918_CMM_PDATA				0X08
#define EM30918_CMM_C0DATA_L 			0X09
#define EM30918_CMM_C0DATA_H 			0X0A
#define EM30918_CMM_OFFSET				0X0F
//wt add for ps calibration start
#define CUST_EM30918_PS_THRES_FAR        60
#define CUST_EM30918_PS_THRES_CLOSE     200
//wt add for ps calibration end


#define EM30918_SUCCESS					0
#define EM30918_ERR_I2C					-1

//static int dev_attr_config(u8 reg,int config,int dev_s,int data);
#endif
