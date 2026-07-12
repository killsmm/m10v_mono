#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include "cap_command.h"
#include "capability_info.h"

void generate_capa_info() { CCapabilityInfo::GetInstance(); }
void delete_capa_info() { CCapabilityInfo::DeleteInstance(); }

int get_Sensor_type(
		unsigned long* type,
		unsigned long* num,
		struct cameraif_string* mode)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	uint32_t sensorType = capa->CamSensor().GetSensorType();
	*type = sensorType;

	std::map<std::string, uint32_t> md;
	switch(sensorType) {
	case E_SENSOR_4SENSORS:
	case E_SENSOR_DUAL:
		md = capa->CamSensor().StitchingType();
		break;
	default:
		md = capa->CamSensor().DolMode();
		break;
	}

	*num = md.size();
	auto i = 0;
	for(auto mit = md.begin(); mit != md.end(); mit++) {
		mit->first.copy(mode[i].str, (mit->first.size() + 1));
		mode[i].camif = mit->second;
		i++;
	}
	
	return D_CM_RESULT_OK;
}

int get_Channel_type(unsigned long* maxnum, unsigned long* chList)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::vector<CCapVideoChannel> vch = capa->CamSensor().VideoCh();
	*maxnum = vch.size();
	auto i = 0;
	for (auto vitr = vch.begin(); vitr != vch.end(); ++vitr) {
		chList[i] = vitr->GetChannel();
		i++;
	}

	return D_CM_RESULT_OK;
}

int make_Video_channel_info(unsigned long type, unsigned long mode)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	if (capa->MakeVideoChannel(type, mode))
		return D_CM_RESULT_OK;
	else
		return D_CM_RESULT_NG;
}

void delete_Video_channel_info()
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa != NULL)
		capa->DeleteVideoChannel();
}

int get_Codec(unsigned long type, struct cameraif_string* codec)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::vector<CCapVideoChannel> vch = capa->CamSensor().VideoCh();
	for (auto vitr = vch.begin(); vitr != vch.end(); ++vitr) {
		if (vitr->GetChannel() == type) {
			struct cameraif_string cam;
			std::map<std::string, uint32_t> mp(vitr->Codec());
			auto mpitr = mp.begin();
			mpitr->first.copy(cam.str, mpitr->first.size() + 1);
			cam.camif = mpitr->second;
			*codec = cam;
			goto end;
		}
	}
	return D_CM_RESULT_NG;
end:
	return D_CM_RESULT_OK;
}

int get_Sd_save_info(unsigned long type, unsigned long* sd)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::vector<CCapVideoChannel> vch = capa->CamSensor().VideoCh();
	for (auto vitr = vch.begin(); vitr != vch.end(); ++vitr) {
		if (vitr->GetChannel() == type) {
			*sd = vitr->GetSdSave();
			goto end;
		}
	}
	return D_CM_RESULT_NG;
end:
	return D_CM_RESULT_OK;
}

int get_Isp_info(unsigned long type, unsigned long* isp)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::vector<CCapVideoChannel> vch = capa->CamSensor().VideoCh();
	for (auto vitr = vch.begin(); vitr != vch.end(); ++vitr) {
		if (vitr->GetChannel() == type) {
			*isp = vitr->GetIspOpt();
			goto end;
		}
	}
	return D_CM_RESULT_NG;
end:
	return D_CM_RESULT_OK;
}

int get_Frame_rate(unsigned long type, unsigned long* num, struct cameraif_number* fr)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::vector<CCapVideoChannel> vch = capa->CamSensor().VideoCh();
	for (auto vitr = vch.begin(); vitr != vch.end(); ++vitr) {
		if (vitr->GetChannel() == type) {
			std::map<uint32_t, uint32_t> frate = vitr->FrameRate();
			*num = frate.size();
			auto i = 0;
			for(auto pair:frate) {
				fr[i].num = pair.first;
				fr[i].camif = pair.second;
				i++;
			}
			goto end;
		}
	}
	return D_CM_RESULT_NG;
end:
	return D_CM_RESULT_OK;
}

int get_Size(unsigned long type, unsigned long* num, struct cameraif_size* size)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::vector<CCapVideoChannel> vch = capa->CamSensor().VideoCh();
	for (auto vitr = vch.begin(); vitr != vch.end(); ++vitr) {
		if (vitr->GetChannel() == type) {
			std::map<std::string, CCapBaseSize> s = vitr->Size();
			*num = s.size();
			auto i = 0;
			for(auto pair:s) {
				pair.first.copy(size[i].str, pair.first.size() + 1);
				size[i].width = pair.second.getSizeWidth();
				size[i].height = pair.second.getSizeHeight();
				size[i].camif = pair.second.getCameraIf();
				i++;
			}
			goto end;
		}
	}
	return D_CM_RESULT_NG;
end:
	return D_CM_RESULT_OK;
}

int get_Bitrate_info(struct bitrate_info* bitrate)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	bitrate->algorithm = capa->Bitrate().GetAlgorithm().to_ulong();
	bitrate->cbrMin = capa->Bitrate().GetCbrMin();
	bitrate->cbrMax = capa->Bitrate().GetCbrMax();
	bitrate->cbrAverage = capa->Bitrate().GetCbrAverage();
	bitrate->cbrRes = capa->Bitrate().GetCbrRes();
	bitrate->vbrMin = capa->Bitrate().GetVbrMin();
	bitrate->vbrMax = capa->Bitrate().GetVbrMax();
	bitrate->vbrAverage = capa->Bitrate().GetVbrAverage();
	bitrate->vbrRes = capa->Bitrate().GetVbrRes();

	return D_CM_RESULT_OK;
}

int get_Range_info(unsigned long type, struct range_info* range)
{
	CCapBaseRange obj;

	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	switch (type) {
	case E_GOP_ST:
		obj = capa->Gop().GopStruct();
		break;
	case E_GOP_V_STG_FMT:
		obj = capa->Gop().VideoStrFmt();
		break;
	case E_AUDIO:
		obj = capa->Audio().Rate();
		break;
	case E_EXPOSURE_TIME:
		obj = capa->ExpTime();
		break;
	case E_FOCUS:
		obj = capa->Focus();
		break;
	case E_IRIS:
		obj = capa->Iris();
		break;
	case E_ZOOM:
		obj = capa->Zoom();
		break;
	case E_PAN:
		obj = capa->Pan();
		break;
	case E_TILT:
		obj = capa->Tilt();
		break;
	case E_ROLL:
		obj = capa->Roll();
		break;
	case E_PRIVACY_MASK_SENSOR:
		obj = capa->PriMask().Sensor();
		break;
	case E_PRIVACY_MASK_AREA_NO:
		obj = capa->PriMask().AreaNo();
		break;
	case E_PRIVACY_MASK_X_COORDINATE:
		obj = capa->PriMask().XCoordinate();
		break;
	case E_PRIVACY_MASK_Y_COORDINATE:
		obj = capa->PriMask().YCoordinate();
		break;
	case E_PRIVACY_MASK_WIDTH:
		obj = capa->PriMask().Width();
		break;
	case E_PRIVACY_MASK_LINES:
		obj = capa->PriMask().Lines();
		break;
	case E_PRIVACY_MASK_Y:
		obj = capa->PriMask().Y();
		break;
	case E_PRIVACY_MASK_CB:
		obj = capa->PriMask().Cb();
		break;
	case E_PRIVACY_MASK_CR:
		obj = capa->PriMask().Cr();
		break;
	case E_BRIGHTNESS:
		obj = capa->Bright();
		break;
	case E_CONTRAST:
		obj = capa->Contrast();
		break;
	case E_HUE:
		obj = capa->Hue();
		break;
	case E_SATURATION:
		obj = capa->Saturation();
		break;
	case E_SHARPNESS:
		obj = capa->SharpEdge().Sharp();
		break;
	case E_EDGE:
		obj = capa->SharpEdge().Edge();
		break;
	case E_GAMMA:
		obj = capa->Gamma();
		break;
	case E_DIGITAL_MULTIPLIER:
		obj = capa->DigiMulti();
		break;
	case E_WHITE_BALANCE_TEMPERATURE:
		obj = capa->WB().Temp();
		break;
	case E_WHITE_BALANCE_COMPONENT:
		obj = capa->WB().Comp();
		break;
	case E_WHITE_BALANCE_COMPONENT_R_GAIN:
		obj = capa->WB().CompRGain();
		break;
	case E_WHITE_BALANCE_COMPONENT_B_GAIN:
		obj = capa->WB().CompBGain();
		break;
	case E_BACKLIGHT:
		obj = capa->Backlight();
		break;
	case E_GAIN:
		obj = capa->Gain();
		break;
	case E_POWER_LINE_FREQUENCY:
		obj = capa->PowerLine();
		break;
	case E_ANALOG_VIDEO_STANDARD_STATUS:
		obj = capa->AVideo().Standard();
		break;
	case E_ANALOG_VIDEO_LOCK_STATUS:
		obj = capa->AVideo().LockStatus();
		break;
	case E_TIME_LAPSE:
		obj = capa->TimeLap();
		break;
	case E_OSD:
		obj = capa->Osd();
		break;
	case E_OSD_SENSOR:
		obj = capa->Osd().Sensor();
		break;
	case E_OSD_AREA_NO:
		obj = capa->Osd().AreaNo();
		break;
	case E_OSD_X_COORDINATE:
		obj = capa->Osd().XCoord();
		break;
	case E_OSD_Y_COORDINATE:
		obj = capa->Osd().YCoord();
		break;
	case E_OSD_DISP_CONTROL_SENSOR:
		obj = capa->Osd().DCtlSensor();
		break;
	case E_OSD_DISP_CONTROL_AREA_NO:
		obj = capa->Osd().DCtlAreaNo();
		break;
	default :
		return D_CM_RESULT_NG;
	}
	range->min = obj.GetMin();
	range->max = obj.GetMax();
	range->def = obj.GetDef();
	range->res = obj.GetRes();

	return D_CM_RESULT_OK;
}

int get_Gop_info(unsigned long type, unsigned long* num, struct cameraif_string* str)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	CCapaBaseCamIf gop;
	if (type == E_GOP_STRUCT)
		gop = capa->Gop().GopStruct();
	else
		gop = capa->Gop().VideoStrFmt();

	std::map<std::string, uint32_t> ci = gop.CamIf();
	*num = ci.size();
	auto i = 0;
	for(auto pair:ci) {
		pair.first.copy(str[i].str, pair.first.size() + 1);
		str[i].camif = pair.second;
		i++;
	}

	return D_CM_RESULT_OK;
}

int get_Ip_info(unsigned long type, unsigned long* ip)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	switch (type) {
	case E_HLC:
		*ip = capa->IpSet().GetHlc();
		break;
	case E_BACKLIGHT:
		*ip = capa->IpSet().GetBacklight();
		break;
	case E_DEFOG:
		*ip = capa->IpSet().GetDefog();
		break;
	case E_TA:
		*ip = capa->IpSet().GetTa();
		break;
	case E_OD:
		*ip = capa->IpSet().GetOd();
		break;
	case E_FD:
		*ip = capa->IpSet().GetFd();
		break;
	case E_FR:
		*ip = capa->IpSet().GetFr();
		break;
	case E_EIS:
		*ip = capa->IpSet().GetEis();
		break;
	case E_METADATA:
		*ip = capa->IpSet().GetMetadata();
		break;
	default :
		return D_CM_RESULT_NG;
	}

	return D_CM_RESULT_OK;
}

int get_Audio_info(unsigned long type, unsigned long* num, struct cameraif_string* info)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	CCapaBaseCamIf audio;
	switch (type) {
	case E_CODEC:
		audio = capa->Audio().Codec();
		break;
	case E_SAMPLING:
		audio = capa->Audio().SampRate();
		break;
	case E_MIC:
		audio = capa->Audio().Mic();
		break;
	case E_CH_TYPE:
		audio = capa->Audio().ChType();
		break;
	default :
		return D_CM_RESULT_NG;
	}

	std::map<std::string, uint32_t> ci = audio.CamIf();
	*num = ci.size();
	auto i = 0;
	for(auto pair:ci) {
		pair.first.copy(info[i].str, pair.first.size() + 1);
		info[i].camif = pair.second;
		i++;
	}

	return D_CM_RESULT_OK;
}

int get_Scan_mode_info(unsigned long* flag, unsigned long* num, struct cameraif_string* info)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	CCapaBaseCamIf scmode = capa->ScanMode();

	std::map<std::string, uint32_t> ci = scmode.CamIf();
	*num = ci.size();
	auto i = 0;
	for(auto pair:ci) {
		pair.first.copy(info[i].str, pair.first.size() + 1);
		info[i].camif = pair.second;
		i++;
	}

	return D_CM_RESULT_OK;
}

int get_Ae_info(struct ae_info* ae)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	ae->manual = capa->Ae().GetManualMode();
	ae->at = capa->Ae().GetAutoMode();
	ae->shutter = capa->Ae().GetShutter();
	ae->aperture = capa->Ae().GetAperture();
	ae->control = capa->Ae().GetAeControl();

	return D_CM_RESULT_OK;
}

int get_Expo_time_info(unsigned long* num, struct expo_time_info* expotime)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::vector<SCapaExpoTimeVal> ept = capa->ExpTime().expoTime();
	*num = ept.size();

	auto i = 0;
	for (auto eitr = ept.begin(); eitr != ept.end(); ++eitr) {
		expotime[i].camif = eitr->GetCameraIf();
		expotime[i].numerator = eitr->GetNumerator();
		expotime[i].denominator = eitr->GetDenominator();
		i++;
	}

	return D_CM_RESULT_OK;
}

int get_Focus_info(struct range_info range, unsigned long* at)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	*at = capa->Focus().GetAutoCtl();

	return D_CM_RESULT_OK;
}

int get_Priv_mask_info(unsigned long type, unsigned long* num, struct cameraif_string* info)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::map<std::string, uint32_t> pm;
	if (type == E_SENSOR_SET)
		pm = capa->PriMask().Sensor().Sensor();
	else
		pm = capa->PriMask().AreaNo().AreaNo();

	*num = pm.size();
	auto i = 0;
	for(auto pair:pm) {
		pair.first.copy(info[i].str, pair.first.size() + 1);
		info[i].camif = pair.second;
		i++;
	}

	return D_CM_RESULT_OK;
}

int get_Hue_info(unsigned long* at)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	*at = capa->Hue().GetHueAuto();

	return D_CM_RESULT_OK;
}

int get_SharpEdge_info(unsigned long type, unsigned long* num, struct cameraif_number* info)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::map<uint32_t, uint32_t> se;
	if (type == E_SHARPNESS_SET)
		se = capa->SharpEdge().Sharp().Sharpness();
	else
		se = capa->SharpEdge().Edge().Edge();

	*num = se.size();
	auto i = 0;
	for(auto pair:se) {
		info[i].num = pair.first;
		info[i].camif = pair.second;
		i++;
	}

	return D_CM_RESULT_OK;
}

int get_Wb_component_info(unsigned long* num, struct cameraif_string* info)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::map<std::string, uint32_t> w = capa->WB().Component();

	*num = w.size();
	auto i = 0;
	for(auto pair:w) {
		pair.first.copy(info[i].str, pair.first.size() + 1);
		info[i].camif = pair.second;
		i++;
	}

	return D_CM_RESULT_OK;
}

int get_Time_lapse_info(unsigned long* num, struct cameraif_number* info)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::map<uint32_t, uint32_t> tl = capa->TimeLap().Interval();

	*num = tl.size();
	auto i = 0;
	for(auto pair:tl) {
		info[i].num = pair.first;
		info[i].camif = pair.second;
		i++;
	}

	return D_CM_RESULT_OK;
}

int get_Osd_info(unsigned long* num, struct cameraif_number* tran, unsigned long* enab)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::map<uint32_t, uint32_t> tp = capa->Osd().TraPer();

	*num = tp.size();
	auto i = 0;
	for(auto pair:tp) {
		tran[i].num = pair.first;
		tran[i].camif = pair.second;
		i++;
	}

	*enab = capa->Osd().GetEnabled();

	return D_CM_RESULT_OK;
}

int get_Osd_detail_info(unsigned long type, unsigned long* num, struct cameraif_string* info)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	std::map<std::string, uint32_t> od;
	switch (type) {
	case E_SENSOR_SET:
		od = capa->Osd().Sensor().Sensor();
		break;
	case E_AREA_NO_SET:
		od = capa->Osd().AreaNo().AreaNo();
		break;
	case E_DSP_CTL_SENSOR_SET:
		od = capa->Osd().DCtlSensor().Sensor();
		break;
	case E_DSP_CTL_AREA_NO_SET:
		od = capa->Osd().DCtlAreaNo().AreaNo();
		break;
	default :
		return D_CM_RESULT_NG;
	}

	*num = od.size();
	auto i = 0;
	for(auto pair:od) {
		pair.first.copy(info[i].str, pair.first.size() + 1);
		info[i].camif = pair.second;
		i++;
	}

	return D_CM_RESULT_OK;
}

int get_Storage_info(unsigned long* save, unsigned long* def)
{
	CCapabilityInfo* capa = CCapabilityInfo::GetInstance();
	if (capa == NULL)
		return D_CM_RESULT_NG;

	*save = capa->Storage().GetSave();
	*def = capa->Storage().GetDefault();

	return D_CM_RESULT_OK;
}

