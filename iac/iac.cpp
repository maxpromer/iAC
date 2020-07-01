#ifndef __iAC_CPP__
#define __iAC_CPP__

#include "iAC.h"
#include "esp_log.h"

iAC::iAC(int bus_ch, int dev_addr)
{
	channel = bus_ch;
	address = dev_addr;
	polling_ms = 50;
}

void iAC::init(void)
{
	Acc = NULL;
	Mag = NULL;

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
		// detect i2c device
		if (i2c->detect(channel, address) == ESP_OK)
		{
			ESP_LOGI("iAC", "Found device");

			if (!Acc)
			{
				Acc = new LSM303AGR_ACC_Sensor(i2c);
			}
			Acc->Enable();
			// Acc->EnableTemperatureSensor();
			Acc->SetFS(acc_range);

			if (!Mag)
			{
				Mag = new LSM303AGR_MAG_Sensor(i2c);
			}
			Mag->Enable();

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
		}
		else
		{
			state = s_error;
		}
		break;

	case s_polling:
	{
		if (is_tickcnt_elapsed(tickcnt, polling_ms))
		{
			tickcnt = get_tickcnt();

			// Read accelerometer LSM303AGR.
			Acc->GetAxes(accelerometer);

			// Read magnetometer LSM303AGR.
			Mag->GetAxes(magnetometer);

			for (int inx = EVENT_SHAKE; inx != EVENT_FREE_FALL; inx++)
			{
				if (callbackDoingFlag[inx])
					continue;

				motion_event event = static_cast<motion_event>(inx);
				if (is_gesture(event))
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
	if (!Acc)
	{
		ESP_LOGI("iAC", "Acc is null");
		return 0;
	}

	ESP_LOGI("iAC", "Acc: %" PRId32 " %" PRId32 " %" PRId32, accelerometer[0], accelerometer[1], accelerometer[2]);

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

int iAC::compass_heading()
{
	if (!Acc)
	{
		ESP_LOGI("iAC", "Acc is null");
		return 0;
	}

	if (!Mag)
	{
		ESP_LOGI("iAC", "Mag is null");
		return 0;
	}

	LSM303AGR_sensor m = {(float)magnetometer[0], (float)magnetometer[1], (float)magnetometer[2]};
	LSM303AGR_sensor a = {(float)accelerometer[0], (float)accelerometer[1], (float)accelerometer[2]};

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
	double heading = 180 * atan2(y_mag_comp, x_mag_comp) / M_PI;

	if (heading <= 0)
	{
		heading = -heading;
	}
	else
	{
		heading = 360 - heading;
	}

	return map(heading, 0, 359, 359, 0);
}

bool iAC::is_gesture(motion_event event)
{
	switch (event)
	{
	case EVENT_SHAKE:
		return acceleration(STRENGTH) > 5000;

	case EVENT_BOARD_UP:
		return acceleration(AXIS_Y) < 0;

	case EVENT_BOARD_DOWN:
		return acceleration(AXIS_Y) > 0;

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
		return acceleration(STRENGTH) > 2000;

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
	if (!Acc)
	{
		ESP_LOGI("iAC", "Acc is null");
		return 0;
	}

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
	if (!Mag)
	{
		ESP_LOGI("iAC", "Mag is null");
		return 0;
	}

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
}

void iAC::accellerometer_range(float range)
{
	if (Acc)
	{
		Acc->SetFS(range);
	}

	acc_range = range;
}

void iAC::sram_write_byte(int addr, int data) {
	if (addr < 0 || addr > 63) {
		return;
	}

	uint8_t buff[2] = { (uint8_t)(addr + 0x20), (uint8_t)data };
	i2c->write(0, 0x6F, buff, 2);
}

void iAC::sram_write_byte(int addr, void *data) {
	if (!data) return;

	sram_write_byte(addr, (int)((uint8_t*)data)[0]);
}

int iAC::sram_read_byte(int addr) {
	if (addr < 0 || addr > 63) {
		return 0;
	}

	uint8_t data;
	addr += 0x20;
	i2c->read(0, 0x6F, (uint8_t*)&addr, 1, (uint8_t*)&data, 1);

	return data;
}

void iAC::sram_write_word(int addr, int16_t data) {
	if (addr < 0 || addr > 63) {
		return;
	}

	uint8_t buff[3] = { (uint8_t)(addr + 0x20) };
	memcpy(&buff[1], &data, 2);
	i2c->write(0, 0x6F, buff, 3);
}

void iAC::sram_write_word(int addr, void *data) {
	if (!data) return;

	sram_write_word(addr, (int16_t)((int16_t*)data)[0]);
}

int16_t iAC::sram_read_word(int addr) {
	if (addr < 0 || addr > 63) {
		return 0;
	}

	int16_t data;
	addr += 0x20;
	i2c->read(0, 0x6F, (uint8_t*)&addr, 1, (uint8_t*)&data, 2);

	return data;
}

void iAC::sram_write_dword(int addr, int32_t data) {
	if (addr < 0 || addr > 63) {
		return;
	}

	addr += 0x20;
	uint8_t buff[5] = { (uint8_t)(addr) };
	memcpy(&buff[1], &data, 4);
	i2c->write(0, 0x6F, buff, 5);
}

void iAC::sram_write_dword(int addr, void *data) {
	if (!data) return;

	sram_write_dword(addr, (int32_t)((int32_t*)data)[0]);
}

int32_t iAC::sram_read_dword(int addr) {
	if (addr < 0 || addr > 63) {
		return 0;
	}

	int32_t data;
	addr += 0x20;
	i2c->read(0, 0x6F, (uint8_t*)&addr, 1, (uint8_t*)&data, 4);

	return data;
}

void iAC::eeprom_write_byte(int addr, int data) {
	if (addr < 0 || addr > 127) {
		return;
	}

	uint8_t buff[2] = { (uint8_t)(addr), (uint8_t)data };
	i2c->write(0, 0x57, buff, 2);
}

void iAC::eeprom_write_byte(int addr, void *data) {
	if (!data) return;

	eeprom_write_byte(addr, (int)((uint8_t*)data)[0]);
}

int iAC::eeprom_read_byte(int addr) {
	if (addr < 0 || addr > 127) {
		return 0;
	}

	uint8_t data;
	i2c->read(0, 0x57, (uint8_t*)&addr, 1, (uint8_t*)&data, 1);

	return data;
}

void iAC::eeprom_write_word(int addr, int16_t data) {
	if (addr < 0 || addr > 127) {
		return;
	}

	uint8_t buff[3] = { (uint8_t)(addr) };
	memcpy(&buff[1], &data, 2);
	i2c->write(0, 0x57, buff, 3);
}

void iAC::eeprom_write_word(int addr, void *data) {
	if (!data) return;

	eeprom_write_word(addr, (int16_t)((int16_t*)data)[0]);
}

int16_t iAC::eeprom_read_word(int addr) {
	if (addr < 0 || addr > 127) {
		return 0;
	}

	int16_t data;
	i2c->read(0, 0x57, (uint8_t*)&addr, 1, (uint8_t*)&data, 2);

	return data;
}

void iAC::eeprom_write_dword(int addr, int32_t data) {
	if (addr < 0 || addr > 127) {
		return;
	}

	uint8_t buff[5] = { (uint8_t)(addr) };
	memcpy(&buff[1], &data, 4);
	i2c->write(0, 0x57, buff, 5);
}

void iAC::eeprom_write_dword(int addr, void *data) {
	if (!data) return;

	eeprom_write_dword(addr, (int32_t)((int32_t*)data)[0]);
}

int32_t iAC::eeprom_read_dword(int addr) {
	if (addr < 0 || addr > 127) {
		return 0;
	}

	int32_t data;
	i2c->read(0, 0x57, (uint8_t*)&addr, 1, (uint8_t*)&data, 4);

	return data;
}

#endif