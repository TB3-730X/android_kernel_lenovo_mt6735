#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/slab.h>
//#include <linux/xlog.h>
#include "kd_camera_typedef.h"

#define PFX "S5K3M2_pdafotp"

#define LOG_INF(format, args...) pr_debug(PFX " [%s] " format, __func__, ##args)

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "s5k3m2_pdafotp.h"



extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);
extern void kdSetI2CSpeed(u16 i2cSpeed);
//extern int iBurstWriteReg_multi(u8 *pData, u32 bytes, u16 i2cId, u16 transfer_length);
extern int iMultiReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId, u8 number);


#define USHORT             unsigned short
#define BYTE               unsigned char
#define Sleep(ms) mdelay(ms)

#define S5K3M2_EEPROM_READ_ID  0xA0
#define S5K3M2_EEPROM_WRITE_ID   0xA3
#define S5K3M2_I2C_SPEED        100  
#define S5K3M2_MAX_OFFSET		0xFFFF

#define DATA_SIZE 2048
BYTE s5k3m2_eeprom_data[DATA_SIZE]= {0};
//static bool get_done = false;
//static int last_size = 0;
//static int last_offset = 0;

/*static bool selective_read_eeprom(kal_uint16 addr, BYTE* data)
{
	char pu_send_cmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
    if(addr > S5K3M2_MAX_OFFSET)
        return false;
	kdSetI2CSpeed(S5K3M2_I2C_SPEED);

	if(iReadRegI2C(pu_send_cmd, 2, (u8*)data, 1, S5K3M2_EEPROM_READ_ID)<0)
		return false;
    return true;
}*/

static kal_uint8 s5k3m2_pdaf_read(kal_uint16 addr)
{   
    kal_uint8 get_byte=0;
    char pusendcmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
	
    kdSetI2CSpeed(S5K3M2_I2C_SPEED); // Add this func to set i2c speed by each sensor
    iReadRegI2C(pusendcmd , 2, (u8*)&get_byte, 1, S5K3M2_EEPROM_READ_ID);
    return get_byte;
}


static bool _read_s5k3m2_eeprom(kal_uint16 addr, BYTE* data, kal_uint32 size ){
	int i = 0;
	//int offset = 0xb9c;
	kal_uint8 flag_proc=0;
	kal_uint16  size_proc1=496;
	kal_uint16  size_proc2=806;
	kal_uint16  size_proc3=102;
	kal_uint32    checksum =0;
	addr=	0xb9c;
	flag_proc=s5k3m2_pdaf_read(0x0b9b);
      if(flag_proc){                 //pdaf_proc1 valid register 1 valid
      	   for(i=0;i<size_proc1;i++){
			data[i]=	s5k3m2_pdaf_read(addr);
			checksum +=data[i];
			//LOG_INF("pdaf data:data[%d]=%d ,address 0x%x\n",i,data[i],addr);
			addr=addr+1;
		}
		checksum=checksum%0xff+1;
		 if(checksum==s5k3m2_pdaf_read(0x0d8c)){
			LOG_INF("read PDAF Proc1 success \n");
		 }else{
			LOG_INF("read PDAF Proc1  checksum failed \n");
			//LOG_INF("checksum %d,read checksum=%d \n",checksum,s5k3m2_pdaf_read(0x0d8c));
			return false;
		 }
	}else{
			LOG_INF("read PDAF Proc1 valid register failed \n");
			return false;
	}
	checksum =0;
	addr = 0x0d8e;
	flag_proc=s5k3m2_pdaf_read(0x0d8d);
      if(flag_proc)                 //pdaf_proc2 valid register 1 valid
      	{
      	   for(i=size_proc1;i<size_proc1+size_proc2;i++){
		data[i]=	s5k3m2_pdaf_read(addr);
		//LOG_INF("pdaf data:data[%d]=0x%x ,address 0x%x\n",i,data[i],addr);
		checksum +=data[i];
		addr++;
		}
		checksum=checksum%0xff+1;
		 if(checksum==s5k3m2_pdaf_read(0x10b4)){
			LOG_INF("read PDAF Proc2 success \n");
		 }else{
			LOG_INF("read PDAF Proc2 checksum failed \n");
			//LOG_INF("i %d ,checksum %d,read checksum=%d \n",i,checksum,s5k3m2_pdaf_read(0x10b4));
			return false;
		 }
		 }else{
				 LOG_INF("read PDAF Proc2 valid register failed \n");
				 return false;
		 }
	 checksum =0;
	addr = 0x10b6;
	flag_proc=s5k3m2_pdaf_read(0x10b5);
      if(flag_proc)                 //pdaf_proc2 valid register 1 valid
      	{
      	   for(i=size_proc1+size_proc2;i<size_proc1+size_proc2+size_proc3;i++){
		data[i]=	s5k3m2_pdaf_read(addr);
		checksum +=data[i];
		//LOG_INF("pdaf data:data[%d]=%d ,address 0x%x\n",i,data[i],addr);
		addr++;
		}
		checksum=checksum%0xff+1;
		 if(checksum==s5k3m2_pdaf_read(0x111c)){
			LOG_INF("read PDAF Proc3 success \n");
		 }else{
			LOG_INF("read PDAF Proc3 checksum  failed \n");
			//LOG_INF("i %d,checksum %d,read checksum=%d \n",i,checksum,s5k3m2_pdaf_read(0x111c));
			return false;
		 }
		 }else{
				 LOG_INF("read PDAF Proc3 valid register failed \n");
				 return false;
	}
	//LOG_INF("i %d \n",i);	 
    return true;
}

bool read_3m2_eeprom( kal_uint16 addr, BYTE* data, kal_uint32 size){
	//int i;
	addr = 0xb9c;
	size = 1404;
	
	LOG_INF("read 3m2 eeprom, size = %d\n", size);
	
	/*if(!get_done || last_size != size || last_offset != addr) {
		if(!_read_3m2_eeprom(addr, s5k3m2_eeprom_data, size)){
	     get_done = 0;
            last_size = 0;
            last_offset = 0;
			return false;
		}
	}*/
	
	if(!_read_s5k3m2_eeprom(addr, s5k3m2_eeprom_data, size)){
		return false;
	}

	memcpy(data, s5k3m2_eeprom_data, size);
    return true;
}
static kal_uint8 s5k3m2_otp_read(kal_uint16 addr)
{   
    kal_uint8 get_byte=0;
    char pusendcmd[2] = {(char)(addr >> 8) , (char)(addr & 0xFF) };
	
    kdSetI2CSpeed(S5K3M2_I2C_SPEED); // Add this func to set i2c speed by each sensor
    iReadRegI2C(pusendcmd , 2, (u8*)&get_byte, 1, S5K3M2_EEPROM_READ_ID);
    return get_byte;
}

kal_uint8  ModuleHouseID = 0;

bool S5K3M2_Otp_Info(void )
{
	S5K3M2_OTP_INFO otp_info;
	kal_uint16 info_flag= 0;
	kal_uint16 checksum=0;
	info_flag=s5k3m2_otp_read(0x0400);

      if(0x01==info_flag){
		otp_info.Module_ID =s5k3m2_otp_read(0x0401);
		otp_info.CV            =s5k3m2_otp_read(0x0402);
		otp_info.Year         =s5k3m2_otp_read(0x0403);
	 	otp_info.Month	=s5k3m2_otp_read(0x0404);
		otp_info.Day		=s5k3m2_otp_read(0x0405);
	 	otp_info.Pl		=s5k3m2_otp_read(0x0406);
		otp_info.Sw		=s5k3m2_otp_read(0x0407);
		otp_info.SensorID	=s5k3m2_otp_read(0x0408);
		otp_info.LensID	=s5k3m2_otp_read(0x0409);
		otp_info.Vcm_ID	=s5k3m2_otp_read(0x040a);
		otp_info.Driver_ID	=s5k3m2_otp_read(0x040b);
		otp_info.IRID		=s5k3m2_otp_read(0x040c);
	 	otp_info.ColorID	=s5k3m2_otp_read(0x040d);
	 	otp_info.AF_flag	=s5k3m2_otp_read(0x040e);
		otp_info.FC_flag	=s5k3m2_otp_read(0x040f);
		otp_info.LS_flag	=s5k3m2_otp_read(0x0410);
	 	otp_info.check_info=s5k3m2_otp_read(0x041b);

		checksum=(otp_info.Module_ID+otp_info.CV+otp_info.Year +otp_info.Month+otp_info.Day+otp_info.Pl+otp_info.Sw+otp_info.SensorID
			  +otp_info.LensID+otp_info.Vcm_ID+otp_info.Driver_ID+otp_info.IRID+otp_info.ColorID+otp_info.AF_flag+otp_info.FC_flag+otp_info.LS_flag)%0xff+1;
		if(otp_info.check_info==checksum){
			ModuleHouseID=otp_info.Module_ID;
			otp_info.checksum_flag = true;
			LOG_INF("S5K3M2:Module information checksum PASS\n");
			//LOG_INF("module Id 0x%x,CV 0x%x Year 0x%x Month 0x%x  Day 0x%x Pl 0x%x Sw 0x%x  SensorID 0x%x LensID 0x%x Vcm_ID 0x%x Driver_ID 0x%x  IRID 0x%x ColorID 0x%x AF_flag 0x%x FC_flag 0x%x LS_flag 0x%x\n",
			  // otp_info.Module_ID,otp_info.CV,otp_info.Year,otp_info.Month,otp_info.Day,otp_info.Pl,otp_info.Sw,otp_info.SensorID,otp_info.LensID,otp_info.Vcm_ID,
			  // otp_info.Driver_ID,otp_info.IRID,otp_info.ColorID,otp_info.AF_flag,otp_info.FC_flag,otp_info.LS_flag);
		}
		else{
			otp_info.checksum_flag = false;
			LOG_INF("S5K3M2:Module information checksum fail ,checksum=%x reg sum=%x\n",checksum,otp_info.check_info);
			/*LOG_INF("module Id 0x%x,CV 0x%x Year 0x%x Month 0x%x  Day 0x%x Pl 0x%x Sw 0x%x	SensorID 0x%x LensID 0x%x Vcm_ID 0x%x Driver_ID 0x%x  IRID 0x%x ColorID 0x%x AF_flag 0x%x FC_flag 0x%x LS_flag 0x%x\n",
					   otp_info.Module_ID,otp_info.CV,otp_info.Year,otp_info.Month,otp_info.Day,otp_info.Pl,otp_info.Sw,otp_info.SensorID,otp_info.LensID,otp_info.Vcm_ID,
					   otp_info.Driver_ID,otp_info.IRID,otp_info.ColorID,otp_info.AF_flag,otp_info.FC_flag,otp_info.LS_flag);*/

		}

	  }
	  else{
		return false;
	  }

	return otp_info.checksum_flag;
	
}

#define  S5K3M2_LSC_SIZE  (0x0b86-0x043b+1)
#define  S5K3M2_LSC_Base_Addr 0x43b
kal_uint8  s5k3m2_lsc[S5K3M2_LSC_SIZE] ={0};

bool S5K3M2_Otp_LSC_Read(void)
{
	kal_uint16 lsc_flag=0,i=0,index=0;
	kal_uint32 checksum=0,check_value=0;;
		
	lsc_flag=s5k3m2_otp_read(0x043a);
	index=S5K3M2_LSC_Base_Addr;
	if(0x01==lsc_flag){
		for(i=0;i<S5K3M2_LSC_SIZE;i++){
			s5k3m2_lsc[i]=s5k3m2_otp_read(index);
		      //LOG_INF("S5K3M2: index=0x%x value=0x%x \n",index,s5k3m2_lsc[i]);
			checksum += s5k3m2_lsc[i];		
			index++;
		}
		check_value = s5k3m2_otp_read(0xb87);
		checksum = (checksum)%0xff + 1;
		if(check_value==checksum){
			LOG_INF("S5K3M2:lsc check PASS, checksum0x%x check_value 0x%x \n",checksum,check_value);
			return true;
		}else{
			LOG_INF("S5K3M2: lsc check fail, checksum0x%x check_value 0x%x \n",checksum,check_value);
			return false;	
		}
		
	}
	else{
		return false;
	}
	
}
/*
#define S5K3M2_AF_SIZE  (0x0b99-0x0b89+1)
#define S5K3M2_AF_Base_Addr 0x0b89

kal_uint8 s5k3m2_af[S5K3M2_AF_SIZE]={0};


bool S5K3M2_Otp_AF_Read(void)
{
	kal_uint16 af_flag=0,i=0,index;
	kal_uint16 checksum=0,check_value=0;
	
       af_flag=s5k3m2_otp_read(0x0b88);
	 index=S5K3M2_AF_Base_Addr;
	if(0x01==af_flag){
		for(i=0;i<S5K3M2_AF_SIZE;i++){
			s5k3m2_af[i]=s5k3m2_otp_read(index);
			//LOG_INF("S5K3M2: index=0x%x value=0x%x \n",index,s5k3m2_af[i]);
			checksum += s5k3m2_af[i];
			index++;
		}
		checksum=checksum%0xff + 1;
		check_value = s5k3m2_otp_read(0x0b9a);
		if(check_value==checksum){
			LOG_INF("S5K3M2: af  check PASS, checksum0x%x check_value 0x%x \n",checksum,check_value);
			return true;
		}else{
			LOG_INF("S5K3M2: af check fail, checksum0x%x check_value 0x%x \n",checksum,check_value);
			return false;
		}
		
	}else{
		return false;
	}

}*/
kal_uint8 s5k3m2_awb[16]={0};

bool S5K3M2_Otp_AWB_Read(void)
{
	kal_uint16 wbcheck = 0, checksum = 0, wb_flag = 0;
	//kal_uint16  r_gain = 0,  b_gain = 0,  g_gain = 0;
	kal_uint16  wb_unit_rg_h=0,
			   wb_unit_rg_l=0,
			   wb_unit_bg_h=0,
			   wb_unit_bg_l=0,
			   wb_unit_gg_h=0,
			   wb_unit_gg_l=0,
			   wb_golden_rg_h=0,
			   wb_golden_rg_l=0,
			   wb_golden_bg_h=0,
			   wb_golden_bg_l=0,
			   wb_golden_gg_h=0,
			   wb_golden_gg_l=0,
			   wb_unit_r_h=0,
			   wb_unit_r_l=0,
			   wb_unit_b_h=0,
			   wb_unit_b_l=0,
			   wb_unit_gr_h=0,
			   wb_unit_gr_l=0,
			   wb_unit_gb_h=0,
			   wb_unit_gb_l=0,
			   wb_golden_r_h=0,
			   wb_golden_r_l=0,
			   wb_golden_b_h=0,
			   wb_golden_b_l=0,
			   wb_golden_gr_h=0,
			   wb_golden_gr_l=0,
			   wb_golden_gb_h=0,
			   wb_golden_gb_l=0 ;

	//kal_uint16 rg_value = 0, bg_value = 0, rg_golden_value=0, bg_golden_value=0;
	wb_flag=s5k3m2_otp_read(0x041c);
	if(0x01==wb_flag){
			   wb_unit_rg_h=s5k3m2_otp_read(0x041d);
			   wb_unit_rg_l=s5k3m2_otp_read(0x041e);
			   wb_unit_bg_h=s5k3m2_otp_read(0x041f);
			   wb_unit_bg_l=s5k3m2_otp_read(0x0420);
			   wb_unit_gg_h=s5k3m2_otp_read(0x0421);
			   wb_unit_gg_l=s5k3m2_otp_read(0x0422);
			   wb_golden_rg_h=s5k3m2_otp_read(0x0423);
			   wb_golden_rg_l=s5k3m2_otp_read(0x0424);
			   wb_golden_bg_h=s5k3m2_otp_read(0x0425);
			   wb_golden_bg_l=s5k3m2_otp_read(0x0426);
			   wb_golden_gg_h=s5k3m2_otp_read(0x0427);
			   wb_golden_gg_l=s5k3m2_otp_read(0x0428);
			   wb_unit_r_h=s5k3m2_otp_read(0x0429);
			   wb_unit_r_l=s5k3m2_otp_read(0x042a);
			   wb_unit_b_h=s5k3m2_otp_read(0x042b);
			   wb_unit_b_l=s5k3m2_otp_read(0x042c);
			   wb_unit_gr_h=s5k3m2_otp_read(0x042d);
			   wb_unit_gr_l=s5k3m2_otp_read(0x042e);
			   wb_unit_gb_h=s5k3m2_otp_read(0x042f);
			   wb_unit_gb_l=s5k3m2_otp_read(0x0430);
			   wb_golden_r_h=s5k3m2_otp_read(0x0431);
			   wb_golden_r_l=s5k3m2_otp_read(0x0432);
			   wb_golden_b_h=s5k3m2_otp_read(0x0433);
			   wb_golden_b_l=s5k3m2_otp_read(0x0434);
			   wb_golden_gr_h=s5k3m2_otp_read(0x0435);
			   wb_golden_gr_l=s5k3m2_otp_read(0x0436);
			   wb_golden_gb_h=s5k3m2_otp_read(0x0437);
			   wb_golden_gb_l=s5k3m2_otp_read(0x0438);
		checksum=(wb_unit_rg_h+wb_unit_rg_l+wb_unit_bg_h+wb_unit_bg_l+wb_unit_gg_h+wb_unit_gg_l+wb_golden_rg_h+wb_golden_rg_l
			+wb_golden_bg_h+wb_golden_bg_l+wb_golden_gg_h+wb_golden_gg_l+wb_unit_r_h+wb_unit_r_l+wb_unit_b_h+wb_unit_b_l
			+wb_unit_gr_h+wb_unit_gr_l+wb_unit_gb_h+wb_unit_gb_l+wb_golden_r_h+wb_golden_r_l+wb_golden_b_h+wb_golden_b_l
			+wb_golden_gr_h+wb_golden_gr_l+wb_golden_gb_h+wb_golden_gb_l)%0xff+1;
		wbcheck=s5k3m2_otp_read(0x0439);
		if(wbcheck==checksum){
			LOG_INF("S5K3M2: wb check PASS, checksum0x%x check_value 0x%x \n",checksum,wbcheck);	
			s5k3m2_awb[0]=wb_unit_r_h;
			s5k3m2_awb[1]= wb_unit_r_l;
			s5k3m2_awb[2]=wb_unit_b_h;
			s5k3m2_awb[3]=wb_unit_b_l;
			s5k3m2_awb[4]=wb_unit_gr_h;
			s5k3m2_awb[5]=wb_unit_gr_l;
			s5k3m2_awb[6]=wb_unit_gb_h;
			s5k3m2_awb[7]=wb_unit_gb_l;
			s5k3m2_awb[8]=wb_golden_r_h;
			s5k3m2_awb[9]=wb_golden_r_l;
			s5k3m2_awb[10]=wb_golden_b_h;
			s5k3m2_awb[11]=wb_golden_b_l;
			s5k3m2_awb[12]=wb_golden_gr_h;
			s5k3m2_awb[13]=wb_golden_gr_l;
			s5k3m2_awb[14]=wb_golden_gb_h;
			s5k3m2_awb[15]=wb_golden_gb_l;
		}else{
				/*printk("S5K3M2:0x%x ,0x%x, 0x%x, 0x%x, 0x%x ,0x%x, 0x%x, 0x%x,  0x%x ,0x%x, 0x%x, 0x%x, 0x%x ,0x%x, 0x%x, 0x%x, 0x%x ,0x%x, 0x%x, 0x%x,0x%x ,0x%x, 0x%x, 0x%x,  0x%x ,0x%x, 0x%x, 0x%x",wb_unit_rg_h,wb_unit_rg_l,wb_unit_bg_h,wb_unit_bg_l,wb_unit_gg_h,wb_unit_gg_l,wb_golden_rg_h,wb_golden_rg_l
			,wb_golden_bg_h,wb_golden_bg_l,wb_golden_gg_h,wb_golden_gg_l,wb_unit_r_h,wb_unit_r_l,wb_unit_b_h,wb_unit_b_l
			,wb_unit_gr_h,wb_unit_gr_l,wb_unit_gb_h,wb_unit_gb_l,wb_golden_r_h,wb_golden_r_l,wb_golden_b_h,wb_golden_b_l
			,wb_golden_gr_h,wb_golden_gr_l,wb_golden_gb_h,wb_golden_gb_l);*/
			LOG_INF("S5K3M2: wb check fail, checksum0x%x check_value 0x%x \n",checksum,wbcheck);	
		}
		return true;
		
	}else{
		return false;
	}

}


