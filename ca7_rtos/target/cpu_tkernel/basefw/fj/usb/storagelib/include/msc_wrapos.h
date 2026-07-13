/**
 * @file		msc_wrapos.h
 * @brief		REALOS System call wrap function header
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef __MSC_WRAPOS_H
#define __MSC_WRAPOS_H

/* Type definition */
typedef int                      MSC_ER;
typedef void                    *MSC_VP;
typedef short                    MSC_ID;
typedef unsigned int             MSC_UINT;
typedef int                      MSC_TMO;
typedef short                    MSC_PRI;
typedef unsigned short           MSC_STAT;

/* Sturcture definition */
typedef struct t_msc_msg{
    struct t_msc_msg            *hmsg;
}T_MSC_MSG;

typedef struct t_msc_rmbx{
    MSC_ID                       wtskid;
    T_MSC_MSG                   *pk_msg;
}T_MSC_RMBX;

typedef struct t_msc_rtsk{
    MSC_STAT                     tskstat;
    MSC_PRI                      tskpri;
    MSC_PRI                      tskbpri;
    MSC_STAT                     tskwait;
    MSC_ID                       wobjid;
    MSC_TMO                      lefttmo;
    MSC_UINT                     actcnt;
    MSC_UINT                     wupcnt;
    MSC_UINT                     suscnt;
    MSC_VP                       stk;
    MSC_VP                       stktop;
}T_MSC_RTSK;


/* Function prototype */
extern void MSC_sta_tsk(void);
extern void MSC_ter_tsk(void);
extern void MSC_ext_tsk(void);
extern MSC_ER MSC_get_blf(MSC_VP *p_blf, MSC_ID mpfid);
extern MSC_ER MSC_rel_blf(MSC_ID mpfid, MSC_VP blf);
extern MSC_ER MSC_snd_msg(MSC_ID mbxid,T_MSC_MSG *pk_msg);
extern MSC_ER MSC_rcv_msg(T_MSC_MSG **pk_msg, MSC_ID mbxid);
extern MSC_ER MSC_prcv_msg(T_MSC_MSG **pk_msg, MSC_ID mbxid);
extern MSC_ER MSC_ref_mbx(T_MSC_RMBX *pref, MSC_ID mbxid);		// add 2008.10.27
extern MSC_ER MSC_chg_ilm(MSC_UINT ilmask);
extern MSC_ER MSC_ref_ilm(MSC_UINT *ilmask);
extern MSC_ER MSC_tslp_tsk(MSC_TMO tout);
extern MSC_ER MSC_ref_tsk(T_MSC_RTSK *rtsk, MSC_ID tid);

/* Memoru Allocation Error */
#define E_NOMEM_USB			(-1)

/* Task status */
#define MSC_TTS_DMT         (0x10)

/* subtask ID */
#define W_TID_MSCP_RCV	(1)
#define W_TID_MSCP_SND	(2)

/* Memory pool ID */
#define	W_MPFID_EXTBUF_HIGH		(1)
#define	W_MPFID_OTHERMSG		(2)

/* Mail Box ID */
#define	W_MID_MSCP_RCV_REQUEST		(1)
#define	W_MID_MSCP_RCV_RESPONSE		(2)
#define	W_MID_MSCP_SND_REQUEST		(3)
#define	W_MID_MSCP_SND_RESPONSE		(4)

extern unsigned long W_MPFID_EXTBUF_HIGH_SIZE;
extern void USB_Buffer_Init(void);

extern void Init_USB_buffer(char *buf_top, unsigned long size, unsigned int max_number);

#endif  /* __MSC_WRAPOS_H */
