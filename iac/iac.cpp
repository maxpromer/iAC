#ifndef __iAC_CPP__
#define __iAC_CPP__

#include "iac.h"

iAC::iAC(int bus_ch, int dev_addr) {
	channel = bus_ch;
	address = dev_addr;
	polling_ms = 300;
}

void iAC::init(void) {
	Acc = NULL;
    Mag = NULL;

	// clear error flag
	error = false;
	// clear initialized flag
	initialized = false;
}

int iAC::prop_count(void) {
	// not supported
	return 0;
}

bool iAC::prop_name(int index, char *name) {
	// not supported
	return false;
}

bool iAC::prop_unit(int index, char *unit) {
	// not supported
	return false;
}

bool iAC::prop_attr(int index, char *attr) {
	// not supported
	return false;
}

bool iAC::prop_read(int index, char *value) {
	// not supported
	return false;
}

bool iAC::prop_write(int index, char *value) {
	// not supported
	return false;
}
// --------------------------------------

void iAC::process(Driver *drv) {
	I2CDev *i2c = (I2CDev *)drv;
	switch (state) {
		case s_detect:
			// detect i2c device
			if (i2c->detect(channel, address) == ESP_OK) {
				if (!Acc) {
					Acc = new LSM303AGR_ACC_Sensor(i2c);
				}
				Acc->Enable();
				Acc->EnableTemperatureSensor();

				if (!Mag) {
					Mag = new LSM303AGR_MAG_Sensor(i2c);
				}
				Mag->Enable();

				// clear error flag
				error = false;
				// set initialized flag
				initialized = true;

				// Go to main state
				state = s_polling;
			} else {
				state = s_error;
			}
			break;
		
		case s_polling: {
			if (is_tickcnt_elapsed(tickcnt, polling_ms)) {
				tickcnt = get_tickcnt();
				
				if (i2c->detect(channel, address) == ESP_OK) {
					state = s_polling;
				} else {
					state = s_error;
				}
			}

			break;
		}
		
		case s_wait:
			if (error) {
				// wait polling_ms timeout
				if (is_tickcnt_elapsed(tickcnt, polling_ms)) {
					state = s_detect;
				}
			}
			break;

		case s_error:
			// set error flag
			error = true;
			// clear initialized flag
			initialized = false;
			// get current tickcnt
			tickcnt = get_tickcnt();
			// goto wait and retry with detect state
			state = s_wait;
			break;

	}
}

// Method
void iAC::on_gesture(motion_event event, void(*callback)(void)) {

}

int32_t iAC::acceleration(acc_meg_axis axis) {
	int32_t accelerometer[3];
	Acc->GetAxes(accelerometer);

	switch(axis) {
		case AXIS_X:
			return accelerometer[0];
		
		case AXIS_Y:
			return accelerometer[1];

		case AXIS_Z:
			return accelerometer[2];
		
		case STRENGTH:
			return sqrt(pow(accelerometer[0], 2) + pow(accelerometer[1], 2) + pow(accelerometer[2], 2));

		default:
			return 0;
		
	}
}

#endif