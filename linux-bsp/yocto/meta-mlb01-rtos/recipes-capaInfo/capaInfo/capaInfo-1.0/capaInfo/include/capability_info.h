#ifndef CAPABILITY_INFO_H_
#define CAPABILITY_INFO_H_
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <exception>
#include <bitset>
#include "capability_std.h"

#define EXCEPTION(message) CException(message, __FILE__, __FUNCTION__, __LINE__)

#define CAPA_PRINT(...)
#define CAPA_PRINT_INFO(...) {printf("[%s:%d] info: ",__FILE__,__LINE__);printf(__VA_ARGS__);}
#define CAPA_PRINT_ERR(...) {printf("[%s:%d] err: ",__FILE__,__LINE__);printf(__VA_ARGS__);}

//Channel Type
typedef enum {
	E_CHANNEL_TYPE_MAIN,		/* Main  */
	E_CHANNEL_TYPE_SUB1,		/* Sub-1 */
	E_CHANNEL_TYPE_SUB2,		/* Sub-2 */
	E_CHANNEL_TYPE_SUB3,		/* Sub-3 */
} E_CHANNEL_TYPE;

typedef enum {
	E_SENSOR_4SENSORS,
	E_SENSOR_DUAL,
	E_SENSOR_SINGLE_DOL_8M,
	E_SENSOR_SINGLE_DOL_5M,
	E_SENSOR_IMX277,
	E_SENSOR_IMX377,
	E_SENSOR_IMX477,
} E_SENSOR_TYPE_SET;


class CException : public std::exception {
public:
	CException(const std::string& aMessage);
	CException(const std::string& aMessage,
		const char* aFile,
		const char* aFuncion,
		const int aLine);
	virtual ~CException() throw();
	const char* getFileName() const;
	const char* getFunctionName() const;
	const int getLineNumber() const;
	virtual const char* what() const throw();

private:
	std::string message_;
	const char* fileName_;
	const char* functionName_;
	int line_;
};

class CCapBaseSize
{
public:
	CCapBaseSize() { cameraif_ = 0; width_ = 0; height_ = 0; }
	virtual ~CCapBaseSize() {}

	void setCameraIf(uint32_t cameraif) { cameraif_ = cameraif; }
	void setSize(uint32_t width, uint32_t height) {
			width_ = width;
			height_ = height; }
	uint32_t getCameraIf() const { return cameraif_; }
	uint32_t getSizeWidth() const { return width_; }
	uint32_t getSizeHeight() const { return height_; }

private:
	uint32_t cameraif_;
	uint32_t width_;
	uint32_t height_;
};

class CCapBaseRange
{
public:
	CCapBaseRange() {
		min_ = 0;
		max_ = 0;
		def_ = 0;
		res_ = 0;
	}
	virtual ~CCapBaseRange() {}

	void SetRange(uint32_t min, uint32_t max, uint32_t def, uint32_t res) {
		min_ = min;
		max_ = max;
		def_ = def;
		res_ = res;
	}
	uint32_t GetMin() const { return min_; }
	uint32_t GetMax() const { return max_; }
	uint32_t GetDef() const { return def_; }
	uint32_t GetRes() const { return res_; }

private:
	uint32_t min_;
	uint32_t max_;
	uint32_t def_;
	uint32_t res_;
};

class CCapaBaseCamIf : public CCapBaseRange
{
public:
	CCapaBaseCamIf() {}
	virtual ~CCapaBaseCamIf() {}

	std::map<std::string, uint32_t>& CamIf() { return camIf_; }

private:
	std::map<std::string, uint32_t> camIf_;
};

class CCapaBaseSup : public CCapBaseRange
{
public:
	CCapaBaseSup() { support_ = 0; }
	virtual ~CCapaBaseSup() {}

	void SetSupport(uint32_t support) { support_ = support; }
	uint32_t GetSupport() const { return support_; }

private:
	uint32_t support_;
};

class CCapVideoChannel
{
public:
	CCapVideoChannel() {}
	virtual ~CCapVideoChannel() {}

	void SetChannel(uint32_t channel) { channel_ = channel; }
	void SetSdSave(uint32_t sdSave) { sdSave_ = sdSave; }
	void SetIspOpt(uint32_t ispOpt) { ispOpt_ = ispOpt; }
	uint32_t GetChannel() const { return channel_; }
	uint32_t GetSdSave() const { return sdSave_; }
	uint32_t GetIspOpt() const { return ispOpt_; }
	std::map<std::string, uint32_t>& Codec() { return codec_; }
	std::map<uint32_t, uint32_t>& FrameRate() { return frameRate_; }
	std::map<std::string, CCapBaseSize>& Size() { return size_; }

private:
	uint32_t channel_;
	std::map<std::string, uint32_t> codec_;
	uint32_t sdSave_;
	uint32_t ispOpt_;
	std::map<uint32_t, uint32_t> frameRate_;
	std::map<std::string, CCapBaseSize> size_;
};

class CCapaCamSensor
{
public:
	CCapaCamSensor() {}
	virtual ~CCapaCamSensor() {}

	void SetSensorType(uint32_t sensorType) { sensorType_ = sensorType; }
	uint32_t GetSensorType() const { return sensorType_; }
	std::vector<CCapVideoChannel>& VideoCh() { return videoCh_; }
	std::map<std::string, uint32_t>& StitchingType() { return stitchingType_; }
	std::map<std::string, uint32_t>& DolMode() { return dolMode_; }
	bool MakeVideoChannel(uint32_t chType, uint32_t mode);
	void DeleteVideoChannel() { videoCh_.clear(); };

private:
	uint32_t sensorType_;
	std::vector<CCapVideoChannel> videoCh_;
	std::map<std::string, uint32_t> stitchingType_;
	std::map<std::string, uint32_t> dolMode_;
	void* addPlus(void* add, uint32_t offset)
	{
		char* p = static_cast<char*>(add);
		return static_cast<void*>(p + offset);
	}
};

class CCapaBitrate
{
public:
	CCapaBitrate() {}
	virtual ~CCapaBitrate() {}

	void SetAlgorithm(uint32_t algorithm) { algorithm_ = algorithm; }
	void SetBitrateCbr(uint32_t min, uint32_t max, uint32_t average, uint32_t res) {
		cbrMin_ = min;
		cbrMax_ = max;
		cbrAverage_ = average;
		cbrRes_ = res;
	}
	void SetBitrateVbr(uint32_t min, uint32_t max, uint32_t average, uint32_t res) {
		vbrMin_ = min;
		vbrMax_ = max;
		vbrAverage_ = average;
		vbrRes_ = res;
	}
	std::bitset<2>& GetAlgorithm() { return algorithm_; }
	uint32_t GetCbrMin() const { return cbrMin_; }
	uint32_t GetCbrMax() const { return cbrMax_; }
	uint32_t GetCbrAverage() const { return cbrAverage_; }
	uint32_t GetCbrRes() const { return cbrRes_; }
	uint32_t GetVbrMin() const { return vbrMin_; }
	uint32_t GetVbrMax() const { return vbrMax_; }
	uint32_t GetVbrAverage() const { return vbrAverage_; }
	uint32_t GetVbrRes() const { return vbrRes_; }

private:
	std::bitset<2> algorithm_;
	uint32_t cbrMin_;
	uint32_t cbrMax_;
	uint32_t cbrAverage_;
	uint32_t cbrRes_;
	uint32_t vbrMin_;
	uint32_t vbrMax_;
	uint32_t vbrAverage_;
	uint32_t vbrRes_;
};

class CCapaGop
{
public:
	CCapaGop() {}
	virtual ~CCapaGop() {}

	CCapaBaseCamIf& GopStruct() { return gopStruct_; }
	CCapaBaseCamIf& VideoStrFmt() { return videoStrageFormat_; }

private:
	CCapaBaseCamIf gopStruct_;
	CCapaBaseCamIf videoStrageFormat_;
};

class CCapaIpSet
{
public:
	CCapaIpSet() {}
	virtual ~CCapaIpSet() {}

	void SetHlc(uint32_t hlc) { hlc_ = hlc; }
	void SetBacklight(uint32_t backlight) { backlight_ = backlight; }
	void SetDefog(uint32_t defog) { defog_ = defog; }
	void SetTa(uint32_t ta) { ta_ = ta; }
	void SetOd(uint32_t od) { od_ = od; }
	void SetFd(uint32_t fd) { fd_ = fd; }
	void SetFr(uint32_t fr) { fr_ = fr; }
	void SetEis(uint32_t eis) { eis_ = eis; }
	void SetMetadata(uint32_t metadata) { metadata_ = metadata; }
	uint32_t GetHlc() const { return hlc_; }
	uint32_t GetBacklight() const { return backlight_; }
	uint32_t GetDefog() const { return defog_; }
	uint32_t GetTa() const { return ta_; }
	uint32_t GetOd() const { return od_; }
	uint32_t GetFd() const { return fd_; }
	uint32_t GetFr() const { return fr_; }
	uint32_t GetEis() const { return eis_; }
	uint32_t GetMetadata() const { return metadata_; }

private:
	uint32_t hlc_;
	uint32_t backlight_;
	uint32_t defog_;
	uint32_t ta_;
	uint32_t od_;
	uint32_t fd_;
	uint32_t fr_;
	uint32_t eis_;
	uint32_t metadata_;
};

class CCapaAudio
{
public:
	CCapaAudio() { support_ = 0; }
	virtual ~CCapaAudio() {}

	void SetAudioSupport(uint32_t support) { support_ = support; }
	uint32_t GetAudioSupport() const { return support_; }
	CCapaBaseCamIf& Codec() { return codec_; }
	CCapaBaseCamIf& SampRate() { return sampRate_; }
	CCapaBaseCamIf& Mic() { return mic_; }
	CCapBaseRange& Rate() { return rate_; }
	CCapaBaseCamIf& ChType() { return chType_; }

private:
	uint32_t support_;
	CCapaBaseCamIf codec_;
	CCapaBaseCamIf sampRate_;
	CCapaBaseCamIf mic_;
	CCapBaseRange rate_;
	CCapaBaseCamIf chType_;
};

class CCapaScanMode : public CCapaBaseCamIf
{
public:
	CCapaScanMode() { support_ = 0; }
	virtual ~CCapaScanMode() {}

	void SetSupport(uint32_t support) { support_ = support; }
	uint32_t GetSupport() const { return support_; }

private:
	uint32_t support_;
};

class CCapaAe
{
public:
	CCapaAe() { support_ = 0; }
	virtual ~CCapaAe() {}

	void SetSupport(uint32_t support) { support_ = support; }
	void SetManualMode(uint32_t manualMode) { manualMode_ = manualMode; }
	void SetAutoMode(uint32_t autoMode) { autoMode_ = autoMode; }
	void SetShutter(uint32_t shutter) { shutter_ = shutter; }
	void SetAperture(uint32_t aperture) { aperture_ = aperture; }
	void SetAeControl(uint32_t aeControl) { aeControl_ = aeControl; }
	uint32_t GetSupport() const { return support_; }
	uint32_t GetManualMode() const { return manualMode_; }
	uint32_t GetAutoMode() const { return autoMode_; }
	uint32_t GetShutter() const { return shutter_; }
	uint32_t GetAperture() const { return aperture_; }
	uint32_t GetAeControl() const { return aeControl_; }

private:
	uint32_t support_;
	uint32_t manualMode_;
	uint32_t autoMode_;
	uint32_t shutter_;
	uint32_t aperture_;
	uint32_t aeControl_;
};

class SCapaExpoTimeVal
{
public:
	SCapaExpoTimeVal() {}
	virtual ~SCapaExpoTimeVal() {}

	void SetCameraIf(uint32_t cameraIf) { cameraIf_ = cameraIf; }
	void SetNumerator(uint32_t numerator) { numerator_ = numerator; }
	void SetDenominator(uint32_t denominator) { denominator_ = denominator; }
	uint32_t GetCameraIf() const { return cameraIf_; }
	uint32_t GetNumerator() const { return numerator_; }
	uint32_t GetDenominator() const { return denominator_; }

private:
	uint32_t cameraIf_;
	uint32_t numerator_;
	uint32_t denominator_;
};

class CCapaExpoTime : public CCapBaseRange
{
public:
	CCapaExpoTime() { support_ = 0; }
	virtual ~CCapaExpoTime() {}

	void SetSupport(uint32_t support) { support_ = support; }
	uint32_t GetSupport() const { return support_; }
	std::vector<SCapaExpoTimeVal>& expoTime() { return pram_; }

private:
	uint32_t support_;
	std::vector<SCapaExpoTimeVal> pram_;
};

class CCapaFocus : public CCapBaseRange
{
public:
	CCapaFocus() { support_ = 0; }
	virtual ~CCapaFocus() {}

	void SetSupport(uint32_t support) { support_ = support; }
	void SetAutoCtl(uint32_t autoCtl) { autoCtl_ = autoCtl; }
	uint32_t GetSupport() const { return support_; }
	uint32_t GetAutoCtl() const { return autoCtl_; }

private:
	uint32_t support_;
	uint32_t autoCtl_;
};

class CCapaSensor : public CCapBaseRange
{
public:
	CCapaSensor() {}
	virtual ~CCapaSensor() {}

	std::map<std::string, uint32_t>& Sensor() { return sensor_; }

private:
	std::map<std::string, uint32_t> sensor_;
};

class CCapaAreaNo : public CCapBaseRange
{
public:
	CCapaAreaNo() {}
	virtual ~CCapaAreaNo() {}

	std::map<std::string, uint32_t>& AreaNo() { return areaNo_; }

private:
	std::map<std::string, uint32_t> areaNo_;
};

class CCapaPrivacyMask
{
public:
	CCapaPrivacyMask() { support_ = 0; }
	virtual ~CCapaPrivacyMask() {}

	void SetSupport(uint32_t support) { support_ = support; }
	uint32_t GetSupport() const { return support_; }
	CCapaSensor& Sensor() { return sensor_; }
	CCapaAreaNo& AreaNo() { return areaNo_; }
	CCapBaseRange& XCoordinate() { return xCoordinate_; }
	CCapBaseRange& YCoordinate() { return yCoordinate_; }
	CCapBaseRange& Width() { return width_; }
	CCapBaseRange& Lines() { return lines_; }
	CCapBaseRange& Y() { return y_; }
	CCapBaseRange& Cb() { return cb_; }
	CCapBaseRange& Cr() { return cr_; }

private:
	uint32_t support_;
	CCapaSensor sensor_;
	CCapaAreaNo areaNo_;
	CCapBaseRange xCoordinate_;
	CCapBaseRange yCoordinate_;
	CCapBaseRange width_;
	CCapBaseRange lines_;
	CCapBaseRange y_;
	CCapBaseRange cb_;
	CCapBaseRange cr_;
};

class CCapaHue : public CCapaBaseSup
{
public:
	CCapaHue() {}
	virtual ~CCapaHue() {}

	void SetHueAuto(uint32_t hueAuto) { hueAuto_ = hueAuto; }
	uint32_t GetHueAuto() const { return hueAuto_; }

private:
	uint32_t hueAuto_;
};

class CCapaSharpness : public CCapBaseRange
{
public:
	CCapaSharpness() {}
	virtual ~CCapaSharpness() {}

	std::map<uint32_t, uint32_t>& Sharpness() { return sharpness_; }

private:
	std::map<uint32_t, uint32_t> sharpness_;
};

class CCapaEdge : public CCapBaseRange
{
public:
	CCapaEdge() {}
	virtual ~CCapaEdge() {}

	std::map<uint32_t, uint32_t>& Edge() { return edge_; }

private:
	std::map<uint32_t, uint32_t> edge_;
};

class CCapaSharpnessEdge
{
public:
	CCapaSharpnessEdge() { support_ = 0; }
	virtual ~CCapaSharpnessEdge() {}

	void SetSupport(uint32_t support) { support_ = support; }
	uint32_t GetSupport() const { return support_; }
	CCapaSharpness& Sharp() { return sharp_; }
	CCapaEdge& Edge() { return edge_; }

private:
	uint32_t support_;
	CCapaSharpness sharp_;
	CCapaEdge edge_;
};

class CCapaWhiteBalance
{
public:
	CCapaWhiteBalance() { support_ = 0; }
	virtual ~CCapaWhiteBalance() {}

	void SetSupport(uint32_t support) { support_ = support; }
	void SetTempSupport(uint32_t tempSupport) { tempSupport_ = tempSupport; }
	void SetCompSupport(uint32_t compSupport) { compSupport_ = compSupport; }
	uint32_t GetSupport() const { return support_; }
	uint32_t GetTempSupport() const { return tempSupport_; }
	uint32_t GetCompSupport() const { return compSupport_; }
	std::map<std::string, uint32_t>& Component() { return component_; }
	CCapBaseRange& Temp() { return temp_; }
	CCapBaseRange& Comp() { return comp_; }
	CCapBaseRange& CompRGain() { return compRGain_; }
	CCapBaseRange& CompBGain() { return compBGain_; }

private:
	uint32_t support_;
	uint32_t tempSupport_;
	CCapBaseRange temp_;
	uint32_t compSupport_;
	std::map<std::string, uint32_t> component_;
	CCapBaseRange comp_;
	CCapBaseRange compRGain_;
	CCapBaseRange compBGain_;
};

class CCapaAnalogVideo
{
public:
	CCapaAnalogVideo() { support_ = 0; }
	virtual ~CCapaAnalogVideo() {}

	void SetSupport(uint32_t support) { support_ = support; }
	uint32_t GetSupport() const { return support_; }
	CCapBaseRange& Standard() { return standard_; }
	CCapBaseRange& LockStatus() { return lockStatus_; }

private:
	uint32_t support_;
	CCapBaseRange standard_;
	CCapBaseRange lockStatus_;
};

class CCapaTimeLapse : public CCapBaseRange
{
public:
	CCapaTimeLapse() { support_ = 0; }
	virtual ~CCapaTimeLapse() {}

	void SetSupport(uint32_t support) { support_ = support; }
	uint32_t GetSupport() const { return support_; }
	std::map<uint32_t, uint32_t>& Interval() { return interval_; }

private:
	uint32_t support_;
	std::map<uint32_t, uint32_t> interval_;
};

class CCapaOsd : public CCapBaseRange
{
public:
	CCapaOsd() { support_ = 0; }
	virtual ~CCapaOsd() {}

	void SetSupport(uint32_t support) { support_ = support; }
	void SetEnabled(uint32_t enabled) { enabled_ = enabled; }
	uint32_t GetSupport() const { return support_; }
	std::map<uint32_t, uint32_t>& TraPer() { return traPer_; }
	CCapaSensor& Sensor() { return sensor_; }
	CCapaAreaNo& AreaNo() { return areaNo_; }
	CCapBaseRange& XCoord() { return xCoordinate_; }
	CCapBaseRange& YCoord() { return yCoordinate_; }
	CCapaSensor& DCtlSensor() { return dCtlSensor_; }
	CCapaAreaNo& DCtlAreaNo() { return dCtlAreaNo_; }
	uint32_t GetEnabled() const { return enabled_; }

private:
	uint32_t support_;
	std::map<uint32_t, uint32_t> traPer_;
	CCapaSensor sensor_;
	CCapaAreaNo areaNo_;
	CCapBaseRange xCoordinate_;
	CCapBaseRange yCoordinate_;
	CCapaSensor dCtlSensor_;
	CCapaAreaNo dCtlAreaNo_;
	uint32_t enabled_;
};

class CCapaStorage
{
public:
	CCapaStorage() { support_ = 0; }
	virtual ~CCapaStorage() {}

	void SetSupport(uint32_t support) { support_ = support; }
	void SetSave(uint32_t save) { save_ = save; }
	void SetDefault(uint32_t def) { def_ = def; }
	uint32_t GetSupport() const { return support_; }
	uint32_t GetSave() const { return save_; }
	uint32_t GetDefault() const { return def_; }

private:
	uint32_t support_;
	uint32_t save_;
	uint32_t def_;
};

class CCapabilityInfo
{
public:
	static CCapabilityInfo* GetInstance();
	static void DeleteInstance();
	bool FirstSetting();
	static bool CameraIfExec(uint32_t cmd);
	static bool CameraIfExec(uint32_t cmd, uint32_t para, uint32_t mode);
	CCapaCamSensor& CamSensor() { return camSensor_; }
	CCapaBitrate& Bitrate() { return bitrate_; }
	CCapaGop& Gop() { return gop_; }
	CCapaIpSet& IpSet() { return ipSet_; }
	CCapaAudio& Audio() { return audio_; }
	CCapaScanMode& ScanMode() { return scanMode_; }
	CCapaAe& Ae() { return ae_; }
	CCapaExpoTime& ExpTime() { return expTime_; }
	CCapaFocus& Focus() { return focus_; }
	CCapaBaseSup& Iris() { return iris_; }
	CCapaBaseSup& Zoom() { return zoom_; }
	CCapaBaseSup& Pan() { return pan_; }
	CCapaBaseSup& Tilt() { return tilt_; }
	CCapaBaseSup& Roll() { return roll_; }
	CCapaPrivacyMask& PriMask() { return priMask_; }
	CCapaBaseSup Bright() { return bright_; }
	CCapaBaseSup Contrast() { return contrast_; }
	CCapaHue& Hue() { return hue_; }
	CCapaBaseSup Saturation() { return saturation_; }
	CCapaSharpnessEdge& SharpEdge() { return sharpEdge_; }
	CCapaBaseSup Gamma() { return gamma_; }
	CCapaBaseSup DigiMulti() { return digiMulti_; }
	CCapaWhiteBalance& WB() { return wB_; }
	CCapaBaseSup Backlight() { return backlight_; }
	CCapaBaseSup Gain() { return gain_; }
	CCapaBaseSup PowerLine() { return powerLine_; }
	CCapaAnalogVideo& AVideo() { return aVideo_; }
	CCapaTimeLapse& TimeLap() { return timeLap_; }
	CCapaOsd& Osd() { return osd_; }
	CCapaStorage& Storage() { return storage_; }

	bool MakeVideoChannel(uint32_t chType, uint32_t mode)
	{
		return camSensor_.MakeVideoChannel(chType, mode);
	}
	void DeleteVideoChannel()
	{
		camSensor_.DeleteVideoChannel();
	}

private:
	CCapaCamSensor camSensor_;
	CCapaBitrate bitrate_;
	CCapaGop gop_;
	CCapaIpSet ipSet_;
	CCapaAudio audio_;
	CCapaScanMode scanMode_;
	CCapaAe ae_;
	CCapaExpoTime expTime_;
	CCapaFocus focus_;
	CCapaBaseSup iris_;
	CCapaBaseSup zoom_;
	CCapaBaseSup pan_;
	CCapaBaseSup tilt_;
	CCapaBaseSup roll_;
	CCapaPrivacyMask priMask_;
	CCapaBaseSup bright_;
	CCapaBaseSup contrast_;
	CCapaHue hue_;
	CCapaBaseSup saturation_;
	CCapaSharpnessEdge sharpEdge_;
	CCapaBaseSup gamma_;
	CCapaBaseSup digiMulti_;
	CCapaWhiteBalance wB_;
	CCapaBaseSup backlight_;
	CCapaBaseSup gain_;
	CCapaBaseSup powerLine_;
	CCapaAnalogVideo aVideo_;
	CCapaTimeLapse timeLap_;
	CCapaOsd osd_;
	CCapaStorage storage_;

	CCapabilityInfo() {}
	virtual ~CCapabilityInfo() {}
	static CCapabilityInfo* instance_;

	void* addPlus(void* add, uint32_t offset)
	{
		char* p = static_cast<char*>(add);
		return static_cast<void*>(p + offset);
	}
};

#endif /* CAPABILITY_INFO_H_ */

