#include <linux/types.h>
#include <mt-plat/charging.h>
#include <mt-plat/upmu_common.h>
#include <linux/delay.h>
#include <linux/reboot.h>
#include <mt-plat/mt_boot.h>
#include <mt-plat/battery_common.h>
#include <mach/mt_charging.h>
#include <mach/mt_pmic.h>
#include "ncp1851.h"

 // ============================================================ //
 //define
 // ============================================================ //
#define STATUS_OK	0
#define STATUS_FAIL	1
#define STATUS_UNSUPPORTED	-1
#define GETARRAYNUM(array) (sizeof(array)/sizeof(array[0]))

#define NCP1851_PWR_PATH

 // ============================================================ //
 //global variable
 // ============================================================ //

#ifndef GPIO_CHR_SPM_PIN
#define GPIO_CHR_SPM_PIN 65
#endif

const unsigned int VBAT_CV_VTH[]=
{
	3300000,    3325000,    3350000,    3375000,
	3400000,    3425000,    3450000,    3475000,
	3500000,    3525000,    3550000,    3575000,
	3600000,    3625000,    3650000,    3675000,
	3700000,    3725000,    3750000,    3775000,
	3800000,    3825000,    3850000,    3875000,
	3900000,    3925000,    3950000,    3975000,
	4000000,    4025000,    4050000,    4075000,
	4100000,    4125000,    4150000,    4175000,
	4200000,    4225000,    4250000,    4275000,
	4300000,    4325000,    4350000,    4375000,
	4400000,    4425000,    4450000,    4475000,
};

const unsigned int CS_VTH[]=
{
	CHARGE_CURRENT_400_00_MA,   CHARGE_CURRENT_500_00_MA,	CHARGE_CURRENT_600_00_MA, CHARGE_CURRENT_700_00_MA,
	CHARGE_CURRENT_800_00_MA,   CHARGE_CURRENT_900_00_MA,	CHARGE_CURRENT_1000_00_MA, CHARGE_CURRENT_1100_00_MA,
	CHARGE_CURRENT_1200_00_MA,   CHARGE_CURRENT_1300_00_MA,	CHARGE_CURRENT_1400_00_MA, CHARGE_CURRENT_1500_00_MA,
	CHARGE_CURRENT_1600_00_MA,
}; 

 const unsigned int INPUT_CS_VTH[]=
 {
	 CHARGE_CURRENT_100_00_MA,  CHARGE_CURRENT_500_00_MA,  CHARGE_CURRENT_900_00_MA,
	 CHARGE_CURRENT_1500_00_MA,  CHARGE_CURRENT_MAX
 }; 

 const unsigned int VCDT_HV_VTH[]=
 {
	  BATTERY_VOLT_04_200000_V, BATTERY_VOLT_04_250000_V,	  BATTERY_VOLT_04_300000_V,   BATTERY_VOLT_04_350000_V,
	  BATTERY_VOLT_04_400000_V, BATTERY_VOLT_04_450000_V,	  BATTERY_VOLT_04_500000_V,   BATTERY_VOLT_04_550000_V,
	  BATTERY_VOLT_04_600000_V, BATTERY_VOLT_06_000000_V,	  BATTERY_VOLT_06_500000_V,   BATTERY_VOLT_07_000000_V,
	  BATTERY_VOLT_07_500000_V, BATTERY_VOLT_08_500000_V,	  BATTERY_VOLT_09_500000_V,   BATTERY_VOLT_10_500000_V		  
 };

 // ============================================================ //
 // function prototype
 // ============================================================ //
 
 
 // ============================================================ //
 //extern variable
 // ============================================================ //
 
 // ============================================================ //
 //extern function
 // ============================================================ //
 extern unsigned int upmu_get_reg_value(unsigned int reg);
 
 // ============================================================ //
 unsigned int charging_value_to_parameter(const unsigned int *parameter, const unsigned int array_size, const unsigned int val)
{
	if (val < array_size)
	{
		return parameter[val];
	}
	else
	{
		battery_log(BAT_LOG_CRTI, "Can't find the parameter \r\n");	
		return parameter[0];
	}
}

 
 unsigned int charging_parameter_to_value(const unsigned int *parameter, const unsigned int array_size, const unsigned int val)
{
	unsigned int i;

	battery_log(BAT_LOG_FULL, "array_size = %d \r\n", array_size);

	for(i=0;i<array_size;i++)
	{
		if (val == *(parameter + i))
		{
			return i;
		}
	}

	battery_log(BAT_LOG_CRTI, "NO register value match. val=%d\r\n", val);
	//TODO: ASSERT(0);	// not find the value
	return 0;
}


 static unsigned int bmt_find_closest_level(const unsigned int *pList,unsigned int number,unsigned int level)
 {
	 unsigned int i;
	 unsigned int max_value_in_last_element;
 
	 if(pList[0] < pList[1])
		 max_value_in_last_element = KAL_TRUE;
	 else
		 max_value_in_last_element = KAL_FALSE;
 
	 if(max_value_in_last_element == KAL_TRUE)
	 {
		 for(i = (number-1); i != 0; i--)	 //max value in the last element
		 {
			 if(pList[i] <= level)
			 {
				 return pList[i];
			 }	  
		 }

 		 battery_log(BAT_LOG_CRTI, "Can't find closest level, small value first \r\n");
		 return pList[0];
		 //return CHARGE_CURRENT_0_00_MA;
	 }
	 else
	 {
		 for(i = 0; i< number; i++)  // max value in the first element
		 {
			 if(pList[i] <= level)
			 {
				 return pList[i];
			 }	  
		 }

		 battery_log(BAT_LOG_CRTI, "Can't find closest level, large value first \r\n"); 	 
		 return pList[number -1];
  		 //return CHARGE_CURRENT_0_00_MA;
	 }
 }

 static unsigned int charging_hw_init(void *data)
{
    unsigned int ncp1851_status;
 	unsigned int status = STATUS_OK;

    if (Enable_BATDRV_LOG == 1) {
        battery_log(BAT_LOG_CRTI, "[BATTERY:ncp1851] ChargerHwInit_ncp1851\n" );
    }

    ncp1851_status = ncp1851_get_chip_status();

    ncp1851_set_otg_en(0x0);
    ncp1851_set_ntc_en(0x1); //enable NTC_EN for 1851 and dis FACTORY mode for 1854
    ncp1851_set_trans_en(0);
    ncp1851_set_tj_warn_opt(0x1);
    ncp1851_set_jeita_opt(0x1);
//  ncp1851_set_int_mask(0x0); //disable all interrupt
    ncp1851_set_int_mask(0x1); //enable all interrupt for boost mode status monitor
    ncp1851_set_tchg_rst(0x1); //reset charge timer
#ifdef NCP1851_PWR_PATH
    ncp1851_set_pwr_path(0x1);
#else
    ncp1851_set_pwr_path(0x0);
#endif

#if 0
    if((ncp1851_status == 0x8) || (ncp1851_status == 0x9) || (ncp1851_status == 0xA)) //WEAK WAIT, WEAK SAFE, WEAK CHARGE
        ncp1851_set_ctrl_vbat(0x1C); //VCHG = 4.0V
    else if(ncp1851_status == 0x4)
        ncp1851_set_ctrl_vbat(0x2A); //VCHG = 4.35V to decrease charge time
    else
        ncp1851_set_ctrl_vbat(0x24); //VCHG = 4.2V
#endif
	ncp1851_set_ctrl_vbat(0x2A); //VBAT = 4.350V

    ncp1851_set_ieoc(0x4); // terminate current = 200mA

    ncp1851_set_iweak(0x3); //weak charge current = 300mA

    ncp1851_set_ctrl_vfet(0x3); // VFET = 3.4V
    ncp1851_set_vchred(0x2); //reduce 200mV (JEITA)
    ncp1851_set_ichred(0x0); //reduce 400mA (JEITA)
    ncp1851_set_batcold(0x5);
    ncp1851_set_bathot(0x3);
    ncp1851_set_batchilly(0x0);
    ncp1851_set_batwarm(0x0);

	return status;
}

 static unsigned int charging_dump_register(void *data)
 {
 	unsigned int status = STATUS_OK;

    battery_log(BAT_LOG_CRTI, "charging_dump_register\r\n");

    ncp1851_dump_register();
   	
	return status;
 }	


 static unsigned int charging_enable(void *data)
 {
 	unsigned int status = STATUS_OK;
	unsigned int enable = *(unsigned int*)(data);

	if(KAL_TRUE == enable)
	{
		ncp1851_set_chg_en(0x1); // charger enable
		//Set SPM = 1
	}
	else
	{
#if defined(CONFIG_USB_MTK_HDRC_HCD)
   		if(mt_usb_is_device())
#endif 			
    	{
			ncp1851_set_chg_en(0x0); // charger disable
    	}
	}
		
	return status;
 }


 static unsigned int charging_set_cv_voltage(void *data)
 {
 	unsigned int status = STATUS_OK;
	unsigned short register_value;
	unsigned int cv_value = *(unsigned int *)(data);	
	
	//if (batt_cust_data.high_battery_voltage_support)
		//cv_value = BATTERY_VOLT_04_350000_V;
	//if(cv_value == BATTERY_VOLT_04_200000_V)
	//{
	    //use nearest value, ncp1851 hit 4.2v level
		//cv_value = 4200000;
 	//}
	register_value = charging_parameter_to_value(VBAT_CV_VTH, GETARRAYNUM(VBAT_CV_VTH), cv_value);
	ncp1851_set_ctrl_vbat(register_value);
	return status;
 } 	


 static unsigned int charging_get_current(void *data)
 {
	unsigned int status = STATUS_OK;

	unsigned char ret_val=0;    
	    
	//Get current level
	ret_val = ncp1851_get_ichg();
	//ncp1851_read_interface(NCP1851_CON15, &ret_val, CON15_ICHG_MASK, CON15_ICHG_SHIFT);						    
	//Parsing
	ret_val = (ret_val*100) + 400;
	*(unsigned int *) data = ret_val;

	return status;
 }


 static unsigned int charging_set_current(void *data)
 {
 	unsigned int status = STATUS_OK;
	unsigned int set_chr_current;
	unsigned int array_size;
	unsigned int register_value;
	unsigned int current_value = *(unsigned int *)data;
	
	array_size = GETARRAYNUM(CS_VTH);
	set_chr_current = bmt_find_closest_level(CS_VTH, array_size, current_value);
	register_value = charging_parameter_to_value(CS_VTH, array_size ,set_chr_current);
	//current set by SW and disable automatic charge current
	ncp1851_set_aicl_en(0x0); //disable AICL
	ncp1851_set_ichg(register_value);       	
	
	return status;
 } 	
 

 static unsigned int charging_set_input_current(void *data)
 {
 	unsigned int status = STATUS_OK;
	unsigned int set_chr_current;
	unsigned int array_size;
	unsigned int register_value;
    
	array_size = GETARRAYNUM(INPUT_CS_VTH);
	set_chr_current = bmt_find_closest_level(INPUT_CS_VTH, array_size, *(unsigned int *)data);
	register_value = charging_parameter_to_value(INPUT_CS_VTH, array_size ,set_chr_current);	
        
	ncp1851_set_iinlim(register_value);
	ncp1851_set_iinset_pin_en(0x0); //Input current limit and AICL control by I2C
	ncp1851_set_iinlim_en(0x1); //enable input current limit
	ncp1851_set_aicl_en(0x0); //disable AICL

	return status;
 } 	


 static unsigned int charging_get_charging_status(void *data)
 {
 	unsigned int status = STATUS_OK;
	unsigned int ret_val;

	ret_val = ncp1851_get_chip_status();
	//check whether chargeing DONE
	// 0x06 0110 means CHARGE DONE
	// 0x07 0111 means DPP
	if ((ret_val == 0x6) || (ret_val == 0x7))
	{
		*(unsigned int *)data = KAL_TRUE;
	} else {
		*(unsigned int *)data = KAL_FALSE;
	}
	
	return status;
 } 	

void kick_charger_wdt(void)
{
	ncp1851_set_wdto_dis(0x1);
	ncp1851_set_wdto_dis(0x0);

}

 static unsigned int charging_reset_watch_dog_timer(void *data)
 {
	 unsigned int status = STATUS_OK;
 
     battery_log(BAT_LOG_CRTI, "charging_reset_watch_dog_timer\r\n");
 
	 kick_charger_wdt();
	 return status;
 }
 
 
static unsigned int charging_set_hv_threshold(void *data)
{
	unsigned int status = STATUS_OK;
	unsigned int set_hv_voltage;
	unsigned int array_size;
	unsigned short register_value;
	unsigned int voltage = *(unsigned int *) (data);

	array_size = GETARRAYNUM(VCDT_HV_VTH);
	set_hv_voltage = bmt_find_closest_level(VCDT_HV_VTH, array_size, voltage);
	register_value = charging_parameter_to_value(VCDT_HV_VTH, array_size, set_hv_voltage);
	pmic_set_register_value(PMIC_RG_VCDT_HV_VTH, register_value);
	return status;
}

static unsigned int charging_get_hv_status(void *data)
{
	unsigned int status = STATUS_OK;

#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
	*(kal_bool *) (data) = 0;
	pr_notice("[charging_get_hv_status] charger ok for bring up.\n");
#else
	*(kal_bool *) (data) = pmic_get_register_value(PMIC_RGS_VCDT_HV_DET);
#endif

	return status;
}

static unsigned int charging_get_battery_status(void *data)
{
	unsigned int status = STATUS_OK;
	unsigned int val = 0;

#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
	*(kal_bool *) (data) = 0;	/* battery exist */
	battery_log(BAT_LOG_CRTI, "[charging_get_battery_status] battery exist for bring up.\n");
#else
	val = pmic_get_register_value(PMIC_BATON_TDET_EN);
	battery_log(BAT_LOG_FULL, "[charging_get_battery_status] BATON_TDET_EN = %d\n", val);
	if (val) {
		pmic_set_register_value(PMIC_BATON_TDET_EN, 1);
		pmic_set_register_value(PMIC_RG_BATON_EN, 1);
		*(kal_bool *) (data) = pmic_get_register_value(PMIC_RGS_BATON_UNDET);
	} else {
		*(kal_bool *) (data) = KAL_FALSE;
	}
#endif

	return status;
}

static unsigned int charging_get_charger_det_status(void *data)
{
	unsigned int status = STATUS_OK;
	unsigned int val = 0;

#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
	val = 1;
	battery_log(BAT_LOG_CRTI, "[charging_get_charger_det_status] chr exist for fpga.\n");
#else
	val = pmic_get_register_value(PMIC_RGS_CHRDET);
#endif

	*(kal_bool *) (data) = val;

	return status;
}

static unsigned int charging_get_charger_type(void *data)
{
	unsigned int status = STATUS_OK;

#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
	*(CHARGER_TYPE *) (data) = STANDARD_HOST;
#else
	*(CHARGER_TYPE *) (data) = hw_charging_get_charger_type();
#endif

	return status;
}

static unsigned int charging_get_is_pcm_timer_trigger(void *data)
{
	unsigned int status = STATUS_OK;
	/*
#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
	*(kal_bool *) (data) = KAL_FALSE;
#else

	if (slp_get_wake_reason() == WR_PCM_TIMER)
		*(kal_bool *) (data) = KAL_TRUE;
	else
		*(kal_bool *) (data) = KAL_FALSE;

	battery_log(BAT_LOG_CRTI, "slp_get_wake_reason=%d\n", slp_get_wake_reason());

#endif
*/
	return status;
}

static unsigned int charging_set_platform_reset(void *data)
{
	unsigned int status = STATUS_OK;

#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
#else
	battery_log(BAT_LOG_CRTI, "charging_set_platform_reset\n");

	kernel_restart("battery service reboot system");
	/* arch_reset(0,NULL); */
#endif

	return status;
}

static unsigned int charging_get_platform_boot_mode(void *data)
{
	unsigned int status = STATUS_OK;

#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
#else
	*(unsigned int *) (data) = get_boot_mode();

	battery_log(BAT_LOG_CRTI, "get_boot_mode=%d\n", get_boot_mode());
#endif

	return status;
}

static unsigned int charging_set_power_off(void *data)
{
	unsigned int status = STATUS_OK;

#if defined(CONFIG_POWER_EXT) || defined(CONFIG_MTK_FPGA)
#else
	battery_log(BAT_LOG_CRTI, "charging_set_power_off\n");
	kernel_power_off();
#endif

	return status;
}

static unsigned int charging_get_power_source(void *data)
{
	return STATUS_UNSUPPORTED;
}

static unsigned int charging_get_csdac_full_flag(void *data)
{
	return STATUS_UNSUPPORTED;
}


static unsigned int charging_set_ta_current_pattern(void *data)
{
	unsigned int status = STATUS_OK;
	unsigned int increase = *(unsigned int *) (data);
	BATTERY_VOLTAGE_ENUM cv_voltage = 0;

#if 0
#if defined(HIGH_BATTERY_VOLTAGE_SUPPORT)
	BATTERY_VOLTAGE_ENUM cv_voltage = BATTERY_VOLT_04_350000_V;
#else
	BATTERY_VOLTAGE_ENUM cv_voltage = BATTERY_VOLT_04_200000_V;
#endif
#endif
	if (batt_cust_data.high_battery_voltage_support)
		cv_voltage = BATTERY_VOLT_04_350000_V;
	else
		cv_voltage = BATTERY_VOLT_04_200000_V;

	charging_set_cv_voltage(&cv_voltage);	/* Set CV */
	ncp1851_set_ichg(0x01);	/* Set charging current 500ma */

	ncp1851_set_iinlim(0x01);	/* set charging iinlim 500ma */

	ncp1851_set_iinlim_en(0x01);	/* enable iinlim */
	ncp1851_set_chg_en(0x01);	/* Enable Charging */

	/* ncp1851_dump_register(); */

	if (increase == KAL_TRUE) {
		ncp1851_set_iinlim(0x0);	/* 100mA */
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() on 1");
		msleep(85);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() off 1");
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() on 2");
		msleep(85);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() off 2");
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() on 3");
		msleep(281);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() off 3");
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() on 4");
		msleep(281);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() off 4");
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() on 5");
		msleep(281);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() off 5");
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() on 6");
		msleep(485);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_increase() off 6");
		msleep(50);

		battery_log(BAT_LOG_CRTI, "mtk_ta_increase() end\n");

		ncp1851_set_iinlim(0x1);	/* 500mA */
		msleep(200);
	} else {
		ncp1851_set_iinlim(0x0);	/* 100mA */
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() on 1");
		msleep(281);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() off 1");
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() on 2");
		msleep(281);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() off 2");
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() on 3");
		msleep(281);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() off 3");
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() on 4");
		msleep(85);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() off 4");
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() on 5");
		msleep(85);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() off 5");
		msleep(85);

		ncp1851_set_iinlim(0x1);	/* 500mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() on 6");
		msleep(485);

		ncp1851_set_iinlim(0x0);	/* 100mA */
		battery_log(BAT_LOG_FULL, "mtk_ta_decrease() off 6");
		msleep(50);

		battery_log(BAT_LOG_CRTI, "mtk_ta_decrease() end\n");

		ncp1851_set_iinlim(0x1);	/* 500mA */
	}

	return status;
}

static unsigned int charging_set_error_state(void *data)
{
	return STATUS_UNSUPPORTED;
}

static unsigned int charging_diso_init(void *data)
{
	return STATUS_UNSUPPORTED;
}

static unsigned int charging_get_diso_state(void *data)
{
	return STATUS_UNSUPPORTED;
}

static unsigned int (*const charging_func[CHARGING_CMD_NUMBER]) (void *data) = {
	charging_hw_init,
	charging_dump_register,
	charging_enable,
	charging_set_cv_voltage,
	charging_get_current,
	charging_set_current,
	charging_set_input_current,
	charging_get_charging_status,
	charging_reset_watch_dog_timer,
	charging_set_hv_threshold,
	charging_get_hv_status,
	charging_get_battery_status,
	charging_get_charger_det_status,
	charging_get_charger_type,
	charging_get_is_pcm_timer_trigger,
	charging_set_platform_reset,
	charging_get_platform_boot_mode,
	charging_set_power_off,
	charging_get_power_source,
	charging_get_csdac_full_flag,
	charging_set_ta_current_pattern,
	charging_set_error_state,
	charging_diso_init,
	charging_get_diso_state
};

/*
* FUNCTION
*		Internal_chr_control_handler
*
* DESCRIPTION
*		 This function is called to set the charger hw
*
* CALLS
*
* PARAMETERS
*		None
*
* RETURNS
*
*
* GLOBALS AFFECTED
*	   None
*/
signed int chr_control_interface(CHARGING_CTRL_CMD cmd, void *data)
{
	signed int status;

	if (cmd < CHARGING_CMD_NUMBER)
		status = charging_func[cmd] (data);
	else
		return STATUS_UNSUPPORTED;

	return status;
}
