/**
 * @file		ChipTest.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#define PTP_CT_FJ_GpioGroupSelect			(0x9001)
#define PTP_CT_FJ_GpioGroupConfig			(0x9002)
#define PTP_CT_FJ_GpioGroupSet				(0x9003)
#define PTP_CT_FJ_GpioGroupGet				(0x9004)
#define PTP_CT_FJ_ConfigButtonDrvEx 		(0x9005)
#define PTP_CT_FJ_AdcRead					(0x9006)

#define PTP_CT_FJ_GPIO_MotorSet			(0x9008)
#define PTP_CT_FJ_SetMShutterController		(0x900A)
#define PTP_CT_FJ_GetMShutterController		(0x900B)
#define PTP_CT_FJ_MShutterOpen				(0x900C)
#define PTP_CT_FJ_MShutterClose				(0x900D)
#define PTP_CT_FJ_ExtInterruptConfigSet		(0x900E)
#define PTP_CT_FJ_ExtInterruptStateSet		(0x900F)

#define PTP_CT_FJ_MountStorageMedia		(0x9010)
#define PTP_CT_FJ_FormatStorageMedia		(0x9011)
#define PTP_CT_FJ_GetStorageMediaStatus		(0x9012)

#define PTP_CT_FJ_SetMode					(0x9020)
#define PTP_CT_FJ_QueryStatus				(0x9021)
#define PTP_CT_FJ_SetParam					(0x9022)
#define PTP_CT_FJ_SetTimer					(0x9023)
#define PTP_CT_FJ_HostTimerInterrupt		(0x9024)
#define PTP_CT_FJ_EnableRTC					(0x9025)
#define PTP_CT_FJ_GetClock					(0x9026)
#define PTP_CT_FJ_SetClock					(0x9027)
#define PTP_CT_FJ_HostProc					(0x9028)
#define PTP_CT_FJ_HostPreBootProc			(0x9029)
#define PTP_CT_FJ_HostGetMessage			(0x902A)
#define PTP_CT_FJ_HostPostMessage			(0x902B)
#define PTP_CT_FJ_PowerOff					(0x902C)
#define PTP_CT_FJ_OsThreadCreateEx			(0x902D)
#define PTP_CT_FJ_OsThreadTerminate		(0x902E)
#define PTP_CT_FJ_OsThreadDelete			(0x902F)

#define PTP_CT_FJ_OsThreadSleepMS			(0x9030)
#define PTP_CT_FJ_OsMutexCreate			(0x9031)
#define PTP_CT_FJ_OsMutexGet				(0x9032)
#define PTP_CT_FJ_OsMutexPut				(0x9033)
#define PTP_CT_FJ_OsMutexDelete				(0x9034)
#define PTP_CT_FJ_OsEventFlagsCreate		(0x9035)
#define PTP_CT_FJ_OsEventFlagsGet			(0x9036)
#define PTP_CT_FJ_OsEventFlagsSet			(0x9037)
#define PTP_CT_FJ_OsEventFlagsDelete		(0x9038)
#define PTP_CT_FJ_DisplayWindow			(0x9039)
#define PTP_CT_FJ_SetSensorWindow			(0x903A)
#define PTP_CT_FJ_GetSensorWindow			(0x903B)
#define PTP_CT_FJ_ZoomFactorSet			(0x903C)
#define PTP_CT_FJ_ConfigNin1Mode			(0x903D)
#define PTP_CT_FJ_HostViewYUVManipulation	(0x903E)
#define PTP_CT_FJ_HostViewDramManipulation	(0x903F)

#define PTP_CT_FJ_SetImageSize				(0x9040)
#define PTP_CT_FJ_SetCompressionRatio		(0x9041)
#define PTP_CT_FJ_ConfigCapture				(0x9042)
//#define PTP_CT_FJ_SetParam					(0x9043)
#define PTP_CT_FJ_ConfirmStore				(0x9044)
#define PTP_CT_FJ_SetSequenceLength		(0x9045)
#define PTP_CT_FJ_GetSequenceLength		(0x9046)
#define PTP_CT_FJ_GetCaptureStatus			(0x9047)
#define PTP_CT_FJ_PipAddPhotoFrame			(0x9048)
#define PTP_CT_FJ_DirectScaleYUV			(0x9049)
#define PTP_CT_FJ_DirectSaveYUV				(0x904A)
#define PTP_CT_FJ_HostCaptureDramManipulation		(0x904B)
#define PTP_CT_FJ_HostCaptureYUVManipulation		(0x904C)
#define PTP_CT_FJ_HostCaptureJpgManipulation		(0x904D)
#define PTP_CT_FJ_HostCaptureSequenceCB			(0x904E)
#define PTP_CT_FJ_HostExiffUpdateCB					(0x904F)

#define PTP_CT_FJ_HostDcfCreateFileCB				(0x9050)
#define PTP_CT_FJ_HostCreateMakerNote				(0x9051)
#define PTP_CT_FJ_HostParseMakerNote				(0x9052)
#define PTP_CT_FJ_DirectFsFileOpen					(0x9053)
#define PTP_CT_FJ_DirectFsFileClose					(0x9054)
#define PTP_CT_FJ_DirectFsFileRead					(0x9055)
#define PTP_CT_FJ_DirectFsFileWrite					(0x9056)
#define PTP_CT_FJ_DirectFsFileSeek					(0x9057)
#define PTP_CT_FJ_DirectFsFileRelSeek				(0x9058)
#define PTP_CT_FJ_DirectFsFileGetLine				(0x9059)
#define PTP_CT_FJ_DirectFsDelete						(0x905A)
#define PTP_CT_FJ_DirectFsDirectoryCreate			(0x905B)
#define PTP_CT_FJ_DirectFsDirectoryChange			(0x905C)
#define PTP_CT_FJ_DirectFsRename					(0x905D)
#define PTP_CT_FJ_DirectFsDirectoryEntryGet			(0x905E)
#define PTP_CT_FJ_DirectFsDirectoryEntryGetEx		(0x905F)

#define PTP_CT_FJ_DirectFsAttributesRead			(0x9060)
#define PTP_CT_FJ_DirectLoadBitmap				(0x9061)
#define PTP_CT_FJ_DirectDestroyBitmap			(0x9062)
#define PTP_CT_FJ_DirectLoadFont				(0x9063)
#define PTP_CT_FJ_DirectSelectFont				(0x9064)
#define PTP_CT_FJ_DirectDestroyFont				(0x9065)
#define PTP_CT_FJ_DirectLoadWav				(0x9066)
#define PTP_CT_FJ_DirectPlayWav					(0x9067)
#define PTP_CT_FJ_DirectDestroyWav				(0x9068)
//#define PTP_CT_FJ_HM_WAV_OPERATION_COMPLETE	(0x9069)
#define PTP_CT_FJ_DirectPoolAlloc				(0x906A)
#define PTP_CT_FJ_DirectPoolFree					(0x906B)
#define PTP_CT_HceCbFuncTooManyBadBlocks		(0x906C)
#define PTP_CT_HceCbSystemDataError			(0x906D)
#define PTP_CT_FJ_GetMemoryStatusParam		(0x906E)
#define PTP_CT_FJ_SetCurrentImage				(0x906F)

#define PTP_CT_FJ_GetDCFInfo			(0x9070)
#define PTP_CT_FJ_SetFileType			(0x9071)
#define PTP_CT_FJ_GetFileAttributes		(0x9072)
#define PTP_CT_FJ_GetImageAttributes	(0x9073)
#define PTP_CT_FJ_GetAudioFileAttr		(0x9074)
#define PTP_CT_FJ_PlaybackZoom			(0x9075)
//#define PTP_CT_FJ_SetMode				(0x9076)
#define PTP_CT_FJ_ConfigThumbnail		(0x9077)
#define PTP_CT_FJ_SetGDA				(0x9078)
//#define PTP_CT_FJ_SetMode				(0x9079)
//#define PTP_CT_FJ_SetParam				(0x907A)
#define PTP_CT_FJ_FileOperation			(0x907B)
//										(0x907C)
//#define PTP_CT_FJ_SetParam				(0x907D)
#define PTP_CT_FJ_EditImage				(0x907E)
//#define PTP_CT_FJ_EditImage				(0x907F)

#define PTP_CT_FJ_GetParam				(0x9081)
//#define PTP_CT_FJ_EditImage				(0x9082)
//#define PTP_CT_FJ_EditImage				(0x9083)
//#define PTP_CT_FJ_EditImage				(0x9084)
//#define PTP_CT_FJ_EditImage				(0x9085)
//#define PTP_CT_FJ_PipAddPhotoFrame		(0x9086)
#define PTP_CT_FJ_SetFormatConvert		(0x9087)
//#define PTP_CT_FJ_EditImage				(0x9088)
#define PTP_CT_FJ_CaptureAudioMessage	(0x9089)
#define PTP_CT_FJ_DPOFInit				(0x908A)
#define PTP_CT_FJ_DPOFAddJob			(0x908B)
#define PTP_CT_FJ_DPOFRemoveJob		(0x908C)
#define PTP_CT_FJ_DPOFGetFirstJobInFile	(0x908D)
#define PTP_CT_FJ_DPOFGetNextJob		(0x908E)
#define PTP_CT_FJ_DPOFGetJobParamInt	(0x908F)

#define PTP_CT_FJ_DPOFGetSrcParam		(0x9090)
#define PTP_CT_FJ_DPOFGetImageData	(0x9091)
#define PTP_CT_FJ_DPOFSetJobParamInt	(0x9092)
#define PTP_CT_FJ_DPOFSetSrcParam		(0x9093)
#define PTP_CT_FJ_DPOFReadWriteFile		(0x9094)
#define PTP_CT_FJ_DPOFSetHeaderParam	(0x9095)
#define PTP_CT_FJ_DPOFRelease			(0x9096)
#define PTP_CT_FJ_PipLayerCreate		(0x9097)
#define PTP_CT_FJ_PipSetAttrib			(0x9098)
#define PTP_CT_FJ_PipModeActivate		(0x9099)
#define PTP_CT_FJ_PipLayerShow			(0x909A)
#define PTP_CT_FJ_PipLayerDestroy		(0x909B)
#define PTP_CT_FJ_Slideshow				(0x909C)
#define PTP_CT_FJ_SetImageEXIF			(0x909D)
#define PTP_CT_FJ_GetImageEXIF			(0x909E)
#define PTP_CT_FJ_SetMemLocationPtr		(0x909F)

//#define PTP_CT_FJ_SetParam				(0x90A0)
#define PTP_CT_FJ_ExiffUpdateCB			(0x90A1)
#define PTP_CT_FJ_CreateMakerNote		(0x90A2)
#define PTP_CT_FJ_ParseMakerNote		(0x90A3)
#define PTP_CT_FJ_CreateAPPn			(0x90A4)
#define PTP_CT_FJ_ParseAPPn			(0x90A5)
#define PTP_CT_FJ_SetDCFFreeChars		(0x90A6)
#define PTP_CT_FJ_SetNextDCFKey		(0x90A7)
#define PTP_CT_FJ_GetParamArray		(0x90A8)
#define PTP_CT_FJ_GetParamLong			(0x90A9)
#define PTP_CT_FJ_MovieCapCfg			(0x90AA)
//#define PTP_CT_FJ_DisplayWindow		(0x90AB)
#define PTP_CT_FJ_SensorWindow			(0x90AC)
//#define PTP_CT_FJ_SetMode				(0x90AD)
#define PTP_CT_FJ_CaptureClipCmd		(0x90AE)
#define PTP_CT_FJ_MoviePlayCfg			(0x90AF)

#define PTP_CT_FJ_PlaybackClipCmd			(0x90B0)
#define PTP_CT_FJ_MovieStillCapture			(0x90B1)
//#define PTP_CT_FJ_SetImageSize				(0x90B2)
#define PTP_CT_FJ_MovHeaderOperation		(0x90B3)
#define PTP_CT_FJ_AudioRecCfg				(0x90B4)
#define PTP_CT_FJ_Beep						(0x90B5)
#define PTP_CT_FJ_AudioParamSet			(0x90B6)
//#define PTP_CT_FJ_GetAudioFileAttr			(0x90B7)
#define PTP_CT_FJ_RecordPlayAudio			(0x90B8)
#define PTP_CT_FJ_RecordPlayAudioByName	(0x90B9)
#define PTP_CT_FJ_StopRecording				(0x90BA)
#define PTP_CT_FJ_DirectLoadWave			(0x90BB)
//#define PTP_CT_FJ_DirectPlayWav				(0x90BC)
//#define PTP_CT_FJ_DirectDestroyWav			(0x90BD)
#define PTP_CT_FJ_GetAudioHdrLength		(0x90BE)
#define PTP_CT_FJ_SetAudioHdrLength			(0x90BF)

#define PTP_CT_FJ_GetAudioHdrInfo	(0x90C0)
#define PTP_CT_FJ_SetAudioHdrInfo	(0x90C1)
#define PTP_CT_FJ_UART_GetChar		(0x90C2)
#define PTP_CT_FJ_UART_Printf		(0x90C3)
#define PTP_CT_FJ_CBUartMonitor		(0x90C4)
#define PTP_CT_FJ_OSDTextOutID		(0x90C5)
#define PTP_CT_FJ_OSDDrawShape		(0x90C6)
#define PTP_CT_FJ_DrawFile				(0x90C7)
#define PTP_CT_FJ_SetVideoMode		(0x90C8)
#define PTP_CT_FJ_SetOSDDisplayAttr	(0x90C9)
#define PTP_CT_FJ_SetOSDCLUTEx		(0x90CA)
#define PTP_CT_FJ_SetDispGamma		(0x90CB)
#define PTP_CT_FJ_SetDisplayMode		(0x90CC)
#define PTP_CT_FJ_OSDTextOut			(0x90CD)
#define PTP_CT_FJ_OSDDrawText			(0x90CE)
#define PTP_CT_FJ_OSDCreateBuffer		(0x90CF)
#define PTP_CT_FJ_SetLong				(0x90DB)
#define PTP_CT_FJ_GetLong				(0x90DC)
#define PTP_CT_FJ_GetFixedImage			(0x90DD)
#define PTP_CT_FJ_GetLastImage			(0x90DE)
#define PTP_CT_FJ_GetLastFileName		(0x90DF)
#define PTP_CT_FJ_SetFileName		(0x90E0)
#define PTP_CT_FJ_SendFixedImage	(0x90E1)

#define PTP_CT_FJ_DebugDataWrite	(0x90F0)
#define PTP_CT_FJ_DebugDataRead	(0x90F1)
#define PTP_CT_FJ_DebugBulkOut		(0x90F2)
//AE
#define PTP_CT_FJ_SetStatisticPeriod  					0x9100
#define PTP_CT_FJ_AESetMode  						0x9101
#define PTP_CT_FJ_AESetWindow 						0x9102
#define PTP_CT_FJ_AECCDCSetBlackLevelCompensation 0x9103
#define PTP_CT_FJ_AESensorSetOBClamp 				0x9104
#define PTP_CT_FJ_AESetComputeValueRatio 			0x9105
#define PTP_CT_FJ_AESetWeight 						0x9106
#define PTP_CT_FJ_AEGetComputeInfo 				0x9107		//NotYet
#define PTP_CT_FJ_SetPreviewExposureIdx			0x9108
#define PTP_CT_FJ_SetCaptureExposureIdx			0x9109
#define PTP_CT_FJ_GetPreviewExposureIdx			0x910A
#define PTP_CT_FJ_GetCaptureExposureIdx			0x910B
#define PTP_CT_FJ_SetPreviewAgcGainIdx				0x910C
#define PTP_CT_FJ_SetCaptureAgcGainIdx				0x910D
#define PTP_CT_FJ_GetPreviewAgcGainIdx				0x910E
#define PTP_CT_FJ_GetCaptureAgcGainIdx				0x910F
#define PTP_CT_FJ_SetPreviewExposureTime			0x9110
#define PTP_CT_FJ_SetCaptureExposureTime			0x9111
#define PTP_CT_FJ_GetPreviewExposureTime			0x9112
#define PTP_CT_FJ_GetCaptureExposureTime			0x9113
#define PTP_CT_FJ_SetPreviewAgcGain				0x9114		//p
#define PTP_CT_FJ_SetCaptureAgcGain					0x9115
#define PTP_CT_FJ_GetPreviewAgcGain				0x9116
#define PTP_CT_FJ_GetCaptureAgcGain				0x9117
#define PTP_CT_FJ_SetShutterClosePos				0x9118
#define PTP_CT_FJ_CBAEProcessOpen					0x9119
//AWB
#define PTP_CT_FJ_AWBSetMode					0x911A
#define PTP_CT_FJ_AWBGetComputeInfo			0x911B
#define PTP_CT_FJ_AWBSetLevelGate				0x911C
#define PTP_CT_FJ_AWBSetPreviewGain			0x911D
#define PTP_CT_FJ_AWBSetCaptureGain			0x911E
#define PTP_CT_FJ_AWBGetPreviewGain			0x911F
#define PTP_CT_FJ_AWBGetCaptureGain			0x9120
#define PTP_CT_FJ_CBAWBProcessOpen			0x9121
//AF
#define PTP_CT_FJ_AFSetMode					0x9122
#define PTP_CT_FJ_AESetWindow_Pos				0x9123
#define PTP_CT_FJ_AESetWindow_Size 			0x9124
#define PTP_CT_FJ_AESetCoring					0x9125
#define PTP_CT_FJ_AESet_HPF1					0x9126
#define PTP_CT_FJ_AESet_HPF2					0x9127
#define PTP_CT_FJ_AESet_FIR3					0x9128
#define PTP_CT_FJ_AESet_Gain					0x9129
#define PTP_CT_FJ_CBWindowAFReady			0x912A
#define PTP_CT_FJ_CBAFProcessOpen			0x912B
#define PTP_CT_FJ_SetStrobeTrigger			0x912C
#define PTP_CT_FJ_SetPreStrobe				0x912D
#define PTP_CT_FJ_SetMainStrobe				0x912E
#define PTP_CT_FJ_AESensorSendTgCommand			0x912F
#define PTP_CT_FJ_AESensorSendAfeCommand			0x9130
#define PTP_CT_FJ_AEGetPreviewRawDataSizde		0x9131
#define PTP_CT_FJ_AESetDeadPixelCorrection			0x9132
#define PTP_CT_FJ_AESetCaptureLSCCalibrationEnable			0x9133
#define PTP_CT_FJ_AEGetCaptureLSCCalibrationEnable			0x9134
#define PTP_CT_FJ_AESetCaptureLSCEnable			0x9135
#define PTP_CT_FJ_AEGetCaptureLSCEnable			0x9136
#define PTP_CT_FJ_AESetCaptureLSCTable			0x9137
#define PTP_CT_FJ_AEGetCaptureLSCTable			0x9138
#define PTP_CT_FJ_AEGetHistogram				0x9139

#define PTP_CT_FJ_Set2ToppolyTPG85			0x9200
#define PTP_CT_FJ_Set2ADDI9000				0x9201
#define PTP_CT_FJ_Lens_Open				0x9202
#define PTP_CT_FJ_Lens_Close				0x9203
#define PTP_CT_FJ_Lens_ZoomMove2Wide		0x9204
#define PTP_CT_FJ_Lens_ZoomMove2Tele		0x9205
#define PTP_CT_FJ_Lens_ZoomMoveByStep		0x9206
#define PTP_CT_FJ_Lens_FocusMove2CCW		0x9207
#define PTP_CT_FJ_Lens_FocusMove2CW		0x9208
#define PTP_CT_FJ_Lens_MShutterOpen		0x9209
#define PTP_CT_FJ_Lens_MShutterClose		0x920A
#define PTP_CT_FJ_Lens_NDOpen				0x920B
#define PTP_CT_FJ_Lens_NDClose				0x920C

//#define USBPF_BIG_ENDIAN_ON
#ifdef USBPF_BIG_ENDIAN_ON // Endian
#define ENDIAN_SWAP64(X)	 ((X&0x00000000000000FF)<<56)
							|((X&0x000000000000FF00)<<40)
							|((X&0x0000000000FF0000)<<24)
							|((X&0x00000000FF000000)<<8)
							|((X&0x000000FF00000000)>>8)
							|((X&0x0000FF0000000000)>>24)
							|((X&0x00FF000000000000)>>40)
							|((X&0xFF00000000000000)>>56)
#define ENDIAN_SWAP32(X)    ((X&0x000000FF)<<24)|((X&0x0000FF00)<<8)|((X&0x00FF0000)>>8)|((X&0xFF000000)>>24)
#define ENDIAN_SWAP16(X)	((X&0x00FF)<<8)|((X&0xFF00)>>8)
#define ENDIAN_SWAP16_PARSE(X)	0xFFFF&X
#else // Endian
#define ENDIAN_SWAP64(X)    0xFFFFFFFFFFFFFFFF&X
#define ENDIAN_SWAP32(X)    0xFFFFFFFF&X
#define ENDIAN_SWAP16(X)	0xFFFF&X
#define ENDIAN_SWAP16_PARSE(X)	((X&0x00FF)<<8)|((X&0xFF00)>>8)
#endif // Endian

UCHAR CHIP_TEST_GpioGroupSelect(VOID);		//0x9001			p
UCHAR CHIP_TEST_GpioGroupConfig(VOID);		//0x9002			p
UCHAR CHIP_TEST_GpioGroupSet(VOID);			//0x9003			p
UCHAR CHIP_TEST_GpioGroupGet(VOID);		//0x9004			p
UCHAR CHIP_TEST_ConfigButtonDrvEx(VOID);	//0x9005			p
UCHAR CHIP_TEST_AdcRead(VOID);				//0x9006			p

UCHAR CHIP_TEST_MountStorageMedia(VOID);		//0x9010		p
UCHAR CHIP_TEST_FormatStorageMedia(VOID);		//0x9011		p
UCHAR CHIP_TEST_GetStorageMediaStatus(VOID);	//0x9012		p

UCHAR CHIP_TEST_SetMode(VOID);			//0x9020			p
UCHAR CHIP_TEST_QueryStatus(VOID);		//0x9021			p
UCHAR CHIP_TEST_SetTimer(VOID);			//0x9023			p
UCHAR CHIP_TEST_HostTimerInterrupt(VOID);	//0x9024		p
UCHAR CHIP_TEST_EnableRTC(VOID);		//0x9025		pv
UCHAR CHIP_TEST_GetClock(VOID);			//0x9026		pv
UCHAR CHIP_TEST_SetClock(VOID);			//0x9027		pv
UCHAR CHIP_TEST_HostProc(VOID);			//0x9028		p
UCHAR CHIP_TEST_HostPreBootProc(VOID);		//0x9029		p
UCHAR CHIP_TEST_HostGetMessage(VOID);		//0x902A		p	
UCHAR CHIP_TEST_HostPostMessage(VOID);	//0x902B		p
UCHAR CHIP_TEST_PowerOff(VOID);				//0x902C		p
UCHAR CHIP_TEST_OsThreadCreateEx(VOID);	//0x902D		p
UCHAR CHIP_TEST_OsThreadTerminate(VOID);	//0x902E		p
UCHAR CHIP_TEST_OsThreadDelete(VOID);		//0x902F		p

UCHAR CHIP_TEST_OsThreadSleepMS(VOID);		//0x9030		p
UCHAR CHIP_TEST_OsMutexCreate(VOID);			//0x9031		p
UCHAR CHIP_TEST_OsMutexGet(VOID);				//0x9032		p
UCHAR CHIP_TEST_OsMutexPut(VOID);				//0x9033		p
UCHAR CHIP_TEST_OsMutexDelete(VOID);			//0x9034		p
UCHAR CHIP_TEST_OsEventFlagsCreate(VOID);		//0x9035		p
UCHAR CHIP_TEST_OsEventFlagsGet(VOID);			//0x9036		NotYet
UCHAR CHIP_TEST_OsEventFlagsSet(VOID);			//0x9037		p
UCHAR CHIP_TEST_OsEventFlagsDelete(VOID);		//0x9038		NotYet
UCHAR CHIP_TEST_DisplayWindow(VOID);				//(0x9039)		p
UCHAR CHIP_TEST_SetSensorWindow(VOID);			//(0x903A)		p
UCHAR CHIP_TEST_GetSensorWindow(VOID);			//(0x903B)		p
UCHAR CHIP_TEST_ZoomFactorSet(VOID);				//(0x903C)		p
UCHAR CHIP_TEST_ConfigNin1Mode(VOID);				//(0x903D)		p
UCHAR CHIP_TEST_HostViewYUVManipulation(VOID);	//(0x903E)		NotYet
UCHAR CHIP_TEST_HostViewDramManipulation(VOID);	//(0x903F)		NotYet

UCHAR CHIP_TEST_SetImageSize(VOID);					//0x9040		p
UCHAR CHIP_TEST_SetCompressionRatio(VOID);		//0x9041		p
UCHAR CHIP_TEST_ConfigC(VOID);						//0x0942		p
UCHAR CHIP_TEST_SetParam(VOID);						//0x9043		p
UCHAR CHIP_TEST_ConfirmStore(VOID);					//0x9044		p
UCHAR CHIP_TEST_SetSequenceLength(VOID);			//0x9045		p
UCHAR CHIP_TEST_GetSequenceLength(VOID);			//0x9046		p
UCHAR CHIP_TEST_GetCaptureStatus(VOID);				//0x9047		p
UCHAR CHIP_TEST_PipAddPhotoFrame(VOID);			//0x9048		NotYet
UCHAR CHIP_TEST_DirectScaleYUV(VOID);				//0x9049		NotYet
UCHAR CHIP_TEST_DirectSaveYUV(VOID);				//0x904A		NotYet
UCHAR CHIP_TEST_HostCaptureDramManipulation(VOID);			//0x904B		NotYet
UCHAR CHIP_TEST_HostCaptureYUVManipulation(VOID);			//0x904C		NotYet
UCHAR CHIP_TEST_HostCaptureJpgManipulation(VOID);			//0x904D		NotYet
UCHAR CHIP_TEST_HostCaptureSequenceCB(VOID);				//0x904E		NotYet
UCHAR CHIP_TEST_HostExiffUpdateCB(VOID);						//0x904F		NotYet
UCHAR CHIP_TEST_HostDcfCreateFileCB(VOID);		//0x9050		NotYet
UCHAR CHIP_TEST_HostCreateMakerNote(VOID);	//0x9051		NotYet
UCHAR CHIP_TEST_HostParseMakerNote(VOID);		//0x9052		NotYet

UCHAR CHIP_TEST_DirectFsFileOpen(VOID);		//0x9053		p
UCHAR CHIP_TEST_DirectFsFileClose(VOID);		//0x9054		p
UCHAR CHIP_TEST_DirectFsFileRead(VOID);		//0x9055		p
UCHAR CHIP_TEST_DirectFsFileWrite(VOID);		//0x9056		p
UCHAR CHIP_TEST_DirectFsFileSeek(VOID);		//0x9057		p
UCHAR CHIP_TEST_DirectFsFileRelSeek(VOID);		//0x9058		p
UCHAR CHIP_TEST_DirectFsFileGetLine(VOID);		//0x9059		p
UCHAR CHIP_TEST_DirectFsDelete(VOID);			//0x905A		NotYet
UCHAR CHIP_TEST_DirectFsDirectoryCreate(VOID);	//0x905B		NotYet
UCHAR CHIP_TEST_DirectFsDirectoryChange(VOID);				//0x905C		NotYet
UCHAR CHIP_TEST_DirectFsRename(VOID);		//0x905D		NotYet
UCHAR CHIP_TEST_DirectFsDirectoryEntryGet(VOID);		//0x905E		NotYet
UCHAR CHIP_TEST_DirectFsDirectoryEntryGetEx(VOID);		//0x905F		NotYet
UCHAR CHIP_TEST_DirectFsAttributesRead(VOID);			//0x9060		NotYet

UCHAR CHIP_TEST_SetCurrentImage(VOID);		//0x906F		p
UCHAR CHIP_TEST_GetDCFInfo(VOID);			//0x9070		p
UCHAR CHIP_TEST_SetFileType(VOID);			//0x9071		p
UCHAR CHIP_TEST_GetFileAttributes(VOID);		//0x9072		p
UCHAR CHIP_TEST_GetImageAttributes(VOID);	//0x9073		p
UCHAR CHIP_TEST_GetAudioFileAttr(VOID);		//0x9074		p
UCHAR CHIP_TEST_PlaybackZoom(VOID);		//0x9075		p
UCHAR CHIP_TEST_ConfigThumbnail(VOID);		//0x9077		p
UCHAR CHIP_TEST_SetGDA(VOID);				//0x9078		p
UCHAR CHIP_TEST_FileOperation(VOID);			//0x907B		p
UCHAR CHIP_TEST_EditImage(VOID);				//0x907E		p
//CHIP_TEST_HM_OPERATION_COMPLETE"	0x9080
UCHAR CHIP_TEST_GetParam(VOID);				//0x9081		p	
UCHAR CHIP_TEST_SetFormatConvert(VOID);				//0x9087		NotYet
UCHAR CHIP_TEST_CaptureAudioMessage(VOID);			//0x9089		p
UCHAR CHIP_TEST_DPOFInit(VOID);							//0x908A		p
UCHAR CHIP_TEST_DPOFAddJob(VOID);						//0x908B		NotYet	
UCHAR CHIP_TEST_DPOFRemoveJob(VOID);					//0x908C		NotYet
UCHAR CHIP_TEST_DPOFGetFirstJobInFile(VOID);			//0x908D		NotYet
UCHAR CHIP_TEST_DPOFGetNextJob(VOID);					//0x908E		NotYet
UCHAR CHIP_TEST_DPOFGetJobParamInt(VOID);			//0x908F		NotYet
UCHAR CHIP_TEST_DPOFGetSrcParam(VOID);				//0x9090		NotYet
UCHAR CHIP_TEST_DPOFGetImageData(VOID);				//0x9091		NotYet
UCHAR CHIP_TEST_DPOFSetJobParamInt(VOID);			//0x9092		p
UCHAR CHIP_TEST_DPOFSetSrcParam(VOID);				//0x9093		NotYet
UCHAR CHIP_TEST_DPOFReadWriteFile(VOID);				//0x9094		p
UCHAR CHIP_TEST_DPOFSetHeaderParam(VOID);			//0x9095		NotYet
UCHAR CHIP_TEST_DPOFRelease(VOID);						//0x9096		p


UCHAR CHIP_TEST_PipLayerCreate(VOID);			//0x9097		NotYet
UCHAR CHIP_TEST_PipSetAttrib(VOID);				//0x9098		NotYet
UCHAR CHIP_TEST_PipModeActivate(VOID);			//0x9099			NotYet	
UCHAR CHIP_TEST_PipLayerShow(VOID);			//0x909A		NotYet
UCHAR CHIP_TEST_PipLayerDestroy(VOID);			//0x909B		NotYet
UCHAR CHIP_TEST_Slideshow(VOID);					//0x909C		NotYet
UCHAR CHIP_TEST_SetImageEXIF(VOID);				//0x909D		p
UCHAR CHIP_TEST_GetImageEXIF(VOID);				//0x909E		p
UCHAR CHIP_TEST_SetMemLocationPtr(VOID);		//0x909F		p

UCHAR CHIP_TEST_ExiffUpdateCB(VOID);			//0x90A1		NotYet
UCHAR CHIP_TEST_CreateMakerNote(VOID);			//0x90A2		NotYet
UCHAR CHIP_TEST_ParseMakerNote(VOID);			//0x90A3		NotYet
UCHAR CHIP_TEST_CreateAPPn(VOID);				//0x90A4		NotYet
UCHAR CHIP_TEST_ParseAPPn(VOID);				//0x90A5		NotYet
UCHAR CHIP_TEST_SetDCFFreeChars(VOID);		//0x90A6		NotYet
UCHAR CHIP_TEST_SetNextDCFKey(VOID);			//0x90A7		p
UCHAR CHIP_TEST_GetParamArray(VOID);			//0x90A8		NotYet
UCHAR CHIP_TEST_GetParamLong(VOID);			//0x90A9		p

UCHAR CHIP_TEST_CaptureClipCmd(VOID);			//0x90AE		p	
UCHAR CHIP_TEST_MoviePlayCfg(VOID);				//0x90AF		p
UCHAR CHIP_TEST_PlaybackClipCmd(VOID);			//0x90B0		p
UCHAR CHIP_TEST_MovieStillCapture(VOID);			//0x90B1		p
UCHAR CHIP_TEST_MovHeaderOperation(VOID);		//0x90B3		NotYet


UCHAR CHIP_TEST_AudioRecCfg(VOID);		//0x90B4		pv
//UCHAR CHIP_TEST_Beep(VOID);					//0x90B5
UCHAR CHIP_TEST_AudioParamSet(VOID);	//0x90B6		pv

UCHAR CHIP_TEST_RecordPlayAudio(VOID);				//0x90B8		pv
UCHAR CHIP_TEST_RecordPlayAudioByName(VOID);	//0x90B9		NotYet
UCHAR CHIP_TEST_StopRecording(VOID);				//0x90BA		p
UCHAR CHIP_TEST_DirectLoadWave(VOID);				//0x90BB		NotYet
//UCHAR CHIP_TEST_DirectPlayWav(VOID);
//UCHAR CHIP_TEST_DirectDestroyWav(VOID);
UCHAR CHIP_TEST_GetAudioHdrLength(VOID);			//0x90BE		p
UCHAR CHIP_TEST_SetAudioHdrLength(VOID);			//0x90BF		p
UCHAR CHIP_TEST_GetAudioHdrInfo(VOID);				//0x90C0		NotYet
UCHAR CHIP_TEST_SetAudioHdrInfo(VOID);				//0x90C1		NotYet

UCHAR CHIP_TEST_UART_GetChar(VOID);				//0x90C2		NotYet	@system
UCHAR CHIP_TEST_UART_Printf(VOID);					//0x90C3		NotYet	@system
UCHAR CHIP_TEST_CBUartMonitor(VOID);				//0x90C4		NotYet	@system
UCHAR CHIP_TEST_OSDTextOutID(VOID);				//0x90C5		p
UCHAR CHIP_TEST_OSDDrawShape(VOID);				//0x90C6		NotYet
UCHAR CHIP_TEST_DrawFile(VOID);						//0x90C7		NotYet
UCHAR CHIP_TEST_SetVideoMode(VOID);				//0x90C8		p
UCHAR CHIP_TEST_SetOSDDisplayAttr(VOID);			//0x90C9		p
UCHAR CHIP_TEST_SetOSDCLUTEx(VOID);				//0x90CA		p
UCHAR CHIP_TEST_SetDispGamma(VOID);				//0x90CB		NotYet
UCHAR CHIP_TEST_SetDisplayMode(VOID);				//0x90CC		p
	
UCHAR CHIP_TEST_OSDTextOut(VOID);					//0x90CD		p
UCHAR CHIP_TEST_OSDDrawText(VOID);					//0x90CE		NotYet
UCHAR CHIP_TEST_OSDCreateBuffer(VOID);				//0x90CF		NotYet
UCHAR CHIP_TEST_OSDDestroyBuffer(VOID);			//0x90D0		NotYet
UCHAR CHIP_TEST_OSDDuplicateBuffer(VOID);			//0x90D1		NotYet
UCHAR CHIP_TEST_LoadFontFile(VOID);					//0x90D3		NotYet
UCHAR CHIP_TEST_LoadStringFile(VOID);				//0x90D4		NotYet
UCHAR CHIP_TEST_ConfigI2CAccess(VOID);				//0x90D8		p
//UCHAR CHIP_TEST_ConfigMemAccess(VOID);			//0x90D9		p
UCHAR CHIP_TEST_MonitorParse(VOID);					//0x90DA		NotYet

UCHAR CHIP_TEST_SetLong(VOID);			//0x90DB
UCHAR CHIP_TEST_GetLong(VOID);		//0x90DC

UCHAR CHIP_TEST_GetFixedImage(VOID);	//	(0x90DD)
UCHAR CHIP_TEST_GetLastImage(VOID);		//(0x90DE)
UCHAR CHIP_TEST_GetLastFileName(VOID);	//	(0x90DF)
UCHAR CHIP_TEST_SetFileName(VOID);	//	(0x90E0)	
UCHAR CHIP_TEST_SendFixedImage(VOID);		//(0x90E1)

UCHAR CHIP_TEST_DebugDataWrite(VOID);		//0x90F0		pv
UCHAR CHIP_TEST_DebugDataRead(VOID);		//0x90F1		pv
UCHAR CHIP_TEST_DebugBulkOut(VOID);		//0x90F2

//AE
UCHAR CHIP_TEST_SetStatisticPeriod(VOID);				//0x9100
UCHAR CHIP_TEST_AESetMode(VOID);					//0x9101
UCHAR CHIP_TEST_AESetWindow(VOID);					//0x9102
UCHAR CHIP_TEST_AECCDCSetBlackLevelCompensation(VOID);			//0x9103
UCHAR CHIP_TEST_AESensorSetOBClamp(VOID);				//0x9104
UCHAR CHIP_TEST_AESetComputeValueRatio(VOID);			//0x9105
UCHAR CHIP_TEST_AESetWeight(VOID);							//0x9106
UCHAR CHIP_TEST_AEGetComputeInfo(VOID);					//0x9107		NotYet
UCHAR CHIP_TEST_SetPreviewExposureIdx(VOID);			//0x9108
UCHAR CHIP_TEST_SetCaptureExposureIdx(VOID);			//0x9109
UCHAR CHIP_TEST_GetPreviewExposureIdx(VOID);			//0x910A
UCHAR CHIP_TEST_GetCaptureExposureIdx(VOID);			//0x910B
UCHAR CHIP_TEST_SetPreviewAgcGainIdx(VOID);			//0x910C
UCHAR CHIP_TEST_SetCaptureAgcGainIdx(VOID);			//0x910D
UCHAR CHIP_TEST_GetPreviewAgcGainIdx(VOID);			//0x910E
UCHAR CHIP_TEST_GetCaptureAgcGainIdx(VOID);			//0x910F
UCHAR CHIP_TEST_SetPreviewExposureTime(VOID);			//0x9110
UCHAR CHIP_TEST_SetCaptureExposureTime(VOID);			//0x9111
UCHAR CHIP_TEST_GetPreviewExposureTime(VOID);			//0x9112
UCHAR CHIP_TEST_GetCaptureExposureTime(VOID);			//0x9113
UCHAR CHIP_TEST_SetPreviewAgcGain(VOID);			//0x9114		p
UCHAR CHIP_TEST_SetCaptureAgcGain(VOID);			//0x9115
UCHAR CHIP_TEST_GetPreviewAgcGain(VOID);			//0x9116
UCHAR CHIP_TEST_GetCaptureAgcGain(VOID);			//0x9117
UCHAR CHIP_TEST_SetShutterClosePos(VOID);			//0x9118
UCHAR CHIP_TEST_CBAEProcessOpen(VOID);					//0x9119
//AWB
UCHAR CHIP_TEST_AWBSetMode(VOID);							//0x911A
UCHAR CHIP_TEST_AWBGetComputeInfo(VOID);					//0x911B
UCHAR CHIP_TEST_AWBSetLevelGate(VOID);					//0x911C
UCHAR CHIP_TEST_AWBSetPreviewGain(VOID);					//0x911D
UCHAR CHIP_TEST_AWBSetCaptureGain(VOID);					//0x911E
UCHAR CHIP_TEST_AWBGetPreviewGain(VOID);					//0x911F
UCHAR CHIP_TEST_AWBGetCaptureGain(VOID);					//0x9120
UCHAR CHIP_TEST_CBAWBProcessOpen(VOID);				//0x9121
//AF
UCHAR CHIP_TEST_AFSetMode(VOID);				//0x9122
UCHAR CHIP_TEST_AESetWindow_Pos(VOID);				//0x9123
UCHAR CHIP_TEST_AESetWindow_Size(VOID);				//0x9124
UCHAR CHIP_TEST_AESetCoring(VOID);				//0x9125
UCHAR CHIP_TEST_AESet_HPF1(VOID);				//0x9126
UCHAR CHIP_TEST_AESet_HPF2(VOID);				//0x9127
UCHAR CHIP_TEST_AESet_FIR3(VOID);				//0x9128
UCHAR CHIP_TEST_AESet_Gain(VOID);				//0x9129
UCHAR CHIP_TEST_CBWindowAFReady(VOID);				//0x912A
UCHAR CHIP_TEST_CBAFProcessOpen(VOID);				//0x912B
UCHAR CHIP_TEST_SetStrobeTrigger(VOID);				//0x912C
UCHAR CHIP_TEST_SetPreStrobe(VOID);				//0x912D
UCHAR CHIP_TEST_SetMainStrobe(VOID);				//0x912E
UCHAR CHIP_TEST_AESensorSendTgCommand(VOID);				//0x912F
UCHAR CHIP_TEST_AESensorSendAfeCommand(VOID);				//0x9130
UCHAR CHIP_TEST_AEGetPreviewRawDataSizde(VOID);				//0x9131
UCHAR CHIP_TEST_AESetDeadPixelCorrection(VOID);				//0x9132
UCHAR CHIP_TEST_AESetCaptureLSCCalibrationEnable(VOID);				//0x9133
UCHAR CHIP_TEST_AEGetCaptureLSCCalibrationEnable(VOID);				//0x9134
UCHAR CHIP_TEST_AESetCaptureLSCEnable(VOID);				//0x9135
UCHAR CHIP_TEST_AEGetCaptureLSCEnable(VOID);				//0x9136
UCHAR CHIP_TEST_AESetCaptureLSCTable(VOID);				//0x9137
UCHAR CHIP_TEST_AEGetCaptureLSCTable(VOID);				//0x9138
UCHAR CHIP_TEST_AEGetHistogram(VOID);				//0x9139

UCHAR CHIP_TEST_Set2ToppolyTPG85(VOID);			//0x9200
UCHAR CHIP_TEST_Set2ADDI9000(VOID);				//0x9201
UCHAR CHIP_TEST_Lens_Open(VOID);			//0x9202
UCHAR CHIP_TEST_Lens_Close(VOID);			//0x9203
UCHAR CHIP_TEST_Lens_ZoomMove2Wide(VOID);			//0x9204
UCHAR CHIP_TEST_Lens_ZoomMove2Tele(VOID);			//0x9205
UCHAR CHIP_TEST_Lens_ZoomMoveByStep(VOID);			//0x9206
UCHAR CHIP_TEST_Lens_FocusMove2CCW(VOID);			//0x9207
UCHAR CHIP_TEST_Lens_FocusMove2CW(VOID);			//0x9208
UCHAR CHIP_TEST_Lens_MShutterOpen(VOID);			//0x9209
UCHAR CHIP_TEST_Lens_MShutterClose(VOID);			//0x920A
UCHAR CHIP_TEST_Lens_NDOpen(VOID);			//0x920B
UCHAR CHIP_TEST_Lens_NDClose(VOID);			//0x920C


//extra base f/w
UCHAR FJ_GetFixedImage(VOID);
UCHAR FJ_GetLastImage(VOID);
UCHAR FJ_Set2ToppolyTPG85(USHORT p1,USHORT p2);
UCHAR FJ_SendFixedImage(VOID);

UCHAR Vender_Chip_Test(VOID);

