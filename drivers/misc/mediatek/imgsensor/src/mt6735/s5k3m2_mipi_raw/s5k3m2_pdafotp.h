#ifndef _S5K3M2MIPI_OTP_H
#define _S5K3M2MIPI_OTP_H

typedef struct  S5K3M2_OTP_INFO{
	kal_uint16  Module_ID;
	kal_uint16  CV;
	kal_uint16  Year;
	kal_uint16  Month;
	kal_uint16  Day;
	kal_uint16  Pl;
	kal_uint16  Sw;
	kal_uint16  SensorID;
	kal_uint16  LensID;
	kal_uint16  Vcm_ID;
	kal_uint16  Driver_ID;
	kal_uint16  IRID;
	kal_uint16  ColorID;
	kal_uint16  AF_flag;
	kal_uint16  FC_flag;
	kal_uint16  LS_flag;
	kal_uint16  check_info;
	bool  checksum_flag;

}S5K3M2_OTP_INFO;





bool S5K3M2_Otp_Info(void );
bool S5K3M2_Otp_LSC_Read(void);
//bool S5K3M2_Otp_AF_Read(void);
bool S5K3M2_Otp_AWB_Read(void);


#endif

