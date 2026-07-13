/**
 * @file		palladium_test.h
 * @brief		Palladium Test driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _PALLADIUM_TEST_H_

/**
@addtogroup arm
@{
	@addtogroup PalladiumTest
	@{
*/

#define _PALLADIUM_TEST_H_


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define P_TIMER_ID_COMMON0				0
#define P_TIMER_ID_COMMON1				1
#define P_TIMER_ID_COMMON2				2
#define P_TIMER_ID_VD0					3
#define P_TIMER_ID_VD1					4
#define P_TIMER_ID_VD2					5

#define P_TIMER_INIT					1
#define P_TIMER_START					2
#define P_TIMER_END						3
#define P_TIMER_CLEAR					4
#define P_TIMER_PRINT					5

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
typedef struct {
	unsigned long		top;		// CHIPTOP
	unsigned long		arm;		// ARM
	unsigned long		cache;		// CACHE
	unsigned long		exiu;		// EXIU
	unsigned long		gic;		// GIC400
	unsigned long		ipcu;		// IPCU
	unsigned long		mcc;		// MCC
	unsigned long		nic;		// NIC400
	unsigned long		timestamp;	// TIMESTAMP
	unsigned long		wdog;		// WATCHDOG
	unsigned long		xdmasnap;	// XDMASNAP
	unsigned long		exs;		// EXS
	unsigned long		gpv;		// GPV
	unsigned long		hdmac;		// HDMAC
	unsigned long		pcie;		// PCIe
	unsigned long		relc;		// RELC
	unsigned long		xdmac;		// XDMAC
	unsigned long		audio;		// AUDIO
	unsigned long		csio;		// CSIO
	unsigned long		hdmac0;		// HDMAC0
	unsigned long		hdmac1;		// HDMAC1
	unsigned long		i2c;		// I2C
	unsigned long		pdm;		// PDM
	unsigned long		pwm;		// PWM
	unsigned long		tmr32;		// 32bit Timer
	unsigned long		tmr64;		// 64bit Timer
	unsigned long		uart;		// UART
	unsigned long		udc;		// UDC
	unsigned long		dummy1;		// dummy1
	unsigned long		dummy2;		// dummy2
} T_DDIM_INFO_DD;

// IM
typedef struct {
	unsigned long		b2r;		// B2R
	unsigned long		bmh;		// BMH
	unsigned long		cnr;		// CNR
	unsigned long		disp;		// DISP
	unsigned long		ela;		// ELA
	unsigned long		hdmi;		// HDMI
	unsigned long		iip;		// IIP
	unsigned long		img;		// IMG
	unsigned long		jpeg;		// JPEG
	unsigned long		mipi;		// MIPI
	unsigned long		mxic;		// MXIC
	unsigned long		pro;		// PRO
	unsigned long		r2y;		// R2Y
	unsigned long		raw;		// RAW
	unsigned long		sdramc;		// SDRAMC
	unsigned long		shdr;		// SHDR
	unsigned long		xch;		// XCH
	unsigned long		fpt;		// FPT
	unsigned long		ltm;		// LTM
} T_DDIM_INFO_IM;

// COM
typedef struct {
	unsigned long		_6a;		// 6x
	unsigned long		_6b;
	unsigned long		_6c;
	unsigned long		_6d;
	unsigned long		_6e;
} T_DDIM_INFO_COM;


//----------------------
// TEST用 PRAMETER
//----------------------
typedef struct {
	T_DDIM_INFO_DD		dd;			// DD
	T_DDIM_INFO_IM		im;			// IM
	T_DDIM_INFO_COM		com;		// com
} T_DDIM_INFO;

extern T_DDIM_INFO gDDIM_Info;		// TEST PARAM

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#if 0
#define Dd_ARM_EI(sr)			swi6(sr)	/**< Set IRQ mask bit */
#define Dd_ARM_DI()				swi7()		/**< Disable IRQ and get IRQ mask bit */
#define __EI()					Dd_ARM_EI(0)	/**< Enable IRQ */
#define __DI()					Dd_ARM_DI()		/**< Disable IRQ */
#endif
/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


// (1) データの入力操作
// void Palladium_Set_In_Localstack(unsigned long in_data_adr   , unsigned long in_data_siz);
// 【引数】
//    in_data_adr [in] 展開したいアドレス
//    in_data_siz [in] 展開したいサイズ
// 【戻り値】
//    なし
void Palladium_Set_In_Localstack(unsigned long in_data_adr   , unsigned long in_data_siz);


// (2) データの出力操作
// VOIF Palladium_Set_Out_Localstack(unsigned long out_data_adr , unsigned long out_data_siz) 
// 【引数】
//    out_data_adr [in] 出力したいアドレス
//    out_data_siz [in] 出力したいサイズ
// 【戻り値】
//    なし
void Palladium_Set_Out_Localstack(unsigned long out_data_adr , unsigned long out_data_siz);


// (3) タイマー操作
//void Palladium_Timer(int timer_id , int timer_data) 
// 【引数】
//    timer_id     [in] 下記から選択
//        P_TIMER_ID_COMMON0
//        P_TIMER_ID_COMMON1
//        P_TIMER_ID_COMMON2
//        P_TIMER_ID_VD0
//        P_TIMER_ID_VD1
//        P_TIMER_ID_VD2
//
//    timer_data   [in] 下記から選択
//        P_TIMER_INIT
//        P_TIMER_START
//        P_TIMER_END
//        P_TIMER_CLEAR
//        P_TIMER_PRINT
//
// 【戻り値】
//    なし
void Palladium_Timer(int timer_id , int timer_data);


// (4)  センサーモデル
//void Palladium_Sensor(void) 
// 【引数】
//    なし
//
// 【戻り値】
//    なし
void Palladium_Sensor(void);


// (5)  テスト強制終了処理
//void Palladium_Force_Stop(void) 
// 【引数】
//    なし
//
// 【戻り値】
//    なし
void Palladium_Force_Stop(void);


// (6)  タートコードを強制的に叩く処理  ※裏技
//void Palladium_Force_Start(void) 
// 【引数】
//    なし
//
// 【戻り値】
//    なし
void Palladium_Force_Start(void);

// (7)  VLD擬似モデル
//void Palladium_Vld_Pseudo(void) 
// 【引数】
//    なし
//
// 【戻り値】
//    なし
void Palladium_Vld_Pseudo(void);

extern	void Pt_Dd_SRLOT_Main( void );
extern	void Pt_Dd_TOP_Main( void );
extern	void Pt_Dd_ARM_Main( void );
extern	void Pt_Dd_CACHE_Main( void );
extern	void Pt_Dd_GIC_Main( void );
extern	void Pt_Dd_MMU_Main( void );
extern	void Pt_Dd_SCU_Main( void );
extern	void Pt_Dd_L2C_Main( void );
extern	void Pt_Dd_NIC_Main( void );
extern	void Pt_Dd_Wdog_Main( void );
extern	void Pt_Dd_PTMR_Main( void );
extern	void Pt_Dd_GTMR_Main( void );
extern	void Pt_Dd_MRBC_Main( void );
extern	void Pt_Dd_GPV_Main( void );
extern	void Pt_Dd_HDMAC0_Main( void );
extern	void Pt_Dd_HDMAC1_Main( void );
extern	void Pt_Dd_XDMAC_Main( void );
extern	void Pt_Dd_IPCU_Main( void );
extern	void Pt_Dd_MCC_Main( void );
extern	void Pt_Dd_EXIU_Main( void );
extern	void Pt_Dd_UART_Main( void );
extern	void Pt_Dd_CSIO_Main( void );
extern	void Pt_Dd_TMR32_Main( void );
extern	void Pt_Dd_PWM_Main( void );
extern	void Pt_Dd_I2C_Main( void );
extern	void Pt_Dd_UDC_Main( void );
extern	void Pt_Dd_AUDIO_Main( void );
extern	void Pt_Dd_RELC_Main( void );
extern	void Pt_Dd_HDMI_Main( void );
extern	void Pt_Dd_EXS_Main( void );
extern	void Pt_Dd_SD_Main( void );
extern	void Pt_Dd_NF_Main( void );
extern	void Pt_Dd_PCIe_Main( void );
extern	void Pt_Dd_USB_Main( void );
extern	void Pt_Dd_ETHER_Main( void );
extern	void Pt_Dd_FPT_Main( void );
extern	void Pt_Dd_DSP_Main( void );
extern	void Pt_Dd_TOP_Main( void );
extern	void Pt_Dd_NETSEC_Main( void );

extern	void Pt_Im_IMG_Main( void );
extern	void Pt_Im_XCH_Main( void );
extern	void Pt_Im_CNR_Main( void );
extern	void Pt_Im_PRO_Main( void );
extern	void Pt_Im_JPEG_Main( void );
extern	void Pt_Im_DISP_Main( void );
extern	void Pt_Im_B2R_Main( void );
extern	void Pt_Im_R2Y_Main( void );
extern	void Pt_Im_IIP_Main( void );
extern	void Pt_Im_ELA_Main( void );
extern	void Pt_Im_FPT_Main( void );
extern	void Pt_Im_MXIC_Main( void );
extern	void Pt_Im_SHDR_Main( void );
extern	void Pt_Im_SDRAMC_Main( void );
extern	void Pt_Im_RAW_Main( void );
extern	void Pt_Im_LTM_Main( void );
extern	void Pt_Im_ME_Main( void );

//sample
extern	void Pt_timer_Int_Handler( void );
#if 0
/**
Set IRQ mask bit
@param[in]	sr	program status included IRQ bit
*/
__swi(6) void swi6(unsigned sr);

/**
Disable IRQ and get program status included IRQ mask bit
@return		program status included IRQ mask bit
*/
__swi(7) unsigned swi7(void);
#endif

#ifdef __cplusplus
}
#endif

/*	@}*/
/*@}*/

#endif
