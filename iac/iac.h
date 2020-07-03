#ifndef __iAC_H__
#define __iAC_H__

#include "driver.h"
#include "device.h"
#include "i2c-dev.h"
#include "kidbright32.h"
#include "ht16k33.h"

#include <math.h>
#include <string.h>
#include <inttypes.h>

enum motion_event
{
	EVENT_SHAKE = 0,
	EVENT_BOARD_UP,
	EVENT_BOARD_DOWN,
	EVENT_SCREEN_UP,
	EVENT_SCREEN_DOWN,
	EVENT_TILT_LEFT,
	EVENT_TILT_RIGHT,
	EVENT_FREE_FALL,
	EVENT_3G,
	EVENT_6G,
	EVENT_8G
};

enum acc_meg_axis
{
	AXIS_X = 0,
	AXIS_Y,
	AXIS_Z,
	STRENGTH,

	AXIS_PITCH,
	AXIS_ROLL
};

#define GRAVITY (9.80665F)

typedef struct
{
	float x;
	float y;
	float z;
} LSM303AGR_sensor;

typedef void (*GestureHandle)(void);

static GestureHandle GestureCallback[11];
static bool callbackDoingFlag[11];
static bool callbackReadyFlag[11];

void eventTask(void *arg);

class iAC : public Device
{
private:
	enum
	{
		s_detect,
		s_polling,
		s_wait,
		s_error
	} state;
	TickType_t tickcnt, polling_tickcnt;

	I2CDev *i2c;

	long map(double x, double in_min, double in_max, double out_min, double out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	LSM303AGR_sensor mag_max = { 253.0, 516.0, 126.0 };
	LSM303AGR_sensor mag_min = { -627.0, -382.0, -766.0 };

	uint8_t acc_range = 8; // 8g

	int32_t accelerometer[3];
	int32_t magnetometer[3];

public:
	// constructor
	iAC(int bus_ch, int dev_addr);

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
	void on_gesture(motion_event event, GestureHandle cb);
	int32_t acceleration(acc_meg_axis axis);
	int compass_heading();
	bool is_gesture(motion_event event, bool blocking = true);
	int rotation(acc_meg_axis axis);
	double magnetic_force(acc_meg_axis axis);
	void calibrate_compass(HT16K33 *);
	void accellerometer_range(float range);

	// SRAM on RTC
	void sram_write_byte(int, int);
	void sram_write_byte(int, void*);
	int sram_read_byte(int);
	void sram_write_word(int, int16_t);
	void sram_write_word(int, void*);
	int16_t sram_read_word(int);
	void sram_write_dword(int, int32_t);
	void sram_write_dword(int, void*);
	int32_t sram_read_dword(int);


	// EEPROM on RTC
	void eeprom_write_byte(int, int);
	void eeprom_write_byte(int, void*);
	int eeprom_read_byte(int);
	void eeprom_write_word(int, int16_t);
	void eeprom_write_word(int, void*);
	int16_t eeprom_read_word(int);
	void eeprom_write_dword(int, int32_t);
	void eeprom_write_dword(int, void*);
	int32_t eeprom_read_dword(int);

};

#endif