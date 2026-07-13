#ifndef __JDSCNR_CSPR_REG_H__
#define __JDSCNR_CSPR_REG_H__


/*	structure of CSPREN (2842_1000h)	*/
union io_cnr_cspr_reg_cspren{
	unsigned long		word;
	struct {
		unsigned long	CSPREN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of CGDKEN (2842_1008h)	*/
union io_cnr_cspr_reg_cgdken{
	unsigned long		word;
	struct {
		unsigned long	CGDKEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MCEN	(2842_1020h)	*/
union io_cnr_cspr_reg_mcen{
	unsigned long		word;
	struct {
		unsigned long	MCEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MCYTHH (2842_1024h)	*/
union io_cnr_cspr_reg_mcythh{
	unsigned long		word;
	struct {
		unsigned long	MCYTHH_0	:8;
		unsigned long	MCYTHH_1	:8;
		unsigned long	MCYTHH_2	:8;
		unsigned long	MCYTHH_3	:8;
	}bit;
};

/*	structure of MCYTHV (2842_1028h)	*/
union io_cnr_cspr_reg_mcythv{
	unsigned long		word;
	struct {
		unsigned long	MCYTHV_0	:8;
		unsigned long	MCYTHV_1	:8;
		unsigned long	MCYTHV_2	:8;
		unsigned long	MCYTHV_3	:8;
	}bit;
};

/*	structure of MCCTHH (2842_102Ch)	*/
union io_cnr_cspr_reg_mccthh{
	unsigned long		word;
	struct {
		unsigned long	MCCTHH_0	:8;
		unsigned long	MCCTHH_1	:8;
		unsigned long	MCCTHH_2	:8;
		unsigned long	MCCTHH_3	:8;
	}bit;
};

/*	structure of MCCTHV (2842_1030h)	*/
union io_cnr_cspr_reg_mccthv{
	unsigned long		word;
	struct {
		unsigned long	MCCTHV_0	:8;
		unsigned long	MCCTHV_1	:8;
		unsigned long	MCCTHV_2	:8;
		unsigned long	MCCTHV_3	:8;
	}bit;
};

/*	structure of MCYDYM (2842_1034h)	*/
union io_cnr_cspr_reg_mcydym{
	unsigned long		word;
	struct {
		unsigned long	MCYDYM	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MCYDYMDT	(2842_1038h)	*/
union io_cnr_cspr_reg_mcydymdt{
	unsigned long		word;
	struct {
		unsigned long	MCYDYMYCR	:8;
		unsigned long				:8;
		unsigned long	MCYDYMYG	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of MCYSCL (2842_1040h)	*/
union io_cnr_cspr_reg_mcyscl{
	unsigned long		word;
	struct {
		unsigned long	MCYSCL	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MCYSCLOF	(2842_1048h)	*/
union io_cnr_cspr_reg_mcysclof_1{
	unsigned long		word;
	struct {
		unsigned long	MCYSCLOF_0	:8;
		unsigned long	MCYSCLOF_1	:8;
		unsigned long	MCYSCLOF_2	:8;
		unsigned long	MCYSCLOF_3	:8;
	}bit;
};
union io_cnr_cspr_reg_mcysclof_2{
	unsigned long		word;
	struct {
		unsigned long	MCYSCLOF_4	:8;
		unsigned long	MCYSCLOF_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_mcysclof{
	union io_cnr_cspr_reg_mcysclof_1	MCYSCLOF1;
	union io_cnr_cspr_reg_mcysclof_2	MCYSCLOF2;
};

/*	structure of MCYSCLGA	(2842_1050h)	*/
union io_cnr_cspr_reg_mcysclga_1{
	unsigned long		word;
	struct {
		long			MCYSCLGA_0	:8;
		long			MCYSCLGA_1	:8;
		long			MCYSCLGA_2	:8;
		long			MCYSCLGA_3	:8;
	}bit;
};
union io_cnr_cspr_reg_mcysclga_2{
	unsigned long		word;
	struct {
		long			MCYSCLGA_4	:8;
		long			MCYSCLGA_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_mcysclga{
	union io_cnr_cspr_reg_mcysclga_1	MCYSCLGA1;
	union io_cnr_cspr_reg_mcysclga_2	MCYSCLGA2;
};

/*	structure of MCYSCLBD	(2842_1058h)	*/
union io_cnr_cspr_reg_mcysclbd_1{
	unsigned long		word;
	struct {
		unsigned long	MCYSCLBD_1	:8;
		unsigned long	MCYSCLBD_2	:8;
		unsigned long	MCYSCLBD_3	:8;
		unsigned long	MCYSCLBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_mcysclbd_2{
	unsigned long		word;
	struct {
		unsigned long	MCYSCLBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_mcysclbd{
	union io_cnr_cspr_reg_mcysclbd_1	MCYSCLBD1;
	union io_cnr_cspr_reg_mcysclbd_2	MCYSCLBD2;
};

/*	structure of MCCSCL (2842_1060h)	*/
union io_cnr_cspr_reg_mccscl{
	unsigned long		word;
	struct {
		unsigned long	MCCSCL	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MCCSCLOF	(2842_1068h)	*/
union io_cnr_cspr_reg_mccsclof_1{
	unsigned long		word;
	struct {
		unsigned long	MCCSCLOF_0	:8;
		unsigned long	MCCSCLOF_1	:8;
		unsigned long	MCCSCLOF_2	:8;
		unsigned long	MCCSCLOF_3	:8;
	}bit;
};
union io_cnr_cspr_reg_mccsclof_2{
	unsigned long		word;
	struct {
		unsigned long	MCCSCLOF_4	:8;
		unsigned long	MCCSCLOF_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_mccsclof{
	union io_cnr_cspr_reg_mccsclof_1	MCCSCLOF1;
	union io_cnr_cspr_reg_mccsclof_2	MCCSCLOF2;
};

/*	structure of MCCSCLGA	(2842_1070h)	*/
union io_cnr_cspr_reg_mccsclga_1{
	unsigned long		word;
	struct {
		long			MCCSCLGA_0	:8;
		long			MCCSCLGA_1	:8;
		long			MCCSCLGA_2	:8;
		long			MCCSCLGA_3	:8;
	}bit;
};
union io_cnr_cspr_reg_mccsclga_2{
	unsigned long		word;
	struct {
		long			MCCSCLGA_4	:8;
		long			MCCSCLGA_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_mccsclga{
	union io_cnr_cspr_reg_mccsclga_1	MCCSCLGA1;
	union io_cnr_cspr_reg_mccsclga_2	MCCSCLGA2;
};

/*	structure of MCCSCLBD	(2842_1078h)	*/
union io_cnr_cspr_reg_mccsclbd_1{
	unsigned long		word;
	struct {
		unsigned long	MCCSCLBD_1	:8;
		unsigned long	MCCSCLBD_2	:8;
		unsigned long	MCCSCLBD_3	:8;
		unsigned long	MCCSCLBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_mccsclbd_2{
	unsigned long		word;
	struct {
		unsigned long	MCCSCLBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_mccsclbd{
	union io_cnr_cspr_reg_mccsclbd_1	MCCSCLBD1;
	union io_cnr_cspr_reg_mccsclbd_2	MCCSCLBD2;
};

/*	structure of MCSSCL (2842_1080h)	*/
union io_cnr_cspr_reg_mcsscl{
	unsigned long		word;
	struct {
		unsigned long	MCSSCL	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MCSSCLOF	(2842_1088h)	*/
union io_cnr_cspr_reg_mcssclof_1{
	unsigned long		word;
	struct {
		unsigned long	MCSSCLOF_0	:8;
		unsigned long	MCSSCLOF_1	:8;
		unsigned long	MCSSCLOF_2	:8;
		unsigned long	MCSSCLOF_3	:8;
	}bit;
};
union io_cnr_cspr_reg_mcssclof_2{
	unsigned long		word;
	struct {
		unsigned long	MCSSCLOF_4	:8;
		unsigned long	MCSSCLOF_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_mcssclof{
	union io_cnr_cspr_reg_mcssclof_1	MCSSCLOF1;
	union io_cnr_cspr_reg_mcssclof_2	MCSSCLOF2;
};

/*	structure of MCSSCLGA	(2842_1090h)	*/
union io_cnr_cspr_reg_mcssclga_1{
	unsigned long		word;
	struct {
		long			MCSSCLGA_0	:8;
		long			MCSSCLGA_1	:8;
		long			MCSSCLGA_2	:8;
		long			MCSSCLGA_3	:8;
	}bit;
};
union io_cnr_cspr_reg_mcssclga_2{
	unsigned long		word;
	struct {
		long			MCSSCLGA_4	:8;
		long			MCSSCLGA_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_mcssclga{
	union io_cnr_cspr_reg_mcssclga_1	MCSSCLGA1;
	union io_cnr_cspr_reg_mcssclga_2	MCSSCLGA2;
};

/*	structure of MCSSCLBD	(2842_1098h)	*/
union io_cnr_cspr_reg_mcssclbd_1{
	unsigned long		word;
	struct {
		unsigned long	MCSSCLBD_1	:8;
		unsigned long	MCSSCLBD_2	:8;
		unsigned long	MCSSCLBD_3	:8;
		unsigned long	MCSSCLBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_mcssclbd_2{
	unsigned long		word;
	struct {
		unsigned long	MCSSCLBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_mcssclbd{
	union io_cnr_cspr_reg_mcssclbd_1	MCSSCLBD1;
	union io_cnr_cspr_reg_mcssclbd_2	MCSSCLBD2;
};

/*	structure of LCEN	(2842_10C0h)	*/
union io_cnr_cspr_reg_lcen{
	unsigned long		word;
	struct {
		unsigned long	LCEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of LCYTHH (2842_10C4h)	*/
union io_cnr_cspr_reg_lcythh{
	unsigned long		word;
	struct {
		unsigned long	LCYTHH_0	:8;
		unsigned long	LCYTHH_1	:8;
		unsigned long	LCYTHH_2	:8;
		unsigned long	LCYTHH_3	:8;
	}bit;
};

/*	structure of LCYTHV (2842_10C8h)	*/
union io_cnr_cspr_reg_lcythv{
	unsigned long		word;
	struct {
		unsigned long	LCYTHV_0	:8;
		unsigned long	LCYTHV_1	:8;
		unsigned long	LCYTHV_2	:8;
		unsigned long	LCYTHV_3	:8;
	}bit;
};

/*	structure of LCCTHH (2842_10CCh)	*/
union io_cnr_cspr_reg_lccthh{
	unsigned long		word;
	struct {
		unsigned long	LCCTHH_0	:8;
		unsigned long	LCCTHH_1	:8;
		unsigned long	LCCTHH_2	:8;
		unsigned long	LCCTHH_3	:8;
	}bit;
};

/*	structure of LCCTHV (2842_10D0h)	*/
union io_cnr_cspr_reg_lccthv{
	unsigned long		word;
	struct {
		unsigned long	LCCTHV_0	:8;
		unsigned long	LCCTHV_1	:8;
		unsigned long	LCCTHV_2	:8;
		unsigned long	LCCTHV_3	:8;
	}bit;
};

/*	structure of LCYDYM (2842_10D4h)	*/
union io_cnr_cspr_reg_lcydym{
	unsigned long		word;
	struct {
		unsigned long	LCYDYM	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of LCYDYMDT	(2842_10D8h)	*/
union io_cnr_cspr_reg_lcydymdt{
	unsigned long		word;
	struct {
		unsigned long	LCYDYMYCR	:8;
		unsigned long				:8;
		unsigned long	LCYDYMYG	:9;
		unsigned long				:7;
	}bit;
};

/*	structure of LCYSCL (2842_10E0h)	*/
union io_cnr_cspr_reg_lcyscl{
	unsigned long		word;
	struct {
		unsigned long	LCYSCL	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of LCYSCLOF	(2842_10E8h)	*/
union io_cnr_cspr_reg_lcysclof_1{
	unsigned long		word;
	struct {
		unsigned long	LCYSCLOF_0	:8;
		unsigned long	LCYSCLOF_1	:8;
		unsigned long	LCYSCLOF_2	:8;
		unsigned long	LCYSCLOF_3	:8;
	}bit;
};
union io_cnr_cspr_reg_lcysclof_2{
	unsigned long		word;
	struct {
		unsigned long	LCYSCLOF_4	:8;
		unsigned long	LCYSCLOF_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_lcysclof{
	union io_cnr_cspr_reg_lcysclof_1	LCYSCLOF1;
	union io_cnr_cspr_reg_lcysclof_2	LCYSCLOF2;
};

/*	structure of LCYSCLGA	(2842_10F0h)	*/
union io_cnr_cspr_reg_lcysclga_1{
	unsigned long		word;
	struct {
		long			LCYSCLGA_0	:8;
		long			LCYSCLGA_1	:8;
		long			LCYSCLGA_2	:8;
		long			LCYSCLGA_3	:8;
	}bit;
};
union io_cnr_cspr_reg_lcysclga_2{
	unsigned long		word;
	struct {
		long			LCYSCLGA_4	:8;
		long			LCYSCLGA_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_lcysclga{
	union io_cnr_cspr_reg_lcysclga_1	LCYSCLGA1;
	union io_cnr_cspr_reg_lcysclga_2	LCYSCLGA2;
};

/*	structure of LCYSCLBD	(2842_10F8h)	*/
union io_cnr_cspr_reg_lcysclbd_1{
	unsigned long		word;
	struct {
		unsigned long	LCYSCLBD_1	:8;
		unsigned long	LCYSCLBD_2	:8;
		unsigned long	LCYSCLBD_3	:8;
		unsigned long	LCYSCLBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_lcysclbd_2{
	unsigned long		word;
	struct {
		unsigned long	LCYSCLBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_lcysclbd{
	union io_cnr_cspr_reg_lcysclbd_1	LCYSCLBD1;
	union io_cnr_cspr_reg_lcysclbd_2	LCYSCLBD2;
};

/*	structure of LCCSCL (2842_1100h)	*/
union io_cnr_cspr_reg_lccscl{
	unsigned long		word;
	struct {
		unsigned long	LCCSCL	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of LCCSCLOF	(2842_1108h)	*/
union io_cnr_cspr_reg_lccsclof_1{
	unsigned long		word;
	struct {
		unsigned long	LCCSCLOF_0	:8;
		unsigned long	LCCSCLOF_1	:8;
		unsigned long	LCCSCLOF_2	:8;
		unsigned long	LCCSCLOF_3	:8;
	}bit;
};
union io_cnr_cspr_reg_lccsclof_2{
	unsigned long		word;
	struct {
		unsigned long	LCCSCLOF_4	:8;
		unsigned long	LCCSCLOF_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_lccsclof{
	union io_cnr_cspr_reg_lccsclof_1	LCCSCLOF1;
	union io_cnr_cspr_reg_lccsclof_2	LCCSCLOF2;
};

/*	structure of LCCSCLGA	(2842_1110h)	*/
union io_cnr_cspr_reg_lccsclga_1{
	unsigned long		word;
	struct {
		long			LCCSCLGA_0	:8;
		long			LCCSCLGA_1	:8;
		long			LCCSCLGA_2	:8;
		long			LCCSCLGA_3	:8;
	}bit;
};
union io_cnr_cspr_reg_lccsclga_2{
	unsigned long		word;
	struct {
		long			LCCSCLGA_4	:8;
		long			LCCSCLGA_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_lccsclga{
	union io_cnr_cspr_reg_lccsclga_1	LCCSCLGA1;
	union io_cnr_cspr_reg_lccsclga_2	LCCSCLGA2;
};

/*	structure of LCCSCLBD	(2842_1118h)	*/
union io_cnr_cspr_reg_lccsclbd_1{
	unsigned long		word;
	struct {
		unsigned long	LCCSCLBD_1	:8;
		unsigned long	LCCSCLBD_2	:8;
		unsigned long	LCCSCLBD_3	:8;
		unsigned long	LCCSCLBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_lccsclbd_2{
	unsigned long		word;
	struct {
		unsigned long	LCCSCLBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_lccsclbd{
	union io_cnr_cspr_reg_lccsclbd_1	LCCSCLBD1;
	union io_cnr_cspr_reg_lccsclbd_2	LCCSCLBD2;
};

/*	structure of LCSSCL (2842_1120h)	*/
union io_cnr_cspr_reg_lcsscl{
	unsigned long		word;
	struct {
		unsigned long	LCSSCL	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of LCSSCLOF	(2842_1128h)	*/
union io_cnr_cspr_reg_lcssclof_1{
	unsigned long		word;
	struct {
		unsigned long	LCSSCLOF_0	:8;
		unsigned long	LCSSCLOF_1	:8;
		unsigned long	LCSSCLOF_2	:8;
		unsigned long	LCSSCLOF_3	:8;
	}bit;
};
union io_cnr_cspr_reg_lcssclof_2{
	unsigned long		word;
	struct {
		unsigned long	LCSSCLOF_4	:8;
		unsigned long	LCSSCLOF_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_lcssclof{
	union io_cnr_cspr_reg_lcssclof_1	LCSSCLOF1;
	union io_cnr_cspr_reg_lcssclof_2	LCSSCLOF2;
};

/*	structure of LCSSCLGA	(2842_1130h)	*/
union io_cnr_cspr_reg_lcssclga_1{
	unsigned long		word;
	struct {
		long			LCSSCLGA_0	:8;
		long			LCSSCLGA_1	:8;
		long			LCSSCLGA_2	:8;
		long			LCSSCLGA_3	:8;
	}bit;
};
union io_cnr_cspr_reg_lcssclga_2{
	unsigned long		word;
	struct {
		long			LCSSCLGA_4	:8;
		long			LCSSCLGA_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_lcssclga{
	union io_cnr_cspr_reg_lcssclga_1	LCSSCLGA1;
	union io_cnr_cspr_reg_lcssclga_2	LCSSCLGA2;
};

/*	structure of LCSSCLBD	(2842_1138h)	*/
union io_cnr_cspr_reg_lcssclbd_1{
	unsigned long		word;
	struct {
		unsigned long	LCSSCLBD_1	:8;
		unsigned long	LCSSCLBD_2	:8;
		unsigned long	LCSSCLBD_3	:8;
		unsigned long	LCSSCLBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_lcssclbd_2{
	unsigned long		word;
	struct {
		unsigned long	LCSSCLBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_lcssclbd{
	union io_cnr_cspr_reg_lcssclbd_1	LCSSCLBD1;
	union io_cnr_cspr_reg_lcssclbd_2	LCSSCLBD2;
};

/*	structure of BLEND	(2842_1160h)	*/
union io_cnr_cspr_reg_blend{
	unsigned long		word;
	struct {
		unsigned long	BLENDMD		:2;
		unsigned long				:6;
		unsigned long	CSPRALPBD	:5;
		unsigned long				:3;
		unsigned long	CSRGEN		:1;
		unsigned long				:3;
		unsigned long	CSRGMD		:3;
		unsigned long				:9;
	}bit;
};

/*	structure of CESCLOF	(2842_1168h)	*/
union io_cnr_cspr_reg_cesclof_1{
	unsigned long		word;
	struct {
		unsigned long	CESCLOF_0	:6;
		unsigned long				:2;
		unsigned long	CESCLOF_1	:6;
		unsigned long				:2;
		unsigned long	CESCLOF_2	:6;
		unsigned long				:2;
		unsigned long	CESCLOF_3	:6;
		unsigned long				:2;
	}bit;
};
union io_cnr_cspr_reg_cesclof_2{
	unsigned long		word;
	struct {
		unsigned long	CESCLOF_4	:6;
		unsigned long				:2;
		unsigned long	CESCLOF_5	:6;
		unsigned long				:2;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_cesclof{
	union io_cnr_cspr_reg_cesclof_1	CESCLOF1;
	union io_cnr_cspr_reg_cesclof_2	CESCLOF2;
};

/*	structure of CESCLGA	(2842_1170h)	*/
union io_cnr_cspr_reg_cesclga_1{
	unsigned long		word;
	struct {
		long			CESCLGA_0	:8;
		long			CESCLGA_1	:8;
		long			CESCLGA_2	:8;
		long			CESCLGA_3	:8;
	}bit;
};
union io_cnr_cspr_reg_cesclga_2{
	unsigned long		word;
	struct {
		long			CESCLGA_4	:8;
		long			CESCLGA_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_cesclga{
	union io_cnr_cspr_reg_cesclga_1	CESCLGA1;
	union io_cnr_cspr_reg_cesclga_2	CESCLGA2;
};

/*	structure of CESCLBD	(2842_1178h)	*/
union io_cnr_cspr_reg_cesclbd_1{
	unsigned long		word;
	struct {
		unsigned long	CESCLBD_1	:8;
		unsigned long	CESCLBD_2	:8;
		unsigned long	CESCLBD_3	:8;
		unsigned long	CESCLBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_cesclbd_2{
	unsigned long		word;
	struct {
		unsigned long	CESCLBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_cesclbd{
	union io_cnr_cspr_reg_cesclbd_1	CESCLBD1;
	union io_cnr_cspr_reg_cesclbd_2	CESCLBD2;
};

/*	structure of CPHPT	(2842_1190h)	*/
union io_cnr_cspr_reg_cphpt{
	unsigned long		word;
	struct {
		long			CDCB	:8;
		long			CDCR	:8;
		unsigned long			:16;
	}bit;
};

/*	structure of CPHPTK	(2842_1194h)	*/
union io_cnr_cspr_reg_cphptk{
	unsigned long		word;
	struct {
		unsigned long	KCB		:10;
		unsigned long			:6;
		unsigned long	KCR		:10;
		unsigned long			:6;
	}bit;
};

/*	structure of CDSCLOF	(2842_11A0h)	*/
union io_cnr_cspr_reg_cdsclof_1{
	unsigned long		word;
	struct {
		unsigned long	CDSCLOF_0	:9;
		unsigned long				:7;
		unsigned long	CDSCLOF_1	:9;
		unsigned long				:7;
	}bit;
};
union io_cnr_cspr_reg_cdsclof_2{
	unsigned long		word;
	struct {
		unsigned long	CDSCLOF_2	:9;
		unsigned long				:7;
		unsigned long	CDSCLOF_3	:9;
		unsigned long				:7;
	}bit;
};
union io_cnr_cspr_reg_cdsclof_3{
	unsigned long		word;
	struct {
		unsigned long	CDSCLOF_4	:9;
		unsigned long				:7;
		unsigned long	CDSCLOF_5	:9;
		unsigned long				:7;
	}bit;
};
struct io_cnr_cspr_reg_cdsclof{
	union io_cnr_cspr_reg_cdsclof_1	CDSCLOF1;
	union io_cnr_cspr_reg_cdsclof_2	CDSCLOF2;
	union io_cnr_cspr_reg_cdsclof_3	CDSCLOF3;
};

/*	structure of CDSCLGA	(2842_11B0h)	*/
union io_cnr_cspr_reg_cdsclga_1{
	unsigned long		word;
	struct {
		long			CDSCLGA_0	:11;
		unsigned long				:5;
		long			CDSCLGA_1	:11;
		unsigned long				:5;
	}bit;
};
union io_cnr_cspr_reg_cdsclga_2{
	unsigned long		word;
	struct {
		long			CDSCLGA_2	:11;
		unsigned long				:5;
		long			CDSCLGA_3	:11;
		unsigned long				:5;
	}bit;
};
union io_cnr_cspr_reg_cdsclga_3{
	unsigned long		word;
	struct {
		long			CDSCLGA_4	:11;
		unsigned long				:5;
		long			CDSCLGA_5	:11;
		unsigned long				:5;
	}bit;
};
struct io_cnr_cspr_reg_cdsclga{
	union io_cnr_cspr_reg_cdsclga_1	CDSCLGA1;
	union io_cnr_cspr_reg_cdsclga_2	CDSCLGA2;
	union io_cnr_cspr_reg_cdsclga_3	CDSCLGA3;
};

/*	structure of CDSCLBD	(2842_11C0h)	*/
union io_cnr_cspr_reg_cdsclbd_1{
	unsigned long		word;
	struct {
		unsigned long	CDSCLBD_1	:11;
		unsigned long				:5;
		unsigned long	CDSCLBD_2	:11;
		unsigned long				:5;
	}bit;
};
union io_cnr_cspr_reg_cdsclbd_2{
	unsigned long		word;
	struct {
		unsigned long	CDSCLBD_3	:11;
		unsigned long				:5;
		unsigned long	CDSCLBD_4	:11;
		unsigned long				:5;
	}bit;
};
union io_cnr_cspr_reg_cdsclbd_3{
	unsigned long		word;
	struct {
		unsigned long	CDSCLBD_5	:11;
		unsigned long				:21;
	}bit;
};
struct io_cnr_cspr_reg_cdsclbd{
	union io_cnr_cspr_reg_cdsclbd_1	CDSCLBD1;
	union io_cnr_cspr_reg_cdsclbd_2	CDSCLBD2;
	union io_cnr_cspr_reg_cdsclbd_3	CDSCLBD3;
};

/*	structure of WHTMD	(2842_11D0h)	*/
union io_cnr_cspr_reg_whtmd{
	unsigned long		word;
	struct {
		unsigned long	WHTEN	:1;
		unsigned long			:7;
		unsigned long	WHTVAL	:5;
		unsigned long			:3;
		unsigned long	WHTBASE	:2;
		unsigned long			:14;
	}bit;
};

/*	structure of SPRS	(2842_1200h)	*/
union io_cnr_cspr_reg_sprs{
	unsigned long		word;
	struct {
		unsigned long	SPRS	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of SPRSCBOF	(2842_1208h)	*/
union io_cnr_cspr_reg_sprscbof_1{
	unsigned long		word;
	struct {
		long			SPRSCBOF_0	:8;
		long			SPRSCBOF_1	:8;
		long			SPRSCBOF_2	:8;
		long			SPRSCBOF_3	:8;
	}bit;
};
union io_cnr_cspr_reg_sprscbof_2{
	unsigned long		word;
	struct {
		long			SPRSCBOF_4	:8;
		long			SPRSCBOF_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_sprscbof{
	union io_cnr_cspr_reg_sprscbof_1	SPRSCBOF1;
	union io_cnr_cspr_reg_sprscbof_2	SPRSCBOF2;
};

/*	structure of SPRSCBGA	(2842_1210h)	*/
union io_cnr_cspr_reg_sprscbga_1{
	unsigned long		word;
	struct {
		long			SPRSCBGA_0	:12;
		unsigned long				:4;
		long			SPRSCBGA_1	:12;
		unsigned long				:4;
	}bit;
};
union io_cnr_cspr_reg_sprscbga_2{
	unsigned long		word;
	struct {
		long			SPRSCBGA_2	:12;
		unsigned long				:4;
		long			SPRSCBGA_3	:12;
		unsigned long				:4;
	}bit;
};
union io_cnr_cspr_reg_sprscbga_3{
	unsigned long		word;
	struct {
		long			SPRSCBGA_4	:12;
		unsigned long				:4;
		long			SPRSCBGA_5	:12;
		unsigned long				:4;
	}bit;
};
struct io_cnr_cspr_reg_sprscbga{
	union io_cnr_cspr_reg_sprscbga_1	SPRSCBGA1;
	union io_cnr_cspr_reg_sprscbga_2	SPRSCBGA2;
	union io_cnr_cspr_reg_sprscbga_3	SPRSCBGA3;
};

/*	structure of SPRSCBBD	(2842_1220h)	*/
union io_cnr_cspr_reg_sprscbbd_1{
	unsigned long		word;
	struct {
		long			SPRSCBBD_1	:8;
		long			SPRSCBBD_2	:8;
		long			SPRSCBBD_3	:8;
		long			SPRSCBBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_sprscbbd_2{
	unsigned long		word;
	struct {
		long			SPRSCBBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_sprscbbd{
	union io_cnr_cspr_reg_sprscbbd_1	SPRSCBBD1;
	union io_cnr_cspr_reg_sprscbbd_2	SPRSCBBD2;
};

/*	structure of SPRSCROF	(2842_1228h)	*/
union io_cnr_cspr_reg_sprscrof_1{
	unsigned long		word;
	struct {
		long			SPRSCROF_0	:8;
		long			SPRSCROF_1	:8;
		long			SPRSCROF_2	:8;
		long			SPRSCROF_3	:8;
	}bit;
};
union io_cnr_cspr_reg_sprscrof_2{
	unsigned long		word;
	struct {
		long			SPRSCROF_4	:8;
		long			SPRSCROF_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_sprscrof{
	union io_cnr_cspr_reg_sprscrof_1	SPRSCROF1;
	union io_cnr_cspr_reg_sprscrof_2	SPRSCROF2;
};

/*	structure of SPRSCRGA	(2842_1230h)	*/
union io_cnr_cspr_reg_sprscrga_1{
	unsigned long		word;
	struct {
		long			SPRSCRGA_0	:12;
		unsigned long				:4;
		long			SPRSCRGA_1	:12;
		unsigned long				:4;
	}bit;
};
union io_cnr_cspr_reg_sprscrga_2{
	unsigned long		word;
	struct {
		long			SPRSCRGA_2	:12;
		unsigned long				:4;
		long			SPRSCRGA_3	:12;
		unsigned long				:4;
	}bit;
};
union io_cnr_cspr_reg_sprscrga_3{
	unsigned long		word;
	struct {
		long			SPRSCRGA_4	:12;
		unsigned long				:4;
		long			SPRSCRGA_5	:12;
		unsigned long				:4;
	}bit;
};
struct io_cnr_cspr_reg_sprscrga{
	union io_cnr_cspr_reg_sprscrga_1	SPRSCRGA1;
	union io_cnr_cspr_reg_sprscrga_2	SPRSCRGA2;
	union io_cnr_cspr_reg_sprscrga_3	SPRSCRGA3;
};

/*	structure of SPRSCRBD	(2842_1240h)	*/
union io_cnr_cspr_reg_sprscrbd_1{
	unsigned long		word;
	struct {
		long			SPRSCRBD_1	:8;
		long			SPRSCRBD_2	:8;
		long			SPRSCRBD_3	:8;
		long			SPRSCRBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_sprscrbd_2{
	unsigned long		word;
	struct {
		long			SPRSCRBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_sprscrbd{
	union io_cnr_cspr_reg_sprscrbd_1	SPRSCRBD1;
	union io_cnr_cspr_reg_sprscrbd_2	SPRSCRBD2;
};

/*	structure of SPRD	(2842_1280h)	*/
union io_cnr_cspr_reg_sprd{
	unsigned long		word;
	struct {
		unsigned long	SPRD	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of SPRDCBOF	(2842_1288h)	*/
union io_cnr_cspr_reg_sprdcbof_1{
	unsigned long		word;
	struct {
		long			SPRDCBOF_0	:8;
		long			SPRDCBOF_1	:8;
		long			SPRDCBOF_2	:8;
		long			SPRDCBOF_3	:8;
	}bit;
};
union io_cnr_cspr_reg_sprdcbof_2{
	unsigned long		word;
	struct {
		long			SPRDCBOF_4	:8;
		long			SPRDCBOF_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_sprdcbof{
	union io_cnr_cspr_reg_sprdcbof_1	SPRDCBOF1;
	union io_cnr_cspr_reg_sprdcbof_2	SPRDCBOF2;
};

/*	structure of SPRDCBGA	(2842_1290h)	*/
union io_cnr_cspr_reg_sprdcbga_1{
	unsigned long		word;
	struct {
		long			SPRDCBGA_0	:12;
		unsigned long				:4;
		long			SPRDCBGA_1	:12;
		unsigned long				:4;
	}bit;
};
union io_cnr_cspr_reg_sprdcbga_2{
	unsigned long		word;
	struct {
		long			SPRDCBGA_2	:12;
		unsigned long				:4;
		long			SPRDCBGA_3	:12;
		unsigned long				:4;
	}bit;
};
union io_cnr_cspr_reg_sprdcbga_3{
	unsigned long		word;
	struct {
		long			SPRDCBGA_4	:12;
		unsigned long				:4;
		long			SPRDCBGA_5	:12;
		unsigned long				:4;
	}bit;
};
struct io_cnr_cspr_reg_sprdcbga{
	union io_cnr_cspr_reg_sprdcbga_1	SPRDCBGA1;
	union io_cnr_cspr_reg_sprdcbga_2	SPRDCBGA2;
	union io_cnr_cspr_reg_sprdcbga_3	SPRDCBGA3;
};

/*	structure of SPRDCBBD	(2842_12A0h)	*/
union io_cnr_cspr_reg_sprdcbbd_1{
	unsigned long		word;
	struct {
		long			SPRDCBBD_1	:8;
		long			SPRDCBBD_2	:8;
		long			SPRDCBBD_3	:8;
		long			SPRDCBBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_sprdcbbd_2{
	unsigned long		word;
	struct {
		long			SPRDCBBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_sprdcbbd{
	union io_cnr_cspr_reg_sprdcbbd_1	SPRDCBBD1;
	union io_cnr_cspr_reg_sprdcbbd_2	SPRDCBBD2;
};

/*	structure of SPRDCROF	(2842_12A8h)	*/
union io_cnr_cspr_reg_sprdcrof_1{
	unsigned long		word;
	struct {
		long			SPRDCROF_0	:8;
		long			SPRDCROF_1	:8;
		long			SPRDCROF_2	:8;
		long			SPRDCROF_3	:8;
	}bit;
};
union io_cnr_cspr_reg_sprdcrof_2{
	unsigned long		word;
	struct {
		long			SPRDCROF_4	:8;
		long			SPRDCROF_5	:8;
		unsigned long				:16;
	}bit;
};
struct io_cnr_cspr_reg_sprdcrof{
	union io_cnr_cspr_reg_sprdcrof_1	SPRDCROF1;
	union io_cnr_cspr_reg_sprdcrof_2	SPRDCROF2;
};

/*	structure of SPRDCRGA	(2842_12B0h)	*/
union io_cnr_cspr_reg_sprdcrga_1{
	unsigned long		word;
	struct {
		long			SPRDCRGA_0	:12;
		unsigned long				:4;
		long			SPRDCRGA_1	:12;
		unsigned long				:4;
	}bit;
};
union io_cnr_cspr_reg_sprdcrga_2{
	unsigned long		word;
	struct {
		long			SPRDCRGA_2	:12;
		unsigned long				:4;
		long			SPRDCRGA_3	:12;
		unsigned long				:4;
	}bit;
};
union io_cnr_cspr_reg_sprdcrga_3{
	unsigned long		word;
	struct {
		long			SPRDCRGA_4	:12;
		unsigned long				:4;
		long			SPRDCRGA_5	:12;
		unsigned long				:4;
	}bit;
};
struct io_cnr_cspr_reg_sprdcrga{
	union io_cnr_cspr_reg_sprdcrga_1	SPRDCRGA1;
	union io_cnr_cspr_reg_sprdcrga_2	SPRDCRGA2;
	union io_cnr_cspr_reg_sprdcrga_3	SPRDCRGA3;
};

/*	structure of SPRDCRBD	(2842_12C0h)	*/
union io_cnr_cspr_reg_sprdcrbd_1{
	unsigned long		word;
	struct {
		long			SPRDCRBD_1	:8;
		long			SPRDCRBD_2	:8;
		long			SPRDCRBD_3	:8;
		long			SPRDCRBD_4	:8;
	}bit;
};
union io_cnr_cspr_reg_sprdcrbd_2{
	unsigned long		word;
	struct {
		long			SPRDCRBD_5	:8;
		unsigned long				:24;
	}bit;
};
struct io_cnr_cspr_reg_sprdcrbd{
	union io_cnr_cspr_reg_sprdcrbd_1	SPRDCRBD1;
	union io_cnr_cspr_reg_sprdcrbd_2	SPRDCRBD2;
};

/* Define i/o mapping */
struct io_cnr_cspr_reg{
	/* JDSIMG */
	union  io_cnr_cspr_reg_cspren	CSPREN;		/* 2842_(1000 - 1003h) */

	unsigned char dmy_1004_1007[0x1008-0x1004];	/* 2842_(1004 - 1007h) */

	union  io_cnr_cspr_reg_cgdken	CGDKEN;		/* 2842_(1008 - 100Bh) */

	unsigned char dmy_100C_101F[0x1020-0x100C];	/* 2842_(100C - 101Fh) */

	union  io_cnr_cspr_reg_mcen		MCEN;		/* 2842_(1020 - 1023h) */
	union  io_cnr_cspr_reg_mcythh	MCYTHH;		/* 2842_(1024 - 1027h) */
	union  io_cnr_cspr_reg_mcythv	MCYTHV;		/* 2842_(1028 - 102Bh) */
	union  io_cnr_cspr_reg_mccthh	MCCTHH;		/* 2842_(102C - 102Fh) */
	union  io_cnr_cspr_reg_mccthv	MCCTHV;		/* 2842_(1030 - 1033h) */
	union  io_cnr_cspr_reg_mcydym	MCYDYM;		/* 2842_(1034 - 1037h) */
	union  io_cnr_cspr_reg_mcydymdt	MCYDYMDT;	/* 2842_(1038 - 103Bh) */

	unsigned char dmy_103C_103F[0x1040-0x103C];	/* 2842_(103C - 103Fh) */

	union  io_cnr_cspr_reg_mcyscl	MCYSCL;		/* 2842_(1040 - 1043h) */

	unsigned char dmy_1044_1047[0x1048-0x1044];	/* 2842_(1044 - 1047h) */

	struct io_cnr_cspr_reg_mcysclof	MCYSCLOF;	/* 2842_(1048 - 104Fh) */
	struct io_cnr_cspr_reg_mcysclga	MCYSCLGA;	/* 2842_(1050 - 1057h) */
	struct io_cnr_cspr_reg_mcysclbd	MCYSCLBD;	/* 2842_(1058 - 105Fh) */
	union  io_cnr_cspr_reg_mccscl	MCCSCL;		/* 2842_(1060 - 1063h) */

	unsigned char dmy_1064_1067[0x1068-0x1064];	/* 2842_(1064 - 1067h) */

	struct io_cnr_cspr_reg_mccsclof	MCCSCLOF;	/* 2842_(1068 - 106Fh) */
	struct io_cnr_cspr_reg_mccsclga	MCCSCLGA;	/* 2842_(1070 - 1077h) */
	struct io_cnr_cspr_reg_mccsclbd	MCCSCLBD;	/* 2842_(1078 - 107Fh) */
	union  io_cnr_cspr_reg_mcsscl	MCSSCL;		/* 2842_(1080 - 1083h) */

	unsigned char dmy_1084_1087[0x1088-0x1084];	/* 2842_(1084 - 1087h) */

	struct io_cnr_cspr_reg_mcssclof	MCSSCLOF;	/* 2842_(1088 - 108Fh) */
	struct io_cnr_cspr_reg_mcssclga	MCSSCLGA;	/* 2842_(1090 - 1097h) */
	struct io_cnr_cspr_reg_mcssclbd	MCSSCLBD;	/* 2842_(1098 - 109Fh) */

	unsigned char dmy_10A0_10BF[0x10C0-0x10A0];	/* 2842_(10A0 - 10BFh) */

	union  io_cnr_cspr_reg_lcen		LCEN;		/* 2842_(10C0 - 10C3h) */
	union  io_cnr_cspr_reg_lcythh	LCYTHH;		/* 2842_(10C4 - 10C7h) */
	union  io_cnr_cspr_reg_lcythv	LCYTHV;		/* 2842_(10C8 - 10CBh) */
	union  io_cnr_cspr_reg_lccthh	LCCTHH;		/* 2842_(10CC - 10CFh) */
	union  io_cnr_cspr_reg_lccthv	LCCTHV;		/* 2842_(10D0 - 10D3h) */
	union  io_cnr_cspr_reg_lcydym	LCYDYM;		/* 2842_(10D4 - 10D7h) */
	union  io_cnr_cspr_reg_lcydymdt	LCYDYMDT;	/* 2842_(10D8 - 10DBh) */

	unsigned char dmy_10DC_10DF[0x10E0-0x10DC];	/* 2842_(10DC - 10DFh) */

	union  io_cnr_cspr_reg_lcyscl	LCYSCL;		/* 2842_(10E0 - 10E3h) */

	unsigned char dmy_10E4_10E7[0x10E8-0x10E4];	/* 2842_(10E4 - 10E7h) */

	struct io_cnr_cspr_reg_lcysclof	LCYSCLOF;	/* 2842_(10E8 - 10EFh) */
	struct io_cnr_cspr_reg_lcysclga	LCYSCLGA;	/* 2842_(10F0 - 10F7h) */
	struct io_cnr_cspr_reg_lcysclbd	LCYSCLBD;	/* 2842_(10F8 - 10FFh) */
	union  io_cnr_cspr_reg_lccscl	LCCSCL;		/* 2842_(1100 - 1103h) */

	unsigned char dmy_1104_1107[0x1108-0x1104];	/* 2842_(1104 - 1107h) */

	struct io_cnr_cspr_reg_lccsclof	LCCSCLOF;	/* 2842_(1108 - 110Fh) */
	struct io_cnr_cspr_reg_lccsclga	LCCSCLGA;	/* 2842_(1110 - 1117h) */
	struct io_cnr_cspr_reg_lccsclbd	LCCSCLBD;	/* 2842_(1118 - 111Fh) */
	union  io_cnr_cspr_reg_lcsscl	LCSSCL;		/* 2842_(1120 - 1123h) */

	unsigned char dmy_1124_1127[0x1128-0x1124];	/* 2842_(1124 - 1127h) */

	struct io_cnr_cspr_reg_lcssclof	LCSSCLOF;	/* 2842_(1128 - 112Fh) */
	struct io_cnr_cspr_reg_lcssclga	LCSSCLGA;	/* 2842_(1130 - 1137h) */
	struct io_cnr_cspr_reg_lcssclbd	LCSSCLBD;	/* 2842_(1138 - 113Fh) */

	unsigned char dmy_1140_115F[0x1160-0x1140];	/* 2842_(1140 - 115Fh) */

	union  io_cnr_cspr_reg_blend 	BLEND;		/* 2842_(1160 - 1163h) */

	unsigned char dmy_1164_1167[0x1168-0x1164];	/* 2842_(1164 - 1167h) */

	struct io_cnr_cspr_reg_cesclof	CESCLOF;	/* 2842_(1168 - 116Fh) */
	struct io_cnr_cspr_reg_cesclga	CESCLGA;	/* 2842_(1170 - 1177h) */
	struct io_cnr_cspr_reg_cesclbd	CESCLBD;	/* 2842_(1178 - 117Fh) */

	unsigned char dmy_1180_118F[0x11A0-0x1190];	/* 2842_(1180 - 118Fh) */

	union  io_cnr_cspr_reg_cphpt	CPHPT;		/* 2842_(1190 - 1193h) */
	union  io_cnr_cspr_reg_cphptk	CPHPTK;		/* 2842_(1194 - 1197h) */

	unsigned char dmy_1198_119F[0x11A0-0x1198];	/* 2842_(1198 - 119Fh) */

	struct io_cnr_cspr_reg_cdsclof	CDSCLOF;	/* 2842_(11A0 - 11ABh) */

	unsigned char dmy_11AC_11AF[0x11B0-0x11AC];	/* 2842_(11AC - 11AFh) */

	struct io_cnr_cspr_reg_cdsclga	CDSCLGA;	/* 2842_(11B0 - 11BBh) */

	unsigned char dmy_11BC_11BF[0x11C0-0x11BC];	/* 2842_(11BC - 11BFh) */

	struct io_cnr_cspr_reg_cdsclbd	CDSCLBD;	/* 2842_(11C0 - 11CBh) */

	unsigned char dmy_11CC_11CF[0x11D0-0x11CC];	/* 2842_(11CC - 11CFh) */

	union  io_cnr_cspr_reg_whtmd	WHTMD;		/* 2842_(11D0 - 11D3h) */

	unsigned char dmy_11D4_11FF[0x1200-0x11D4];	/* 2842_(11D4 - 11FFh) */

	union  io_cnr_cspr_reg_sprs		SPRS;		/* 2842_(1200 - 1203h) */

	unsigned char dmy_1204_1207[0x1208-0x1204];	/* 2842_(1204 - 1207h) */

	struct io_cnr_cspr_reg_sprscbof	SPRSCBOF;	/* 2842_(1208 - 120Fh) */
	struct io_cnr_cspr_reg_sprscbga	SPRSCBGA;	/* 2842_(1210 - 121Bh) */

	unsigned char dmy_121C_121F[0x1220-0x121C];	/* 2842_(121C - 121Fh) */

	struct io_cnr_cspr_reg_sprscbbd	SPRSCBBD;	/* 2842_(1220 - 1227h) */
	struct io_cnr_cspr_reg_sprscrof	SPRSCROF;	/* 2842_(1228 - 122Fh) */
	struct io_cnr_cspr_reg_sprscrga	SPRSCRGA;	/* 2842_(1230 - 123Bh) */

	unsigned char dmy_123C_123F[0x1240-0x123C];	/* 2842_(123C - 123Fh) */

	struct io_cnr_cspr_reg_sprscrbd	SPRSCRBD;	/* 2842_(1240 - 1247h) */

	unsigned char dmy_1248_127F[0x1280-0x1248];	/* 2842_(1248 - 127Fh) */

	union  io_cnr_cspr_reg_sprd		SPRD;		/* 2842_(1280 - 1283h) */

	unsigned char dmy_1284_1287[0x1288-0x1284];	/* 2842_(1284 - 1287h) */

	struct io_cnr_cspr_reg_sprdcbof	SPRDCBOF;	/* 2842_(1288 - 128Fh) */
	struct io_cnr_cspr_reg_sprdcbga	SPRDCBGA;	/* 2842_(1290 - 129Bh) */

	unsigned char dmy_129C_129F[0x12A0-0x129C];	/* 2842_(129C - 129Fh) */

	struct io_cnr_cspr_reg_sprdcbbd	SPRDCBBD;	/* 2842_(12A0 - 12A7h) */
	struct io_cnr_cspr_reg_sprdcrof	SPRDCROF;	/* 2842_(12A8 - 12AFh) */
	struct io_cnr_cspr_reg_sprdcrga	SPRDCRGA;	/* 2842_(12B0 - 12BBh) */

	unsigned char dmy_12BC_12BF[0x12C0-0x12BC];	/* 2842_(12BC - 12BFh) */

	struct io_cnr_cspr_reg_sprdcrbd	SPRDCRBD;	/* 2842_(12C0 - 12C7h) */

	unsigned char dmy_12C8_1FFF[0x2000-0x12C8];	/* 2842_(12C8 - 1FFFh) */

};

#endif	  // #define __JDSCNR_CSPR_REG_H__
