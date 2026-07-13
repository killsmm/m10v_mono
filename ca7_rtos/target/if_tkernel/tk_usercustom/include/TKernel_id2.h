/**
 * @file		TKernel_id2.h
 * @brief		Task metadata.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _TKERNEL_ID2_H_
#define _TKERNEL_ID2_H_


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/* Task priority id */
#define	PRI_FS_RECEIVER							51
// --- REMOVE_DCF_EXIF BEGIN ---
#define	PRI_DCF_RECEIVER						52
// --- REMOVE_DCF_EXIF END ---
#define	PRI_BOOT								17
#define	PRI_BOOTLOADER							60
#define	PRI_UART_CONTROLLER						61
#define	PRI_BOOT_CONTROLLER						19
#define	PRI_COMMAND_OBSERVER					56
#ifdef CO_IM_TUNING_ON
#define	PRI_IQ_COMMAND_OBSERVER					56
#endif
#define	PRI_BF_SUB1								25
#define	PRI_BF_SUB2								25
#define	PRI_AUDIO_ENCDEC						27
#define	PRI_AUDIO_CAP							27
#define	PRI_AUDIO_OUT							27
#define	PRI_BASEFW								24
// --- REMOVE_FD BEGIN ---
#define	PRI_AHIP_FD								35
// --- REMOVE_FD END ---
// --- REMOVE_PD BEGIN ---
#define PRI_AHIP_PD                             35
// --- REMOVE_PD END ---
#define	PRI_HDMI_DETECT							20
#define	PRI_HDMI_CONTROLLER						20
#define	PRI_HOST_MOVIE_DEBUG_WRITE				51
#define	PRI_HOST_MOVIE_DEBUG_FEED				51
#define	PRI_HOST_MOVIE_DEBUG_FREE_H264			33
#define	PRI_HOST_MOVIE_DEBUG_FREE_H265			33
#define	PRI_HOST_AUDIO_DEBUG					51
#define	PRI_HOST_STILL_DEBUG					51
#define	PRI_H264_DISP							31
#define	PRI_H264_CC								32
#define	PRI_H264_ENT							33
#define	PRI_H264_VECTOR							33
#define	PRI_H264_CONTROLLER						36
#define	PRI_H264_DISP_LCD						25
#define	PRI_H264_DISP_HDMI						25
#define	PRI_H265_DISP							31
#define	PRI_H265_CC								32
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	PRI_H265_CC_2							32
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
#define	PRI_H265_ENT							33
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	PRI_H265_ENT_INT						9
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
#define	PRI_H265_CONTROLLER						36
#define	PRI_H265_SIRIUS_CONT					4
#define	PRI_H265_DISP_LCD						25
#define	PRI_H265_DISP_HDMI						25
#define	PRI_DEMUX								27
#define	PRI_UART_RX								56
#define	PRI_HOST_MAIN							51
#define	PRI_MULFILE_CTRL						60
#define	PRI_HOST_SUB							27
#define	PRI_HOST_BOOT_INIT_NF					61
#define	PRI_HOST_BOOT_INIT_SD					61
#define	PRI_IPCU_TASK0							60
#define	PRI_IPCU_TASK1							60
#define	PRI_IPCU_TASK2							60
#define	PRI_IPCU_TASK3							60
#define	PRI_IPCU_TASK4							60
#define	PRI_IPCU_TASK5							60
#define	PRI_IPCU_TASK6							60
#define	PRI_IPCU_TASK7							60
#define	PRI_IPCU_TASK8							60
#define	PRI_IPCU_TASK9							60
#define	PRI_IPCU_TASK10							60
#define	PRI_IPCU_TASK11							60
#define	PRI_IPCU_TASK12							60
#define	PRI_IPCU_TASK13							60
#define	PRI_IPCU_TASK14							60
#define	PRI_IPCU_TASK15							60
#define	PRI_IPCU_TASK16							60
#define	PRI_IPCU_TASK17							60
#define	PRI_IPCUFS_RECEIVER						51
#define	PRI_VBD_RECEIVER						51
#define	PRI_HOST_GYRO							26
#define	PRI_PCIE_RC_CONTROLLER					27
#define	PRI_PCIE_EP_CONTROLLER					27
#define	PRI_SLIMBUS_CONTROLLER					27
// --- REMOVE_USB BEGIN ---
#define	PRI_USB_MAIN							30
#define	PRI_USB_ENUM_ERR						32
#define	PRI_USB_STORAGE							55
#define	PRI_USB_DPS_SUBSYS						26
#define	PRI_USB_DPSTSK							25
#define	PRI_USB_PTP								24
#define	PRI_USB_MTP								23
#define	PRI_USB_HOTPLUG							55
#define	PRI_MSCP_RCV							53
#define	PRI_MSCP_SND							53
// --- REMOVE_USB END ---
#define	PRI_GPU_CONTROLLER						61
#define	PRI_IDLE0								137		// InitTask -1
#define	PRI_IDLE1								137		// InitTask -1
#define	PRI_IDLE2								137		// InitTask -1
// --- REMOVE_PANO BEGIN ---
#define	PRI_AHIP_PANO							35
// --- REMOVE_PANO END ---
#define	PRI_PMU0								2
#define	PRI_PMU1								2
#define	PRI_PMU2								2
#define	PRI_L1L2CACHE0							1
#define	PRI_L1L2CACHE1							1
#define	PRI_L1L2CACHE2							1
// --- REMOVE_INTERCORE_CMD BEGIN ---
#define PRI_INTERCORE_CMD						60
// --- REMOVE_INTERCORE_CMD END ---
// --- REMOVE_CAMERAIF BEGIN ---
#define PRI_IPCU_FINISHCB						60
// --- REMOVE_CAMERAIF END ---
//#define PRI_KEYEVT_CONTROLLER					61
#define PRI_EVT_OBSERVER						61
#define	PRI_HOST_CTRL							60
#define	PRI_IPCU_SET_OSD_CONFIG					51
#define	PRI_GYRO_OUTPUT							27
#define	PRI_SYSTEM								10
#define	PRI_USER								10
#define	PRI_USER_SYNC							25
#define	PRI_USER_RECORD							25//9
#define	PRI_DETECT								60
#define PRI_AWB									21
#define PRI_AE									20
#define PRI_AF									20
#define PRI_GYRO_DMP							25
#define PRI_IPU_GMD								15
#define PRI_IPU_ME								15
#define PRI_IPU_SHDR							15
#define PRI_IPU_VHDR							15
#define PRI_SENSOR_ACCESS						9
#define PRI_OS_IF								(PRI_IPCU_TASK1)
#define PRI_IPU_VIS								15
#define PRI_IPU_EIS								15
#define PRI_IPU_SGDE							15
#define PRI_LDC_CALC                            15
#define PRI_IQ_ACCESS							20
#define PRI_QR_MARKER							15
#define PRI_MOTOR_CALC_0						9
#define PRI_MOTOR_CALC_1						9
#define PRI_UV_CONVERT      					15
#define PRI_UVC_COPY							19 // Higher than IQ_ACCESS
#define PRI_IIP_SCALE      						(PRI_IPU_SHDR)
#define PRI_OSD_BLEND      						(PRI_IPU_SGDE)

/* Task stack size [Byte] */
#define	STK_TASK_FS_RECEIVER					0x2000
// --- REMOVE_DCF_EXIF BEGIN ---
#define	STK_TASK_DCF_RECEIVER					0x3000
// --- REMOVE_DCF_EXIF END ---
#define	STK_TASK_BOOT							0x2000
#define	STK_TASK_BOOT_LOADER					0x1000
#define	STK_TASK_UART_CONTROLLER				0x1000
#define	STK_TASK_BOOT_CONTROLLER				0x1000
#define	STK_TASK_COMMAND_OBSERVER				0x8000
#ifdef CO_IM_TUNING_ON
#define	STK_TASK_IQ_COMMAND_OBSERVER			0x1000
#endif
#define	STK_TASK_AWB							0x1000
#define	STK_TASK_BF_SUB1						0x2000
#define	STK_TASK_BF_SUB2						0x2000
#define	STK_TASK_AUDIO_ENCDEC					0x3800
#define	STK_TASK_AUDIO_CAP						0x3000
#define	STK_TASK_AUDIO_OUT						0x3000
#define	STK_TASK_BASEFW							0x6000
// --- REMOVE_FD BEGIN ---
#define	STK_TASK_AHIP_FD						0x1000
// --- REMOVE_FD END ---
// --- REMOVE_PD BEGIN ---
#define STK_TASK_AHIP_PD                        0x8000
// --- REMOVE_PD END ---
#define	STK_TASK_AE								0x1000
#define	STK_TASK_HDMI_DETECT					0x2000
#define	STK_TASK_HDMI_CONTROLLER				0x2000
#define	STK_TASK_HOST_MOVIE_DEBUG_WRITE			0x2000
#define	STK_TASK_HOST_MOVIE_DEBUG_FEED			0x5000
#define	STK_TASK_HOST_MOVIE_DEBUG_FREE_H264		0x2000
#define	STK_TASK_HOST_MOVIE_DEBUG_FREE_H265		0x2000
#define	STK_TASK_HOST_AUDIO_DEBUG				0x2000
#define	STK_TASK_HOST_STILL_DEBUG				0x2000
#define	STK_TASK_H264_DISP						0x2000
#define	STK_TASK_H264_CC						0x2000
#define	STK_TASK_H264_ENT						0x2000
#define	STK_TASK_H264_VECTOR					0x2000
#define	STK_TASK_H264_CONTROLLER				0x4000
#define	STK_TASK_H264_DISP_LCD					0x2000
#define	STK_TASK_H264_DISP_HDMI					0x2000
#define	STK_TASK_H265_DISP						0x2000
#define	STK_TASK_H265_CC						0x2000
// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#define	STK_TASK_H265_CC_2						0x2000
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---
#define	STK_TASK_H265_ENT						0x2000
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
#define	STK_TASK_H265_ENT_INT					0x2000
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
#define	STK_TASK_H265_CONTROLLER				0x4000
#define	STK_TASK_H265_SIRIUS_CONT				0x2000
#define	STK_TASK_H265_DISP_LCD					0x2000
#define	STK_TASK_H265_DISP_HDMI					0x2000
#define	STK_TASK_DEMUX							0x7000
#define	STK_TASK_UART_RX						0x8000
#define	STK_TASK_HOST_MAIN						0x2000
#define	STK_TASK_MULFILE_CTRL					0x2000
#define	STK_TASK_HOST_SUB						0x2000
#define	STK_TASK_HOST_BOOT_INIT_NF				0x2000
#define	STK_TASK_HOST_BOOT_INIT_SD				0x2000
#define	STK_TASK_IPCU_TASK0						0x1000
#define	STK_TASK_IPCU_TASK1						0x1000
#define	STK_TASK_IPCU_TASK2						0x1000
#define	STK_TASK_IPCU_TASK3						0x1000
#define	STK_TASK_IPCU_TASK4						0x1000
#define	STK_TASK_IPCU_TASK5						0x1000
#define	STK_TASK_IPCU_TASK6						0x1000
#define	STK_TASK_IPCU_TASK7						0x1000
#define	STK_TASK_IPCU_TASK8						0x1000
#define	STK_TASK_IPCU_TASK9						0x1000
#define	STK_TASK_IPCU_TASK10					0x1000
#define	STK_TASK_IPCU_TASK11					0x1000
#define	STK_TASK_IPCU_TASK12					0x1000
#define	STK_TASK_IPCU_TASK13					0x1000
#define	STK_TASK_IPCU_TASK14					0x1000
#define	STK_TASK_IPCU_TASK15					0x1000
#define	STK_TASK_IPCU_TASK16					0x1000
#define	STK_TASK_IPCU_TASK17					0x1000
#define	STK_TASK_IPCUFS_RECEIVER				0x2000
#define	STK_TASK_VBD_RECEIVER					0x2000
#define	STK_TASK_HOST_GYRO						0x2000
#define	STK_TASK_PCIE_RC_CONTROLLER				0x1000
#define	STK_TASK_PCIE_EP_CONTROLLER				0x1000
#define	STK_TASK_SLIMBUS_CONTROLLER				0x1000
// --- REMOVE_USB BEGIN ---
#define	STK_TASK_USB_MAIN						0x1000
#define	STK_TASK_USB_ENUM_ERR					0x400
#define	STK_TASK_USB_STORAGE					0x1600
#define	STK_TASK_USB_DPS_SUBSYS					0x2000
#define	STK_TASK_USB_DPSTSK						0x1000
#define	STK_TASK_USB_PTP						0x1000
#define	STK_TASK_USB_MTP						0x1000
#define	STK_TASK_USB_HOTPLUG					0x1000
#define	STK_TASK_MSCP_RCV						0x1000
#define	STK_TASK_MSCP_SND						0x1000
// --- REMOVE_USB END ---
#define	STK_TASK_GPU_CONTROLLER					0x1000
#define	STK_IDLE0								0x80
#define	STK_IDLE1								0x80
#define	STK_IDLE2								0x80
// --- REMOVE_FD BEGIN ---
#define	STK_TASK_AHIP_PANO						0x1000
// --- REMOVE_FD END ---
#define	STK_TASK_PMU0							0x400
#define	STK_TASK_PMU1							0x400
#define	STK_TASK_PMU2							0x400
#define	STK_TASK_L1L2CACHE0						0x2000
#define	STK_TASK_L1L2CACHE1						0x2000
#define	STK_TASK_L1L2CACHE2						0x2000
// --- REMOVE_INTERCORE_CMD BEGIN ---
#define	STK_TASK_INTERCORE_CMD					0x8000
// --- REMOVE_INTERCORE_CMD END ---
// --- REMOVE_CAMERAIF BEGIN ---
#define	STK_TASK_IPCU_FINISHCB					0x1000
// --- REMOVE_CAMERAIF END ---
#if 0    // special key control
#define	STK_TASK_KEYEVT_CONTROLLER				0x8000
#else
#define	STK_TASK_EVT_OBSERVER					0x8000
#endif
#define	STK_TASK_HOST_CTRL						0x1000
#define STK_IPCU_SET_OSD_CONFIG					0x2000
#define STK_GYRO_OUTPUT							0x2000
#define STK_SYSTEM								0x1000
#define STK_USER								0x1000
#define STK_USER_SYNC							0x1000
#define STK_USER_RECORD							0x1000
#define STK_DETECT								0x2000
#define STK_AWB									0x1000
#define STK_AE									0x1000
#define STK_AF									0x1000
#define STK_TASK_GYRO_DMP						0x1000
#define STK_TASK_IPU_GMD						0x1000
#define STK_TASK_IPU_ME							0x1000
#define STK_TASK_IPU_SHDR						0x1000
#define STK_TASK_IPU_VHDR						0x1000
#define STK_TASK_SENSOR_ACCESS					0x1000
#define STK_TASK_OS_IF							0x1000
#define STK_TASK_IPU_VIS						0x1000
#define STK_TASK_IPU_EIS						0x1000
#define STK_TASK_IPU_SGDE						0x1000
#define STK_TASK_IQ_ACCESS						0x1000
#define STK_TASK_QR_MARKER						0x8000
#define STK_TASK_MOTOR_CALC_0					0x1000
#define STK_TASK_MOTOR_CALC_1					0x1000
#define STK_TASK_UV_CONVERT 					0x1000
#define STK_TASK_UVC_COPY						0x1000
#define STK_TASK_IIP_SCALE						0x1000
#define STK_TASK_OSD_BLEND						0x1000
#define STK_TASK_LDC_CALC						0x1000

#endif // _TKERNEL_ID2_H_
