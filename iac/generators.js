const iAC_BEGIN_WORD = 'DEV_I2C1.iAC(0, 0x3C)';

Blockly.JavaScript['iac_on_gesture'] = function (block) {
	var dropdown_gesture = block.getFieldValue('gesture');
	var statements_callback = Blockly.JavaScript.statementToCode(block, 'callback');
	// TODO: Assemble JavaScript into code variable.
	var code = `${iAC_BEGIN_WORD}.on_gesture(${dropdown_gesture}, []() { ${statements_callback} });\n`;
	return code;
};


Blockly.JavaScript['iac_acceleration'] = function (block) {
	var dropdown_axis = block.getFieldValue('axis');
	var code = `${iAC_BEGIN_WORD}.acceleration(${dropdown_axis})\n`;
	return [code, Blockly.JavaScript.ORDER_NONE];
};

Blockly.JavaScript['iac_compass_heading'] = function (block) {
	// TODO: Assemble JavaScript into code variable.
	var code = '...';
	// TODO: Change ORDER_NONE to the correct strength.
	return [code, Blockly.JavaScript.ORDER_NONE];
};

Blockly.JavaScript['iac_is_gesture'] = function (block) {
	var dropdown_gesture = block.getFieldValue('gesture');
	// TODO: Assemble JavaScript into code variable.
	var code = '...';
	// TODO: Change ORDER_NONE to the correct strength.
	return [code, Blockly.JavaScript.ORDER_NONE];
};

Blockly.JavaScript['iac_rotation'] = function (block) {
	var dropdown_axis = block.getFieldValue('axis');
	// TODO: Assemble JavaScript into code variable.
	var code = '...';
	// TODO: Change ORDER_NONE to the correct strength.
	return [code, Blockly.JavaScript.ORDER_NONE];
};

Blockly.JavaScript['iac_magnetic_force'] = function (block) {
	var dropdown_axis = block.getFieldValue('axis');
	// TODO: Assemble JavaScript into code variable.
	var code = '...';
	// TODO: Change ORDER_NONE to the correct strength.
	return [code, Blockly.JavaScript.ORDER_NONE];
};

Blockly.JavaScript['iac_calibrate_compass'] = function (block) {
	// TODO: Assemble JavaScript into code variable.
	var code = '...;\n';
	return code;
};

Blockly.JavaScript['iac_set_accellerometer_range'] = function (block) {
	var dropdown_range = block.getFieldValue('range');
	// TODO: Assemble JavaScript into code variable.
	var code = '...;\n';
	return code;
};

