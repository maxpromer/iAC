Blockly.Blocks['iac_on_gesture'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_on_gesture",
			"message0": Blockly.Msg.IAC_ON_GESTURE_MESSAGE,
			"args0": [
				{
					"type": "field_dropdown",
					"name": "gesture",
					"options": [
						[
							Blockly.Msg.IAC_GESTURE_SHAKE_MESSAGE,
							"EVENT_SHAKE"
						],
						[
							Blockly.Msg.IAC_GESTURE_BOARD_UP_MESSAGE,
							"EVENT_BOARD_UP"
						],
						[
							Blockly.Msg.IAC_GESTURE_BOARD_DOWN_MESSAGE,
							"EVENT_BOARD_DOWN"
						],
						[
							Blockly.Msg.IAC_GESTURE_SCREEON_UP_MESSAGE,
							"EVENT_SCREEN_UP"
						],
						[
							Blockly.Msg.IAC_GESTURE_SCREEON_DOWN_MESSAGE,
							"EVENT_SCREEN_DOWN"
						],
						[
							Blockly.Msg.IAC_GESTURE_TILT_LEFT_MESSAGE,
							"EVENT_TILT_LEFT"
						],
						[
							Blockly.Msg.IAC_GESTURE_TILT_RIGHT_MESSAGE,
							"EVENT_TILT_RIGHT"
						],
						[
							Blockly.Msg.IAC_GESTURE_FREE_FALL_MESSAGE,
							"EVENT_FREE_FALL"
						],/*
						[
							Blockly.Msg.IAC_GESTURE_3G_MESSAGE,
							"EVENT_3G"
						],
						[
							Blockly.Msg.IAC_GESTURE_6G_MESSAGE,
							"EVENT_6G"
						],
						[
							Blockly.Msg.IAC_GESTURE_8G_MESSAGE,
							"EVENT_8G"
						]*/
					]
				},
				{
					"type": "input_dummy"
				},
				{
					"type": "input_statement",
					"name": "callback"
				}
			],
			"previousStatement": null,
			"nextStatement": null,
			"colour": 230,
			"tooltip": Blockly.Msg.IAC_ON_GESTURE_TOOLTIP,
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_acceleration'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_acceleration",
			"message0": Blockly.Msg.IAC_ACCELERATION_MESSAGE,
			"args0": [
				{
					"type": "field_dropdown",
					"name": "axis",
					"options": [
						[
							"x",
							"AXIS_X"
						],
						[
							"y",
							"AXIS_Y"
						],
						[
							"z",
							"AXIS_Z"
						],
						[
							"strength",
							"STRENGTH"
						]
					]
				}
			],
			"output": null,
			"colour": 230,
			"tooltip": Blockly.Msg.IAC_ACCELERATION_TOOLTIP,
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_compass_heading'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_compass_heading",
			"message0": Blockly.Msg.IAC_COMPASS_HEADING_MESSAGE,
			"output": null,
			"colour": 230,
			"tooltip": Blockly.Msg.IAC_COMPASS_HEADING_TOOLTIP,
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_is_gesture'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_is_gesture",
			"message0": Blockly.Msg.IAC_IS_GESTURE_MESSAGE,
			"args0": [
				{
					"type": "field_dropdown",
					"name": "gesture",
					"options": [
						[
							Blockly.Msg.IAC_GESTURE_SHAKE_MESSAGE,
							"EVENT_SHAKE"
						],
						[
							Blockly.Msg.IAC_GESTURE_BOARD_UP_MESSAGE,
							"EVENT_BOARD_UP"
						],
						[
							Blockly.Msg.IAC_GESTURE_BOARD_DOWN_MESSAGE,
							"EVENT_BOARD_DOWN"
						],
						[
							Blockly.Msg.IAC_GESTURE_SCREEON_UP_MESSAGE,
							"EVENT_SCREEN_UP"
						],
						[
							Blockly.Msg.IAC_GESTURE_SCREEON_DOWN_MESSAGE,
							"EVENT_SCREEN_DOWN"
						],
						[
							Blockly.Msg.IAC_GESTURE_TILT_LEFT_MESSAGE,
							"EVENT_TILT_LEFT"
						],
						[
							Blockly.Msg.IAC_GESTURE_TILT_RIGHT_MESSAGE,
							"EVENT_TILT_RIGHT"
						],
						[
							Blockly.Msg.IAC_GESTURE_FREE_FALL_MESSAGE,
							"EVENT_FREE_FALL"
						],/*
						[
							Blockly.Msg.IAC_GESTURE_3G_MESSAGE,
							"EVENT_3G"
						],
						[
							Blockly.Msg.IAC_GESTURE_6G_MESSAGE,
							"EVENT_6G"
						],
						[
							Blockly.Msg.IAC_GESTURE_8G_MESSAGE,
							"EVENT_8G"
						]*/
					]
				}
			],
			"output": null,
			"colour": 230,
			"tooltip": "",
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_rotation'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_rotation",
			"message0": Blockly.Msg.IAC_ROTATION_MESSAGE,
			"args0": [
				{
					"type": "field_dropdown",
					"name": "axis",
					"options": [
						[
							Blockly.Msg.IAC_PITCH_MESSAGE,
							"AXIS_PITCH"
						],
						[
							Blockly.Msg.IAC_ROLL_MESSAGE,
							"AXIS_ROLL"
						]
					]
				}
			],
			"output": null,
			"colour": 230,
			"tooltip": "",
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_magnetic_force'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_magnetic_force",
			"message0": Blockly.Msg.IAC_MAGNETIC_FORCE_MESSAGE,
			"args0": [
				{
					"type": "field_dropdown",
					"name": "axis",
					"options": [
						[
							"x",
							"AXIS_X"
						],
						[
							"y",
							"AXIS_Y"
						],
						[
							"z",
							"AXIS_Z"
						],
						[
							"strength",
							"STRENGTH"
						]
					]
				}
			],
			"output": null,
			"colour": 230,
			"tooltip": Blockly.Msg.IAC_MAGNETIC_FORCE_TOOLTIP,
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_calibrate_compass'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_calibrate_compass",
			"message0": Blockly.Msg.IAC_CALIBATE_COMPASS_MESSAGE,
			"previousStatement": null,
			"nextStatement": null,
			"colour": 230,
			"tooltip": Blockly.Msg.IAC_CALIBATE_COMPASS_TOOLTIP,
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_set_accellerometer_range'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_set_accellerometer_range",
			"message0": Blockly.Msg.IAC_SET_ACCELLEROMETER_MESSAGE,
			"args0": [
				{
					"type": "field_dropdown",
					"name": "range",
					"options": [
						[
							"2g",
							"2"
						],
						[
							"4g",
							"4"
						],
						[
							"8g",
							"8"
						],
						[
							"16g",
							"16"
						]
					]
				}
			],
			"previousStatement": null,
			"nextStatement": null,
			"colour": 230,
			"tooltip": Blockly.Msg.IAC_SET_ACCELLEROMETER_TOOLTIP,
			"helpUrl": ""
		});
	}
};
/*
Blockly.Blocks['sram_write_byte'] = {
	init: function () {
		this.jsonInit({
			"type": "sram_write_byte",
			"message0": Blockly.Msg.IAC_SRAM_WRITE_BYTE_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				},
				{
					"type": "input_value",
					"name": "data"
				}
			],
			"inputsInline": true,
			"previousStatement": null,
			"nextStatement": null,
			"colour": 300,
			"tooltip": Blockly.Msg.IAC_SRAM_WRITE_BYTE_TOOLTIP,
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['sram_read_byte'] = {
	init: function () {
		this.jsonInit({
			"type": "sram_read_byte",
			"message0": Blockly.Msg.IAC_SRAM_READ_BYTE_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				}
			],
			"inputsInline": true,
			"output": "Number",
			"colour": 300,
			"tooltip": Blockly.Msg.IAC_SRAM_READ_BYTE_TOOLTIP,
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['sram_write_word'] = {
	init: function () {
		this.jsonInit({
			"type": "sram_write_word",
			"message0": Blockly.Msg.IAC_SRAM_WRITE_WORD_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				},
				{
					"type": "input_value",
					"name": "data"
				}
			],
			"inputsInline": true,
			"previousStatement": null,
			"nextStatement": null,
			"colour": 300,
			"tooltip": Blockly.Msg.IAC_SRAM_WRITE_WORD_TOOLTIP,
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['sram_read_word'] = {
	init: function () {
		this.jsonInit({
			"type": "sram_read_word",
			"message0": Blockly.Msg.IAC_SRAM_READ_WORD_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				}
			],
			"inputsInline": true,
			"output": "Number",
			"colour": 300,
			"tooltip": Blockly.Msg.IAC_SRAM_READ_WORD_TOOLTIP,
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['sram_write_dword'] = {
	init: function () {
		this.jsonInit({
			"type": "sram_write_dword",
			"message0": Blockly.Msg.IAC_SRAM_WRITE_DWORD_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				},
				{
					"type": "input_value",
					"name": "data"
				}
			],
			"inputsInline": true,
			"previousStatement": null,
			"nextStatement": null,
			"colour": 300,
			"tooltip": Blockly.Msg.IAC_SRAM_WRITE_DWORD_TOOLTIP,
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['sram_read_dword'] = {
	init: function () {
		this.jsonInit({
			"type": "sram_read_dword",
			"message0": Blockly.Msg.IAC_SRAM_READ_DWORD_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				}
			],
			"inputsInline": true,
			"output": "Number",
			"colour": 300,
			"tooltip": Blockly.Msg.IAC_SRAM_READ_DWORD_TOOLTIP,
			"helpUrl": ""
		});
	}
};*/

Blockly.Blocks['eeprom_write_byte'] = {
	init: function () {
		this.jsonInit({
			"type": "eeprom_write_byte",
			"message0": Blockly.Msg.IAC_EEPROM_WRITE_BYTE_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				},
				{
					"type": "input_value",
					"name": "data"
				}
			],
			"inputsInline": true,
			"previousStatement": null,
			"nextStatement": null,
			"colour": 300,
			"tooltip": Blockly.Msg.IAC_EEPROM_WRITE_BYTE_TOOLTIP,
			"helpUrl": ""
		});
	},
	xmlToolbox: function() {
		return `
		<block type="eeprom_write_byte">
		  	<value name="address">
				<shadow type="math_number">
					<field name="VALUE">0</field>
				</shadow>
			</value>
			<value name="data">
				<shadow type="math_number">
					<field name="VALUE">0</field>
				</shadow>
			</value>
		</block>
		`;
	}
};

Blockly.Blocks['eeprom_read_byte'] = {
	init: function () {
		this.jsonInit({
			"type": "eeprom_read_byte",
			"message0": Blockly.Msg.IAC_EEPROM_READ_BYTE_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				}
			],
			"inputsInline": true,
			"output": "Number",
			"colour": 300,
			"tooltip": Blockly.Msg.IAC_EEPROM_READ_BYTE_TOOLTIP,
			"helpUrl": ""
		});
	},
	xmlToolbox: function() {
		return `
		<block type="eeprom_read_byte">
		  	<value name="address">
				<shadow type="math_number">
					<field name="VALUE">0</field>
				</shadow>
			</value>
		</block>
		`;
	}
};

Blockly.Blocks['eeprom_write_word'] = {
	init: function () {
		this.jsonInit({
			"type": "eeprom_write_word",
			"message0": Blockly.Msg.IAC_EEPROM_WRITE_WORD_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				},
				{
					"type": "input_value",
					"name": "data"
				}
			],
			"inputsInline": true,
			"previousStatement": null,
			"nextStatement": null,
			"colour": 180,
			"tooltip": Blockly.Msg.IAC_EEPROM_WRITE_WORD_TOOLTIP,
			"helpUrl": ""
		});
	},
	xmlToolbox: function() {
		return `
		<block type="eeprom_write_word">
		  	<value name="address">
				<shadow type="math_number">
					<field name="VALUE">0</field>
				</shadow>
			</value>
			<value name="data">
				<shadow type="math_number">
					<field name="VALUE">0</field>
				</shadow>
			</value>
		</block>
		`;
	}
};

Blockly.Blocks['eeprom_read_word'] = {
	init: function () {
		this.jsonInit({
			"type": "eeprom_read_word",
			"message0": Blockly.Msg.IAC_EEPROM_READ_WORD_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				}
			],
			"inputsInline": true,
			"output": "Number",
			"colour": 180,
			"tooltip": Blockly.Msg.IAC_EEPROM_READ_WORD_TOOLTIP,
			"helpUrl": ""
		});
	},
	xmlToolbox: function() {
		return `
		<block type="eeprom_read_word">
		  	<value name="address">
				<shadow type="math_number">
					<field name="VALUE">0</field>
				</shadow>
			</value>
		</block>
		`;
	}
};

Blockly.Blocks['eeprom_write_dword'] = {
	init: function () {
		this.jsonInit({
			"type": "eeprom_write_dword",
			"message0": Blockly.Msg.IAC_EEPROM_WRITE_DWORD_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				},
				{
					"type": "input_value",
					"name": "data"
				}
			],
			"inputsInline": true,
			"previousStatement": null,
			"nextStatement": null,
			"colour": 135,
			"tooltip": Blockly.Msg.IAC_EEPROM_WRITE_DWORD_TOOLTIP,
			"helpUrl": ""
		});
	},
	xmlToolbox: function() {
		return `
		<block type="eeprom_write_dword">
		  	<value name="address">
				<shadow type="math_number">
					<field name="VALUE">0</field>
				</shadow>
			</value>
			<value name="data">
				<shadow type="math_number">
					<field name="VALUE">0</field>
				</shadow>
			</value>
		</block>
		`;
	}
};

Blockly.Blocks['eeprom_read_dword'] = {
	init: function () {
		this.jsonInit({
			"type": "eeprom_read_dword",
			"message0": Blockly.Msg.IAC_EEPROM_READ_DWORD_MESSAGE,
			"args0": [
				{
					"type": "input_value",
					"name": "address",
					"check": "Number"
				}
			],
			"inputsInline": true,
			"output": "Number",
			"colour": 135,
			"tooltip": Blockly.Msg.IAC_EEPROM_READ_DWORD_TOOLTIP,
			"helpUrl": ""
		});
	},
	xmlToolbox: function() {
		return `
		<block type="eeprom_read_dword">
		  	<value name="address">
				<shadow type="math_number">
					<field name="VALUE">0</field>
				</shadow>
			</value>
		</block>
		`;
	}
};
