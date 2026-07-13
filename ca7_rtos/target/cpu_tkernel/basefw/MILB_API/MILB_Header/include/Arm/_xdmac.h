/**
 * @file  _xdmac.h
 * @brief  Definition XDMAC Macro I/O register
 * @note  None
 * @attention None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/* structure of xdacs					(Base = 1914_0000h, SNAP Base = 1C25_0000h) */
union io_xdmac_xdacs {
	unsigned long		word;
	struct {
		unsigned long					:16;
		unsigned long	XS				:1;
		unsigned long					:3;
		unsigned long	LP				:1;
		unsigned long					:3;
		unsigned long	CP				:1;
		unsigned long					:3;
		unsigned long	XE				:1;
		unsigned long					:3;
	}bit;
};

/* structure of xdsac					(Base + 0x1Ch,4Ch,7Ch,ACh,DCh,10Ch,13Ch,16Ch) */
union io_xdmac_xdsac {
	unsigned long		word;
	struct {
		unsigned long	SRL 			:1;
		unsigned long					:1;
		unsigned long	SAF 			:1;
		unsigned long					:5;
		unsigned long	SBL 			:4;
		unsigned long					:4;
		unsigned long	SBS 			:2;
		unsigned long					:14;
	}bit;
};

/* structure of xddac					(Base + 0x20h,50h,80h,B0h,E0h,110h,140h,170h) */
union io_xdmac_xddac {
	unsigned long		word;
	struct {
		unsigned long	DRL 			:1;
		unsigned long					:1;
		unsigned long	DAF 			:1;
		unsigned long					:5;
		unsigned long	DBL 			:4;
		unsigned long					:4;
		unsigned long	DBS 			:2;
		unsigned long					:14;
	}bit;
};

/* structure of xddes					(Base + 0x28h,58h,88h,B8h,E8h,118h,148h,178h) */
union io_xdmac_xddes {
	unsigned long		word;
	struct {
		unsigned long	TI				:1;
		unsigned long	EI				:1;
		unsigned long					:2;
		unsigned long	AT				:1;
		unsigned long					:3;
		unsigned long	BR				:1;
		unsigned long					:3;
		unsigned long	BT				:1;
		unsigned long					:2;
		unsigned long	SA				:1;
		unsigned long					:4;
		unsigned long	TF				:4;
		unsigned long	SE				:1;
		unsigned long					:3;
		unsigned long	CE				:1;
		unsigned long					:3;
	}bit;
};

/* structure of xddpc					(Base + 0x2Ch,5Ch,8Ch,BCh,ECh,11Ch,14Ch,17Ch) */
union io_xdmac_xddpc {
	unsigned long		word;
	struct {
		unsigned long	DP				:3;
		unsigned long					:1;
		unsigned long	SP				:3;
		unsigned long					:25;
	}bit;
};

/* structure of xddsd					(Base + 0x30h,60h,90h,C0h,F0h,120h,150h,180h) */
union io_xdmac_xddsd {
	unsigned long		word;
	struct {
		unsigned long	IS				:4;
		unsigned long					:12;
		unsigned long	TS				:1;
		unsigned long					:15;
	}bit;
};

/* structure of element					(Base + 0x10h,40h,70h,A0h,D0h,100h,130h,160h) */
struct io_xdmac_element {
	unsigned long						XDTBC;					// Base_(0010 - 0013) Transfer Byte Count Register
	unsigned long						XDSSA;					// Base_(0014 - 0017) Source Start Address Register
	unsigned long						XDDSA;					// Base_(0018 - 001B) Destination Start Address Register
	union io_xdmac_xdsac				XDSAC;					// Base_(001C - 001F) Source Access Configuration Register
	union io_xdmac_xddac				XDDAC;					// Base_(0020 - 0023) Destination Access Configuration Register
	unsigned long						XDDCC;					// Base_(0024 - 0027) Descriptor Chain Configuration Register
	union io_xdmac_xddes				XDDES;					// Base_(0028 - 002B) DMA Enable Setting Register
	union io_xdmac_xddpc				XDDPC;					// Base_(002C - 002F) DMA Protection Control Register
	union io_xdmac_xddsd				XDDSD;					// Base_(0030 - 0033) DMA Status Display Register
	unsigned char						dmy_34_3F[0x40 - 0x34];	// Base_(0034 - 003F) Reserved
};

/* XDMAC register							*/
struct io_xdmac {
	union io_xdmac_xdacs				XDACS;					// Base_(0000 - 0003) All Channel Setting Register
	unsigned char						dmy_04_0F[0x10 - 0x04];	// Base_(0004 - 000F) Reserved
	struct io_xdmac_element CH[8];
};

extern volatile struct io_xdmac		IO_XDMAC;
extern volatile struct io_xdmac		IO_XDMACS;
