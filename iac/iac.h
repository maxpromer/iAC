#ifndef __iAC_H__
#define __iAC_H__

#include "driver.h"
#include "device.h"
#include "i2c-dev.h"
#include "kidbright32.h"

#include <math.h>

#include "LSM303AGR_ACC_Sensor.h"
#include "LSM303AGR_MAG_Sensor.h"

enum motion_event {
	EVENT_SHAKE,
	EVENT_LOGO_UP,
	EVENT_LOGO_DOWN,
	EVENT_SCREEN_UP,
	EVENT_SCREEN_DOWN,
	EVENT_TILT_RIGHT,
	EVENT_FREE_FALL,
	EVENT_3G,
	EVENT_6G,
	EVENT_8G
};

enum acc_meg_axis {
	AXIS_X = 0,
	AXIS_Y,
	AXIS_Z,
	STRENGTH,

	AXIS_PITCH,
	AXIS_ROLL
};

class iAC : public Device {
	private:		
		enum {
			s_detect,
			s_polling,
			s_wait,
			s_error
		} state;
		TickType_t tickcnt, polling_tickcnt;

		LSM303AGR_ACC_Sensor *Acc;
		LSM303AGR_MAG_Sensor *Mag;

	public:
		// constructor
		iAC(int bus_ch, int dev_addr) ;
		
		// override
		void init(void);
		void process(Driver *drv);
		int prop_count(void);
		bool prop_name(int index, char *name);
		bool prop_unit(int index, char *unit);
		bool prop_attr(int index, char *attr);
		bool prop_read(int index, char *value);
		bool prop_write(int index, char *value);
		
		// method
		void on_gesture(motion_event event, void(*callback)(void)) ;
		int32_t acceleration(acc_meg_axis axis) ;

		
};

#endif