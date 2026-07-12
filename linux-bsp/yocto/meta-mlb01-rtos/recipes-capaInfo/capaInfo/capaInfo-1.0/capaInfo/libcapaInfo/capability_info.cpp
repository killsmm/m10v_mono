#include <unistd.h>
#include <algorithm>
#include <iterator>
#include <cstring>
#include "capability_info.h"
#include "capability_shmem.h"
extern "C"{
#include "cif_stream.h"
}

using namespace std;

#define INTCAST(x) static_cast<int>(x)

CException::CException(const string& aMessage)
	: message_(aMessage)
{
}

CException::CException(const string& aMessage, const char* aFile,
	const char* aFunction, const int aLine)
	: message_(aMessage)
	, fileName_(aFile)
	, functionName_(aFunction)
	, line_(aLine)
{
}

CException::~CException() throw() {}

const char* CException::what() const throw() {
	return message_.c_str();
}

const char* CException::getFileName() const {
	return fileName_;
}

const char* CException::getFunctionName() const {
	return functionName_;
}

const int CException::getLineNumber() const {
	return line_;
}

// ----------------------------------------------------

CCapabilityInfo* CCapabilityInfo::instance_ = NULL;
CCapabilityInfo* CCapabilityInfo::GetInstance()
{
	if(! instance_) {
		instance_ = new CCapabilityInfo;
		if (instance_->FirstSetting() == false)
			return NULL;
	}
	return instance_;
}

void CCapabilityInfo::DeleteInstance()
{
	CapSharedMemory::DeleteInstance();
	delete instance_;
	instance_ = NULL;
}

bool CCapabilityInfo::CameraIfExec(uint32_t cmd)
{
	struct camera_if_cmd camif;

	CapSharedMemory* shm_inst = CapSharedMemory::GetInstance();
	if (shm_inst == NULL)
		return false;

	memset(&camif, 0, sizeof(camif));
	camif.cmd_set = E_CAM_IF_COM_GET_CAPA_INFO;
	camif.cmd = static_cast<E_CAM_IF_SUB_COM>(cmd);
	camif.send_param[0] = shm_inst->GetPhyAddr();
	int ret = camera_if_command(&camif);
	if (ret != 0 || camif.recv_param[0] != 0) {
		return false;
	}
	return true;
}

bool CCapabilityInfo::CameraIfExec(uint32_t cmd, uint32_t para, uint32_t mode)
{
	struct camera_if_cmd camif;

	CapSharedMemory* shm_inst = CapSharedMemory::GetInstance();
	if (shm_inst == NULL)
		return false;

	memset(&camif, 0, sizeof(camif));
	camif.cmd_set = E_CAM_IF_COM_GET_CAPA_INFO;
	camif.cmd = static_cast<E_CAM_IF_SUB_COM>(cmd);
	camif.send_param[0] = shm_inst->GetPhyAddr();
	camif.send_param[1] = para;
	camif.send_param[2] = mode;
	int ret = camera_if_command(&camif);
	if (ret != 0 || camif.recv_param[0] != 0) {
		return false;
	}
	return true;
}

bool CCapabilityInfo::FirstSetting()
{
	unsigned long cnt = 0;
	void* addr;

	/* Shared memory address initialize for capability */
	CapSharedMemory* shm_inst = CapSharedMemory::GetInstance();
	if (shm_inst == NULL)
		return false;

	/* Get shared memory top address */
	void* topAddr = shm_inst->GetShmAddr();

	/* Get Sensor Type */
	if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_SENSOR_TYPE))
		return false;
	struct cap_sensor_type* sensor = static_cast<cap_sensor_type*>(topAddr);
	/* Set Video Channel[Channel] */
	camSensor_.SetSensorType(sensor->sensorType);
	/* Set Cammera Sensor */
	for (cnt = 0; cnt < sensor->maxStream; cnt++) {
		CCapVideoChannel v;
		v.SetChannel(sensor->streamType[cnt]);
		camSensor_.VideoCh().push_back(v);
	}

	switch(sensor->sensorType) {
		case E_SENSOR_4SENSORS:
		case E_SENSOR_DUAL:
		{
			/* Get Stitching Type */
			if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_STITCHING_TYPE))
				return false;
			struct cap_stitching_type* stitchType =
				static_cast<cap_stitching_type*>(topAddr);

			/* Set Stitching Type */
			void* stitchTypeAddr = addPlus(topAddr, stitchType->stitchingTypeOffset);
			for (cnt = 0; cnt < stitchType->maxStitchingType; cnt++) {
				addr = addPlus(stitchTypeAddr, (sizeof(cap_cameraIf_char) * cnt));
				struct cap_cameraIf_char* stitch;
				stitch = static_cast<cap_cameraIf_char*>(addr);
				camSensor_.StitchingType().insert(
					std::make_pair(stitch->value, stitch->cameraIf));
			}
			break;
		}
	default:
		{
			/* Get DOL Mode */
			if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_DOL_MODE))
				return false;
			struct cap_dol_mode* dolMode = static_cast<cap_dol_mode*>(topAddr);
			/* Set DOL Mode */
			void* dolModeAddr = addPlus(topAddr, dolMode->dolModeOffset);
			for (cnt = 0; cnt < dolMode->maxDolMode; cnt++) {
				addr = addPlus(dolModeAddr, (sizeof(cap_cameraIf_char) * cnt));
				struct cap_cameraIf_char* dol;
				dol = static_cast<cap_cameraIf_char*>(addr);
				camSensor_.DolMode().insert(
					std::make_pair(dol->value, dol->cameraIf));
			}
			break;
		}
	}

	/* Get Bitrate */
	if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_BITRATE_INFO))
		return false;
	struct cap_bitrate_info* bitrate = static_cast<cap_bitrate_info*>(topAddr);
	/* Set Bitrate */
	bitrate_.SetAlgorithm(bitrate->algorithm);
	bitrate_.SetBitrateCbr(
		bitrate->cbrMin, bitrate->cbrMax,
		bitrate->cbrAverage, bitrate->cbrRes);
	bitrate_.SetBitrateVbr(
		bitrate->vbrMin, bitrate->vbrMax,
		bitrate->vbrAverage, bitrate->vbrRes);

	/* Get GOP */
	if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_GOP_INFO))
		return false;
	struct cap_gop_info* gop = static_cast<cap_gop_info*>(topAddr);
	/* Set GOP */
	// GOP Struct
	for (cnt = 0; cnt < gop->gopStruct.maxNum; cnt++) {
		gop_.GopStruct().CamIf().insert(std::make_pair(
			gop->gopStruct.g_struct[cnt].value,
			gop->gopStruct.g_struct[cnt].cameraIf));
	}
	gop_.GopStruct().SetRange(
		gop->gopStruct.range.min,
		gop->gopStruct.range.max,
		gop->gopStruct.range.def,
		gop->gopStruct.range.res);
	// Video strage format
	for (cnt = 0; cnt < gop->format.maxNum; cnt++) {
		gop_.GopStruct().CamIf().insert(std::make_pair(
			gop->format.g_format[cnt].value,
			gop->format.g_format[cnt].cameraIf));
	}
	gop_.GopStruct().SetRange(
		gop->format.range.min,
		gop->format.range.max,
		gop->format.range.def,
		gop->format.range.res);

	/* Get IP */
	if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_IP_INFO))
		return false;
	struct cap_ip_setting* ip = static_cast<cap_ip_setting*>(topAddr);
	/* Set IP */
	ipSet_.SetHlc(ip->hlc);
	ipSet_.SetBacklight(ip->backlight);
	ipSet_.SetDefog(ip->defog);
	ipSet_.SetTa(ip->ta);
	ipSet_.SetOd(ip->od);
	ipSet_.SetFd(ip->fd);
	ipSet_.SetFr(ip->fr);
	ipSet_.SetEis(ip->eis);
	ipSet_.SetMetadata(ip->metadata);

	/* Get Audio */
	if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_AUDIO_INFO))
		return false;
	struct cap_audio_channel* audio = static_cast<cap_audio_channel*>(topAddr);
	/* Set Audio */
	audio_.SetAudioSupport(audio->suport);
	if (audio->suport) {
		// Codec
		for (cnt = 0; cnt < audio->codec.maxNum; cnt++) {
			audio_.Codec().CamIf().insert(std::make_pair(
				audio->codec.mode[cnt].value,
				audio->codec.mode[cnt].cameraIf));
		}
		audio_.Codec().SetRange(
			audio->codec.range.min,
			audio->codec.range.max,
			audio->codec.range.def,
			audio->codec.range.res);
		// Sampling
		for (cnt = 0; cnt < audio->sampRate.maxNum; cnt++) {
			audio_.SampRate().CamIf().insert(std::make_pair(
				audio->sampRate.rate[cnt].value,
				audio->sampRate.rate[cnt].cameraIf));
		}
		audio_.SampRate().SetRange(
			audio->sampRate.range.min,
			audio->sampRate.range.max,
			audio->sampRate.range.def,
			audio->sampRate.range.res);
		// Mic
		for (cnt = 0; cnt < audio->mic.maxNum; cnt++) {
			audio_.Mic().CamIf().insert(std::make_pair(
				audio->mic.mic[cnt].value,
				audio->mic.mic[cnt].cameraIf));
		}
		audio_.Mic().SetRange(
			audio->mic.range.min,
			audio->mic.range.max,
			audio->mic.range.def,
			audio->mic.range.res);
		// Bit Rate
		audio_.Rate().SetRange(
			audio->bitRate.min,
			audio->bitRate.max,
			audio->bitRate.def,
			audio->bitRate.res);
		// Channel Type
		for (cnt = 0; cnt < audio->channelType.maxNum; cnt++) {
			audio_.ChType().CamIf().insert(std::make_pair(
				audio->channelType.chtyp[cnt].value,
				audio->channelType.chtyp[cnt].cameraIf));
		}
		audio_.ChType().SetRange(
			audio->channelType.range.min,
			audio->channelType.range.max,
			audio->channelType.range.def,
			audio->channelType.range.res);
	}

	/* Get Camera Terminal */
	if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_CAMERA_TERM_SUPPORT))
		return false;
	struct cap_camera_terminal* terminal =
		static_cast<cap_camera_terminal*>(topAddr);
	/* Set Camera Terminal */
	scanMode_.SetSupport(terminal->scanningMode);
	ae_.SetSupport(terminal->autoExposure);
	expTime_.SetSupport(terminal->exposureTime);
	focus_.SetSupport(terminal->focus);
	iris_.SetSupport(terminal->iris);
	zoom_.SetSupport(terminal->zoom);
	pan_.SetSupport(terminal->pan);
	tilt_.SetSupport(terminal->tilt);
	roll_.SetSupport(terminal->roll);
	priMask_.SetSupport(terminal->privacy);

	if (scanMode_.GetSupport()) {
		/* Get Scanning Mode */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_SCANNING_MODE)) {
			scanMode_.SetSupport(0);
			return false;
		}
		struct cap_scanning_mode* scanMode =
			static_cast<cap_scanning_mode*>(topAddr);
		/* Set Camera Terminal */
		for (cnt = 0; cnt < scanMode->maxNum; cnt++) {
			scanMode_.CamIf().insert(std::make_pair(
				scanMode->mode[cnt].value,
				scanMode->mode[cnt].cameraIf));
		}
	}

	if (ae_.GetSupport()) {
		/* Get AE */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_AE)) {
			ae_.SetSupport(0);
			return false;
		}
		struct cap_auto_exposure* ae =
			static_cast<cap_auto_exposure*>(topAddr);
		/* Set AE */
		ae_.SetManualMode(ae->modeManual);
		ae_.SetAutoMode(ae->modeAuto);
		ae_.SetShutter(ae->shutterPriority);
		ae_.SetAperture(ae->aperturePriority);
		ae_.SetAeControl(ae->aePriorityCtl);
	}

	if (expTime_.GetSupport()) {
		/* Get Exposure Time */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_EXPO_TIME)) {
			expTime_.SetSupport(0);
			return false;
		}
		struct cap_exposure_time* exptime =
			static_cast<cap_exposure_time*>(topAddr);
		/* Set Exposure Time */
		for (cnt = 0; cnt < exptime->maxNum; cnt++) {
			SCapaExpoTimeVal et;
			et.SetCameraIf(exptime->param[cnt].cameraIf);
			et.SetNumerator(exptime->param[cnt].numerator);
			et.SetDenominator(exptime->param[cnt].denominator);
			expTime_.expoTime().push_back(et);
		}
		expTime_.SetRange(
			exptime->range.min,
			exptime->range.max,
			exptime->range.def,
			exptime->range.res);
	}

	if (focus_.GetSupport()) {
		/* Get Focus */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_FOCUS)) {
			focus_.SetSupport(0);
			return false;
		}
		struct cap_focus* focus = static_cast<cap_focus*>(topAddr);
		/* Set Focus */
		focus_.SetRange(
			focus->range.min,
			focus->range.max,
			focus->range.def,
			focus->range.res);
		focus_.SetAutoCtl(focus->focusAutoCtl);
	}

	if (iris_.GetSupport()) {
		/* Get Iris */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_IRIS)) {
			iris_.SetSupport(0);
			return false;
		}
		struct cap_iris* iris = static_cast<cap_iris*>(topAddr);
		/* Set Iris */
		iris_.SetRange(
			iris->range.min,
			iris->range.max,
			iris->range.def,
			iris->range.res);
	}

	if (zoom_.GetSupport()) {
		/* Get Zoom */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_ZOOM)) {
			zoom_.SetSupport(0);
			return false;
		}
		struct cap_zoom* zoom = static_cast<cap_zoom*>(topAddr);
		/* Set Zoom */
		zoom_.SetRange(
			zoom->range.min,
			zoom->range.max,
			zoom->range.def,
			zoom->range.res);
	}

	if (pan_.GetSupport()) {
		/* Get Pan */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_PAN)) {
			pan_.SetSupport(0);
			return false;
		}
		struct cap_zoom* pan = static_cast<cap_zoom*>(topAddr);
		/* Set Pan */
		pan_.SetRange(
			pan->range.min,
			pan->range.max,
			pan->range.def,
			pan->range.res);
	}

	if (tilt_.GetSupport()) {
		/* Get Tilt */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_TILT)) {
			tilt_.SetSupport(0);
			return false;
		}
		struct cap_tilt* tilt = static_cast<cap_tilt*>(topAddr);
		/* Set Tilt */
		tilt_.SetRange(
			tilt->range.min,
			tilt->range.max,
			tilt->range.def,
			tilt->range.res);
	}

	if (roll_.GetSupport()) {
		/* Get Roll */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_ROLL)) {
			roll_.SetSupport(0);
			return false;
		}
		struct cap_roll* roll = static_cast<cap_roll*>(topAddr);
		/* Set Roll */
		roll_.SetRange(
			roll->range.min,
			roll->range.max,
			roll->range.def,
			roll->range.res);
	}

	if (priMask_.GetSupport()) {
		/* Get Privacy Mask */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_PRIVACY_MASK)) {
			priMask_.SetSupport(0);
			return false;
		}
		struct cap_privacy_mask* priMask =
			static_cast<cap_privacy_mask*>(topAddr);
		/* Set Privacy Mask */
		// Sensor
		for (cnt = 0; cnt < priMask->sensor.maxNum; cnt++) {
			priMask_.Sensor().Sensor().insert(std::make_pair(
				priMask->sensor.pos[cnt].value,
				priMask->sensor.pos[cnt].cameraIf));
		}
		priMask_.Sensor().SetRange(
			priMask->sensor.range.min,
			priMask->sensor.range.max,
			priMask->sensor.range.def,
			priMask->sensor.range.res);
		// Area No
		for (cnt = 0; cnt < priMask->areaNum.maxNum; cnt++) {
			priMask_.AreaNo().AreaNo().insert(std::make_pair(
				priMask->areaNum.num[cnt].value,
				priMask->areaNum.num[cnt].cameraIf));
		}
		priMask_.AreaNo().SetRange(
			priMask->areaNum.range.min,
			priMask->areaNum.range.max,
			priMask->areaNum.range.def,
			priMask->areaNum.range.res);
		// X Coordinate
		priMask_.XCoordinate().SetRange(
			priMask->xCoordinate.min,
			priMask->xCoordinate.max,
			priMask->xCoordinate.def,
			priMask->xCoordinate.res);
		// Y Coordinate
		priMask_.YCoordinate().SetRange(
			priMask->yCoordinate.min,
			priMask->yCoordinate.max,
			priMask->yCoordinate.def,
			priMask->yCoordinate.res);
		// Width
		priMask_.Width().SetRange(
			priMask->width.min,
			priMask->width.max,
			priMask->width.def,
			priMask->width.res);
		// Lines
		priMask_.Lines().SetRange(
			priMask->lines.min,
			priMask->lines.max,
			priMask->lines.def,
			priMask->lines.res);
		// Y
		priMask_.Y().SetRange(
			priMask->y.min,
			priMask->y.max,
			priMask->y.def,
			priMask->y.res);
		// Cb
		priMask_.Cb().SetRange(
			priMask->cb.min,
			priMask->cb.max,
			priMask->cb.def,
			priMask->cb.res);
		// Cr
		priMask_.Cr().SetRange(
			priMask->cr.min,
			priMask->cr.max,
			priMask->cr.def,
			priMask->cr.res);
	}

	/* Get Processing Unit */
	if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_PROC_UNIT_SUPPORT))
		return false;
	struct cap_processing_unit* procUnit =
		static_cast<cap_processing_unit*>(topAddr);
	/* Set Processing Unit */
	bright_.SetSupport(procUnit->brightness);
	contrast_.SetSupport(procUnit->contrast);
	hue_.SetSupport(procUnit->hue);
	saturation_.SetSupport(procUnit->saturation);
	sharpEdge_.SetSupport(procUnit->sharpness);
	gamma_.SetSupport(procUnit->gamma);
	digiMulti_.SetSupport(procUnit->digitalMultiplierZoom);
	wB_.SetSupport(procUnit->whiteBalance);
	backlight_.SetSupport(procUnit->backlight);
	gain_.SetSupport(procUnit->gain);
	powerLine_.SetSupport(procUnit->powerLineFrequency);
	aVideo_.SetSupport(procUnit->analogVideo);

	if (bright_.GetSupport()) {
		/* Get Brightness */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_BRIGHTNESS)) {
			bright_.SetSupport(0);
			return false;
		}
		struct cap_brightness* bright =
			static_cast<cap_brightness*>(topAddr);
		/* Set Brightness */
		bright_.SetRange(
			bright->range.min,
			bright->range.max,
			bright->range.def,
			bright->range.res);
	}

	if (contrast_.GetSupport()) {
		/* Get Contrast */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_CONTRAST)) {
			contrast_.SetSupport(0);
			return false;
		}
		struct cap_contrast* contrast =
			static_cast<cap_contrast*>(topAddr);
		/* Set Contrast */
		contrast_.SetRange(
			contrast->range.min,
			contrast->range.max,
			contrast->range.def,
			contrast->range.res);
	}

	if (hue_.GetSupport()) {
		/* Get Hue */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_HUE)) {
			hue_.SetSupport(0);
			return false;
		}
		struct cap_hue* hue =
			static_cast<cap_hue*>(topAddr);
		/* Set Hue */
		hue_.SetHueAuto(hue->autoSupport);
		hue_.SetRange(
			hue->range.min,
			hue->range.max,
			hue->range.def,
			hue->range.res);
	}

	if (saturation_.GetSupport()) {
		/* Get Saturation */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_SATURATION)) {
			saturation_.SetSupport(0);
			return false;
		}
		struct cap_saturation* saturation =
			static_cast<cap_saturation*>(topAddr);
		/* Set Saturation */
		saturation_.SetRange(
			saturation->range.min,
			saturation->range.max,
			saturation->range.def,
			saturation->range.res);
	}

	if (sharpEdge_.GetSupport()) {
		/* Get Sharpness and Edge */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_SHARPNESS)) {
			sharpEdge_.SetSupport(0);
			return false;
		}
		struct cap_sharpness_edge* sharpEdge =
			static_cast<cap_sharpness_edge*>(topAddr);
		/* Set Sharpness and Edge */
		// Sharpness
		for (cnt = 0; cnt < sharpEdge->sharpness.maxNum; cnt++) {
			sharpEdge_.Sharp().Sharpness().insert(std::make_pair(
				sharpEdge->sharpness.param[cnt].numVal,
				sharpEdge->sharpness.param[cnt].cameraIf));
		}
		sharpEdge_.Sharp().SetRange(
			sharpEdge->sharpness.range.min,
			sharpEdge->sharpness.range.max,
			sharpEdge->sharpness.range.def,
			sharpEdge->sharpness.range.res);
		// Edge
		for (cnt = 0; cnt < sharpEdge->edge.maxNum; cnt++) {
			sharpEdge_.Edge().Edge().insert(std::make_pair(
				sharpEdge->edge.param[cnt].numVal,
				sharpEdge->edge.param[cnt].cameraIf));
		}
		sharpEdge_.Edge().SetRange(
			sharpEdge->edge.range.min,
			sharpEdge->edge.range.max,
			sharpEdge->edge.range.def,
			sharpEdge->edge.range.res);
	}

	if (gamma_.GetSupport()) {
		/* Get Gamma */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_GAMMA)) {
			gamma_.SetSupport(0);
			return false;
		}
		struct cap_gamma* gamma =
			static_cast<cap_gamma*>(topAddr);
		/* Set Gamma */
		gamma_.SetRange(
			gamma->range.min,
			gamma->range.max,
			gamma->range.def,
			gamma->range.res);
	}

	if (digiMulti_.GetSupport()) {
		/* Get Digital Multiplier(Zoom) */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_DIGI_MULTIPLIER)) {
			digiMulti_.SetSupport(0);
			return false;
		}
		struct cap_digital_multi_zoom* digiMulti =
			static_cast<cap_digital_multi_zoom*>(topAddr);
		/* Set Digital Multiplier(Zoom) */
		digiMulti_.SetRange(
			digiMulti->range.min,
			digiMulti->range.max,
			digiMulti->range.def,
			digiMulti->range.res);
	}

	if (wB_.GetSupport()) {
		/* Get White Balance */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_WHITE_BALANCE)) {
			wB_.SetSupport(0);
			return false;
		}
		struct cap_wb_component* wb =
			static_cast<cap_wb_component*>(topAddr);
		/* Set White Balance */
		wB_.SetTempSupport(wb->wbTempSupport);
		wB_.Temp().SetRange(
			wb->wbTemp.min,
			wb->wbTemp.max,
			wb->wbTemp.def,
			wb->wbTemp.res);
		wB_.SetCompSupport(wb->wbCompSupport);
		for (cnt = 0; cnt < wb->maxNum; cnt++) {
			wB_.Component().insert(std::make_pair(
				wb->param[cnt].value,
				wb->param[cnt].cameraIf));
		}
		wB_.Comp().SetRange(
			wb->range.min,
			wb->range.max,
			wb->range.def,
			wb->range.res);
		wB_.CompRGain().SetRange(
			wb->rGain.min,
			wb->rGain.max,
			wb->rGain.def,
			wb->rGain.res);
		wB_.CompBGain().SetRange(
			wb->bGain.min,
			wb->bGain.max,
			wb->bGain.def,
			wb->bGain.res);
	}

	if (backlight_.GetSupport()) {
		/* Get Backlight Compensation */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_BACKLIGHT)) {
			backlight_.SetSupport(0);
			return false;
		}
		struct cap_backlight* backlight =
			static_cast<cap_backlight*>(topAddr);
		/* Set Backlight Compensation */
		backlight_.SetRange(
			backlight->range.min,
			backlight->range.max,
			backlight->range.def,
			backlight->range.res);
	}

	if (gain_.GetSupport()) {
		/* Get Gain */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_GAIN)) {
			gain_.SetSupport(0);
			return false;
		}
		struct cap_gain* gain =
			static_cast<cap_gain*>(topAddr);
		/* Set Gain */
		gain_.SetRange(
			gain->range.min,
			gain->range.max,
			gain->range.def,
			gain->range.res);
	}

	if (powerLine_.GetSupport()) {
		/* Get Power Line Frequency */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_POW_LINE_FREQ)) {
			powerLine_.SetSupport(0);
			return false;
		}
		struct cap_power_line_frequency* powerLine =
			static_cast<cap_power_line_frequency*>(topAddr);
		/* Set Power Line Frequency */
		powerLine_.SetRange(
			powerLine->range.min,
			powerLine->range.max,
			powerLine->range.def,
			powerLine->range.res);
	}

	if (aVideo_.GetSupport()) {
		/* Get Analog Video Standard Status */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_ANA_VIDEO)) {
			aVideo_.SetSupport(0);
			return false;
		}
		struct cap_analog_video* analogVideo =
			static_cast<cap_analog_video*>(topAddr);
		/* Set Analog Video Standard Status */
		aVideo_.Standard().SetRange(
			analogVideo->standardStatus.min,
			analogVideo->standardStatus.max,
			analogVideo->standardStatus.def,
			analogVideo->standardStatus.res);
		/* Set Analog Video Lock Status */
		aVideo_.LockStatus().SetRange(
			analogVideo->lockStatus.min,
			analogVideo->lockStatus.max,
			analogVideo->lockStatus.def,
			analogVideo->lockStatus.res);
	}

	/* Get Specific function */
	if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_SPEC_FUNC_SUPPORT))
		return false;
	struct cap_specific_function* specFunc =
		static_cast<cap_specific_function*>(topAddr);
	/* Set Specific function */
	timeLap_.SetSupport(specFunc->timeLapse);
	osd_.SetSupport(specFunc->osd);
	storage_.SetSupport(specFunc->storage);

	if (timeLap_.GetSupport()) {
		/* Get Time Lapse */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_TIME_LAPSE)) {
			timeLap_.SetSupport(0);
			return false;
		}
		struct cap_time_lapse* timelap =
			static_cast<cap_time_lapse*>(topAddr);
		/* Set Time Lapse */
		for (cnt = 0; cnt < timelap->maxNum; cnt++) {
			timeLap_.Interval().insert(std::make_pair(
				timelap->interval[cnt].numVal,
				timelap->interval[cnt].cameraIf));
		}
		timeLap_.SetRange(
			timelap->range.min,
			timelap->range.max,
			timelap->range.def,
			timelap->range.res);
	}

	if (osd_.GetSupport()) {
		/* Get OSD */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_OSD)) {
			osd_.SetSupport(0);
			return false;
		}
		struct cap_osd* osd = static_cast<cap_osd*>(topAddr);
		/* Set OSD */
		// Style
		for (cnt = 0; cnt < osd->style.maxNum; cnt++) {
			osd_.TraPer().insert(std::make_pair(
				osd->style.trans_perency[cnt].numVal,
				osd->style.trans_perency[cnt].cameraIf));
		}
		osd_.SetRange(
			osd->style.range.min,
			osd->style.range.max,
			osd->style.range.def,
			osd->style.range.res);
		// Area Select
		for (cnt = 0; cnt < osd->areaSelect.sensor.maxNum; cnt++) {
			osd_.Sensor().Sensor().insert(std::make_pair(
				osd->areaSelect.sensor.pos[cnt].value,
				osd->areaSelect.sensor.pos[cnt].cameraIf));
		}
		osd_.Sensor().SetRange(
			osd->areaSelect.sensor.range.min,
			osd->areaSelect.sensor.range.max,
			osd->areaSelect.sensor.range.def,
			osd->areaSelect.sensor.range.res);
		for (cnt = 0; cnt < osd->areaSelect.sensor.maxNum; cnt++) {
			osd_.AreaNo().AreaNo().insert(std::make_pair(
				osd->areaSelect.areaNum.num[cnt].value,
				osd->areaSelect.areaNum.num[cnt].cameraIf));
		}
		osd_.AreaNo().SetRange(
			osd->areaSelect.areaNum.range.min,
			osd->areaSelect.areaNum.range.max,
			osd->areaSelect.areaNum.range.def,
			osd->areaSelect.areaNum.range.res);
		osd_.XCoord().SetRange(
			osd->areaSelect.xCoordinate.min,
			osd->areaSelect.xCoordinate.max,
			osd->areaSelect.xCoordinate.def,
			osd->areaSelect.xCoordinate.res);
		osd_.YCoord().SetRange(
			osd->areaSelect.yCoordinate.min,
			osd->areaSelect.yCoordinate.max,
			osd->areaSelect.yCoordinate.def,
			osd->areaSelect.yCoordinate.res);
		// Disp Control
		for (cnt = 0; cnt < osd->dispControl.sensor.maxNum; cnt++) {
			osd_.DCtlSensor().Sensor().insert(std::make_pair(
				osd->areaSelect.sensor.pos[cnt].value,
				osd->areaSelect.sensor.pos[cnt].cameraIf));
		}
		osd_.DCtlSensor().SetRange(
			osd->dispControl.sensor.range.min,
			osd->dispControl.sensor.range.max,
			osd->dispControl.sensor.range.def,
			osd->dispControl.sensor.range.res);
		for (cnt = 0; cnt < osd->dispControl.areaNum.maxNum; cnt++) {
			osd_.DCtlAreaNo().AreaNo().insert(std::make_pair(
				osd->dispControl.areaNum.num[cnt].value,
				osd->dispControl.areaNum.num[cnt].cameraIf));
		}
		osd_.DCtlAreaNo().SetRange(
			osd->dispControl.areaNum.range.min,
			osd->dispControl.areaNum.range.max,
			osd->dispControl.areaNum.range.def,
			osd->dispControl.areaNum.range.res);
		osd_.SetEnabled(osd->dispControl.enabled);
	}

	if (storage_.GetSupport()) {
		/* Get Storage */
		if (!CameraIfExec(E_CAM_IF_SUB_COM_CP_STORAGE)) {
			storage_.SetSupport(0);
			return false;
		}
		struct cap_storage* storage =
			static_cast<cap_storage*>(topAddr);
		/* Set Storage */
		storage_.SetSave(storage->sdSave.saveSetting);
		storage_.SetDefault(storage->sdSave.def);
	}
	return true;
}

bool CCapaCamSensor::MakeVideoChannel(uint32_t chType, uint32_t mode)
{
	int find = 0;
	unsigned long cnt = 0;
	void* addr;

	/* Shared memory address initialize for capability */
	CapSharedMemory* shm_inst = CapSharedMemory::GetInstance();
	if (shm_inst == NULL)
		return false;

	/* Get shared memory top address */
	void* topAddr = shm_inst->GetShmAddr();

	/* Get Video Channel */
	if (!CCapabilityInfo::CameraIfExec(
		E_CAM_IF_SUB_COM_CP_STREAM_INFO, chType, mode))
		return false;

	struct cap_video_stream_channel* sensor =
		static_cast<cap_video_stream_channel*>(topAddr);
	/* Set Video Channel */
	auto it_vc = videoCh_.begin();
	while (it_vc != videoCh_.end()) {
		if (it_vc->GetChannel() == chType) {
			find = 1;
			break;
		} else
			++it_vc;
	}

	if (find != 0) {
		void* cadecAddr = addPlus(topAddr, sensor->codecOffset);
		for (cnt = 0; cnt < sensor->maxCodec; cnt++) {
			addr = addPlus(cadecAddr, (sizeof(cap_cameraIf_char) * cnt));
			struct cap_cameraIf_char* cd;
			cd = static_cast<cap_cameraIf_char*>(addr);
			it_vc->Codec().insert(
				std::make_pair(cd->value, cd->cameraIf));
		}
		it_vc->SetSdSave(sensor->sdSave);
		it_vc->SetIspOpt(sensor->ispOption);
		void* frameRateAddr = addPlus(topAddr, sensor->frameRateOffset);
		for (cnt = 0; cnt < sensor->maxFrameRate; cnt++) {
			addr = addPlus(frameRateAddr, (sizeof(cap_cameraIf_num) * cnt));
			struct cap_cameraIf_num* fr;
			fr = static_cast<cap_cameraIf_num*>(addr);
			it_vc->FrameRate().insert(
				std::make_pair(fr->numVal, fr->cameraIf));
		}
		void* SizeAddr = addPlus(topAddr, sensor->imgSizeOffset);
		for (cnt = 0; cnt < sensor->maxImgSize; cnt++) {
			addr = addPlus(SizeAddr, (sizeof(cap_imageSize_info) * cnt));
			struct cap_imageSize_info* is;
			is = static_cast<cap_imageSize_info*>(addr);
			CCapBaseSize s;
			s.setSize(is->wSize, is->hSize);
			s.setCameraIf(is->cameraIf);
			it_vc->Size().insert(
				std::make_pair(is->size, s));
		}
	} else
		return false;

	return true;
}

