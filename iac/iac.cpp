#ifndef __iAC_CPP__
#define __iAC_CPP__

#include "iAC.h"
#include "esp_log.h"

#define ACC_ADDR 0x19
#define MEG_ADDR 0x1E

#define ACCRANGR2FSMODE(a) (a == 2 ? 0b00 : a == 4 ? 0b01 : a == 8 ? 0b10 : 0b11)

const long long LSM303AGR_ACC_Sensitivity_List[3][4] = {
	/* HR 12-bit */
	{
		980,   /* FS @2g */
		1950,  /* FS @4g */
		3900,  /* FS @8g */
		11720, /* FS @16g */
	},

	/* Normal 10-bit */
	{
		3900,  /* FS @2g */
		7820,  /* FS @4g */
		15630, /* FS @8g */
		46900, /* FS @16g */
	},

	/* LP 8-bit */
	{
		15630,	/* FS @2g */
		31260,	/* FS @4g */
		62520,	/* FS @8g */
		187580, /* FS @16g */
	},
};

iAC::iAC(int bus_ch, int dev_addr)
{
	channel = bus_ch;
	address = dev_addr;
	polling_ms = 40;
}

void iAC::init(void)
{
	// Debug
	esp_log_level_set("*", ESP_LOG_INFO);

	// clear error flag
	error = false;
	// clear initialized flag
	initialized = false;

	state = s_detect;
}

int iAC::prop_count(void)
{
	// not supported
	return 0;
}

bool iAC::prop_name(int index, char *name)
{
	// not supported
	return false;
}

bool iAC::prop_unit(int index, char *unit)
{
	// not supported
	return false;
}

bool iAC::prop_attr(int index, char *attr)
{
	// not supported
	return false;
}

bool iAC::prop_read(int index, char *value)
{
	// not supported
	return false;
}

bool iAC::prop_write(int index, char *value)
{
	// not supported
	return false;
}
// --------------------------------------

void iAC::process(Driver *drv)
{
	i2c = (I2CDev *)drv;
	switch (state)
	{
	case s_detect:
	{
		// detect acc i2c device
		if (i2c->detect(channel, ACC_ADDR) != ESP_OK)
		{
			state = s_error;
			break;
		}

		ESP_LOGI("iAC", "Found Acc device");

		uint8_t buff[2];

		/* ACC Setup */

		// FIFO mode selection
		buff[0] = 0x2E;									  // FIFO_CTRL_REG_A
		buff[1] = (0b00 << 6) | (0 << 5) | (0b0000 << 0); // Bypass mode, trigger event allows triggering signal on INT1
		if (i2c->write(channel, ACC_ADDR, buff, 2) != ESP_OK)
		{
			state = s_error;
			break;
		}

		// Set Data rate
		buff[0] = 0x20;														 // CTRL_REG1_A
		buff[1] = (0b0101 << 4) | (0 << 3) | (1 << 2) | (1 << 1) | (1 << 0); // 100Hz, Normal mode, XYZ-axis enable
		if (i2c->write(channel, ACC_ADDR, buff, 2) != ESP_OK)
		{
			state = s_error;
			break;
		}

		// Set Full-scale selection
		buff[0] = 0x23;																						   // CTRL_REG4_A
		buff[1] = (0 << 7) | (0 << 6) | (ACCRANGR2FSMODE(acc_range) << 4) | (0 << 3) | (0b00 << 1) | (0 << 0); //  continuous update, data LSb at lower address,  Full-scale selection, Normal mode, self-test disabled, SPI 3-wire disabled
		if (i2c->write(channel, ACC_ADDR, buff, 2) != ESP_OK)
		{
			state = s_error;
			break;
		}
		/* ACC Setup Done */

		/* MEG Setup */

		if (i2c->detect(channel, MEG_ADDR) != ESP_OK)
		{
			state = s_error;
			break;
		}

		// MAG Select mode
		buff[0] = 0x60; // CFG_REG_A_M
		/*
					BIT 7: 1 - temperature compensation enable
					BIT 6: 0 - normal mode
					BIT 5: 0 - SOFT_RST
					BIT 4: 0 - high-resolution mode
					BIT 3-2: 11 - 100Hz
					BIT 1-0: 11 - Continuous mode
					*/
		buff[1] = (1 << 7) | (0 << 6) | (0 << 5) | (0 << 4) | (0b11 << 2) | (0b00 << 0);
		if (i2c->write(channel, MEG_ADDR, buff, 2) != ESP_OK)
		{
			state = s_error;
			break;
		}

		// MAG Digital Low Pass enabled
		buff[0] = 0x61; // CFG_REG_B_M
		/*
					BIT 7-5: 000
					BIT 4: 0 - Enables offset cancellation in single measurement mode
					BIT 3: 0 - If ‘1’, the interrupt block recognition checks data after the hard-iron correction to discover the interrrupt.
					BIT 2: 0 - set pulse is released every 63 ODR; 
					BIT 1: 0 - offset cancellation disabled
					BIT 0: 1 - digital filter enabled
					*/
		buff[1] = 0b00000001;
		if (i2c->write(channel, MEG_ADDR, buff, 2) != ESP_OK)
		{
			state = s_error;
			break;
		}

		/* MEG Setup Done */

		for (int i = 0; i < 11; i++)
		{
			callbackDoingFlag[i] = false;
			callbackReadyFlag[i] = false;
		}

		// clear error flag
		error = false;
		// set initialized flag
		initialized = true;

		// Go to main state
		state = s_polling;

		break;
	}

	case s_polling:
	{
		if (is_tickcnt_elapsed(tickcnt, polling_ms))
		{
			tickcnt = get_tickcnt();

			/* op mode is Normal 10-bit */
			int op_mode = 1;
			int shift = 6;
			int fs_mode = ACCRANGR2FSMODE(acc_range);
			uint8_t reg = 0;

			int16_t rawData[3];

			memset(rawData, 0, 6);
			for (int i = 0; i < 6; i++)
			{
				reg = 0x28 + i; // Acc Raw Data
				if (i2c->read(channel, ACC_ADDR, &reg, 1, &((uint8_t *)rawData)[i], 1) != ESP_OK)
				{
					state = s_error;
					break;
				}
			}

			accelerometer[0] = ((rawData[0] >> shift) * LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;
			accelerometer[1] = ((rawData[1] >> shift) * LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;
			accelerometer[2] = ((rawData[2] >> shift) * LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;

			// Read magnetometer LSM303AGR.
			memset(rawData, 0, 6);
			for (int i = 0; i < 6; i++)
			{
				reg = 0x68 + i; // OUTX_L_REG_M : Meg Raw Data
				if (i2c->read(channel, MEG_ADDR, &reg, 1, &((uint8_t *)rawData)[i], 1) != ESP_OK)
				{
					state = s_error;
					break;
				}
			}

			magnetometer[0] = rawData[0] * 1.5f;
			magnetometer[1] = rawData[1] * 1.5f;
			magnetometer[2] = rawData[2] * 1.5f;

			for (int inx = EVENT_SHAKE; inx <= EVENT_FREE_FALL; inx++)
			{
				if (callbackDoingFlag[inx])
					continue;

				motion_event event = static_cast<motion_event>(inx);
				if (is_gesture(event, false)) // non-blocking
				{
					if (GestureCallback[inx] && callbackReadyFlag[inx] == false)
					{
						int *handleNum = (int *)malloc(sizeof(int));
						*handleNum = inx;

						callbackDoingFlag[*handleNum] = true;

						xTaskCreate(eventTask, "eventTask", 2048, handleNum, 1, NULL);
					}
				}
				else
				{
					callbackReadyFlag[inx] = false;
				}
			}
		}

		break;
	}

	case s_wait:
		if (error)
		{
			// wait polling_ms timeout
			if (is_tickcnt_elapsed(tickcnt, polling_ms))
			{
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

void eventTask(void *arg)
{
	int *handleNum = (int *)arg;

	if (GestureCallback[*handleNum])
	{
		GestureCallback[*handleNum]();
	}

	callbackDoingFlag[*handleNum] = false;
	callbackReadyFlag[*handleNum] = true;

	free(handleNum);

	vTaskDelete(NULL);
}

// Method
void iAC::on_gesture(motion_event event, GestureHandle cb)
{
	GestureCallback[(int)event] = cb;
}

int32_t iAC::acceleration(acc_meg_axis axis)
{
	// ESP_LOGI("iAC", "Acc: %" PRId32 " %" PRId32 " %" PRId32, accelerometer[0], accelerometer[1], accelerometer[2]);

	switch (axis)
	{
	case AXIS_X:
		return -accelerometer[0];

	case AXIS_Y:
		return -accelerometer[1];

	case AXIS_Z:
		return -accelerometer[2];

	case STRENGTH:
		return sqrt(pow(accelerometer[0], 2) + pow(accelerometer[1], 2) + pow(accelerometer[2], 2));

	default:
		return 0;
	}
}

int iAC::compass_heading(HT16K33 *ht16k33)
{
	if (!CalibrateMag)
	{
		if (!loadCalibrateFromSRAM())
		{
			calibrate_compass(ht16k33);
		}
		CalibrateMag = true;
	}

	LSM303AGR_sensor m = {(float)magnetometer[0], (float)magnetometer[1], (float)magnetometer[2]};
	LSM303AGR_sensor a = {(float)accelerometer[0], (float)accelerometer[1], (float)accelerometer[2]};
	/*
	if (m.x < mag_min.x)
	{
		mag_min.x = m.x;
	}
	if (m.x > mag_max.x)
	{
		mag_max.x = m.x;
	}

	if (m.y < mag_min.y)
	{
		mag_min.y = m.y;
	}
	if (m.y > mag_max.y)
	{
		mag_max.y = m.y;
	}

	if (m.z < mag_min.z)
	{
		mag_min.z = m.z;
	}
	if (m.z > mag_max.z)
	{
		mag_max.z = m.z;
	}
*/
	// use calibration values to shift and scale magnetometer measurements
	double x_mag = (0.0 + m.x - mag_min.x) / (mag_max.x - mag_min.x) * 2 - 1;
	double y_mag = (0.0 + m.y - mag_min.y) / (mag_max.y - mag_min.y) * 2 - 1;
	double z_mag = (0.0 + m.z - mag_min.z) / (mag_max.z - mag_min.z) * 2 - 1;
	//SerialPort.printf("Mag norm (x, y, z): (%f, %f, %f)\n", x_mag, y_mag, z_mag);

	// Normalize acceleration measurements so they range from 0 to 1
	double s = sqrt(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2));
	double xAccelNorm = a.x / s;
	double yAccelNorm = a.y / s;
	//DF("Acc norm (x, y): (%f, %f)\n", xAccelNorm, yAccelNorm);

	double pitch = asin(-xAccelNorm);
	double roll = asin(yAccelNorm / cos(pitch));

	// tilt compensated magnetic sensor measurements
	double x_mag_comp = x_mag * cos(pitch) + z_mag * sin(pitch);
	double y_mag_comp = x_mag * sin(roll) * sin(pitch) + y_mag * cos(roll) - z_mag * sin(roll) * cos(pitch);

	// arctangent of y/x converted to degrees
	double heading = 180 * atan2(x_mag_comp, y_mag_comp) / M_PI;

	if (heading <= 0)
	{
		heading = -heading;
	}
	else
	{
		heading = 360 - heading;
	}

	return heading;
}

bool iAC::is_gesture(motion_event event, bool blocking)
{
	switch (event)
	{
	case EVENT_SHAKE:
		return acceleration(STRENGTH) > 5000;

	case EVENT_BOARD_UP:
		return acceleration(AXIS_Y) < -600;

	case EVENT_BOARD_DOWN:
		return acceleration(AXIS_Y) > 600;

	case EVENT_SCREEN_UP:
	{
		int pitch = rotation(AXIS_PITCH);
		return pitch >= -30 && pitch <= 30;
	}

	case EVENT_SCREEN_DOWN:
	{
		int pitch = rotation(AXIS_PITCH);
		return pitch >= 150 || pitch <= -150;
	}

	case EVENT_TILT_LEFT:
	{
		int roll = rotation(AXIS_ROLL);
		return roll <= -30;
	}

	case EVENT_TILT_RIGHT:
	{
		int roll = rotation(AXIS_ROLL);
		return roll >= 30;
	}

	case EVENT_FREE_FALL:
	{
		if (blocking)
		{
			bool lowStrengthContinue = false;
			// ESP_LOGI("iAC", "Start free fall %lld", esp_timer_get_time());
			for (int i = 0; i < 240; i += 40)
			{
				if (acceleration(STRENGTH) < 500)
				{
					lowStrengthContinue = true;
					vTaskDelay(40 / portTICK_RATE_MS);
				}
				else
				{
					lowStrengthContinue = false;
					break;
				}
			}
			// if (lowStrengthContinue) ESP_LOGI("iAC", "Stop free fall %lld", esp_timer_get_time());
			return lowStrengthContinue;
		}
		else
		{
			static bool startCalcLowStrengthContinue = false;
			static unsigned long xStartCalc;
			if (acceleration(STRENGTH) < 500)
			{
				if (!startCalcLowStrengthContinue)
				{
					xStartCalc = esp_timer_get_time();
					startCalcLowStrengthContinue = true;
					// ESP_LOGI("iAC", "Start free fall %lld", esp_timer_get_time());
				}
				else
				{
					if ((esp_timer_get_time() - xStartCalc) >= 220E3)
					{ // over 220 mS
						startCalcLowStrengthContinue = false;
						// ESP_LOGI("iAC", "End free fall - 2");
						return true;
					}
					else
					{
						// ESP_LOGI("iAC", "free fall %lld", esp_timer_get_time() - xStartCalc);
					}
				}
			}
			else
			{
				// ESP_LOGI("iAC", "End free fall - 1 %lld", esp_timer_get_time() - xStartCalc);
				xStartCalc = 0;
				startCalcLowStrengthContinue = false;
				return false;
			}
		}
	}

	case EVENT_3G:
		return acceleration(STRENGTH) > 3000;

	case EVENT_6G:
		return acceleration(STRENGTH) > 6000;

	case EVENT_8G:
		return acceleration(STRENGTH) > 8000;

	default:
		return false;
	}
}

int iAC::rotation(acc_meg_axis axis)
{
	double x_g_value = -accelerometer[0] / 1000.0; /* Acceleration in x-direction in g units */
	double y_g_value = -accelerometer[1] / 1000.0; /* Acceleration in y-direction in g units */
	double z_g_value = -accelerometer[2] / 1000.0; /* Acceleration in z-direction in g units */

	double roll = (((atan2(z_g_value, x_g_value) * 180) / 3.14) + 180);
	double pitch = (((atan2(y_g_value, z_g_value) * 180) / 3.14) + 180);

	roll = (roll >= 270) ? (270 - roll) : (roll >= 90) ? (fmod(90 - roll, -180) + 180) : -90 - roll;
	pitch = 180 - pitch;

	switch (axis)
	{
	case AXIS_PITCH:
		return pitch;

	case AXIS_ROLL:
		return roll;

	default:
		return 0.0;
	}
}

double iAC::magnetic_force(acc_meg_axis axis)
{
	ESP_LOGI("iAC", "Mag[mGauss]: %" PRId32 " %" PRId32 " %" PRId32, magnetometer[0], magnetometer[1], magnetometer[2]);

	switch (axis)
	{
	case AXIS_X:
		return magnetometer[0] * 0.1;

	case AXIS_Y:
		return magnetometer[1] * 0.1;

	case AXIS_Z:
		return magnetometer[2] * 0.1;

	case STRENGTH:
		return sqrt(pow(magnetometer[0], 2) + pow(magnetometer[1], 2) + pow(magnetometer[2], 2)) * 0.1;

	default:
		return 0.0;
	}
}

void iAC::calibrate_compass(HT16K33 *ht16k33)
{
	ht16k33->scroll((char *)"TILT TO FILL SCREEN", true);
	ht16k33->wait_idle();

	uint8_t display[16];
	memset(display, 0, 16);

	uint8_t before_index_cols = -1;
	uint8_t before_index_rows = -1;

	int delay = 0;
	bool pixelShow = false;
	while (1)
	{
		LSM303AGR_sensor m = {(float)magnetometer[0], (float)magnetometer[1], (float)magnetometer[2]};
		if (m.x < mag_min.x)
		{
			mag_min.x = m.x;
		}
		if (m.x > mag_max.x)
		{
			mag_max.x = m.x;
		}

		if (m.y < mag_min.y)
		{
			mag_min.y = m.y;
		}
		if (m.y > mag_max.y)
		{
			mag_max.y = m.y;
		}

		if (m.z < mag_min.z)
		{
			mag_min.z = m.z;
		}
		if (m.z > mag_max.z)
		{
			mag_max.z = m.z;
		}

		double x_g_value = -accelerometer[0] / 1000.0; /* Acceleration in x-direction in g units */
		double y_g_value = -accelerometer[1] / 1000.0; /* Acceleration in y-direction in g units */
		double z_g_value = -accelerometer[2] / 1000.0; /* Acceleration in z-direction in g units */

		double roll = (((atan2(z_g_value, x_g_value) * 180) / 3.14) + 180);
		double pitch = (((atan2(y_g_value, z_g_value) * 180) / 3.14) + 180);

		roll = (roll >= 270) ? (270 - roll) : (roll >= 90) ? (fmod(90 - roll, -180) + 180) : -90 - roll;
		pitch = 180 - pitch;

		// memset(display, 0, 16);
		uint8_t index_cols = map(roll, -60, 60, 0, 15);
		uint8_t index_rows = map(pitch, -60, 60, 0, 7);

		if (before_index_cols == -1 || before_index_rows == -1)
		{
			before_index_cols = index_cols;
			before_index_rows = index_rows;
		}

		if (index_cols >= 0 && index_cols <= 15 && index_rows >= 0 && index_rows <= 7)
		{
			if ((index_cols != before_index_cols) || (index_rows != before_index_rows))
			{
				display[before_index_cols] |= 0x80 >> before_index_rows;
			}
			if (pixelShow)
			{
				display[index_cols] |= 0x80 >> index_rows;

				bool fullScreen = true;
				for (int i = 0; i < 16; i++)
				{
					if (display[i] != 0xFF)
					{
						fullScreen = false;
						break;
					}
				}
				if (fullScreen)
					break;
			}
			else
			{
				display[index_cols] &= ~(0x80 >> index_rows);
			}
			if (delay >= 200)
			{
				delay = 0;
				pixelShow = !pixelShow;
			}
		}

		before_index_cols = index_cols;
		before_index_rows = index_rows;

		ht16k33->show(display);

		vTaskDelay(50 / portTICK_RATE_MS);
		delay += 50;
	}

	ht16k33->scroll((char *)"FINISH", true);

	saveCalibrateIntoSRAM();

	ht16k33->wait_idle();

	CalibrateMag = true;

	/*
	ht16k33->show((uint8_t *)"\x3c\x42\x81\x81\x81\x42\x24\x18\x18\x24\x42\x81\x81\x81\x42\x3c");
	while (acceleration(STRENGTH) < 1800)
		vTaskDelay(50 / portTICK_RATE_MS);

	bool screenFlag = true;
	TickType_t timeout = get_tickcnt(), blink = get_tickcnt();
	while (!is_tickcnt_elapsed(timeout, 2000))
	{
		if (acceleration(STRENGTH) > 1800)
		{
			timeout = get_tickcnt();
		}

		LSM303AGR_sensor m = {(float)magnetometer[0], (float)magnetometer[1], (float)magnetometer[2]};
		if (m.x < mag_min.x)
		{
			mag_min.x = m.x;
		}
		if (m.x > mag_max.x)
		{
			mag_max.x = m.x;
		}

		if (m.y < mag_min.y)
		{
			mag_min.y = m.y;
		}
		if (m.y > mag_max.y)
		{
			mag_max.y = m.y;
		}

		if (m.z < mag_min.z)
		{
			mag_min.z = m.z;
		}
		if (m.z > mag_max.z)
		{
			mag_max.z = m.z;
		}

		if (is_tickcnt_elapsed(blink, 300))
		{
			blink = get_tickcnt();

			screenFlag = !screenFlag;

			if (screenFlag)
			{
				ht16k33->show((uint8_t *)"\x3c\x42\x81\x81\x81\x42\x24\x18\x18\x24\x42\x81\x81\x81\x42\x3c");
			}
			else
			{
				ht16k33->show((uint8_t *)"\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0");
			}
		}

		vTaskDelay(50 / portTICK_RATE_MS);
	}

	ht16k33->show((uint8_t *)"\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0");
	*/
}

void iAC::accellerometer_range(float range)
{
	acc_range = range;

	uint8_t buff[2];

	// Set Full-scale selection
	buff[0] = 0x23;																						   // CTRL_REG4_A
	buff[1] = (0 << 7) | (0 << 6) | (ACCRANGR2FSMODE(acc_range) << 4) | (0 << 3) | (0b00 << 1) | (0 << 0); //  continuous update, data LSb at lower address,  Full-scale selection, Normal mode, self-test disabled, SPI 3-wire disabled

	i2c->write(channel, address, buff, 2);
}

void iAC::sram_write_byte(int addr, int data)
{
	if (addr < 0 || addr > 63)
	{
		return;
	}

	uint8_t buff[2] = {(uint8_t)(addr + 0x20), (uint8_t)data};
	i2c->write(0, 0x6F, buff, 2);
}

void iAC::sram_write_byte(int addr, void *data)
{
	if (!data)
		return;

	sram_write_byte(addr, (int)((uint8_t *)data)[0]);
}

int iAC::sram_read_byte(int addr)
{
	if (addr < 0 || addr > 63)
	{
		return 0;
	}

	uint8_t data;
	addr += 0x20;
	i2c->read(0, 0x6F, (uint8_t *)&addr, 1, (uint8_t *)&data, 1);

	return data;
}

void iAC::sram_write_word(int addr, int16_t data)
{
	if (addr < 0 || addr > 63)
	{
		return;
	}

	uint8_t buff[3] = {(uint8_t)(addr + 0x20)};
	memcpy(&buff[1], &data, 2);
	i2c->write(0, 0x6F, buff, 3);
}

void iAC::sram_write_word(int addr, void *data)
{
	if (!data)
		return;

	sram_write_word(addr, (int16_t)((int16_t *)data)[0]);
}

int16_t iAC::sram_read_word(int addr)
{
	if (addr < 0 || addr > 63)
	{
		return 0;
	}

	int16_t data;
	addr += 0x20;
	i2c->read(0, 0x6F, (uint8_t *)&addr, 1, (uint8_t *)&data, 2);

	return data;
}

void iAC::sram_write_dword(int addr, int32_t data)
{
	if (addr < 0 || addr > 63)
	{
		return;
	}

	addr += 0x20;
	uint8_t buff[5] = {(uint8_t)(addr)};
	memcpy(&buff[1], &data, 4);
	i2c->write(0, 0x6F, buff, 5);
}

void iAC::sram_write_dword(int addr, void *data)
{
	if (!data)
		return;

	sram_write_dword(addr, (int32_t)((int32_t *)data)[0]);
}

int32_t iAC::sram_read_dword(int addr)
{
	if (addr < 0 || addr > 63)
	{
		return 0;
	}

	int32_t data;
	addr += 0x20;
	i2c->read(0, 0x6F, (uint8_t *)&addr, 1, (uint8_t *)&data, 4);

	return data;
}

void iAC::sram_write_block(int addr, uint8_t *data, uint8_t size)
{
	if (addr < 0 || addr > 63)
	{
		return;
	}

	addr += 0x20;
	uint8_t buff[1 + size] = {(uint8_t)(addr)};
	memcpy(&buff[1], data, size);
	i2c->write(0, 0x6F, buff, 1 + size);
}

void iAC::sram_read_block(int addr, uint8_t *data, uint8_t size)
{
	if (addr < 0 || addr > 63)
	{
		return;
	}

	addr += 0x20;
	i2c->read(0, 0x6F, (uint8_t *)&addr, 1, data, size);
}

void iAC::eeprom_write_byte(int addr, int data)
{
	if (addr < 0 || addr > 127)
	{
		return;
	}

	uint8_t buff[2] = {(uint8_t)(addr), (uint8_t)data};
	i2c->write(0, 0x57, buff, 2);
	vTaskDelay(20 / portTICK_RATE_MS);
}

void iAC::eeprom_write_byte(int addr, void *data)
{
	if (!data)
		return;

	eeprom_write_byte(addr, (int)((uint8_t *)data)[0]);
}

int iAC::eeprom_read_byte(int addr)
{
	if (addr < 0 || addr > 127)
	{
		return 0;
	}

	uint8_t data;
	i2c->read(0, 0x57, (uint8_t *)&addr, 1, (uint8_t *)&data, 1);

	return data;
}

void iAC::eeprom_write_word(int addr, int16_t data)
{
	if (addr < 0 || addr > 127)
	{
		return;
	}

	uint8_t buff[3] = {(uint8_t)(addr)};
	memcpy(&buff[1], &data, 2);
	i2c->write(0, 0x57, buff, 3);
	vTaskDelay(20 / portTICK_RATE_MS);
}

void iAC::eeprom_write_word(int addr, void *data)
{
	if (!data)
		return;

	eeprom_write_word(addr, (int16_t)((int16_t *)data)[0]);
}

int16_t iAC::eeprom_read_word(int addr)
{
	if (addr < 0 || addr > 127)
	{
		return 0;
	}

	int16_t data;
	i2c->read(0, 0x57, (uint8_t *)&addr, 1, (uint8_t *)&data, 2);

	return data;
}

void iAC::eeprom_write_dword(int addr, int32_t data)
{
	if (addr < 0 || addr > 127)
	{
		return;
	}

	uint8_t buff[5] = {(uint8_t)(addr)};
	memcpy(&buff[1], &data, 4);
	i2c->write(0, 0x57, buff, 5);
	vTaskDelay(20 / portTICK_RATE_MS);
}

void iAC::eeprom_write_dword(int addr, void *data)
{
	if (!data)
		return;

	eeprom_write_dword(addr, (int32_t)((int32_t *)data)[0]);
}

int32_t iAC::eeprom_read_dword(int addr)
{
	if (addr < 0 || addr > 127)
	{
		return 0;
	}

	int32_t data;
	i2c->read(0, 0x57, (uint8_t *)&addr, 1, (uint8_t *)&data, 4);

	return data;
}

bool iAC::loadCalibrateFromSRAM()
{
	int16_t dataBuffer[7]; // XYZ in int16_t x2 and CRC in int16_t
	// sram_read_block(0, (uint8_t*) dataBuffer, 7 * sizeof(int16_t));
	for (int i = 0; i < 7; i++)
	{
		dataBuffer[i] = sram_read_word(i * 2);
		// ESP_LOGI("SRAM", "Load: %d: 0x%.4X", i, dataBuffer[i]);
	}

	int16_t crc = calCRC((uint8_t *)dataBuffer, 6 * sizeof(int16_t));
	if (crc == dataBuffer[6])
	{
		mag_min.x = dataBuffer[0];
		mag_min.y = dataBuffer[1];
		mag_min.z = dataBuffer[2];
		mag_max.x = dataBuffer[3];
		mag_max.y = dataBuffer[4];
		mag_max.z = dataBuffer[5];

		// ESP_LOGI("iAC", "Load Calibrate From SRAM -> OK");

		return true;
	}

	// ESP_LOGI("iAC", "Load Calibrate From SRAM -> Fail, 0x%x", crc);

	return false;
}

void iAC::saveCalibrateIntoSRAM()
{
	int16_t dataBuffer[7]; // XYZ in int16_t x2 and CRC in int16_t
	dataBuffer[0] = mag_min.x;
	dataBuffer[1] = mag_min.y;
	dataBuffer[2] = mag_min.z;
	dataBuffer[3] = mag_max.x;
	dataBuffer[4] = mag_max.y;
	dataBuffer[5] = mag_max.z;
	dataBuffer[6] = calCRC((uint8_t *)dataBuffer, 6 * sizeof(int16_t));

	// sram_write_block(0, (uint8_t*) dataBuffer, 7 * sizeof(int16_t));
	for (int i = 0; i < 7; i++)
	{
		sram_write_word(i * 2, dataBuffer[i]);
		// ESP_LOGI("SRAM", "Save: %d: 0x%.4X", i, dataBuffer[i]);
	}

	// ESP_LOGI("iAC", "Save Calibrate From SRAM -> OK");
}

int16_t iAC::calCRC(uint8_t *data, uint8_t size)
{
	int16_t sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum += data[i];
	}
	sum = ~sum;
	return sum;
}

#endif