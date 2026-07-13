/**
 * @file		osd_icon_comm.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_ICON_COMM_H_
#define _OSD_ICON_COMM_H_

#include "osd_draw_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_BF_OSD_ICON_COMM_STR_ID_SURPRISE_MARK		(0)		/**< Surprise mark */
#define D_BF_OSD_ICON_COMM_STR_ID_AND_MARK			(1)		/**< And_mark */
#define D_BF_OSD_ICON_COMM_STR_ID_DASH_MARK			(2)		/**< Dash mark */
#define D_BF_OSD_ICON_COMM_STR_ID_LEFT_PARENTHESIS	(3)		/**< LEFT_parenthesis */
#define D_BF_OSD_ICON_COMM_STR_ID_RIGHT_PARENTHESIS	(4)		/**< RIGHT_parenthesis */
#define D_BF_OSD_ICON_COMM_STR_ID_CROSS_MARK		(5)		/**< Cross_mark */
#define D_BF_OSD_ICON_COMM_STR_ID_PLUS_MARK			(6)		/**< Plus_mark */
#define D_BF_OSD_ICON_COMM_STR_ID_COMMA_MARK		(7)		/**< Comma mark */
#define D_BF_OSD_ICON_COMM_STR_ID_MINUS_MARK		(8)		/**< Minus mark */
#define D_BF_OSD_ICON_COMM_STR_ID_DOT_MARK			(9)		/**< Dot mark */
#define D_BF_OSD_ICON_COMM_STR_ID_SLASH_MARK		(10)	/**< Slash mark */
#define D_BF_OSD_ICON_COMM_STR_ID_NUM_ZERO			(11)	/**< 0 */
#define D_BF_OSD_ICON_COMM_STR_ID_012				(12)	/**< ID=12 data */
#define D_BF_OSD_ICON_COMM_STR_ID_013				(13)	/**< ID=13 data */
#define D_BF_OSD_ICON_COMM_STR_ID_014				(14)	/**< ID=14 data */
#define D_BF_OSD_ICON_COMM_STR_ID_015				(15)	/**< ID=15 data */
#define D_BF_OSD_ICON_COMM_STR_ID_016				(16)	/**< ID=16 data */
#define D_BF_OSD_ICON_COMM_STR_ID_017				(17)	/**< ID=17 data */
#define D_BF_OSD_ICON_COMM_STR_ID_018				(18)	/**< ID=18 data */
#define D_BF_OSD_ICON_COMM_STR_ID_019				(19)	/**< ID=19 data */
#define D_BF_OSD_ICON_COMM_STR_ID_NUM_NINE			(20)	/**< 9 */
#define D_BF_OSD_ICON_COMM_STR_ID_COLON_MARK		(21)	/**< Colon mark */
#define D_BF_OSD_ICON_COMM_STR_ID_QUESTION_MARK		(22)	/**< question mark */
#define D_BF_OSD_ICON_COMM_STR_ID_UPPER_A			(23)	/**< A */
#define D_BF_OSD_ICON_COMM_STR_ID_024				(24)	/**< ID=24 data */
#define D_BF_OSD_ICON_COMM_STR_ID_025				(25)	/**< ID=25 data */
#define D_BF_OSD_ICON_COMM_STR_ID_026				(26)	/**< ID=26 data */
#define D_BF_OSD_ICON_COMM_STR_ID_027				(27)	/**< ID=27 data */
#define D_BF_OSD_ICON_COMM_STR_ID_028				(28)	/**< ID=28 data */
#define D_BF_OSD_ICON_COMM_STR_ID_029				(29)	/**< ID=29 data */
#define D_BF_OSD_ICON_COMM_STR_ID_030				(30)	/**< ID=30 data */
#define D_BF_OSD_ICON_COMM_STR_ID_031				(31)	/**< ID=31 data */
#define D_BF_OSD_ICON_COMM_STR_ID_032				(32)	/**< ID=32 data */
#define D_BF_OSD_ICON_COMM_STR_ID_033				(33)	/**< ID=33 data */
#define D_BF_OSD_ICON_COMM_STR_ID_034				(34)	/**< ID=34 data */
#define D_BF_OSD_ICON_COMM_STR_ID_035				(35)	/**< ID=35 data */
#define D_BF_OSD_ICON_COMM_STR_ID_036				(36)	/**< ID=36 data */
#define D_BF_OSD_ICON_COMM_STR_ID_037				(37)	/**< ID=37 data */
#define D_BF_OSD_ICON_COMM_STR_ID_038				(38)	/**< ID=38 data */
#define D_BF_OSD_ICON_COMM_STR_ID_039				(39)	/**< ID=39 data */
#define D_BF_OSD_ICON_COMM_STR_ID_040				(40)	/**< ID=40 data */
#define D_BF_OSD_ICON_COMM_STR_ID_041				(41)	/**< ID=41 data */
#define D_BF_OSD_ICON_COMM_STR_ID_042				(42)	/**< ID=42 data */
#define D_BF_OSD_ICON_COMM_STR_ID_043				(43)	/**< ID=43 data */
#define D_BF_OSD_ICON_COMM_STR_ID_044				(44)	/**< ID=44 data */
#define D_BF_OSD_ICON_COMM_STR_ID_045				(45)	/**< ID=45 data */
#define D_BF_OSD_ICON_COMM_STR_ID_046				(46)	/**< ID=46 data */
#define D_BF_OSD_ICON_COMM_STR_ID_047				(47)	/**< ID=47 data */
#define D_BF_OSD_ICON_COMM_STR_ID_UPPER_Z			(48)	/**< Z */
#define D_BF_OSD_ICON_COMM_STR_ID_UNDERLINE_MARK	(49)	/**< Under line */
#define D_BF_OSD_ICON_COMM_STR_ID_LOW_A				(50)	/**< a */
#define D_BF_OSD_ICON_COMM_STR_ID_051				(51)	/**< ID=51 data */
#define D_BF_OSD_ICON_COMM_STR_ID_052				(52)	/**< ID=52 data */
#define D_BF_OSD_ICON_COMM_STR_ID_053				(53)	/**< ID=53 data */
#define D_BF_OSD_ICON_COMM_STR_ID_054				(54)	/**< ID=54 data */
#define D_BF_OSD_ICON_COMM_STR_ID_055				(55)	/**< ID=55 data */
#define D_BF_OSD_ICON_COMM_STR_ID_056				(56)	/**< ID=56 data */
#define D_BF_OSD_ICON_COMM_STR_ID_057				(57)	/**< ID=57 data */
#define D_BF_OSD_ICON_COMM_STR_ID_058				(58)	/**< ID=58 data */
#define D_BF_OSD_ICON_COMM_STR_ID_059				(59)	/**< ID=59 data */
#define D_BF_OSD_ICON_COMM_STR_ID_060				(60)	/**< ID=60 data */
#define D_BF_OSD_ICON_COMM_STR_ID_061				(61)	/**< ID=61 data */
#define D_BF_OSD_ICON_COMM_STR_ID_062				(62)	/**< ID=62 data */
#define D_BF_OSD_ICON_COMM_STR_ID_063				(63)	/**< ID=63 data */
#define D_BF_OSD_ICON_COMM_STR_ID_064				(64)	/**< ID=64 data */
#define D_BF_OSD_ICON_COMM_STR_ID_065				(65)	/**< ID=65 data */
#define D_BF_OSD_ICON_COMM_STR_ID_066				(66)	/**< ID=66 data */
#define D_BF_OSD_ICON_COMM_STR_ID_067				(67)	/**< ID=67 data */
#define D_BF_OSD_ICON_COMM_STR_ID_068				(68)	/**< ID=68 data */
#define D_BF_OSD_ICON_COMM_STR_ID_069				(69)	/**< ID=69 data */
#define D_BF_OSD_ICON_COMM_STR_ID_070				(70)	/**< ID=70 data */
#define D_BF_OSD_ICON_COMM_STR_ID_071				(71)	/**< ID=71 data */
#define D_BF_OSD_ICON_COMM_STR_ID_072				(72)	/**< ID=72 data */
#define D_BF_OSD_ICON_COMM_STR_ID_073				(73)	/**< ID=73 data */
#define D_BF_OSD_ICON_COMM_STR_ID_074				(74)	/**< ID=74 data */
#define D_BF_OSD_ICON_COMM_STR_ID_LOW_Z				(75)	/**< z */
#define D_BF_OSD_ICON_COMM_STR_ID_076				(76)	/**< ID=76 data */
#define D_BF_OSD_ICON_COMM_STR_ID_077				(77)	/**< ID=77 data */
#define D_BF_OSD_ICON_COMM_STR_ID_078				(78)	/**< ID=78 data */
#define D_BF_OSD_ICON_COMM_STR_ID_079				(79)	/**< ID=79 data */
#define D_BF_OSD_ICON_COMM_STR_ID_080				(80)	/**< ID=80 data */
#define D_BF_OSD_ICON_COMM_STR_ID_081				(81)	/**< ID=81 data */
#define D_BF_OSD_ICON_COMM_STR_ID_082				(82)	/**< ID=82 data */
#define D_BF_OSD_ICON_COMM_STR_ID_083				(83)	/**< ID=83 data */
#define D_BF_OSD_ICON_COMM_STR_ID_084				(84)	/**< ID=84 data */
#define D_BF_OSD_ICON_COMM_STR_ID_085				(85)	/**< ID=85 data */
#define D_BF_OSD_ICON_COMM_STR_ID_086				(86)	/**< ID=86 data */
#define D_BF_OSD_ICON_COMM_STR_ID_087				(87)	/**< ID=87 data */
#define D_BF_OSD_ICON_COMM_STR_ID_088				(88)	/**< ID=88 data */
#define D_BF_OSD_ICON_COMM_STR_ID_089				(89)	/**< ID=89 data */
#define D_BF_OSD_ICON_COMM_STR_ID_090				(90)	/**< ID=90 data */
#define D_BF_OSD_ICON_COMM_STR_ID_091				(91)	/**< ID=91 data */
#define D_BF_OSD_ICON_COMM_STR_ID_092				(92)	/**< ID=92 data */
#define D_BF_OSD_ICON_COMM_STR_ID_093				(93)	/**< ID=93 data */
#define D_BF_OSD_ICON_COMM_STR_ID_094				(94)	/**< ID=94 data */
#define D_BF_OSD_ICON_COMM_STR_ID_095				(95)	/**< ID=95 data */
#define D_BF_OSD_ICON_COMM_STR_ID_096				(96)	/**< ID=96 data */
#define D_BF_OSD_ICON_COMM_STR_ID_097				(97)	/**< ID=97 data */
#define D_BF_OSD_ICON_COMM_STR_ID_098				(98)	/**< ID=98 data */
#define D_BF_OSD_ICON_COMM_STR_ID_099				(99)	/**< ID=99 data */
#define D_BF_OSD_ICON_COMM_STR_ID_100				(100)	/**< ID=100 data */
#define D_BF_OSD_ICON_COMM_STR_ID_101				(101)	/**< ID=101 data */
#define D_BF_OSD_ICON_COMM_STR_ID_102				(102)	/**< ID=102 data */
#define D_BF_OSD_ICON_COMM_STR_ID_103				(103)	/**< ID=103 data */
#define D_BF_OSD_ICON_COMM_STR_ID_104				(104)	/**< ID=104 data */
#define D_BF_OSD_ICON_COMM_STR_ID_105				(105)	/**< ID=105 data */
#define D_BF_OSD_ICON_COMM_STR_ID_106				(106)	/**< ID=106 data */
#define D_BF_OSD_ICON_COMM_STR_ID_107				(107)	/**< ID=107 data */
#define D_BF_OSD_ICON_COMM_STR_ID_108				(108)	/**< ID=108 data */
#define D_BF_OSD_ICON_COMM_STR_ID_109				(109)	/**< ID=109 data */
#define D_BF_OSD_ICON_COMM_STR_ID_110				(110)	/**< ID=110 data */
#define D_BF_OSD_ICON_COMM_STR_ID_111				(111)	/**< ID=111 data */
#define D_BF_OSD_ICON_COMM_STR_ID_112				(112)	/**< ID=112 data */
#define D_BF_OSD_ICON_COMM_STR_ID_113				(113)	/**< ID=113 data */
#define D_BF_OSD_ICON_COMM_STR_ID_114				(114)	/**< ID=114 data */
#define D_BF_OSD_ICON_COMM_STR_ID_115				(115)	/**< ID=115 data */
#define D_BF_OSD_ICON_COMM_STR_ID_116				(116)	/**< ID=116 data */
#define D_BF_OSD_ICON_COMM_STR_ID_117				(117)	/**< ID=117 data */
#define D_BF_OSD_ICON_COMM_STR_ID_118				(118)	/**< ID=118 data */
#define D_BF_OSD_ICON_COMM_STR_ID_119				(119)	/**< ID=119 data */
#define D_BF_OSD_ICON_COMM_STR_ID_120				(120)	/**< ID=120 data */
#define D_BF_OSD_ICON_COMM_STR_ID_121				(121)	/**< ID=121 data */
#define D_BF_OSD_ICON_COMM_STR_ID_122				(122)	/**< ID=122 data */
#define D_BF_OSD_ICON_COMM_STR_ID_123				(123)	/**< ID=123 data */
#define D_BF_OSD_ICON_COMM_STR_ID_124				(124)	/**< ID=124 data */
#define D_BF_OSD_ICON_COMM_STR_ID_125				(125)	/**< ID=125 data */
#define D_BF_OSD_ICON_COMM_STR_ID_126				(126)	/**< ID=126 data */
#define D_BF_OSD_ICON_COMM_STR_ID_127				(127)	/**< ID=127 data */
#define D_BF_OSD_ICON_COMM_STR_ID_128				(128)	/**< ID=128 data */
#define D_BF_OSD_ICON_COMM_STR_ID_129				(129)	/**< ID=129 data */
#define D_BF_OSD_ICON_COMM_STR_ID_130				(130)	/**< ID=130 data */
#define D_BF_OSD_ICON_COMM_STR_ID_131				(131)	/**< ID=131 data */
#define D_BF_OSD_ICON_COMM_STR_ID_132				(132)	/**< ID=132 data */
#define D_BF_OSD_ICON_COMM_STR_ID_133				(133)	/**< ID=133 data */
#define D_BF_OSD_ICON_COMM_STR_ID_134				(134)	/**< ID=134 data */
#define D_BF_OSD_ICON_COMM_STR_ID_135				(135)	/**< ID=135 data */
#define D_BF_OSD_ICON_COMM_STR_ID_136				(136)	/**< ID=136 data */
#define D_BF_OSD_ICON_COMM_STR_ID_137				(137)	/**< ID=137 data */
#define D_BF_OSD_ICON_COMM_STR_ID_138				(138)	/**< ID=138 data */
#define D_BF_OSD_ICON_COMM_STR_ID_139				(139)	/**< ID=139 data */
#define D_BF_OSD_ICON_COMM_STR_ID_140				(140)	/**< ID=140 data */
#define D_BF_OSD_ICON_COMM_STR_ID_141				(141)	/**< ID=141 data */
#define D_BF_OSD_ICON_COMM_STR_ID_142				(142)	/**< ID=142 data */
#define D_BF_OSD_ICON_COMM_STR_ID_DEGREE_MARK		(143)	/**< Degree mark */
#define D_BF_OSD_ICON_COMM_STR_ID_SPACE_MARK		(144)	/**< Space mark */
#define D_BF_OSD_ICON_COMM_STR_ID_145				(145)	/**< ID=145 data */
#define D_BF_OSD_ICON_COMM_STR_ID_146				(146)	/**< ID=146 data */
#define D_BF_OSD_ICON_COMM_STR_ID_147				(147)	/**< ID=147 data */
#define D_BF_OSD_ICON_COMM_STR_ID_148				(148)	/**< ID=148 data */

#define D_BF_OSD_ICON_COMM_ALPHA_TRANS				(0)		/**< Alpha trans */
#define D_BF_OSD_ICON_COMM_HEIGHT_MASK			(0x3FFF)	/**< Height mask */

#define D_BF_OSD_ICON_ID_CAPTURE					(71)	/**< ID:71 - SCENE */
#define D_BF_OSD_ICON_ID_VIDEO						(92)	/**< ID:92 - MOVIE REC */
#define D_BF_OSD_ICON_ID_RECORDING					(93)	/**< ID:93 - REC */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Icon format selection */
typedef enum {
	E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON,		/**< VGR icon */
	E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_1bit,	/**< VGR icon 1/8 byte(Palette format) */
	E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_2bit,	/**< VGR icon 1/4 byte(Palette format) */
	E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_4bit,	/**< VGR icon 0.5 byte(Palette format) */
	E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_8bit,	/**< VGR icon 1 byte(Palette format) */
	E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_16bit,	/**< VGR icon 2 byte(Palette format) */
} E_BF_OSD_ICON_COMM_ICON_FMT;

/** Icon size */
typedef enum {
	E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH = 0,	/**< width */
	E_BF_OSD_ICON_COMM_ICON_SIZE_LINES,		/**< lines */
	E_BF_OSD_ICON_COMM_ICON_SIZE_END		/**< Number of enums */
} E_BF_OSD_ICON_COMM_ICON_SIZE;

/** Icon format */
typedef enum {
	E_BF_OSD_ICON_COMM_ICON_FORMAT_RGBA  = 0x0000,	/**< RGBA */
	E_BF_OSD_ICON_COMM_ICON_FORMAT_16BIT = 0x4000,	/**< 16bit */
	E_BF_OSD_ICON_COMM_ICON_FORMAT_8BIT  = 0x8000,	/**< 8bit */
	E_BF_OSD_ICON_COMM_ICON_FORMAT_4BIT  = 0xC000,	/**< 4bit */
	E_BF_OSD_ICON_COMM_ICON_FORMAT_2BIT  = 0xA000,	/**< 2bit */
	E_BF_OSD_ICON_COMM_ICON_FORMAT_1BIT  = 0xE000,	/**< 1bit */
	E_BF_OSD_ICON_COMM_ICON_FORMAT_NON   = ~E_BF_OSD_ICON_COMM_ICON_FORMAT_1BIT	/**< None */
} E_BF_OSD_ICON_COMM_ICON_FORMAT;

/** Compress format */
typedef enum {
	E_BF_OSD_ICON_COMM_COMPRESS_FORMAT_NONE = 0,	/**< None */
	E_BF_OSD_ICON_COMM_COMPRESS_FORMAT_LZW,			/**< LZW */
	E_BF_OSD_ICON_COMM_COMPRESS_FORMAT_OTHER		/**< Other */
} E_BF_OSD_ICON_COMM_COMPRESS_FORMAT;


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Icon information */
typedef struct {
	USHORT	mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_END];	/**< size */
	CHAR*	mBuffer;		/**< address */
	USHORT	icon_format;	/**< format */
} T_BF_OSD_ICON_COMM_ICON_INFO;

/** Icon number format */
typedef union {
	UINT32 word;
	struct {
		UINT32 Num:28;			/**< Icon num */
		UINT32 RgbaXxxx:2;		/**< 0=RGBA8888  1:RGBA4444 */
		UINT32 Compress_Flag:2;	/**< Compress format. See @ref E_BF_OSD_ICON_COMM_COMPRESS_FORMAT */
	} Icon_Num_flag;
}U_BF_OSD_ICON_COMM_ICON_NUM_FORMAT;

/** Icon table ID */
typedef union {
	UINT32 tableID_iconID;
	struct {
		USHORT iconID;	/**< Index of Icon file format table. See @ref T_BF_OSD_ICON_COMM_ICON_FILE_FMT */
		USHORT tableID;	/**< Draw object type. See @ref E_Draw_Type */
	} short_data;
}U_BF_OSD_ICON_COMM_ICON_TABLE_ID;

/** Icon file format */
typedef struct {
	USHORT	mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_END];	/**< icon size */
	UINT32	mIcon_offset;	/**< icon file address offset */
} T_BF_OSD_ICON_COMM_ICON_FILE_FMT;

/** String information */
typedef struct {
	BYTE	icon_id_num;		/**< Number of ICON-ID included in 1 string. */
	USHORT* icon_id_list_ptr;	/**< String-ID (ICON-ID list) */
} T_BF_OSD_ICON_COMM_STRING_INFO;

/** Parameter for cache */
typedef struct {
	U_BF_OSD_ICON_COMM_ICON_TABLE_ID IconKind_id;	/**< Icon table ID */
	UINT32 compressed_size;							/**< Compressed icon data size */
	UINT32 uncompressed_size;						/**< unompressed icon data size */
	T_BF_OSD_ICON_COMM_ICON_FILE_FMT* iconList;		/**< Icon file format table. */
	U_BF_OSD_ICON_COMM_ICON_NUM_FORMAT Compress_Format_Icon_num;	/**< Icon number format */
} T_BF_OSD_ICON_COMM_PARAMETER4CACHE;

/** String file format */
typedef struct {
	USHORT	icon_id_num;		/**< Number of ICON-ID included in 1 string. */
	USHORT	string_data_offset;	/**< String id buffer offset(from head of buffer) */
} T_BF_OSD_ICON_COMM_STRING_FILE_FMT;

/** Draw icon information */
typedef struct {
	T_POS							string_iocn_pos;	/**< Icon display positon */
	T_BF_OSD_ICON_COMM_ICON_INFO	icon_data;			/**< Icon data */
	T_Rect							display_window;		/**< Icon displaywindow */
	BOOL							draw_flag;			/**< TRUE :draw */
	OSD_USHORT_INT* 				rgba_palette_table;	/**< palette table */
	BYTE							start_pos_is_odd;	/**< X position is odd */
} T_BF_OSD_ICON_COMM_DRAW_ICON_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern E_Draw_Type gBF_Osd_Icon_Font_ID;

extern E_FJ_OSD_ICON_BOOT gBF_Osd_Icon_Font_Boot_ID;

/** valid data num in String_Table */
extern UINT32* gBF_Osd_Icon_String_Table_Num[E_Draw_Type_End];

/** valid data num in Icon_Table/Font */
extern UINT32* gBF_Osd_Icon_Icon_Table_Num[E_Draw_Type_End];

/** last icon's size that compressed */
extern UINT32 gBF_Osd_Icon_Icon_Table_Last_offeset[E_Draw_Type_End];

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
Blend the icon data with displaying current when icon draw
@param[in]   global_width_display	global width
@param[in]   iconInfo	icon information
@param[in]   alpha		alpha blend
@param[in]   gr_Write_infor		destination address
@note        Blend the content with displaying when icon draw.
*/
extern VOID BF_Osd_Icon_Comm_Draw_Icon_By_Bending(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no, 
				UINT32 global_width_display, 
				T_BF_OSD_ICON_COMM_DRAW_ICON_INFO const* const iconInfo,BYTE alpha,
				ULONG gr_Write_infor);

/**
Re-caculate the address of icon by displaywindow
@param[in]	Font_ID		Font OS_USER_ID
@param[out]	icon_data	Icon data
@retval	TURE	used from the 0~4 bit of icon/font data if it's FONT data
@retval	FALSE	used from the 4~7 bit of icon/font data
 */
extern BYTE BF_Osd_Icon_Comm_Recalculate_Icon_Addr(E_BF_OSD_ICON_COMM_ICON_FMT Font_ID, T_BF_OSD_ICON_COMM_DRAW_ICON_INFO* iconInfo);

/**
Get RGBA Table index
@param[in]	a_icon_id	ICON ID.
@param[in]	iconInfo	ICON Draw Information.
@param[in]	iconList	Icon Buffer Information.
*/
extern VOID BF_Osd_Icon_Comm_Set_Icon_Data(T_BF_OSD_ICON_COMM_PARAMETER4CACHE* get_icon_buffer, T_BF_OSD_ICON_COMM_ICON_INFO* iconInfo);

/**
Get String OS_USER_ID from ASCII code
@param[in]	ascii	ASCII code
@return 0xffff: error other: Font OS_USER_ID
@note	it can be convertd Latin 1 only
*/
extern UINT16 BF_Osd_Icon_Comm_Conv_Ascii_To_Icon_ID(BYTE ascii);

/**
Clear ICON data loading state.
*/
extern VOID BF_Osd_Icon_Comm_Clear_Resource_Load_State(VOID);

/**
Copy RGBA data to SDRAM
@param[in]	iconInfo		ICON Draw Information.
@param[in]	DspWidth		Global size of LCD
@param[in]	WriteInfo		Write address
*/
extern VOID BF_Osd_Icon_Comm_Copy_Box_To_Box(T_BF_OSD_ICON_COMM_DRAW_ICON_INFO const* const iconInfo,
                                      UINT32 DspWidth,ULONG WriteInfo);

/**
Get total width and max height in the string and icon address
@param[in]	disp_blocsk_no	Common block number.
@param[in]	String_id		icon string
@param[in]	icon_num		String_id's size
@param[out]	string_size		total width and max height
@param[out]	iconInfo		address
@return		Icon num or -1 if NG
*/
extern INT32 BF_Osd_Icon_Calculate_String_Size(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no, 
											   E_Draw_Type font_id, USHORT const* const icon_id_list_ptr,
											   INT32 icon_num, USHORT string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_END],
											   T_BF_OSD_ICON_COMM_DRAW_ICON_INFO iconInfo[MAX_ICON_NUM_IN_1STRING]);

/**
Get string data and draw rectangle
@param[in]	disp_block_no	Common block number.
@param[in]	uwStringID	string OS_USER_ID
@param[in]	StringTable_ID	String Table OS_USER_ID
@param[out]	iconInfo	Font's data list
@param[out]	string_size	text's size
@return text's num -1:NG
*/
extern SHORT BF_Osd_Icon_Get_String_Draw_Range(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no,
											   USHORT uwStringID, E_Draw_Type Font_ID,
											   T_BF_OSD_ICON_COMM_DRAW_ICON_INFO iconInfo[MAX_ICON_NUM_IN_1STRING],
											   USHORT string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_END]);

/**
Whether the resource data of ICON is loaded is returned.
@param[in]	font_id			ICON type.
@retval		TRUE  Loaded
@retval		FALSE Not loaded
*/
extern BOOL BF_Osd_Icon_Is_Icon_Loaded(E_Draw_Type font_id);

/**
Whether the resource data of string is loaded is returned.
@param[in]	font_id			ICON type.
@retval		TRUE  Loaded
@retval		FALSE Not loaded
*/
extern BOOL BF_Osd_Icon_Is_String_Loaded(E_Draw_Type font_id);

extern BOOL BF_Osd_Icon_Is_Icon_Boot_Loaded(E_FJ_OSD_ICON_BOOT font_id);

extern BOOL BF_Osd_Icon_Is_String_Boot_Loaded(E_FJ_OSD_ICON_BOOT font_id);

/**
Clear ICON data loading state.
*/
extern VOID BF_Osd_Icon_Clear_Resource_Load_State(VOID);

/**
Load a ICON/FONT file to memory
@param[in]	szFontName		font name.
@param[in]	font_ID			ICON type.
@return		GUI string buffer address for display
*/
extern FJ_ERR_CODE BF_Osd_Icon_Load_Icon_File(CHAR const* const szFontName, E_Draw_Type font_ID);

/**
Load string file.
@param[in]	string_table_file	File path of string file.
@param[in]	icon_fontID			ICON type.
*/
extern FJ_ERR_CODE BF_Osd_Icon_Load_String_File(CHAR const* const string_table_file, E_Draw_Type icon_fontID);

/**
Load a RGBA8888 ICON palette table file to memory
@param[in]	RGBPaletteTableName	palette table name.
@param[in]	icon_fontID			ICON type.
@return		1 is OK and 0 is NG
*/
extern FJ_ERR_CODE BF_Osd_Icon_Load_RGB_Palette_Table(const char* RGBPaletteTableName, E_Draw_Type icon_fontID);

/**
Get FontIcon_Buffer Address for Display.
@param[in]	icon_fontID		ICON type.
@param[in]	max_size		max size.
@param[in]	if_exclude_size	exclude flag.
@return		GUI Y Address for display
*/
extern ULONG BF_Osd_Icon_Get_Icon_Buffer_Addr(E_Draw_Type icon_fontID, UINT32* max_size, BOOL if_exclude_size);

/**
Get string buffer address.
@param[in]	icon_fontID		ICON type.
@param[in]	max_size		max size.
@param[in]	if_exclude_size	exclude flag.
@return		GUI string buffer address for display
*/
extern ULONG BF_Osd_Icon_Get_String_Buffer_Addr(E_Draw_Type icon_fontID, UINT32* max_size, BOOL if_exclude_size);

/**
Get palette buffer address.
@param[in]	icon_fontID		ICON type.
@param[in]	max_size		max size.
@param[in]	if_exclude_size	exclude flag.
@return		GUI palette buffer address for display
*/
extern ULONG BF_Osd_Icon_Get_Palette_Buffer_Addr(E_Draw_Type icon_fontID, UINT32* max_size, BOOL if_exclude_size);

extern FJ_ERR_CODE BF_Osd_Icon_Load_Icon_File_Boot( const E_FJ_OSD_ICON_BOOT type, UINT32 addr );

extern FJ_ERR_CODE BF_Osd_Icon_Load_Icon_String_Boot( const E_FJ_OSD_ICON_BOOT type, UINT32 addr );

extern VOID BF_Osd_Icon_Config( VOID );

extern VOID BF_Osd_Icon_Draw( const INT32 icon_type );

extern VOID BF_Osd_Icon_Erase( const INT32 icon_type );

#endif // _OSD_ICON_COMM_H_
