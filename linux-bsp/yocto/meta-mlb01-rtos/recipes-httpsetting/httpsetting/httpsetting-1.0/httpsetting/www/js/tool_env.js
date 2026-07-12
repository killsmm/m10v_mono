// JavaScript Document
// Default
// -------------------------------------------------------------------
// Check environment information
// -------------------------------------------------------------------
function env_check()
{
	window.alert(
		"No register of environment files.\n"
	  + "Show by default settings.");
	return true;
}

// -------------------------------------------------------------------
// Get Resolution init value
// -------------------------------------------------------------------
function env_resolution_init(camera_mode)
{
	return camera_mode == 2 ? 7 : 1;
}

// -------------------------------------------------------------------
// Get the CameraMode UI contents
// -------------------------------------------------------------------
function env_get_uiList_cameramode(camera_mode)
{
	return [
		"<option value='1'>1. Idle</option>",
		"<option value='2'>2. Video Preview</option>",
		"<option value='3'>3. Still Capture</option>"
	];
}

// -------------------------------------------------------------------
// Get the Resolution's UI contents
// -------------------------------------------------------------------
function env_get_uiList_resolution(camera_mode)
{
	var ret;
	
	// Create contents of relationary Camera Mode
	switch (camera_mode)
	{
		case 2:
			// Preview Mode
			ret = [
				"<option value='7'>3840x2160 - (60fps)</option>",
				"<option value='9'>1920x1080 - (60fps)</option>"
			];
			break;
		case 3:
			// Single Capture Mode
			ret = [
				"<option value='1'>4000x3000</option>"
			];
			break;
		default:
			// Othre case
			ret = [
				"<option value='0'>---</option>"
			];			
			break;
	}
	
	return ret;
}

// -------------------------------------------------------------------
// Get the AE Metering Mode's UI contents
// -------------------------------------------------------------------
function env_get_uiList_aemetering()
{
	return [
		"<option value='0'>0. Center</option>",
		"<option value='1'>1. Spot</option>",
		"<option value='2'>2. Average</option>",
		"<option value='3'>3. Mode 3</option>",
		"<option value='4'>4. Mode 4</option>",
		"<option value='5'>5. Mode 5</option>",
		"<option value='6'>6. Mode 6</option>",
		"<option value='7'>7. Mode 7</option>"
	];
}

// -------------------------------------------------------------------
// Get the AE.bin's UI contents
// -------------------------------------------------------------------
function env_get_uiList_aebin()
{
	return [
		"<option value='-1'>-. no-allocation</option>",
		"<option value='1'>1. AE_S_CAP</option>",
		"<option value='2'>2. AE_S_MON</option>",
		"<option value='3'>3. ---</option>",
		"<option value='4'>4. AE_V_FHD</option>",
		"<option value='5'>5. AE_V_4K</option>"
	];
}

// -------------------------------------------------------------------
// Get the IQ.bin's UI contents
// -------------------------------------------------------------------
function env_get_uiList_iqbin()
{
	return [
		"<option value='-1'>-. no-allocation</option>",
		"<option value='0'>0. IQ_COMM</option>",
		"<option value='1'>1. IQ_0</option>",
		"<option value='2'>2. IQ_1</option>",
		"<option value='3'>3. IQ_2</option>",
		"<option value='4'>4. IQ_3</option>",
		"<option value='5'>5. IQ_4</option>",
		"<option value='6'>6. IQ_5</option>"
	];
}

// -------------------------------------------------------------------
// Get the ISO's UI contents
// -------------------------------------------------------------------
function env_get_uiList_iso(isAutoInclude)
{
	var ret;

	if (!isAutoInclude)
	{
		ret = [
			"<option value='1'>ISO100</option>",
			"<option value='2'>ISO200</option>",
			"<option value='3'>ISO400</option>",
			"<option value='4'>ISO800</option>",
			"<option value='5'>ISO1600</option>",
			"<option value='6'>ISO3200</option>",
			"<option value='7'>ISO6400</option>"
		];
	}
	else 
	{
		ret = [
			"<option value='0'>Auto</option>",
			"<option value='1'>ISO100</option>",
			"<option value='2'>ISO200</option>",
			"<option value='3'>ISO400</option>",
			"<option value='4'>ISO800</option>",
			"<option value='5'>ISO1600</option>",
			"<option value='6'>ISO3200</option>",
			"<option value='7'>ISO6400</option>"
		];
	}

	return ret;
}

// -------------------------------------------------------------------
// Get the OB init value
// -------------------------------------------------------------------
function env_get_ob_init()
{
	return {
		// IMX277
		"r":  [-800,-800,-800,-800,-800,-800,-800],
		"gr": [-800,-800,-800,-800,-800,-800,-800],
		"gb": [-800,-800,-800,-800,-800,-800,-800],
		"b":  [-800,-800,-800,-800,-800,-800,-800]
	};
}

// -------------------------------------------------------------------
// Get AE.bin index
// -------------------------------------------------------------------
function env_get_ae_bin_index(camera_mode, resolution)
{
	var ret = -1;

	switch (camera_mode)
	{
		case 2: // Video Preview
			ret = resolution == 9 ? 4 : 5;
			break;
		case 3: // Still Capture
			ret = 1;
			break;
		default:
			break;
	}

	return ret;
}

// -------------------------------------------------------------------
// Get IQ.bin index
// -------------------------------------------------------------------
function env_get_iq_bin_index(camera_mode, resolution)
{
	var ret = -1;

	switch (camera_mode)
	{
		case 2: // Video Preview
			ret = 3;
			break;
		case 3: // Still Capture
			ret = 2;
			break;
		default:
			break;
	}

	return ret;
}

// -------------------------------------------------------------------
// Disabled UI
// -------------------------------------------------------------------
function env_disable_ui()
{
	return [
		"#Group_AE_Default_AEMode"
	];
}