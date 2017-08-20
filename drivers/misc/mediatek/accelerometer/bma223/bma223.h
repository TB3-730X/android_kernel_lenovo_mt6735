#ifndef BMA150_H
#define BMA150_H

#include <linux/ioctl.h>

#define BMA223_I2C_SLAVE_WRITE_ADDR		0x18

	 /* BMA223 Register Map  (Please refer to BMA150 Specifications) */
#define BMA223_REG_DEVID				0x00
#define BMA223_FIXED_DEVID			0x03
#define BMA223_REG_OFSX				0x16
#define BMA223_REG_OFSX_HIGH			0x1A
#define BMA223_REG_BW_RATE			0x10
#define BMA223_BW_MASK				0x1f
#define BMA223_BW_200HZ				0x0d
#define BMA223_BW_100HZ				0x0c
#define BMA223_BW_50HZ				0x0b
#define BMA223_BW_25HZ				0x0a
#define BMA223_REG_POWER_CTL		0x11
#define BMA223_REG_DATA_FORMAT		0x0f
#define BMA223_RANGE_MASK			0x0f
#define BMA223_RANGE_2G				0x03
#define BMA223_RANGE_4G				0x05
#define BMA223_RANGE_8G				0x08
#define BMA223_REG_DATAXLOW			0x03
#define BMA223_REG_DATA_RESOLUTION	0x14
#define BMA223_MEASURE_MODE			0x80
#define BMA223_SELF_TEST				0x32
#define BMA223_SELF_TEST_AXIS_X		0x01
#define BMA223_SELF_TEST_AXIS_Y		0x02
#define BMA223_SELF_TEST_AXIS_Z		0x03
#define BMA223_SELF_TEST_POSITIVE	0x00
#define BMA223_SELF_TEST_NEGATIVE	0x04
#define BMA223_INT_REG_1				0x16
#define BMA223_INT_REG_2				0x17


#define BMA223_SUCCESS						0
#define BMA223_ERR_I2C						-1
#define BMA223_ERR_STATUS					-3
#define BMA223_ERR_SETUP_FAILURE			-4
#define BMA223_ERR_GETGSENSORDATA			-5
#define BMA223_ERR_IDENTIFICATION			-6



#define BMA223_BUFSIZE				256

#define BMA223_AXES_NUM        3

/*----------------------------------------------------------------------------*/
enum CUST_ACTION {
	BMA223_CUST_ACTION_SET_CUST = 1,
	BMA223_CUST_ACTION_SET_CALI,
	BMA223_CUST_ACTION_RESET_CALI
};
/*----------------------------------------------------------------------------*/
struct BMA223_CUST {
	uint16_t action;
};
/*----------------------------------------------------------------------------*/
struct BMA223_SET_CUST {
	uint16_t action;
	uint16_t part;
	int32_t data[0];
};
/*----------------------------------------------------------------------------*/
struct BMA223_SET_CALI {
	uint16_t action;
	int32_t data[BMA223_AXES_NUM];
};
/*----------------------------------------------------------------------------*/
union BMA223_CUST_DATA {
	uint32_t data[10];
	struct BMA223_CUST cust;
	struct BMA223_SET_CUST setCust;
	struct BMA223_SET_CALI setCali;
	struct BMA223_CUST resetCali;
};
/*----------------------------------------------------------------------------*/

#endif
