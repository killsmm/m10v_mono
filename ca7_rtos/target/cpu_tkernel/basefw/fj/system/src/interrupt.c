/**
 * @file		interrupt.c
 * @brief		interrupt control api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "interrupt.h"
#include "debug.h"
#include "dd_arm.h"
#include "fj_host.h"
#include "fj_host.h"
#include "fj_param.h"
#include "fj_peripheral.h"
#include "dd_audio.h"
#include "dd_uart.h"
#include "dd_csio.h"
#include "dd_spi.h"
#include "dd_i2c.h"
#include "dd_pwm.h"
#include "dd_pdm.h"
#include "dd_tmr32.h"
#include "dd_top.h"
#include "dd_udc.h"
#include "dd_wdog.h"
#include "dd_exs.h"
#include "dd_hdmac.h"
#include "dd_hdmac0.h"
#include "dd_hdmac1.h"
#include "dd_xdmac.h"
#include "dd_xdmasnap.h"
#include "dd_pcie_rc.h"
#include "dd_pcie_ep.h"
#include "dd_slimbus.h"
#include "im_b2r.h"
#include "im_bmh.h"
#include "im_cnr.h"
#include "im_disp.h"
#include "im_hdmi.h"
#include "im_mipi.h"
#include "im_ela.h"
#include "im_fpt.h"
#include "im_iip.h"
#include "im_jpeg.h"
#include "im_ltm.h"
#include "im_mxic.h"
#include "im_pro.h"
#include "im_r2y.h"
#include "im_raw.h"
#include "im_rdma.h"
#include "im_sdramc.h"
#include "im_shdr.h"
#include "im_xch.h"
#include "im_me.h"
#include "device_config.h"
#include "mwnf.h"
#include "mwsdio.h"
#include "querystatus.h"
#include "ipcu.h"
#include "RB.h"
#include "h265_sirius.h"
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
#include "vdi_userfunc.h"
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
#include "bf_startup.h"
#include "os_user_custom.h"
#include "fs_user_custom.h"
// ### REMOVE_RELEASE BEGIN
#include "dd_relc.h"
// ### REMOVE_RELEASE END
#include "dd_gic.h"
#include "dd_exiu.h"
#include "mw_xdmac.h"
#include "media_state.h"
#include "ogma_irq.h"
#include "interrupt_set.h"
// --- REMOVE_USB BEGIN ---
#include "USBHDC_Dev_Api.h"
// --- REMOVE_USB END ---
#include "em_if_ext.h"

#include "ip_ahip.h"
/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_INTERRUPT_PRI_MAX				(31)

// trigger parameter
#define D_BF_INTERRUPT_EXT_LOW_LEVEL		(0)
#define D_BF_INTERRUPT_EXT_HIGH_LEVEL		(1)
#define D_BF_INTERRUPT_EXT_RISING_EDGE		(2)
#define D_BF_INTERRUPT_EXT_FALLING_EDGE		(3)


/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/** ExtInterrupt config */
typedef enum {
	E_EINT16 = 16,
	E_EINT_MIN = E_EINT16,
	E_EINT17,
	E_EINT18,
	E_EINT19,
	E_EINT20,
	E_EINT21,
	E_EINT22,
	E_EINT23,
	E_EINT24,
	E_EINT25,
	E_EINT26,
	E_EINT27,
	E_EINT28,
	E_EINT29,
	E_EINT30,
	E_EINT31,
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	E_EINT32,
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
	E_EINT_MAX
} E_EINT;

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Management information of External Interrupt
typedef struct {
	FJ_GPIO_PORT			port;					// Port name
	UINT32					int_num;				// (0 - 113)
	EXT_INT_CALLBACK		int_handler;			// Call back function pointer
	E_EINT					eint;					// (0 - 31)
} E_INTERRUPT_EXT_MNG;

void* gGIC_Vector_Table[320] =
{
	BF_Sgi_Interrupt0,								// 000:
	BF_Sgi_Interrupt1,								// 001:
	BF_Sgi_Interrupt2,								// 002:
	BF_Sgi_Interrupt3,								// 003:
	BF_Sgi_Interrupt4,								// 004:
	BF_Sgi_Interrupt5,								// 005:
	BF_Sgi_Interrupt6,								// 006:
	BF_Sgi_Interrupt7,								// 007:
	BF_Sgi_Interrupt8,								// 008:
	BF_Sgi_Interrupt9,								// 009:
	BF_Sgi_Interrupt10,								// 010:
	BF_Sgi_Interrupt11,								// 011:
	BF_Sgi_Interrupt12,								// 012:
	BF_Sgi_Interrupt13,								// 013:
	BF_Sgi_Interrupt14,								// 014:
	BF_Sgi_Interrupt15,								// 015:
	BF_Ppi_Interrupt0,								// 016:
	BF_Ppi_Interrupt1,								// 017:
	BF_Ppi_Interrupt2,								// 018:
	BF_Ppi_Interrupt3,								// 019:
	BF_Ppi_Interrupt4,								// 020:
	BF_Ppi_Interrupt5,								// 021:
	BF_Ppi_Interrupt6,								// 022:
	BF_Ppi_Interrupt7,								// 023:
	BF_Ppi_Interrupt8,								// 024:
	BF_Ppi_Interrupt9,								// 025:
	BF_Ppi_Interrupt10,								// 026:
	BF_Ppi_Interrupt11,								// 027:
	BF_Ppi_Interrupt12,								// 028:
	BF_Ppi_Interrupt13,								// 029:
	BF_Ppi_Interrupt14,								// 030:
	BF_Ppi_Interrupt15,								// 031:
	BF_CortexA7_Cpu0_Communication_Ch_Receive_Int,	// 032:
	BF_CortexA7_Cpu0_Communication_Ch_Trnsmit_Int,	// 033:
	BF_CortexA7_Cpu1_Communication_Ch_Receive_Int,	// 034:
	BF_CortexA7_Cpu1_Communication_Ch_Trnsmit_Int,	// 035:
	BF_CortexA7_Cpu2_Communication_Ch_Receive_Int,	// 036:
	BF_CortexA7_Cpu2_Communication_Ch_Trnsmit_Int,	// 037:
	BF_CortexA7_Cpu3_Communication_Ch_Receive_Int,	// 038:
	BF_CortexA7_Cpu3_Communication_Ch_Trnsmit_Int,	// 039:
	BF_CortexA7_Cpu0_Crosstrigger_Int,				// 040:
	BF_CortexA7_Cpu1_Crosstrigger_Int,				// 041:
	BF_CortexA7_Cpu2_Crosstrigger_Int,				// 042:
	BF_CortexA7_Cpu3_Crosstrigger_Int,				// 043:
	BF_CortexA7_Cpu0_System_Metrics_Int,			// 044:
	BF_CortexA7_Cpu1_System_Metrics_Int,			// 045:
	BF_CortexA7_Cpu2_System_Metrics_Int,			// 046:
	BF_CortexA7_Cpu3_System_Metrics_Int,			// 047:
	BF_Axi_Transaction_Error_Int,					// 048:
	BF_Snap_Peri_Xdmac_Ch0_Int,						// 049:
	BF_Snap_Peri_Xdmac_Ch1_Int,						// 050:
	BF_Snap_Peri_Xdmac_Ch2_Int,						// 051:
	BF_Snap_Peri_Xdmac_Ch3_Int,						// 052:
	BF_Watchdog_Timer_Int,							// 053:
	BF_Ipcu0_Ch0_Int,								// 054:
	BF_Ipcu0_Ch1_Int,								// 055:
	BF_Ipcu0_Ch2_Int,								// 056:
	BF_Ipcu0_Ch3_Int,								// 057:
	BF_Ipcu0_Ch4_Int,								// 058:
	BF_Ipcu0_Ch5_Int,								// 059:
	BF_Ipcu0_Ch6_Int,								// 060:
	BF_Ipcu0_Ch7_Int,								// 061:
	BF_Ipcu0_Ch8_Int,								// 062:
	BF_Ipcu0_Ch9_Int,								// 063:
	BF_Ipcu0_Ch10_Int,								// 064:
	BF_Ipcu0_Ch11_Int,								// 065:
	BF_Ipcu0_Ch12_Int,								// 066:
	BF_Ipcu0_Ch13_Int,								// 067:
	BF_Ipcu0_Ch14_Int,								// 068:
	BF_Ipcu0_Ch15_Int,								// 069:
	BF_Ipcu1_Ch0_Int,								// 070:
	BF_Ipcu1_Ch1_Int,								// 071:
	BF_Ipcu1_Ch2_Int,								// 072:
	BF_Ipcu1_Ch3_Int,								// 073:
	BF_Ipcu1_Ch4_Int,								// 074:
	BF_Ipcu1_Ch5_Int,								// 075:
	BF_Ipcu1_Ch6_Int,								// 076:
	BF_Ipcu1_Ch7_Int,								// 077:
	BF_Ipcu1_Ch8_Int,								// 078:
	BF_Ipcu1_Ch9_Int,								// 079:
	BF_Ipcu1_Ch10_Int,								// 080:
	BF_Ipcu1_Ch11_Int,								// 081:
	BF_Ipcu1_Ch12_Int,								// 082:
	BF_Ipcu1_Ch13_Int,								// 083:
	BF_Ipcu1_Ch14_Int,								// 084:
	BF_Ipcu1_Ch15_Int,								// 085:
	BF_External_Ch16_Int,							// 086:
	BF_External_Ch17_Int,							// 087:
	BF_External_Ch18_Int,							// 088:
	BF_External_Ch19_Int,							// 089:
	BF_External_Ch20_Int,							// 090:
	BF_External_Ch21_Int,							// 091:
	BF_External_Ch22_Int,							// 092:
	BF_External_Ch23_Int,							// 093:
	BF_External_Ch24_Int,							// 094:
	BF_External_Ch25_Int,							// 095:
	BF_External_Ch26_Int,							// 096:
	BF_External_Ch27_Int,							// 097:
	BF_External_Ch28_Int,							// 098:
	BF_External_Ch29_Int,							// 099:
	BF_External_Ch30_Int,							// 100:
	BF_External_Ch31_Int,							// 101:
	BF_Pmc_Dmac_Adc_Int,							// 102:
	BF_Pmc_Dmac_Receive_Int,						// 103:
	BF_Pmc_Dmac_Uart_Transmission_Data_Int,			// 104:
	BF_Pmc_Dmac_Qspi_Transmission_Int,				// 105:
	BF_Pmc_Dmac_Qspi_Receive_Int,					// 106:
	BF_Pmc_Uart_Receive_Relation_Int,				// 107:
	BF_Pmc_Uart_Transmission_Relation_Int,			// 108:
	BF_Pmc_Qspi_Int,								// 109:
	BF_Calendar_Alarm_Int,							// 110:
	BF_Pmc_32bit_Timer_Under_Flow_Int,				// 111:
	BF_Adc_Int,										// 112:
	BF_Pmc_I2c_Int,									// 113:
	BF_Pmc_Watchdog_Int,							// 114:
	BF_Temperature_Int,								// 115:
	BF_Pmu_Int,										// 116:
	BF_Pmc_Gpio_Int,								// 117:
	BF_Timer_Ch0_Int,								// 118:
	BF_Timer_Ch1_Int,								// 119:
	BF_Timer_Ch2_Int,								// 120:
	BF_Timer_Ch3_Int,								// 121:
	BF_Timer_Ch4_Int,								// 122:
	BF_Timer_Ch5_Int,								// 123:
	BF_Timer_Ch6_Int,								// 124:
	BF_Timer_Ch7_Int,								// 125:
	BF_Timer_Ch8_Int,								// 126:
	BF_Timer_Ch9_Int,								// 127:
	BF_Timer_Ch10_Int,								// 128:
	BF_Timer_Ch11_Int,								// 129:
	BF_Timer_Ch12_Int,								// 130:
	BF_Timer_Ch13_Int,								// 131:
	NULL,											// 132:
	BF_I2c_Ch0_Int,									// 133:
	BF_I2c_Ch1_Int,									// 134:
	BF_I2c_Ch2_Int,									// 135:
	BF_Updown_Counter_Ch0_Int,						// 136:
	BF_Updown_Counter_Ch1_Int,						// 137:
	BF_Updown_Counter_Ch2_Int,						// 138:
	BF_Updown_Counter_Ch3_Int,						// 139:
	BF_Updown_Counter_Ch4_Int,						// 140:
	BF_Updown_Counter_Ch5_Int,						// 141:
	BF_Pwm_Ch0_Ch3_Int,								// 142:
	BF_Pwm_Ch4_Ch7_Int,								// 143:
	BF_Pwm_Ch8_Ch11_Int,							// 144:
	BF_Pwm_Ch12_Ch15_Int,							// 145:
	BF_Pwm_Ch0_Ch3_Stp_Int,							// 146:
	BF_Pwm_Ch4_Ch7_Stp_Int,							// 147:
	BF_Pwm_Ch8_Ch11_Stp_Int,						// 148:
	BF_Pwm_Ch12_Ch15_Stp_Int,						// 149:
	BF_Audio_If_Ch0_Int,							// 150:
	BF_Audio_If_Ch1_Int,							// 151:
	BF_Audio_If_Ch2_Int,							// 152:
	BF_Audio_If_Ch3_Int,							// 153:
	BF_Audio_If_Ch4_Int,							// 154:
	BF_Audio_If_Ch5_Int,							// 155:
	BF_Peri_Hdmac0_Ch0_Int,							// 156:
	BF_Peri_Hdmac0_Ch1_Int,							// 157:
	BF_Peri_Hdmac0_Ch2_Int,							// 158:
	BF_Peri_Hdmac0_Ch3_Int,							// 159:
	BF_Peri_Hdmac0_Ch4_Int,							// 160:
	BF_Peri_Hdmac0_Ch5_Int,							// 161:
	BF_Peri_Hdmac0_Ch6_Int,							// 162:
	BF_Peri_Hdmac0_Ch7_Int,							// 163:
	BF_Peri_Hdmac1_Ch0_Int,							// 164:
	BF_Peri_Hdmac1_Ch1_Int,							// 165:
	BF_Peri_Hdmac1_Ch2_Int,							// 166:
	BF_Peri_Hdmac1_Ch3_Int,							// 167:
	BF_Peri_Hdmac1_Ch4_Int,							// 168:
	BF_Peri_Hdmac1_Ch5_Int,							// 169:
	BF_Peri_Hdmac1_Ch6_Int,							// 170:
	BF_Peri_Hdmac1_Ch7_Int,							// 171:
	BF_Uart_Ch0_Reception_Int,						// 172:
	BF_Uart_Ch1_Reception_Int,						// 173:
	BF_Uart_Ch2_Reception_Int,						// 174:
	BF_Uart_Ch3_Reception_Int,						// 175:
	BF_Uart_Ch4_Reception_Int,						// 176:
	BF_Uart_Ch5_Reception_Int,						// 177:
	BF_Uart_Ch6_Reception_Int,						// 178:
	BF_Uart_Ch7_Reception_Int,						// 179:
	BF_Uart_Ch0_Transmission_Int,					// 180:
	BF_Uart_Ch1_Transmission_Int,					// 181:
	BF_Uart_Ch2_Transmission_Int,					// 182:
	BF_Uart_Ch3_Transmission_Int,					// 183:
	BF_Uart_Ch4_Transmission_Int,					// 184:
	BF_Uart_Ch5_Transmission_Int,					// 185:
	BF_Uart_Ch6_Transmission_Int,					// 186:
	BF_Uart_Ch7_Transmission_Int,					// 187:
	BF_Spi_Ch0_Int,									// 188:
	BF_Spi_Ch1_Int,									// 189:
	BF_Spi_Ch2_Int,									// 190:
	BF_Pdm_Ch0_Dma0_Int,							// 191:
	BF_Pdm_Ch0_Dma1_Int,							// 192:
	BF_Pdm_Ch0_Fifo_Overflow_Int,					// 193:
	BF_Pdm_Ch1_Dma0_Int,							// 194:
	BF_Pdm_Ch1_Dma1_Int,							// 195:
	BF_Pdm_Ch1_Fifo_Overflow_Int,					// 196:
	BF_Slimbus_Ch0_Int,								// 197:
	BF_Slimbus_Ch1_Int,								// 198:
	BF_Slimbus_Ch2_Int,								// 199:
	BF_Slimbus_Ch3_Int,								// 200:
	BF_Sdramc_Ch0_Int,								// 221:
	BF_Sdramc_Ch1_Int,								// 202:
	BF_Mxic_Unit0_Int,								// 203:
	BF_Mxic_Unit1_Int,								// 204:
	BF_Mxic_Unit2_Int,								// 205:
	BF_Mxic_Unit3_Int,								// 206:
	BF_Mxic_Unit4_Int,								// 207:
	BF_Mxic_Unit5_Int,								// 208:
	BF_Mxic_Unit6_Int,								// 209:
	BF_Sen_Int,										// 210:
	BF_Sen_Vd_Int,									// 211:
	BF_Sen_Hd_Int,									// 212:
	BF_Sensor_If_Macro0_Int,						// 213:
	BF_Sensor_If_Macro1_Int,						// 214:
	BF_Sensor_If_Macro2_Int,						// 215:
	BF_Sensor_If_Macro3_Int,						// 216:
	BF_Sro_Unit1_Int,								// 217:
	BF_Sro_Unit1_Vd_Int,							// 218:
	BF_Sro_Unit1_Hd_Int,							// 219:
	BF_Sro_Unit2_Int,								// 220:
	BF_Sro_Unit2_Vd_Int,							// 221:
	BF_Sro_Unit2_Hd_Int,							// 222:
	BF_B2b_Unit1_Int,								// 223:
	BF_B2b_Unit1_Vd_Int,							// 224:
	BF_B2b_Unit1_Hd_Int,							// 225:
	BF_B2b_Unit2_Int,								// 226:
	BF_B2b_Unit2_Vd_Int,							// 227:
	BF_B2b_Unit2_Hd_Int,							// 228:
	BF_Stat_Int,									// 229:
	BF_Pas_Int,										// 230:
	BF_Pas_Vd_Int,									// 231:
	BF_Pas_Hd_Int,									// 232:
	BF_B2r_Unit1_Int,								// 233:
	BF_B2r_Unit2_Int,								// 234:
	BF_Ltm_Unit1_Int,								// 235:
	BF_Ltm_Unit2_Int,								// 236:
	BF_R2y_Unit1_Int,								// 237:
	BF_R2y_Unit2_Int,								// 238:
	BF_Cnr_Unit1_Int,								// 239:
	BF_Cnr_Unit2_Int,								// 240:
	BF_Iip_Int,										// 241:
	BF_Shdr_Int,									// 242:
	BF_Shdr_Line_Int,								// 243:
	BF_Me_Int,										// 244:
	BF_Graphics_Int,								// 245:
	BF_Jpeg_Int,									// 246:
	BF_Raw_Int,										// 247:
	BF_Ipu_Int,										// 248:
	BF_Ela_Unit_Int,								// 249:
	BF_Xch_Int,										// 250:
	BF_Fpt_Unit0_Int,								// 251:
	BF_Fpt_Unit1_Int,								// 252:
	BF_Disp_Lcd_Int,								// 253:
	BF_Disp_Hdmi_Int,								// 254:
	BF_Disp_Mipi_Dsi_Int,							// 255:
	BF_Mipi_Dsi_Tx_Int,								// 256:
	BF_Disp_Dwch_Int,								// 257:
	BF_Rdma_Int,									// 258:
	BF_Hdmi_Master_Int,								// 259:
	BF_Hdmi_Cec_Int,								// 260:
	BF_BMH_Unit0_Int,								// 261:
	BF_BMH_Unit1_Int,								// 262:
	BF_Ribery_Rbr_Int,								// 263:
	BF_Ribery_Rwdt_Int,								// 264:
	BF_Interprocessor0_Int,							// 265:
	BF_Interprocessor1_Int,							// 266:
	BF_Interprocessor2_Int,							// 267:
	BF_Interprocessor3_Int,							// 268:
	BF_Dsp_Peripheral_Unit_Ipcu0_Int,				// 269:
	BF_Dsp_Peripheral_Unit_Ipcu1_Int,				// 260:
	BF_Dsp_Peripheral_Unit_Ipcu2_Int,				// 271:
	BF_Dsp_Peripheral_Unit_Ipcu3_Int,				// 272:
	BF_Dsp_Debug_Int,								// 273:
	BF_Nand_Flash_Int,								// 274:
	BF_Pcie_Ch0_Msi_Int,							// 275:
	BF_Pcie_Ch0_System_Error_Int,					// 276:
	BF_Pcie_Ch0_Intx_Int,							// 277:
	BF_Pcie_Ch0_Core_Int,							// 278:
	BF_Pcie_Ch0_DMA_Int,							// 279:
	BF_Pcie_Ch1_Msi_Int,							// 280:
	BF_Pcie_Ch1_System_Error_Int,					// 281:
	BF_Pcie_Ch1_Intx_Int,							// 282:
	BF_Pcie_Ch1_Core_Int,							// 283:
	BF_Pcie_Ch1_DMA_Int,							// 284:
	BF_Exs_Xdmac_Ch0_Int,							// 285:
	BF_Exs_Xdmac_Ch1_Int,							// 286:
	BF_Exs_Xdmac_Ch2_Int,							// 287:
	BF_Exs_Xdmac_Ch3_Int,							// 288:
	BF_Exs_Hdmac_Ch0_Int,							// 289:
	BF_Exs_Hdmac_Ch1_Int,							// 290:
	BF_Exs_Hdmac_Ch2_Int,							// 291:
	BF_Exs_Hdmac_Ch3_Int,							// 292:
	BF_Usb20_Hdc_Int,								// 293:
	BF_Usb20_Host_Ehci_Int,							// 294:
	BF_Usb20_Host_Ohci_Int,							// 295:
	BF_Usb30_Int,									// 296:
	BF_Sd_Ch3_Int,									// 297:
	BF_Sd_Ch3_Sdio_Int,								// 298:
	BF_Sd_Ch3_Wakeup_Int,							// 299:
	BF_Sd_Ch2_Uhs1_Int,								// 300:
	BF_Sd_Ch2_Sdio_Int,								// 301:
	BF_Sd_Ch2_Uhs1_Wakeup_Int,						// 302:
	BF_Sd_Ch2_Uhs2_Int,								// 303:
	BF_Sd_Ch2_Uhs2_Wakeup_Int,						// 304:
	BF_Sd_Ch1_Int,									// 305:
	BF_Sd_Ch1_Sdio_Int,								// 306:
	BF_Sd_Ch1_Wakeup_Int,							// 307:
	BF_Emmc_Int,									// 308:
	BF_Emmc_Wakeup_Int,								// 309:
	BF_Relc_Int,									// 310:
	BF_Netsec_A_Int,								// 311:
	BF_Netsec_B_Int,								// 312:
	BF_Sd_Pcie_Detect_Int,							// 313:
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
    NULL,											// 314:
    NULL,											// 315:
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	BF_Wave_Int,									// 314:
    BF_External_Ch32_Int,							// 315:
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
    NULL,											// 316:
    NULL,											// 317:
    NULL,											// 318:
    NULL,											// 319:
};

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static volatile E_INTERRUPT_EXT_MNG	gBF_Interrupt_Ext_Mng[E_EINT_MAX - E_EINT16] = {
	//	Port name  int_num  CB	 fpint
	{FJ_GPIO_P60,	86,	   NULL,	E_EINT16},		// E_EINT16
	{FJ_GPIO_P61,	87,	   NULL,	E_EINT17},		// E_EINT17
	{FJ_GPIO_P62,	88,	   NULL,	E_EINT18},		// E_EINT18
	{FJ_GPIO_P63,	89,	   NULL,	E_EINT19},		// E_EINT19
	{FJ_GPIO_P64,	90,	   NULL,	E_EINT20},		// E_EINT20
	{FJ_GPIO_P65,	91,	   NULL,	E_EINT21},		// E_EINT21
	{FJ_GPIO_P66,	92,	   NULL,	E_EINT22},		// E_EINT22
	{FJ_GPIO_P67,	93,	   NULL,	E_EINT23},		// E_EINT23
	{FJ_GPIO_P70,	94,	   NULL,	E_EINT24},		// E_EINT24
	{FJ_GPIO_P71,	95,	   NULL,	E_EINT25},		// E_EINT25
	{FJ_GPIO_P72,	96,	   NULL,	E_EINT26},		// E_EINT26
	{FJ_GPIO_P73,	97,	   NULL,	E_EINT27},		// E_EINT27
	{FJ_GPIO_P74,	98,	   NULL,	E_EINT28},		// E_EINT28
	{FJ_GPIO_P75,	99,	   NULL,	E_EINT29},		// E_EINT29
	{FJ_GPIO_P76,	100,   NULL,	E_EINT30},		// E_EINT30
	{FJ_GPIO_P77,	101,   NULL,	E_EINT31},		// E_EINT31
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	{FJ_GPIO_PB4,	315,   NULL,	E_EINT32},		// E_EINT32
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
};

extern UCHAR gDD_PCIE_RC_RE_TYPE;
extern UCHAR gDD_PCIE_EP_RE_TYPE;

static ULONG  gbf_ext_interrupt_spin_lock __attribute__((section(".LOCK_SECTION"), aligned(64)));

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// interrupt handler definition
#define DEF_INT_HANDLER_0(inth_fnc) VOID inth_fnc(VOID) {}
#define DEF_INT_HANDLER_1(inth_fnc, body_fnc, arg) \
	VOID inth_fnc(VOID) {save_vfp_regs(); body_fnc arg; load_vfp_regs();}

#define save_vfp_regs()	__asm__ __volatile__( \
	"mov   r1, sp\n\t" \
	"bic   r0, r1, #7\n\t" \
	"sub   r0, #4\n\t" \
	"mov   sp, r0\n\t" \
	"push  {r1}\n\t" \
	"vpush {d0-d15}\n\t" \
	"vpush {d16-d31}\n\t" \
	"vmrs  r1, FPSCR\n\t" \
	"push  {r1}\n\t" \
	"vmrs  r1, FPEXC\n\t" \
	"push  {r1}\n\t" \
	: : : );

#define load_vfp_regs()	__asm__ __volatile__( \
	"pop   {r1}\n\t" \
	"vmsr  FPEXC, r1\n\t" \
	"pop   {r1}\n\t" \
	"vmsr  FPSCR, r1\n\t" \
	"vpop  {d16-d31}\n\t" \
	"vpop  {d0-d15}\n\t" \
	"pop   {r1}\n\t" \
	"mov   sp, r1\n\t" \
	: : : );


/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
// Call user External Interrupt handler EINT16-31 (INTID=86-101)
static VOID external_int_handler(E_EINT eint)
{
	UINT32 ext_int_num = eint - E_EINT16;
	UINT32 ch_num;
	FJ_GPIO_PORT port;

	ch_num = gBF_Interrupt_Ext_Mng[ext_int_num].eint;
	port = gBF_Interrupt_Ext_Mng[ext_int_num].port;

	// Clear the interrupts requests by writing to the "Interrupt Clear Register (EIREQCLR)".
	Dd_EXIU_Set_EIREQCLR_Ch(ch_num);

	if( gBF_Interrupt_Ext_Mng[ext_int_num].int_handler ) {
		(*gBF_Interrupt_Ext_Mng[ext_int_num].int_handler)( port );
	}
}

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/
/* ----------------------------------------------------------------------------- */
/* Interrupt Handler															 */
/* ----------------------------------------------------------------------------- */
// SGI Interrupt0
DEF_INT_HANDLER_0( BF_Sgi_Interrupt0 )

// SGI Interrupt1
DEF_INT_HANDLER_0( BF_Sgi_Interrupt1 )

// SGI Interrupt2
DEF_INT_HANDLER_0( BF_Sgi_Interrupt2 )

// SGI Interrupt3
DEF_INT_HANDLER_0( BF_Sgi_Interrupt3 )

// SGI Interrupt4
DEF_INT_HANDLER_0( BF_Sgi_Interrupt4 )

// SGI Interrupt5
DEF_INT_HANDLER_0( BF_Sgi_Interrupt5 )

// SGI Interrupt6
DEF_INT_HANDLER_0( BF_Sgi_Interrupt6 )

// SGI Interrupt7
DEF_INT_HANDLER_0( BF_Sgi_Interrupt7 )

// SGI Interrupt8
DEF_INT_HANDLER_0( BF_Sgi_Interrupt8 )

// SGI Interrupt9
DEF_INT_HANDLER_0( BF_Sgi_Interrupt9 )

// SGI Interrupt10
DEF_INT_HANDLER_0( BF_Sgi_Interrupt10 )

// SGI Interrupt11
DEF_INT_HANDLER_0( BF_Sgi_Interrupt11 )

// SGI Interrupt12
DEF_INT_HANDLER_0( BF_Sgi_Interrupt12 )

// SGI Interrupt13
DEF_INT_HANDLER_0( BF_Sgi_Interrupt13 )

// SGI Interrupt14
DEF_INT_HANDLER_0( BF_Sgi_Interrupt14 )

// SGI Interrupt15
DEF_INT_HANDLER_0( BF_Sgi_Interrupt15 )

// PPI Interrupt0
DEF_INT_HANDLER_0( BF_Ppi_Interrupt0 )

// PPI Interrupt1
DEF_INT_HANDLER_0( BF_Ppi_Interrupt1 )

// PPI Interrupt2
DEF_INT_HANDLER_0( BF_Ppi_Interrupt2 )

// PPI Interrupt3
DEF_INT_HANDLER_0( BF_Ppi_Interrupt3 )

// PPI Interrupt4
DEF_INT_HANDLER_0( BF_Ppi_Interrupt4 )

// PPI Interrupt5
DEF_INT_HANDLER_0( BF_Ppi_Interrupt5 )

// PPI Interrupt6
DEF_INT_HANDLER_0( BF_Ppi_Interrupt6 )

// PPI Interrupt7
DEF_INT_HANDLER_0( BF_Ppi_Interrupt7 )

// PPI Interrupt8
DEF_INT_HANDLER_0( BF_Ppi_Interrupt8 )

// PPI Interrupt9
DEF_INT_HANDLER_0( BF_Ppi_Interrupt9 )

// PPI Interrupt10
DEF_INT_HANDLER_0( BF_Ppi_Interrupt10 )

// PPI Interrupt11
DEF_INT_HANDLER_0( BF_Ppi_Interrupt11 )

// PPI Interrupt12
DEF_INT_HANDLER_0( BF_Ppi_Interrupt12 )

// PPI Interrupt13
DEF_INT_HANDLER_0( BF_Ppi_Interrupt13 )

// PPI Interrupt14
DEF_INT_HANDLER_0( BF_Ppi_Interrupt14 )

// PPI Interrupt15
DEF_INT_HANDLER_0( BF_Ppi_Interrupt15 )

// Cortex-A7 CPU0 Communication Receive
DEF_INT_HANDLER_0( BF_CortexA7_Cpu0_Communication_Ch_Receive_Int )

// Cortex-A7 CPU0 Communication Trnsmit
DEF_INT_HANDLER_0( BF_CortexA7_Cpu0_Communication_Ch_Trnsmit_Int )

// Cortex-A7 CPU1 Communication Receive
DEF_INT_HANDLER_0( BF_CortexA7_Cpu1_Communication_Ch_Receive_Int )

// Cortex-A7 CPU1 Communication Trnsmit
DEF_INT_HANDLER_0( BF_CortexA7_Cpu1_Communication_Ch_Trnsmit_Int )

// Cortex-A7 CPU2 Communication Receive
DEF_INT_HANDLER_0( BF_CortexA7_Cpu2_Communication_Ch_Receive_Int )

// Cortex-A7 CPU2 Communication Trnsmit
DEF_INT_HANDLER_0( BF_CortexA7_Cpu2_Communication_Ch_Trnsmit_Int )

// Cortex-A7 CPU3 Communication Receive
DEF_INT_HANDLER_0( BF_CortexA7_Cpu3_Communication_Ch_Receive_Int )

// Cortex-A7 CPU3 Communication Trnsmit
DEF_INT_HANDLER_0( BF_CortexA7_Cpu3_Communication_Ch_Trnsmit_Int )

// Cortex-A7 CPU0 Crosstrigger
DEF_INT_HANDLER_0( BF_CortexA7_Cpu0_Crosstrigger_Int )

// Cortex-A7 CPU1 Crosstrigger
DEF_INT_HANDLER_0( BF_CortexA7_Cpu1_Crosstrigger_Int )

// Cortex-A7 CPU2 Crosstrigger
DEF_INT_HANDLER_0( BF_CortexA7_Cpu2_Crosstrigger_Int )

// Cortex-A7 CPU3 Crosstrigger
DEF_INT_HANDLER_0( BF_CortexA7_Cpu3_Crosstrigger_Int )

// Cortex-A7 CPU0 System Metrics
DEF_INT_HANDLER_0( BF_CortexA7_Cpu0_System_Metrics_Int )

// Cortex-A7 CPU1 System Metrics
DEF_INT_HANDLER_0( BF_CortexA7_Cpu1_System_Metrics_Int )

// Cortex-A7 CPU2 System Metrics
DEF_INT_HANDLER_0( BF_CortexA7_Cpu2_System_Metrics_Int )

// Cortex-A7 CPU3 System Metrics
DEF_INT_HANDLER_0( BF_CortexA7_Cpu3_System_Metrics_Int )

// AXI Transaction Error
DEF_INT_HANDLER_0( BF_Axi_Transaction_Error_Int )

// SNAP Peri XDMAC ch0
DEF_INT_HANDLER_1( BF_Snap_Peri_Xdmac_Ch0_Int, Dd_XDMASNAP_Int_Handler, (0) )

// SNAP Peri XDMAC ch1
DEF_INT_HANDLER_1( BF_Snap_Peri_Xdmac_Ch1_Int, Dd_XDMASNAP_Int_Handler, (1) )

// SNAP Peri XDMAC ch2
DEF_INT_HANDLER_1( BF_Snap_Peri_Xdmac_Ch2_Int, Dd_XDMASNAP_Int_Handler, (2) )

// SNAP Peri XDMAC ch3
DEF_INT_HANDLER_1( BF_Snap_Peri_Xdmac_Ch3_Int, Dd_XDMASNAP_Int_Handler, (3) )

// Watchdog Timer
DEF_INT_HANDLER_1( BF_Watchdog_Timer_Int, Dd_WDOG_Int_Handler, () )

// IPCU0 interrupt ch0
DEF_INT_HANDLER_1( BF_Ipcu0_Ch0_Int, BF_IPCU_Int_Handler_Req, (0) )

// IPCU0 interrupt ch1
DEF_INT_HANDLER_1( BF_Ipcu0_Ch1_Int, BF_IPCU_Int_Handler_Req, (1) )

// IPCU0 interrupt ch2
DEF_INT_HANDLER_1( BF_Ipcu0_Ch2_Int, BF_IPCU_Int_Handler_Req, (2) )

// IPCU0 interrupt ch3
DEF_INT_HANDLER_1( BF_Ipcu0_Ch3_Int, BF_IPCU_Int_Handler_Req, (3) )

// IPCU0 interrupt ch4
DEF_INT_HANDLER_1( BF_Ipcu0_Ch4_Int, BF_IPCU_Int_Handler_Req, (4) )

// IPCU0 interrupt ch5
DEF_INT_HANDLER_1( BF_Ipcu0_Ch5_Int, BF_IPCU_Int_Handler_Req, (5) )

// IPCU0 interrupt ch6
DEF_INT_HANDLER_1( BF_Ipcu0_Ch6_Int, BF_IPCU_Int_Handler_Req, (6) )

// IPCU0 interrupt ch7
DEF_INT_HANDLER_1( BF_Ipcu0_Ch7_Int, BF_IPCU_Int_Handler_Req, (7) )

// IPCU0 interrupt ch8
DEF_INT_HANDLER_1( BF_Ipcu0_Ch8_Int, BF_IPCU_Int_Handler_Ack, (8) )

// IPCU0 interrupt ch9
DEF_INT_HANDLER_1( BF_Ipcu0_Ch9_Int, BF_IPCU_Int_Handler_Ack, (9) )

// IPCU0 interrupt ch10
DEF_INT_HANDLER_1( BF_Ipcu0_Ch10_Int, BF_IPCU_Int_Handler_Ack, (10) )

// IPCU0 interrupt ch11
DEF_INT_HANDLER_1( BF_Ipcu0_Ch11_Int, BF_IPCU_Int_Handler_Ack, (11) )

// IPCU0 interrupt ch12
DEF_INT_HANDLER_1( BF_Ipcu0_Ch12_Int, BF_IPCU_Int_Handler_Ack, (12) )

// IPCU0 interrupt ch13
DEF_INT_HANDLER_1( BF_Ipcu0_Ch13_Int, BF_IPCU_Int_Handler_Ack, (13) )

// IPCU0 interrupt ch14
DEF_INT_HANDLER_1( BF_Ipcu0_Ch14_Int, BF_IPCU_Int_Handler_Ack, (14) )

// IPCU0 interrupt ch15
DEF_INT_HANDLER_1( BF_Ipcu0_Ch15_Int, BF_IPCU_Int_Handler_Ack, (15) )

// IPCU1 interrupt ch0
DEF_INT_HANDLER_1( BF_Ipcu1_Ch0_Int, BF_IPCU_Int_Handler_Req, (16) )

// IPCU1 interrupt ch1
DEF_INT_HANDLER_1( BF_Ipcu1_Ch1_Int, BF_IPCU_Int_Handler_Req, (17) )

// IPCU1 interrupt ch2
DEF_INT_HANDLER_1( BF_Ipcu1_Ch2_Int, BF_IPCU_Int_Handler_Req, (18) )

// IPCU1 interrupt ch3
DEF_INT_HANDLER_1( BF_Ipcu1_Ch3_Int, BF_IPCU_Int_Handler_Req, (19) )

// IPCU1 interrupt ch4
DEF_INT_HANDLER_1( BF_Ipcu1_Ch4_Int, BF_IPCU_Int_Handler_Req_CM0, (20) )

// IPCU1 interrupt ch5
DEF_INT_HANDLER_0( BF_Ipcu1_Ch5_Int )

// IPCU1 interrupt ch6
DEF_INT_HANDLER_1( BF_Ipcu1_Ch6_Int, BF_IPCU_Int_Handler_Ack, (22) )

// IPCU1 interrupt ch7
DEF_INT_HANDLER_1( BF_Ipcu1_Ch7_Int, BF_IPCU_Int_Handler_Ack, (23) )

// IPCU1 interrupt ch8
DEF_INT_HANDLER_1( BF_Ipcu1_Ch8_Int, BF_IPCU_Int_Handler_Ack, (24) )

// IPCU1 interrupt ch9
DEF_INT_HANDLER_1( BF_Ipcu1_Ch9_Int, BF_IPCU_Int_Handler_Ack, (25) )

// IPCU1 interrupt ch10
DEF_INT_HANDLER_0( BF_Ipcu1_Ch10_Int )

// IPCU1 interrupt ch11
DEF_INT_HANDLER_1( BF_Ipcu1_Ch11_Int, BF_IPCU_Int_Handler_Ack_CM0, (27) )

// IPCU1 interrupt ch12
DEF_INT_HANDLER_0( BF_Ipcu1_Ch12_Int )

// IPCU1 interrupt ch13
DEF_INT_HANDLER_0( BF_Ipcu1_Ch13_Int )

// IPCU1 interrupt ch14
DEF_INT_HANDLER_1( BF_Ipcu1_Ch14_Int, BF_IPCU_Int_Handler_Ack_CM0, (30) )

// IPCU1 interrupt ch15
DEF_INT_HANDLER_0( BF_Ipcu1_Ch15_Int )

// External Interrupt ch16
DEF_INT_HANDLER_1( BF_External_Ch16_Int, external_int_handler, (E_EINT16) )

// External Interrupt ch17
DEF_INT_HANDLER_1( BF_External_Ch17_Int, external_int_handler, (E_EINT17) )

// External Interrupt ch18
DEF_INT_HANDLER_1( BF_External_Ch18_Int, external_int_handler, (E_EINT18) )

// External Interrupt ch19
DEF_INT_HANDLER_1( BF_External_Ch19_Int, external_int_handler, (E_EINT19) )

// External Interrupt ch20
DEF_INT_HANDLER_1( BF_External_Ch20_Int, external_int_handler, (E_EINT20) )

// External Interrupt ch21
DEF_INT_HANDLER_1( BF_External_Ch21_Int, external_int_handler, (E_EINT21) )

// External Interrupt ch22
DEF_INT_HANDLER_1( BF_External_Ch22_Int, external_int_handler, (E_EINT22) )

// External Interrupt ch23
DEF_INT_HANDLER_1( BF_External_Ch23_Int, external_int_handler, (E_EINT23) )

// External Interrupt ch24
DEF_INT_HANDLER_1( BF_External_Ch24_Int, external_int_handler, (E_EINT24) )

// External Interrupt ch25
DEF_INT_HANDLER_1( BF_External_Ch25_Int, external_int_handler, (E_EINT25) )

// External Interrupt ch26
DEF_INT_HANDLER_1( BF_External_Ch26_Int, external_int_handler, (E_EINT26) )

// External Interrupt ch27
DEF_INT_HANDLER_1( BF_External_Ch27_Int, external_int_handler, (E_EINT27) )

// External Interrupt ch28
DEF_INT_HANDLER_1( BF_External_Ch28_Int, external_int_handler, (E_EINT28) )

// External Interrupt ch29
DEF_INT_HANDLER_1( BF_External_Ch29_Int, external_int_handler, (E_EINT29) )

// External Interrupt ch30
DEF_INT_HANDLER_1( BF_External_Ch30_Int, external_int_handler, (E_EINT30) )

// External Interrupt ch31
DEF_INT_HANDLER_1( BF_External_Ch31_Int, external_int_handler, (E_EINT31) )

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
// External Interrupt ch32
DEF_INT_HANDLER_1( BF_External_Ch32_Int, external_int_handler, (E_EINT32) )
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// PMC DMAC ADC.
DEF_INT_HANDLER_0( BF_Pmc_Dmac_Adc_Int )

// PMC DMAC Receive.
DEF_INT_HANDLER_0( BF_Pmc_Dmac_Receive_Int )

// PMC DMAC UART Transmission Data.
DEF_INT_HANDLER_0( BF_Pmc_Dmac_Uart_Transmission_Data_Int )

// PMC DMAC QSPI Transmission.
DEF_INT_HANDLER_0( BF_Pmc_Dmac_Qspi_Transmission_Int )

// PMC DMAC QSPI Receive.
DEF_INT_HANDLER_0( BF_Pmc_Dmac_Qspi_Receive_Int )

// PMC UART Receive Relation.
DEF_INT_HANDLER_0( BF_Pmc_Uart_Receive_Relation_Int )

// PMC UART Transmission Relation.
DEF_INT_HANDLER_0( BF_Pmc_Uart_Transmission_Relation_Int )

// PMC QSPI.
DEF_INT_HANDLER_0( BF_Pmc_Qspi_Int )

// CALENDAR Alarm.
DEF_INT_HANDLER_0( BF_Calendar_Alarm_Int )

// PMC 32bit Timer Under Flow.
DEF_INT_HANDLER_0( BF_Pmc_32bit_Timer_Under_Flow_Int )

// ADC.
DEF_INT_HANDLER_0( BF_Adc_Int )

// PMC I2C.
DEF_INT_HANDLER_0( BF_Pmc_I2c_Int )

// PMC Watchdog.
DEF_INT_HANDLER_0( BF_Pmc_Watchdog_Int )

// Temperature.
DEF_INT_HANDLER_0( BF_Temperature_Int )

// PMU.
DEF_INT_HANDLER_0( BF_Pmu_Int )

// PMC Gpio.
DEF_INT_HANDLER_0( BF_Pmc_Gpio_Int )

// Timer ch0
DEF_INT_HANDLER_1( BF_Timer_Ch0_Int, Dd_TMR32_Int_Handler, (0) )

// Timer ch1
DEF_INT_HANDLER_1( BF_Timer_Ch1_Int, Dd_TMR32_Int_Handler, (1) )

// Timer ch2
DEF_INT_HANDLER_1( BF_Timer_Ch2_Int, Dd_TMR32_Int_Handler, (2) )

// Timer ch3
DEF_INT_HANDLER_1( BF_Timer_Ch3_Int, Dd_TMR32_Int_Handler, (3) )

// Timer ch4
DEF_INT_HANDLER_1( BF_Timer_Ch4_Int, Dd_TMR32_Int_Handler, (4) )

// Timer ch5
DEF_INT_HANDLER_1( BF_Timer_Ch5_Int, Dd_TMR32_Int_Handler, (5) )

// Timer ch6
DEF_INT_HANDLER_1( BF_Timer_Ch6_Int, Dd_TMR32_Int_Handler, (6) )

// Timer ch7
DEF_INT_HANDLER_1( BF_Timer_Ch7_Int, Dd_TMR32_Int_Handler, (7) )

// Timer ch8
DEF_INT_HANDLER_1( BF_Timer_Ch8_Int, Dd_TMR32_Int_Handler, (8) )

// Timer ch9
DEF_INT_HANDLER_1( BF_Timer_Ch9_Int, Dd_TMR32_Int_Handler, (9) )

// Timer ch10
DEF_INT_HANDLER_1( BF_Timer_Ch10_Int, Dd_TMR32_Int_Handler, (10) )

// Timer ch11
DEF_INT_HANDLER_1( BF_Timer_Ch11_Int, Dd_TMR32_Int_Handler, (11) )

// Timer ch12
DEF_INT_HANDLER_1( BF_Timer_Ch12_Int, Dd_TMR32_Int_Handler, (12) )

// Timer ch13
DEF_INT_HANDLER_1( BF_Timer_Ch13_Int, Dd_TMR32_Int_Handler, (13) )

// I2C ch0
DEF_INT_HANDLER_1( BF_I2c_Ch0_Int, Dd_I2C_Int_Handler, (0) )

// I2C ch1
DEF_INT_HANDLER_1( BF_I2c_Ch1_Int, Dd_I2C_Int_Handler, (1) )

// I2C ch2
DEF_INT_HANDLER_1( BF_I2c_Ch2_Int, Dd_I2C_Int_Handler, (2) )

// Up/Down Counter ch0
DEF_INT_HANDLER_1( BF_Updown_Counter_Ch0_Int, Dd_UDC_Int_Handler, (0) )

// Up/Down Counter ch1
DEF_INT_HANDLER_1( BF_Updown_Counter_Ch1_Int, Dd_UDC_Int_Handler, (1) )

// Up/Down Counter ch2
DEF_INT_HANDLER_1( BF_Updown_Counter_Ch2_Int, Dd_UDC_Int_Handler, (2) )

// Up/Down Counter ch3
DEF_INT_HANDLER_1( BF_Updown_Counter_Ch3_Int, Dd_UDC_Int_Handler, (3) )

// Up/Down Counter ch4
DEF_INT_HANDLER_1( BF_Updown_Counter_Ch4_Int, Dd_UDC_Int_Handler, (4) )

// Up/Down Counter ch5
DEF_INT_HANDLER_1( BF_Updown_Counter_Ch5_Int, Dd_UDC_Int_Handler, (5) )

// PWM ch0-3
DEF_INT_HANDLER_1( BF_Pwm_Ch0_Ch3_Int, Dd_PWM_Int_Handler, (0) )

// PWM ch4-7
DEF_INT_HANDLER_1( BF_Pwm_Ch4_Ch7_Int, Dd_PWM_Int_Handler, (4) )

// PWM ch8-11
DEF_INT_HANDLER_1( BF_Pwm_Ch8_Ch11_Int, Dd_PWM_Int_Handler, (8) )

// PWM ch12-15
DEF_INT_HANDLER_1( BF_Pwm_Ch12_Ch15_Int, Dd_PWM_Int_Handler, (12) )

// PWM ch0-3 STP
DEF_INT_HANDLER_1( BF_Pwm_Ch0_Ch3_Stp_Int, Dd_PWM_Int_Handler_STP, (0) )

// PWM ch4-7 STP
DEF_INT_HANDLER_1( BF_Pwm_Ch4_Ch7_Stp_Int, Dd_PWM_Int_Handler_STP, (4) )

// PWM ch8-11 STP
DEF_INT_HANDLER_1( BF_Pwm_Ch8_Ch11_Stp_Int, Dd_PWM_Int_Handler_STP, (8) )

// PWM ch12-15 STP
DEF_INT_HANDLER_1( BF_Pwm_Ch12_Ch15_Stp_Int, Dd_PWM_Int_Handler_STP, (12) )

// Audio I/F ch0
DEF_INT_HANDLER_1( BF_Audio_If_Ch0_Int, Dd_Audio_Int_Handler, (0) )

// Audio I/F ch1
DEF_INT_HANDLER_1( BF_Audio_If_Ch1_Int, Dd_Audio_Int_Handler, (1) )

// Audio I/F ch2
DEF_INT_HANDLER_1( BF_Audio_If_Ch2_Int, Dd_Audio_Int_Handler, (2) )

// Audio I/F ch3
DEF_INT_HANDLER_1( BF_Audio_If_Ch3_Int, Dd_Audio_Int_Handler, (3) )

// Audio I/F ch4
DEF_INT_HANDLER_1( BF_Audio_If_Ch4_Int, Dd_Audio_Int_Handler, (4) )

// Audio I/F ch5
DEF_INT_HANDLER_1( BF_Audio_If_Ch5_Int, Dd_Audio_Int_Handler, (5) )

// Peripheral HDMAC0 ch0
DEF_INT_HANDLER_1( BF_Peri_Hdmac0_Ch0_Int, Dd_HDMAC0_Int_Handler, (0) )

// Peripheral HDMAC0 ch1
DEF_INT_HANDLER_1( BF_Peri_Hdmac0_Ch1_Int, Dd_HDMAC0_Int_Handler, (1) )

// Peripheral HDMAC0 ch2
DEF_INT_HANDLER_1( BF_Peri_Hdmac0_Ch2_Int, Dd_HDMAC0_Int_Handler, (2) )

// Peripheral HDMAC0 ch3
DEF_INT_HANDLER_1( BF_Peri_Hdmac0_Ch3_Int, Dd_HDMAC0_Int_Handler, (3) )

// Peripheral HDMAC0 ch4
DEF_INT_HANDLER_1( BF_Peri_Hdmac0_Ch4_Int, Dd_HDMAC0_Int_Handler, (4) )

// Peripheral HDMAC0 ch5
DEF_INT_HANDLER_1( BF_Peri_Hdmac0_Ch5_Int, Dd_HDMAC0_Int_Handler_2, (5) )

// Peripheral HDMAC0 ch6
DEF_INT_HANDLER_1( BF_Peri_Hdmac0_Ch6_Int, Dd_HDMAC0_Int_Handler_2, (6) )

// Peripheral HDMAC0 ch7
DEF_INT_HANDLER_1( BF_Peri_Hdmac0_Ch7_Int, Dd_HDMAC0_Int_Handler, (7) )

// Peripheral HDMAC1 ch0
DEF_INT_HANDLER_1( BF_Peri_Hdmac1_Ch0_Int, Dd_HDMAC1_Int_Handler, (0) )

// Peripheral HDMAC1 ch1
DEF_INT_HANDLER_1( BF_Peri_Hdmac1_Ch1_Int, Dd_HDMAC1_Int_Handler, (1) )

// Peripheral HDMAC1 ch2
DEF_INT_HANDLER_1( BF_Peri_Hdmac1_Ch2_Int, Dd_HDMAC1_Int_Handler, (2) )

// Peripheral HDMAC1 ch3
DEF_INT_HANDLER_1( BF_Peri_Hdmac1_Ch3_Int, Dd_HDMAC1_Int_Handler, (3) )

// Peripheral HDMAC1 ch4
DEF_INT_HANDLER_1( BF_Peri_Hdmac1_Ch4_Int, Dd_HDMAC1_Int_Handler, (4) )

// Peripheral HDMAC1 ch5
DEF_INT_HANDLER_1( BF_Peri_Hdmac1_Ch5_Int, Dd_HDMAC1_Int_Handler, (5) )

// Peripheral HDMAC1 ch6
DEF_INT_HANDLER_1( BF_Peri_Hdmac1_Ch6_Int, Dd_HDMAC1_Int_Handler, (6) )

// Peripheral HDMAC1 ch7
DEF_INT_HANDLER_1( BF_Peri_Hdmac1_Ch7_Int, Dd_HDMAC1_Int_Handler, (7) )

// UART ch0 Reception
DEF_INT_HANDLER_1( BF_Uart_Ch0_Reception_Int, Dd_USIO_Int_Handler_Rx, (0) )

// UART ch1 Reception
DEF_INT_HANDLER_1( BF_Uart_Ch1_Reception_Int, Dd_USIO_Int_Handler_Rx, (1) )

// UART ch2 Reception
DEF_INT_HANDLER_1( BF_Uart_Ch2_Reception_Int, Dd_USIO_Int_Handler_Rx, (2) )

// UART ch3 Reception
DEF_INT_HANDLER_1( BF_Uart_Ch3_Reception_Int, Dd_USIO_Int_Handler_Rx, (3) )

// UART ch4 Reception
DEF_INT_HANDLER_1( BF_Uart_Ch4_Reception_Int, Dd_USIO_Int_Handler_Rx, (4) )

// UART ch5 Reception
DEF_INT_HANDLER_1( BF_Uart_Ch5_Reception_Int, Dd_USIO_Int_Handler_Rx, (5) )

// UART ch6 Reception
DEF_INT_HANDLER_1( BF_Uart_Ch6_Reception_Int, Dd_USIO_Int_Handler_Rx, (6) )

// UART ch7 Reception
DEF_INT_HANDLER_1( BF_Uart_Ch7_Reception_Int, Dd_USIO_Int_Handler_Rx, (7) )

// UART ch0 Transmission
DEF_INT_HANDLER_1( BF_Uart_Ch0_Transmission_Int, Dd_USIO_Int_Handler_Tx, (0) )

// UART ch1 Transmission
DEF_INT_HANDLER_1( BF_Uart_Ch1_Transmission_Int, Dd_USIO_Int_Handler_Tx, (1) )

// UART ch2 Transmission
DEF_INT_HANDLER_1( BF_Uart_Ch2_Transmission_Int, Dd_USIO_Int_Handler_Tx, (2) )

// UART ch3 Transmission
DEF_INT_HANDLER_1( BF_Uart_Ch3_Transmission_Int, Dd_USIO_Int_Handler_Tx, (3) )

// UART ch4 Transmission
DEF_INT_HANDLER_1( BF_Uart_Ch4_Transmission_Int, Dd_USIO_Int_Handler_Tx, (4) )

// UART ch5 Transmission
DEF_INT_HANDLER_1( BF_Uart_Ch5_Transmission_Int, Dd_USIO_Int_Handler_Tx, (5) )

// UART ch6 Transmission
DEF_INT_HANDLER_1( BF_Uart_Ch6_Transmission_Int, Dd_USIO_Int_Handler_Tx, (6) )

// UART ch7 Transmission
DEF_INT_HANDLER_1( BF_Uart_Ch7_Transmission_Int, Dd_USIO_Int_Handler_Tx, (7) )

// SPI ch0
DEF_INT_HANDLER_1( BF_Spi_Ch0_Int, Dd_SPI_Int_Handler, (0) )

// SPI ch1
DEF_INT_HANDLER_1( BF_Spi_Ch1_Int, Dd_SPI_Int_Handler, (1) )

// SPI ch2
DEF_INT_HANDLER_1( BF_Spi_Ch2_Int, Dd_SPI_Int_Handler, (2) )

// PDM ch0 DMA0
DEF_INT_HANDLER_1( BF_Pdm_Ch0_Dma0_Int, Dd_Pdm_Dma0_Int_Handler, (0) )

// PDM ch0 DMA1
DEF_INT_HANDLER_1( BF_Pdm_Ch0_Dma1_Int, Dd_Pdm_Dma1_Int_Handler, (0) )

// PDM ch0 Fifo Overflow
DEF_INT_HANDLER_1( BF_Pdm_Ch0_Fifo_Overflow_Int, Dd_Pdm_OverFlow_Int_Handler, (0) )

// PDM ch1 DMA0
DEF_INT_HANDLER_1( BF_Pdm_Ch1_Dma0_Int, Dd_Pdm_Dma0_Int_Handler, (1) )

// PDM ch1 DMA1
DEF_INT_HANDLER_1( BF_Pdm_Ch1_Dma1_Int, Dd_Pdm_Dma1_Int_Handler, (1) )

// PDM ch1 Fifo Overflow
DEF_INT_HANDLER_1( BF_Pdm_Ch1_Fifo_Overflow_Int, Dd_Pdm_OverFlow_Int_Handler, (1) )

// SLIMBUS ch0
DEF_INT_HANDLER_1( BF_Slimbus_Ch0_Int, Dd_Slimbus_Int_Handler, (E_DD_SLIMBUS_CH0) )

// SLIMBUS ch1
DEF_INT_HANDLER_1( BF_Slimbus_Ch1_Int, Dd_Slimbus_Int_Handler, (E_DD_SLIMBUS_CH1) )

// SLIMBUS ch2
DEF_INT_HANDLER_1( BF_Slimbus_Ch2_Int, Dd_Slimbus_Int_Handler, (E_DD_SLIMBUS_CH2) )

// SLIMBUS ch3
DEF_INT_HANDLER_1( BF_Slimbus_Ch3_Int, Dd_Slimbus_Int_Handler, (E_DD_SLIMBUS_CH3) )

// Image SDRAMC0
DEF_INT_HANDLER_1( BF_Sdramc_Ch0_Int, Im_SDRAMC_Int_Handler, (E_IM_SDRAMC_CH_0) )

// Image SDRAMC1
DEF_INT_HANDLER_1( BF_Sdramc_Ch1_Int, Im_SDRAMC_Int_Handler, (E_IM_SDRAMC_CH_1) )

// Image MXIC Unit0
DEF_INT_HANDLER_1( BF_Mxic_Unit0_Int, Im_MXIC_Int_Handler, (E_IM_MXIC_UNIT_0) )

// Image MXIC Unit1
DEF_INT_HANDLER_1( BF_Mxic_Unit1_Int, Im_MXIC_Int_Handler, (E_IM_MXIC_UNIT_1) )

// Image MXIC Unit2
DEF_INT_HANDLER_1( BF_Mxic_Unit2_Int, Im_MXIC_Int_Handler, (E_IM_MXIC_UNIT_2) )

// Image MXIC Unit3
DEF_INT_HANDLER_1( BF_Mxic_Unit3_Int, Im_MXIC_Int_Handler, (E_IM_MXIC_UNIT_3) )

// Image MXIC Unit4
DEF_INT_HANDLER_1( BF_Mxic_Unit4_Int, Im_MXIC_Int_Handler, (E_IM_MXIC_UNIT_4) )

// Image MXIC Unit5
DEF_INT_HANDLER_1( BF_Mxic_Unit5_Int, Im_MXIC_Int_Handler, (E_IM_MXIC_UNIT_5) )

// Image MXIC Unit6
DEF_INT_HANDLER_1( BF_Mxic_Unit6_Int, Im_MXIC_Int_Handler, (E_IM_MXIC_UNIT_6) )

// Image SEN
DEF_INT_HANDLER_1( BF_Sen_Int, Im_PRO_SEN_Int_Handler, () )

// Image SEN VD
DEF_INT_HANDLER_1( BF_Sen_Vd_Int, Im_PRO_SEN_VD_Int_Handler, () )

// Image SEN HD
DEF_INT_HANDLER_1( BF_Sen_Hd_Int, Im_PRO_SEN_HD_Int_Handler, () )

// Image SENSOR IF Macro0
DEF_INT_HANDLER_1( BF_Sensor_If_Macro0_Int, Im_PRO_SEN_Sensor_If_Int_Handler_0, () )

// Image SENSOR IF Macro1
DEF_INT_HANDLER_1( BF_Sensor_If_Macro1_Int, Im_PRO_SEN_Sensor_If_Int_Handler_1, () )

// Image SENSOR IF Macro2
DEF_INT_HANDLER_1( BF_Sensor_If_Macro2_Int, Im_PRO_SEN_Sensor_If_Int_Handler_2, () )

// Image SENSOR IF Macro3
DEF_INT_HANDLER_1( BF_Sensor_If_Macro3_Int, Im_PRO_SEN_Sensor_If_Int_Handler_3, () )

// Image SRO Unit1
DEF_INT_HANDLER_1( BF_Sro_Unit1_Int, Im_PRO_SROTOP_Pipe1_Int_Handler, () )

// Image SRO Unit1 VD
DEF_INT_HANDLER_1( BF_Sro_Unit1_Vd_Int, Im_PRO_SROTOP_Pipe1_VD_Int_Handler, () )

// Image SRO Unit1 HD
DEF_INT_HANDLER_1( BF_Sro_Unit1_Hd_Int, Im_PRO_SROTOP_Pipe1_HD_Int_Handler, () )

// Image SRO Unit2
DEF_INT_HANDLER_1( BF_Sro_Unit2_Int, Im_PRO_SROTOP_Pipe2_Int_Handler, () )

// Image SRO Unit2 VD
DEF_INT_HANDLER_1( BF_Sro_Unit2_Vd_Int, Im_PRO_SROTOP_Pipe2_VD_Int_Handler, () )

// Image SRO Unit2 HD
DEF_INT_HANDLER_1( BF_Sro_Unit2_Hd_Int, Im_PRO_SROTOP_Pipe2_HD_Int_Handler, () )

// Image B2B Unit1
DEF_INT_HANDLER_1( BF_B2b_Unit1_Int, Im_PRO_B2BTOP_Pipe1_Int_Handler, () )

// Image B2B Unit1 VD
DEF_INT_HANDLER_1( BF_B2b_Unit1_Vd_Int, Im_PRO_B2BTOP_Pipe1_VD_Int_Handler, () )

// Image B2B Unit1 HD
DEF_INT_HANDLER_1( BF_B2b_Unit1_Hd_Int, Im_PRO_B2BTOP_Pipe1_HD_Int_Handler, () )

// Image B2B Unit2
DEF_INT_HANDLER_1( BF_B2b_Unit2_Int, Im_PRO_B2BTOP_Pipe2_Int_Handler, () )

// Image B2B Unit2 VD
DEF_INT_HANDLER_1( BF_B2b_Unit2_Vd_Int, Im_PRO_B2BTOP_Pipe2_VD_Int_Handler, () )

// Image B2B Unit2 HD
DEF_INT_HANDLER_1( BF_B2b_Unit2_Hd_Int, Im_PRO_B2BTOP_Pipe2_HD_Int_Handler, () )

// Image STAT
DEF_INT_HANDLER_1( BF_Stat_Int, Im_PRO_STAT_Int_Handler, () )

// Image PAS
DEF_INT_HANDLER_1( BF_Pas_Int, Im_PRO_PASTOP_Int_Handler, () )

// Image PAS VD
DEF_INT_HANDLER_1( BF_Pas_Vd_Int, Im_PRO_PASTOP_VD_Int_Handler, () )

// Image PAS HD
DEF_INT_HANDLER_1( BF_Pas_Hd_Int, Im_PRO_PASTOP_HD_Int_Handler, () )

// Image B2R Unit1
DEF_INT_HANDLER_1( BF_B2r_Unit1_Int, Im_B2R_Int_Handler, (D_IM_B2R_PIPE1) )

// Image B2R Unit2
DEF_INT_HANDLER_1( BF_B2r_Unit2_Int, Im_B2R_Int_Handler, (D_IM_B2R_PIPE2) )

// Image LTM Unit1
DEF_INT_HANDLER_1( BF_Ltm_Unit1_Int, Im_LTM_Int_Handler, (D_IM_LTM_PIPE1) )

// Image LTM Unit2
DEF_INT_HANDLER_1( BF_Ltm_Unit2_Int, Im_LTM_Int_Handler, (D_IM_LTM_PIPE2) )

// Image R2Y Unit1
DEF_INT_HANDLER_1( BF_R2y_Unit1_Int, Im_R2Y_Int_Handler, (D_IM_R2Y_PIPE1) )

// Image R2Y Unit2
DEF_INT_HANDLER_1( BF_R2y_Unit2_Int, Im_R2Y_Int_Handler, (D_IM_R2Y_PIPE2) )

// Image CNR Unit1
DEF_INT_HANDLER_1( BF_Cnr_Unit1_Int, Im_CNR0_Int_Handler, () )

// Image CNR Unit2
DEF_INT_HANDLER_1( BF_Cnr_Unit2_Int, Im_CNR1_Int_Handler, () )

// Image IIP
DEF_INT_HANDLER_1( BF_Iip_Int, Im_IIP_Int_Handler, () )

// Image SHDR
DEF_INT_HANDLER_1( BF_Shdr_Int, Im_SHDR_Int_Handler, () )

// Image SHDR Line
DEF_INT_HANDLER_1( BF_Shdr_Line_Int, Im_SHDR_Line_Int_Handler, () )

// Image ME
DEF_INT_HANDLER_1( BF_Me_Int, Im_ME_Int_Handler, () )

// Image Graphics
DEF_INT_HANDLER_0( BF_Graphics_Int )

// Image JPEG
DEF_INT_HANDLER_1( BF_Jpeg_Int, Im_JPEG_Int_Handler, () )

// Image RAW
DEF_INT_HANDLER_1( BF_Raw_Int, Im_RAW_Int_Handler, () )

// Image IPU
static VOID BF_Ipu_Int_Sub(VOID) __attribute__ ((noinline));
static VOID BF_Ipu_Int_Sub(VOID)
{
// --- REMOVE_AHIP BEGIN ---
	IP_AHIP_Int_Handler(0);
//	FJ_HostAHIPIntCB();
// --- REMOVE_AHIP END ---
}
DEF_INT_HANDLER_1( BF_Ipu_Int, BF_Ipu_Int_Sub, () )

// Image ELA Unit
DEF_INT_HANDLER_1( BF_Ela_Unit_Int, Im_ELA_Int_Handler, () )

// Image Xch
DEF_INT_HANDLER_1( BF_Xch_Int, Im_Xch_Int_Handler, () )

// Image FPT Unit0
DEF_INT_HANDLER_1( BF_Fpt_Unit0_Int, Im_FPT_Int_Handler, (0) )

// Image FPT Unit1
DEF_INT_HANDLER_1( BF_Fpt_Unit1_Int, Im_FPT_Int_Handler, (1) )

// Image DISP LCD
DEF_INT_HANDLER_1( BF_Disp_Lcd_Int, Im_DISP_Int_Handler, () )

// Image DISP HDMI
DEF_INT_HANDLER_1( BF_Disp_Hdmi_Int, Im_DISP_Int_Handler, () )

// Image DISP MIPI DSI
DEF_INT_HANDLER_1( BF_Disp_Mipi_Dsi_Int, Im_DISP_Int_Handler, () )

// Image MIPI DSI Tx
DEF_INT_HANDLER_1( BF_Mipi_Dsi_Tx_Int, Im_MIPI_Int_Handler, () )

// Image DISP DWCH
DEF_INT_HANDLER_0( BF_Disp_Dwch_Int )
	// Corresponding to the version 1.0 or later.

// Image RDMA
DEF_INT_HANDLER_1( BF_Rdma_Int, Im_RDMA_Int_Handler, () )

// HDMI Master
DEF_INT_HANDLER_1( BF_Hdmi_Master_Int, Im_HDMI_Int_Handler, () )

// HDMI Cec
DEF_INT_HANDLER_1( BF_Hdmi_Cec_Int, Im_HDMI_Int_Handler, () )

// BMH Unit0
DEF_INT_HANDLER_1( BF_BMH_Unit0_Int, Im_BMH_Int_Handler, (0) )

// BMH Unit1
DEF_INT_HANDLER_1( BF_BMH_Unit1_Int, Im_BMH_Int_Handler, (1) )

// RIBERY RBR
DEF_INT_HANDLER_1( BF_Ribery_Rbr_Int, RB_HRBR_IntHandler, () )

// RIBERY RWDT
DEF_INT_HANDLER_1( BF_Ribery_Rwdt_Int, RB_HRWDT_IntHandler, () )

// INTERPROCESSOR0
DEF_INT_HANDLER_1( BF_Interprocessor0_Int, H265_Sirius_dummy_system_inHandler, () )

// INTERPROCESSOR1
DEF_INT_HANDLER_0( BF_Interprocessor1_Int )

// INTERPROCESSOR2
DEF_INT_HANDLER_1( BF_Interprocessor2_Int, H265_Sirius_dummy_dec_inHandler, () )

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
// INTERPROCESSOR3
DEF_INT_HANDLER_1( BF_Interprocessor3_Int, H265_Sirius_dummy_enc_inHandler, () )
// --- REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
DEF_INT_HANDLER_0( BF_Interprocessor3_Int )
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---

// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
// HEVC Encoder
DEF_INT_HANDLER_1( BF_Wave_Int, vpu_irq_handler, () )
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

// DSP Peripheral Unit Ipcu0
DEF_INT_HANDLER_1( BF_Dsp_Peripheral_Unit_Ipcu0_Int, FJ_HostDspIPCU0IntCB, () )

// DSP Peripheral Unit Ipcu1
DEF_INT_HANDLER_1( BF_Dsp_Peripheral_Unit_Ipcu1_Int, FJ_HostDspIPCU1IntCB, () )

// DSP Peripheral Unit Ipcu2
DEF_INT_HANDLER_1( BF_Dsp_Peripheral_Unit_Ipcu2_Int, FJ_HostDspIPCU2IntCB, () )

// DSP Peripheral Unit Ipcu3
DEF_INT_HANDLER_1( BF_Dsp_Peripheral_Unit_Ipcu3_Int, FJ_HostDspIPCU3IntCB, () )

// DSP Debug
DEF_INT_HANDLER_0( BF_Dsp_Debug_Int )

// NandFlash
DEF_INT_HANDLER_1( BF_Nand_Flash_Int, MWNF_Int_Handler, () )

// PCIE ch0 Msi
DEF_INT_HANDLER_1( BF_Pcie_Ch0_Msi_Int, Dd_Pcie_Rc_Ch0_Int_Trs_Ics_Msi_Handler, () )

// PCIE ch0 System Error
DEF_INT_HANDLER_1( BF_Pcie_Ch0_System_Error_Int, Dd_Pcie_Rc_Ch0_Int_Sys_Err_Handler, () )

// PCIE ch0 Intx
static VOID BF_Pcie_Ch0_Intx_Int_Sub(VOID) __attribute__ ((noinline));
static VOID BF_Pcie_Ch0_Intx_Int_Sub(VOID)
{
	if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC){
		Dd_Pcie_Rc_Ch0_Int_Ep_Handler();
	}
	else if(gDD_PCIE_EP_RE_TYPE == E_DD_PCIE_RE_TYPE_EP){
		Dd_Pcie_Ep_Ch0_Int_Ep_Handler();
	}
}
DEF_INT_HANDLER_1( BF_Pcie_Ch0_Intx_Int, BF_Pcie_Ch0_Intx_Int_Sub, () );

// PCIE ch0 Core
static VOID BF_Pcie_Ch0_Core_Int_Sub(VOID) __attribute__ ((noinline));
static VOID BF_Pcie_Ch0_Core_Int_Sub(VOID)
{
	if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC){
		Dd_Pcie_Rc_Ch0_Int_Own_Handler();
	}
	else if(gDD_PCIE_EP_RE_TYPE == E_DD_PCIE_RE_TYPE_EP){
		Dd_Pcie_Ep_Ch0_Int_Own_Handler();
	}
}
DEF_INT_HANDLER_1( BF_Pcie_Ch0_Core_Int, BF_Pcie_Ch0_Core_Int_Sub, () )

// PCIE ch0 DMA
static VOID BF_Pcie_Ch0_DMA_Int_Sub(VOID) __attribute__ ((noinline));
static VOID BF_Pcie_Ch0_DMA_Int_Sub(VOID)
{
	if(gDD_PCIE_RC_RE_TYPE == E_DD_PCIE_RE_TYPE_RC){
		Dd_Pcie_Rc_Ch0_Int_Dma_Handler();
	}
	else if(gDD_PCIE_EP_RE_TYPE == E_DD_PCIE_RE_TYPE_EP){
		Dd_Pcie_Ep_Ch0_Int_Dma_Handler();
	}
}
DEF_INT_HANDLER_1( BF_Pcie_Ch0_DMA_Int, BF_Pcie_Ch0_DMA_Int_Sub, () )

// PCIE ch1 Msi
DEF_INT_HANDLER_1( BF_Pcie_Ch1_Msi_Int, Dd_Pcie_Rc_Ch1_Int_Trs_Ics_Msi_Handler, () )

// PCIE ch1 System Error
DEF_INT_HANDLER_1( BF_Pcie_Ch1_System_Error_Int, Dd_Pcie_Rc_Ch1_Int_Sys_Err_Handler, () )

// PCIE ch1 Intx
DEF_INT_HANDLER_1( BF_Pcie_Ch1_Intx_Int, Dd_Pcie_Rc_Ch1_Int_Ep_Handler, () )

// PCIE ch1 Core
DEF_INT_HANDLER_1( BF_Pcie_Ch1_Core_Int, Dd_Pcie_Rc_Ch1_Int_Own_Handler, () )

// PCIE ch1 DMA
DEF_INT_HANDLER_1( BF_Pcie_Ch1_DMA_Int, Dd_Pcie_Rc_Ch1_Int_Dma_Handler, () )

// External Storage XDMAC ch0
DEF_INT_HANDLER_1( BF_Exs_Xdmac_Ch0_Int, Dd_XDMAC_Int_Handler, (0) )

// External Storage XDMAC ch1
DEF_INT_HANDLER_1( BF_Exs_Xdmac_Ch1_Int, Dd_XDMAC_Int_Handler, (1) )

// External Storage XDMAC ch2
DEF_INT_HANDLER_1( BF_Exs_Xdmac_Ch2_Int, Dd_XDMAC_Int_Handler, (2) )

// External Storage XDMAC ch3
DEF_INT_HANDLER_1( BF_Exs_Xdmac_Ch3_Int, Dd_XDMAC_Int_Handler, (3) )

// External Storage HDMAC ch0
DEF_INT_HANDLER_1( BF_Exs_Hdmac_Ch0_Int, Dd_HDMAC_Int_Handler, (0) )

// External Storage HDMAC ch1
DEF_INT_HANDLER_1( BF_Exs_Hdmac_Ch1_Int, Dd_HDMAC_Int_Handler, (1) )

// External Storage HDMAC ch2
DEF_INT_HANDLER_1( BF_Exs_Hdmac_Ch2_Int, Dd_HDMAC_Int_Handler, (2) )

// External Storage HDMAC ch3
DEF_INT_HANDLER_1( BF_Exs_Hdmac_Ch3_Int, Dd_HDMAC_Int_Handler, (3) )

// USB20 Hdc
// --- REMOVE_USB BEGIN ---
#if !defined(CO_LINUX_VERSION_ON) && defined(CO_USB_ON)
DEF_INT_HANDLER_1( BF_Usb20_Hdc_Int, HdcDev_IntHandler, () )
#else
// --- REMOVE_USB END ---
DEF_INT_HANDLER_0( BF_Usb20_Hdc_Int )
// --- REMOVE_USB BEGIN ---
#endif
// --- REMOVE_USB END ---

// USB20 Host EHCI
DEF_INT_HANDLER_0( BF_Usb20_Host_Ehci_Int )

// USB20 Host OHCI
DEF_INT_HANDLER_0( BF_Usb20_Host_Ohci_Int )

// USB30
DEF_INT_HANDLER_0( BF_Usb30_Int )

// SD ch3(SD Int)
DEF_INT_HANDLER_1( BF_Sd_Ch3_Int, MWSDIO0_Ex3_IntHandler, () )

// SD ch3(SDIO Int)
DEF_INT_HANDLER_1( BF_Sd_Ch3_Sdio_Int, MWSDIO0_Ex3_IntHandler, () )

// SD ch3(Wakeup Int)
DEF_INT_HANDLER_0( BF_Sd_Ch3_Wakeup_Int )

// SD ch2(SD Int from UHS1)
DEF_INT_HANDLER_1( BF_Sd_Ch2_Uhs1_Int, MWSDIO1_Ex3_IntHandler, () )

// SD ch2(SDIO Int)
DEF_INT_HANDLER_1( BF_Sd_Ch2_Sdio_Int, MWSDIO1_Ex3_IntHandler, () )

// SD ch2(Wakeup Int from UHS1)
DEF_INT_HANDLER_0( BF_Sd_Ch2_Uhs1_Wakeup_Int )

// SD ch2(SD Int from UHS2)
DEF_INT_HANDLER_1( BF_Sd_Ch2_Uhs2_Int, MWSDIO1_Uhs2_IntHandler, () )

// SD ch2(Wakeup Int from UHS2)
DEF_INT_HANDLER_0( BF_Sd_Ch2_Uhs2_Wakeup_Int )

// SD ch1(SD Int)
DEF_INT_HANDLER_0( BF_Sd_Ch1_Int )

// SD ch1(SDIO Int)
DEF_INT_HANDLER_0( BF_Sd_Ch1_Sdio_Int )

// SD ch1(Wakeup Int)
DEF_INT_HANDLER_0( BF_Sd_Ch1_Wakeup_Int )

// eMMC Int
DEF_INT_HANDLER_1( BF_Emmc_Int, MWEM_IntHandler, () )

// eMMC Wakeup Int
DEF_INT_HANDLER_0( BF_Emmc_Wakeup_Int )

// RELC
// ### REMOVE_RELEASE BEGIN
#if 1
DEF_INT_HANDLER_1( BF_Relc_Int, Dd_RELC_Int_Handler, () )
#else
// ### REMOVE_RELEASE END
DEF_INT_HANDLER_0( BF_Relc_Int )
// ### REMOVE_RELEASE BEGIN
#endif
// ### REMOVE_RELEASE END

// NETSEC A
DEF_INT_HANDLER_1( BF_Netsec_A_Int, BF_Netsec_Int_Handler, () )

// NETSEC B
DEF_INT_HANDLER_0( BF_Netsec_B_Int )

// SD and PCIe card detect
static VOID BF_Sd_Pcie_Detect_Int_Sub(VOID) __attribute__ ((noinline));
static VOID BF_Sd_Pcie_Detect_Int_Sub(VOID)
{
	ULONG intst;

	Dd_EXS_Int_Handler(&intst);

	if((intst & (D_DD_EXS_SD4_RIS_BIT | D_DD_EXS_SD4_FIS_BIT)) != 0) {
		// SD4 card detect
		MWSDIO0_Exs_IntHandler(intst);
	}
	else if((intst & (D_DD_EXS_SD3_RIS_BIT | D_DD_EXS_SD3_FIS_BIT)) != 0) {
		// SD3 card detect
		MWSDIO1_Exs_IntHandler(intst);
	}
	else if((intst & (D_DD_EXS_PCIe0_RIS_BIT | D_DD_EXS_PCIe0_FIS_BIT)) != 0) {
		// PCIe0 card detect
		Dd_Pcie_Rc_Ch0_Int_Exs_Handler(intst);
	}
	else if((intst & (D_DD_EXS_PCIe1_RIS_BIT | D_DD_EXS_PCIe1_FIS_BIT)) != 0) {
		// PCIe1 card detect
		Dd_Pcie_Rc_Ch1_Int_Exs_Handler(intst);
	}
	else{
		// Do nothing.
	}
}
DEF_INT_HANDLER_1( BF_Sd_Pcie_Detect_Int, BF_Sd_Pcie_Detect_Int_Sub, () )

/* ----------------------------------------------------------------------------- */
/* Base F/W API																	 */
/* ----------------------------------------------------------------------------- */
/**
Set configuration of External Interrupt.
 */
FJ_ERR_CODE fj_ext_interrupt_set_config( FJ_GPIO_PORT port, FJ_EXT_INT_CONFIG* int_config )
{
	UINT32 ext_int_num;
	UINT32 ch_num;

	if (int_config->int_priority > D_BF_INTERRUPT_PRI_MAX) {
		BF_Debug_Print_Error(("[fj_ext_interrupt_set_config() Error!(Line=%d)] int_priority=%d \n", __LINE__, int_config->int_priority));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if (int_config->trigger_type >= FJ_EXT_INT_TRIGGER_TYPE_MAX) {
		BF_Debug_Print_Error(("[fj_ext_interrupt_set_config() Error!(Line=%d)] trigger_type=%d \n", __LINE__, int_config->trigger_type));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if ((int_config->cpu_target < FJ_EXT_INT_TARGET_CPU_0) || (int_config->cpu_target > FJ_EXT_INT_TARGET_CPU_0_1_2_3)) {
		BF_Debug_Print_Error(("[fj_ext_interrupt_set_config() Error!(Line=%d)] cpu_target=0x%d \n", __LINE__, int_config->cpu_target));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	if (int_config->int_callback == NULL) {
		BF_Debug_Print_Error(("[fj_ext_interrupt_set_config() Error!(Line=%d)] int_handler=%10p \n", __LINE__, int_config->int_callback));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	for( ext_int_num = 0; ext_int_num < (E_EINT_MAX - E_EINT16); ext_int_num++ ){
		if( port == gBF_Interrupt_Ext_Mng[ext_int_num].port ){
			break;
		}
	}

	if( ext_int_num >= (E_EINT_MAX - E_EINT16) ){
		BF_Debug_Print_Error(("[fj_ext_interrupt_set_config() Error!(Line=%d)] Port=%d \n", __LINE__, port));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	Dd_ARM_Critical_Section_Start( gbf_ext_interrupt_spin_lock );
	
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
	if( port == FJ_GPIO_PB4 ){
		Dd_Top_Set_Gpio_Function( E_DD_TOP_GPIO_PB4, D_DD_TOP_GPIO_FUNC_HWMODE );	// EINT32 input enable
	}
// --- REMOVE_ES3_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---

	gBF_Interrupt_Ext_Mng[ext_int_num].int_handler = int_config->int_callback;
	ch_num = gBF_Interrupt_Ext_Mng[ext_int_num].eint;

	// Set the interrupt mask in the "Interrupt Masking Register (EIMASK)". 
	Dd_EXIU_Set_EIMASK_Mask(ch_num);

	Dd_EXIU_Set_EISRCSEL_Ext(ch_num);

	// Set the external interrupt request signal in the "External Interrupt Level Setting Register (EILVL)". 
	if( int_config->trigger_type == D_BF_INTERRUPT_EXT_LOW_LEVEL || int_config->trigger_type == D_BF_INTERRUPT_EXT_FALLING_EDGE ) {
		Dd_EXIU_Set_EILVL_Lo(ch_num);
	}
	else if( int_config->trigger_type == D_BF_INTERRUPT_EXT_HIGH_LEVEL || int_config->trigger_type == D_BF_INTERRUPT_EXT_RISING_EDGE ) {
		Dd_EXIU_Set_EILVL_Hi(ch_num);
	}

	// Set the external interrupt request signal in the "Interrupt Detection Method Setting Register (EIEDG)". 
	if( int_config->trigger_type == D_BF_INTERRUPT_EXT_LOW_LEVEL || int_config->trigger_type == D_BF_INTERRUPT_EXT_HIGH_LEVEL ){
		Dd_EXIU_Set_EIEDG_Level(ch_num);
	}
	else if( int_config->trigger_type == D_BF_INTERRUPT_EXT_FALLING_EDGE || int_config->trigger_type == D_BF_INTERRUPT_EXT_RISING_EDGE ){
		Dd_EXIU_Set_EIEDG_Edge(ch_num);
	}

	// Clear the interrupts requests by writing to the "Interrupt Clear Register (EIREQCLR)". 
	Dd_EXIU_Set_EIREQCLR_Ch(ch_num);

	// In order to release the mask on the interrupt ports, set the "Interrupt Masking Register (EIMASK)". 
	Dd_EXIU_Set_EIMASK_Release(ch_num);

	OS_User_DisableInt((OS_USER_INTID)gBF_Interrupt_Ext_Mng[ext_int_num].int_num);

	Dd_ARM_Critical_Section_End( gbf_ext_interrupt_spin_lock );

	return FJ_ERR_OK;
}

/**
Set enable of External Interrupt.
*/
FJ_ERR_CODE fj_ext_interrupt_set_enable( FJ_GPIO_PORT port, UCHAR enable )
{
	UINT32 ext_int_num;
	UINT32 ch_num;

	for( ext_int_num = 0; ext_int_num < (E_EINT_MAX - E_EINT16) ; ext_int_num++ ) {
		if( port == gBF_Interrupt_Ext_Mng[ext_int_num].port ) {
			break;
		}
	}

	if( ext_int_num == (E_EINT_MAX - E_EINT16) ) {
		BF_Debug_Print_Error(("[fj_ext_interrupt_set_config() Error!(Line=%d)] Port=%d \n", __LINE__, port));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	Dd_ARM_Critical_Section_Start( gbf_ext_interrupt_spin_lock );
	
	ch_num = gBF_Interrupt_Ext_Mng[ext_int_num].eint;

	if( gBF_Interrupt_Ext_Mng[ext_int_num].int_handler ) {
		if( enable == 0 ) {
			// Set the interrupt mask in the "Interrupt Masking Register (EIMASK)". 
			Dd_EXIU_Set_EIMASK_Mask(ch_num);
			// Clear the interrupts requests by writing to the "Interrupt Clear Register (EIREQCLR)". 
			Dd_EXIU_Set_EIREQCLR_Ch(ch_num);
			// Set GIC Disable and Enable contrl.
			OS_User_DisableInt((OS_USER_INTID)gBF_Interrupt_Ext_Mng[ext_int_num].int_num);
		}
		else {
			// Set GIC Disable and Enable contrl.
			OS_User_EnableInt((OS_USER_INTID)gBF_Interrupt_Ext_Mng[ext_int_num].int_num, (OS_USER_INTLV)D_DD_GIC_PRI15);
			// Clear the interrupts requests by writing to the "Interrupt Clear Register (EIREQCLR)". 
			Dd_EXIU_Set_EIREQCLR_Ch(ch_num);
			// In order to release the mask on the interrupt ports, set the "Interrupt Masking Register (EIMASK)". 
			Dd_EXIU_Set_EIMASK_Release(ch_num);
		}
	}

	Dd_ARM_Critical_Section_End( gbf_ext_interrupt_spin_lock );
	
	return FJ_ERR_OK;
}

/**
Set trigger level of External Interrupt.
*/
FJ_ERR_CODE fj_ext_interrupt_set_level( FJ_GPIO_PORT port, UCHAR level )
{
	UINT32 ext_int_num;
	UINT32 ch_num;

	for( ext_int_num = 0; ext_int_num < (E_EINT_MAX - E_EINT16); ext_int_num++ ){
		if( port == gBF_Interrupt_Ext_Mng[ext_int_num].port ){
			break;
		}
	}

	if( ext_int_num == (E_EINT_MAX - E_EINT16) ){
		BF_Debug_Print_Error(("[fj_ext_interrupt_set_level() Error!] Port=%d \n", port));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	Dd_ARM_Critical_Section_Start( gbf_ext_interrupt_spin_lock );
	
	ch_num = gBF_Interrupt_Ext_Mng[ext_int_num].eint;

	// Set the interrupt mask in the "Interrupt Masking Register (EIMASK)". 
	Dd_EXIU_Set_EIMASK_Mask(ch_num);

	if( level == D_BF_INTERRUPT_EXT_LOW_LEVEL || level == D_BF_INTERRUPT_EXT_FALLING_EDGE ){
		Dd_EXIU_Set_EILVL_Lo(ch_num);
	}
	else if( level == D_BF_INTERRUPT_EXT_HIGH_LEVEL || level == D_BF_INTERRUPT_EXT_RISING_EDGE ){
		Dd_EXIU_Set_EILVL_Hi(ch_num);
	}

	// Clear the interrupts requests by writing to the "Interrupt Clear Register (EIREQCLR)". 
	Dd_EXIU_Set_EIREQCLR_Ch(ch_num);
	// In order to release the mask on the interrupt ports, set the "Interrupt Masking Register (EIMASK)". 
	Dd_EXIU_Set_EIMASK_Release(ch_num);

	Dd_ARM_Critical_Section_End( gbf_ext_interrupt_spin_lock );

	return FJ_ERR_OK;
}

/**
Get trigger level of External Interrupt.
*/
FJ_ERR_CODE fj_ext_interrupt_get_level( FJ_GPIO_PORT port, UCHAR* level )
{
	INT32 ret;
	UINT32 ext_int_num;
	UINT32 ch_num;

	for( ext_int_num = 0; ext_int_num < (E_EINT_MAX - E_EINT16); ext_int_num++ ){
		if( port == gBF_Interrupt_Ext_Mng[ext_int_num].port ){
			break;
		}
	}

	if( ext_int_num == (E_EINT_MAX - E_EINT16) ){
		BF_Debug_Print_Error(("[fj_ext_interrupt_get_level() Error!] Port=%d \n", port));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ch_num = gBF_Interrupt_Ext_Mng[ext_int_num].eint;

	ret = Dd_EXIU_Get_EILVL(ch_num, (UINT32*)level);
	if ( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(("[fj_ext_interrupt_get_level() Error!] ret=%d \n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_ext_interrupt_clear_req( FJ_GPIO_PORT port )
{
	INT32 ret;
	UINT32 ext_int_num;
	UINT32 ch_num;

	for( ext_int_num = 0; ext_int_num < (E_EINT_MAX - E_EINT16); ext_int_num++ ){
		if( port == gBF_Interrupt_Ext_Mng[ext_int_num].port ){
			break;
		}
	}

	if( ext_int_num == (E_EINT_MAX - E_EINT16) ){
		BF_Debug_Print_Error(("[fj_ext_interrupt_clear_req() Error!] Port=%d \n", port));
		return FJ_ERR_NG_INPUT_PARAM;
	}

	ch_num = gBF_Interrupt_Ext_Mng[ext_int_num].eint;

	ret = Dd_EXIU_Set_EIREQCLR_Ch(ch_num);
	if ( ret != D_DDIM_OK ) {
		BF_Debug_Print_Error(("[fj_ext_interrupt_clear_req() Error!] ret=%d \n", ret));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
Set resource to Generic Interrupt Controller.
*/
FJ_ERR_CODE fj_gic_ctrl(UINT32 intid, ULONG set_enable, LONG intrpt_pri, E_FJ_GIC_TARGET_CPU cpu_target)
{
	LONG target;
	
	if ( cpu_target == E_FJ_GIC_TARGET_OWN_CPU ){
		target = (1 << Dd_ARM_Get_CPU_ID());
	}
	else {
		target = cpu_target;
	}
	
	if( set_enable==1 ){
		Interrupt_ReSet_Enable(intid, intrpt_pri, target);
	}
	else
	{
		OS_User_DisableInt((OS_USER_INTID)intid);
	}
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_gic_clear_pending(UINT32 intid)
{
	if( D_DD_GIC_INTID_MAX <= intid ) {
		return FJ_ERR_NG;
	}
	Dd_GIC_DIST_Set_ICPendr( (intid >> 5), (1ul << (31 & intid)) );
	return FJ_ERR_OK;
}
