Blockly.Blocks['iac_on_gesture'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_on_gesture",
			"message0": "on %1 %2 %3",
			"args0": [
				{
					"type": "field_dropdown",
					"name": "gesture",
					"options": [
						[
							"shake",
							"EVENT_SHAKE"
						],
						[
							"board up",
							"EVENT_BOARD_UP"
						],
						[
							"board down",
							"EVENT_BOARD_DOWN"
						],
						[
							"screen up",
							"EVENT_SCREEN_UP"
						],
						[
							"screen down",
							"EVENT_SCREEN_DOWN"
						],
						[
							"tilt left",
							"EVENT_TILT_LEFT"
						],
						[
							"tilt right",
							"EVENT_TILT_RIGHT"
						],
						[
							"free fall",
							"EVENT_FREE_FALL"
						],
						[
							"3g",
							"EVENT_3G"
						],
						[
							"6g",
							"EVENT_6G"
						],
						[
							"8g",
							"EVENT_8G"
						]
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
			"tooltip": "",
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_acceleration'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_acceleration",
			"message0": "acceleration (mg) %1",
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
			"tooltip": "",
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_compass_heading'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_compass_heading",
			"message0": "compass heading (°)",
			"output": null,
			"colour": 230,
			"tooltip": "",
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_is_gesture'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_is_gesture",
			"message0": "is %1 gesture",
			"args0": [
				{
					"type": "field_dropdown",
					"name": "gesture",
					"options": [
						[
							"shake",
							"EVENT_SHAKE"
						],
						[
							"board up",
							"EVENT_BOARD_UP"
						],
						[
							"board down",
							"EVENT_BOARD_DOWN"
						],
						[
							"screen up",
							"EVENT_SCREEN_UP"
						],
						[
							"screen down",
							"EVENT_SCREEN_DOWN"
						],
						[
							"tilt left",
							"EVENT_TILT_LEFT"
						],
						[
							"tilt right",
							"EVENT_TILT_RIGHT"
						],
						[
							"free fall",
							"EVENT_FREE_FALL"
						],
						[
							"3g",
							"EVENT_3G"
						],
						[
							"6g",
							"EVENT_6G"
						],
						[
							"8g",
							"EVENT_8G"
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

Blockly.Blocks['iac_rotation'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_rotation",
			"message0": "rotation (°) %1",
			"args0": [
				{
					"type": "field_dropdown",
					"name": "axis",
					"options": [
						[
							"pitch",
							"AXIS_PITCH"
						],
						[
							"roll",
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
			"message0": "magnetic force (uT) %1",
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
			"tooltip": "",
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_calibrate_compass'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_calibrate_compass",
			"message0": "calibrate compass",
			"previousStatement": null,
			"nextStatement": null,
			"colour": 230,
			"tooltip": "",
			"helpUrl": ""
		});
	}
};

Blockly.Blocks['iac_set_accellerometer_range'] = {
	init: function () {
		this.jsonInit({
			"type": "iac_set_accellerometer_range",
			"message0": "set accellerometer range %1",
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
			"tooltip": "",
			"helpUrl": ""
		});
	}
};
