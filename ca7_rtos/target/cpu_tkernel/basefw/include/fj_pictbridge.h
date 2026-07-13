/**
 * @file		fj_pictbridge.h
 * @brief		PictBridge function set.
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
 
#ifndef _FJ_PICTBRIDGE_H
#define _FJ_PICTBRIDGE_H

#include "fj_usb.h"
#include "fj_host.h"
#include "MessageQueue.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pictbridge_overview
@{
	The pictbridge controls can be achieved by using this API. 
@}*//* --- end of fj_pictbridge_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @weakgroup fj_pictbridge_sequence
@{
	- @ref Connect_PictBridge_Mode_Figure
	- @ref Start_printting_job_Figure
	- @ref Resume_printting_job_Figure
	- @ref Abort_printting_job_Figure
	<hr>
	@section Connect_PictBridge_Mode_Figure Connect PictBridge Mode
	@image html fj_pictbridge_connect_pictbridge_mode.png "Fig. Connect PictBridge Mode"

	@section Start_printting_job_Figure Start printing job
	@image html fj_pictbridge_start_printing_job.png	"Fig. Start printting job"

	@section Resume_printting_job_Figure Resume printing job
	@image html fj_pictbridge_resume_printing_job.png "Fig. Resume printting job"	
	
	@section Abort_printting_job_Figure Abort printing job
	@image html fj_pictbridge_abort_printing_job.png "Fig. Abort printting job"	
@}*//* --- end of fj_pictbridge_sequence */


/** @weakgroup fj_pictbridge_definition
@{*/	// fj_pictbridge_definition group
/******************************************************************************
definition
******************************************************************************/

#define FJ_DPS_CAPABILITY_MAX_ITEMS		(32)	/**< capability max item value = 32 */

//Qualities
#define XML_QUALITY_DEFAULT		(0x50000000)		/**< default quality */ 
#define XML_QUALITY_NORMAL		(0x50010000)		/**< normal quality */ 
#define XML_QUALITY_DRAFT		(0x50020000)		/**< draft quality */ 
#define XML_QUALITY_FINE		(0x50030000)		/**< fine quality */ 

//paperSize
#define XML_PAPERSIZE_DEFAULT	(0x51000000)		/**< default paper size */
#define XML_PAPERSIZE_L			(0x51010000)		/**< L paper size */
#define XML_PAPERSIZE_2L		(0x51020000)		/**< 2L paper size */
#define XML_PAPERSIZE_HAGAKI	(0x51030000)		/**< HAGAKI paper size */
#define XML_PAPERSIZE_CARD		(0x51040000)		/**< CARD paper size */
#define XML_PAPERSIZE_100X150	(0x51050000)		/**< 100X150 paper size */
#define XML_PAPERSIZE_4X6		(0x51060000)		/**< 4X6 paper size */
#define XML_PAPERSIZE_8X10		(0x51070000)		/**< 8X10 paper size */
#define XML_PAPERSIZE_LETTER	(0x51080000)		/**< LETTER paper size */
#define XML_PAPERSIZE_11X17		(0x510A0000)		/**< 11X17 paper size */
#define XML_PAPERSIZE_A0		(0x51100000)		/**< A0 paper size */
#define XML_PAPERSIZE_A1		(0x51110000)		/**< A1 paper size */
#define XML_PAPERSIZE_A2		(0x51120000)		/**< A2 paper size */
#define XML_PAPERSIZE_A3		(0x51130000)		/**< A3 paper size */
#define XML_PAPERSIZE_A4		(0x51140000)		/**< A4 paper size */
#define XML_PAPERSIZE_A5		(0x51150000)		/**< A5 paper size */
#define XML_PAPERSIZE_A6		(0x51160000)		/**< A6 paper size */
#define XML_PAPERSIZE_A7		(0x51170000)		/**< A7 paper size */
#define XML_PAPERSIZE_A8		(0x51180000)		/**< A8 paper size */
#define XML_PAPERSIZE_A9		(0x51190000)		/**< A9 paper size */
#define XML_PAPERSIZE_B0		(0x51200000)		/**< B0 paper size */
#define XML_PAPERSIZE_B1		(0x51210000)		/**< B1 paper size */
#define XML_PAPERSIZE_B2		(0x51220000)		/**< B2 paper size */
#define XML_PAPERSIZE_B3		(0x51230000)		/**< B3 paper size */
#define XML_PAPERSIZE_B4		(0x51240000)		/**< B4 paper size */
#define XML_PAPERSIZE_B5		(0x51250000)		/**< B5 paper size */
#define XML_PAPERSIZE_B6		(0x51260000)		/**< B6 paper size */
#define XML_PAPERSIZE_B7		(0x51270000)		/**< B7 paper size */
#define XML_PAPERSIZE_B8		(0x51280000)		/**< B8 paper size */
#define XML_PAPERSIZE_B9		(0x51290000)		/**< B9 paper size */
#define XML_PAPERSIZE_89ROLL	(0x51810000)		/**< 89ROLL paper size */
#define XML_PAPERSIZE_127ROLL	(0x51820000)		/**< 127ROLL paper size */
#define XML_PAPERSIZE_100ROLL	(0x51860000)		/**< 100ROLL paper size */
#define XML_PAPERSIZE_210ROLL	(0x51940000)		/**< 210ROLL paper size */

//PaperType
#define XML_PAPERTYPE_DEFAULT		(0x52000000)		/**< default paper type */
#define XML_PAPERTYPE_PLAIN			(0x52010000)		/**< plain paper type */
#define XML_PAPERTYPE_PHOTO			(0x52020000)		/**< photo paper type */
#define XML_PAPERTYPE_FAST_PHOTO	(0x52030000)		/**< fast photo type*/

//PaperType Minor
#define XML_PAPERTYPE_MINOR_DEFAULT						(0x0000)		/**< paper type minor : default */
#define XML_PAPERTYPE_MINOR_STATIONERY					(0x0001)		/**< paper type minor : stationery */
#define XML_PAPERTYPE_MINOR_STATIONERY_COATED			(0x0002)		/**< paper type minor : stationery coated */
#define XML_PAPERTYPE_MINOR_STATIONERY_INKJET			(0x0003)		/**< paper type minor : stationery inkjet */
#define XML_PAPERTYPE_MINOR_STATIONERY_PREPRINTED		(0x0004)		/**< paper type minor : stationery preprinted */
#define XML_PAPERTYPE_MINOR_STATIONERY_LETTERHEAD		(0x0005)		/**< paper type minor : stationery letterhead */
#define XML_PAPERTYPE_MINOR_STATIONERY_PREPUNCHED		(0x0006)		/**< paper type minor : stationery prepunched */
#define XML_PAPERTYPE_MINOR_STATIONERY_FINE				(0x0007)		/**< paper type minor : stationery fine */
#define XML_PAPERTYPE_MINOR_STATIONERY_HEAVYWEIGHT		(0x0008)		/**< paper type minor : stationery heavyweight */
#define XML_PAPERTYPE_MINOR_STATIONERY_LIGHTWEIGHT		(0x0009)		/**< paper type minor : stationery lightweight */
#define XML_PAPERTYPE_MINOR_TRANSPARENCY				(0x000A)		/**< paper type minor : transparency */
#define XML_PAPERTYPE_MINOR_ENVELOPE					(0x000B)		/**< paper type minor : envelope */
#define XML_PAPERTYPE_MINOR_ENVELOPE_PLAIN				(0x000C)		/**< paper type minor : envelope plain */
#define XML_PAPERTYPE_MINOR_ENVELOPE_WINDOW				(0x000D)		/**< paper type minor : envelope window */
#define XML_PAPERTYPE_MINOR_CONTINUOUS					(0x000E)		/**< paper type minor : continuous */
#define XML_PAPERTYPE_MINOR_CONTINUOUS_LONG				(0x000F)		/**< paper type minor : continuous long */
#define XML_PAPERTYPE_MINOR_CONTINUOUS_SHORT			(0x0010)		/**< paper type minor : continuous short */
#define XML_PAPERTYPE_MINOR_TAB_STOCK					(0x0011)		/**< paper type minor : tab stock */
#define XML_PAPERTYPE_MINOR_PRECUT_TAB					(0x0012)		/**< paper type minor : precut tab */
#define XML_PAPERTYPE_MINOR_FULLCUT_TAB					(0x0013)		/**< paper type minor : fullcut tab */
#define XML_PAPERTYPE_MINOR_MULTIPART_FORM				(0x0014)		/**< paper type minor : multipart form */
#define XML_PAPERTYPE_MINOR_LABEL						(0x0015)		/**< paper type minor : label */
#define XML_PAPERTYPE_MINOR_MULTILAYER					(0x0016)		/**< paper type minor : multilayer */
#define XML_PAPERTYPE_MINOR_SCREEN						(0x0017)		/**< paper type minor : screen */
#define XML_PAPERTYPE_MINOR_SCREEN_PAGED				(0x0018)		/**< paper type minor : screen paged */
#define XML_PAPERTYPE_MINOR_PHOTOGRAPHIC				(0x0019)		/**< paper type minor : photographic */
#define XML_PAPERTYPE_MINOR_PHOTOGRAPHIC_GLOSSY			(0x001A)		/**< paper type minor : photographic glossy */
#define XML_PAPERTYPE_MINOR_PHOTOGRAPHIC_HIGHGLOSS		(0x001B)		/**< paper type minor : photographic highgloss */
#define XML_PAPERTYPE_MINOR_PHOTOGRAPHIC_SEMIGLOSS		(0x001C)		/**< paper type minor : photographic semigloss */
#define XML_PAPERTYPE_MINOR_PHOTOGRAPHIC_SATIN			(0x001D)		/**< paper type minor : photographic satin */
#define XML_PAPERTYPE_MINOR_PHOTOGRAPHIC_MATTE			(0x001E)		/**< paper type minor : photographic matte */
#define XML_PAPERTYPE_MINOR_PHOTOGRAPHIC_FILM			(0x001F)		/**< paper type minor : photographic film */
#define XML_PAPERTYPE_MINOR_BACKPRINT_FILM				(0x0020)		/**< paper type minor : backprint film */
#define XML_PAPERTYPE_MINOR_CARDSTOCK					(0x0021)		/**< paper type minor : cardstock */

//fileType
#define XML_FILETYPE_DEFULT				(0x53000000)		/**< filetype : default */
#define XML_FILETYPE_EXIF_JPEG			(0x53010000)		/**< filetype : exif jpeg */
#define XML_FILETYPE_EXIF				(0x53020000)		/**< filetype : exif */
#define XML_FILETYPE_JPEG				(0x53030000)		/**< filetype : jpeg */
#define XML_FILETYPE_TIFF_EP			(0x53040000)		/**< filetype : tiff ep */
#define XML_FILETYPE_FLASHPIX			(0x53050000)		/**< filetype : flashpix */
#define XML_FILETYPE_BMP				(0x53060000)		/**< filetype : bmp */
#define XML_FILETYPE_CIFF				(0x53070000)		/**< filetype : ciff */
#define XML_FILETYPE_GIF				(0x53080000)		/**< filetype : gif */
#define XML_FILETYPE_JFIF				(0x53090000)		/**< filetype : jfif */
#define XML_FILETYPE_PCD				(0x530A0000)		/**< filetype : pcd */
#define XML_FILETYPE_PICT				(0x530B0000)		/**< filetype : pict */
#define XML_FILETYPE_PNG				(0x530C0000)		/**< filetype : png */
#define XML_FILETYPE_TIFF				(0x530D0000)		/**< filetype : tiff */
#define XML_FILETYPE_TIFF_IT			(0x530E0000)		/**< filetype : tiff it */
#define XML_FILETYPE_JP2				(0x530F0000)		/**< filetype : jp2 */
#define XML_FILETYPE_JPX				(0x53100000)		/**< filetype : jpx */
#define XML_FILETYPE_UNDEF_NONIMAGE		(0x53110000)		/**< filetype : undef nonimage */
#define XML_FILETYPE_ASSOCIATION		(0x53120000)		/**< filetype : association */
#define XML_FILETYPE_SCRIPT				(0x53130000)		/**< filetype : script */
#define XML_FILETYPE_EXECUTABLE			(0x53140000)		/**< filetype : executable */
#define XML_FILETYPE_TEXT				(0x53150000)		/**< filetype : text */
#define XML_FILETYPE_HTML				(0x53160000)		/**< filetype : html */
#define XML_FILETYPE_XHTML_PRINT		(0x53170000)		/**< filetype : xhtml print */
#define XML_FILETYPE_DPOF				(0x53180000)		/**< filetype : dpof */
#define XML_FILETYPE_AIFF				(0x53190000)		/**< filetype : aiff */
#define XML_FILETYPE_WAV				(0x531A0000)		/**< filetype : wav */
#define XML_FILETYPE_MP3				(0x531B0000)		/**< filetype : mp3 */
#define XML_FILETYPE_AVI				(0x531C0000)		/**< filetype : avi */
#define XML_FILETYPE_MPEG				(0x531D0000)		/**< filetype : mpeg */
#define XML_FILETYPE_ASF				(0x531E0000)		/**< filetype : asf */

//datePrint
#define XML_DATEPRINT_DEFAULT			(0x54000000)		/**< default date printf type */
#define XML_DATEPRINT_OFF				(0x54010000)		/**< date printf OFF */
#define XML_DATEPRINT_ON				(0x54020000)		/**< date printf ON */

//fileNamePrint
#define XML_FILENAME_PRINT_DEFAULT		(0x55000000)		/**< default */
#define XML_FILENAME_PRINT_OFF			(0x55010000)		/**< printf filename OFF */
#define XML_FILENAME_PRINT_ON			(0x55020000)		/**< printf filename ON */ 

//imageOptimize
#define XML_IMAGEOPTIMIZE_DEFAULT		(0x56000000)		/**< default image optimize */ 
#define XML_IMAGEOPTIMIZE_OFF			(0x56010000)		/**< image optimize OFF */ 
#define XML_IMAGEOPTIMIZE_ON			(0x56020000)		/**< image optimize ON */ 

//layouts
#define XML_LAYOUT_DEFAULT		(0x57000000)		/**< default layout */
#define XML_LAYOUT_1UP			(0x57010000)		/**< layout=1up */
#define XML_LAYOUT_2UP			(0x57020000)		/**< layout=2up */
#define XML_LAYOUT_3UP			(0x57030000)		/**< layout=3up */
#define XML_LAYOUT_4UP			(0x57040000)		/**< layout=4up */
#define XML_LAYOUT_5UP			(0x57050000)		/**< layout=5up */
#define XML_LAYOUT_6UP			(0x57060000)		/**< layout=6up */
#define XML_LAYOUT_7UP			(0x57070000)		/**< layout=7up */
#define XML_LAYOUT_8UP			(0x57080000)		/**< layout=8up */
#define XML_LAYOUT_9UP			(0x57090000)		/**< layout=9up */
#define XML_LAYOUT_10UP			(0x570A0000)		/**< layout=10up */
#define XML_LAYOUT_11UP			(0x570B0000)		/**< layout=11up */
#define XML_LAYOUT_12UP			(0x570C0000)		/**< layout=12up */
#define XML_LAYOUT_13UP			(0x570D0000)		/**< layout=13up */
#define XML_LAYOUT_14UP			(0x570E0000)		/**< layout=14up */
#define XML_LAYOUT_15UP			(0x570F0000)		/**< layout=15up */
#define XML_LAYOUT_16UP			(0x57100000)		/**< layout=16up */
#define XML_LAYOUT_17UP			(0x57110000)		/**< layout=17up */
#define XML_LAYOUT_18UP			(0x57120000)		/**< layout=18up */
#define XML_LAYOUT_19UP			(0x57130000)		/**< layout=19up */
#define XML_LAYOUT_20UP			(0x57140000)		/**< layout=20up */
#define XML_LAYOUT_21UP			(0x57150000)		/**< layout=21up */
#define XML_LAYOUT_22UP			(0x57160000)		/**< layout=22up */
#define XML_LAYOUT_23UP			(0x57170000)		/**< layout=23up */
#define XML_LAYOUT_24UP			(0x57180000)		/**< layout=24up */
#define XML_LAYOUT_25UP			(0x57190000)		/**< layout=25up */
#define XML_LAYOUT_26UP			(0x571A0000)		/**< layout=26up */
#define XML_LAYOUT_27UP			(0x571B0000)		/**< layout=27up */
#define XML_LAYOUT_28UP			(0x571C0000)		/**< layout=28up */
#define XML_LAYOUT_29UP			(0x571D0000)		/**< layout=29up */
#define XML_LAYOUT_30UP			(0x571E0000)		/**< layout=30up */
#define XML_LAYOUT_31UP			(0x571F0000)		/**< layout=31up */
#define XML_LAYOUT_32UP			(0x57200000)		/**< layout=32up */
#define XML_LAYOUT_33UP			(0x57210000)		/**< layout=33up */
#define XML_LAYOUT_34UP			(0x57220000)		/**< layout=34up */
#define XML_LAYOUT_35UP			(0x57230000)		/**< layout=35up */
#define XML_LAYOUT_36UP			(0x57240000)		/**< layout=36up */
#define XML_LAYOUT_37UP			(0x57250000)		/**< layout=37up */
#define XML_LAYOUT_38UP			(0x57260000)		/**< layout=38up */
#define XML_LAYOUT_39UP			(0x57270000)		/**< layout=39up */
#define XML_LAYOUT_40UP			(0x57280000)		/**< layout=40up */
#define XML_LAYOUT_41UP			(0x57290000)		/**< layout=41up */
#define XML_LAYOUT_42UP			(0x572A0000)		/**< layout=42up */
#define XML_LAYOUT_43UP			(0x572B0000)		/**< layout=43up */
#define XML_LAYOUT_44UP			(0x572C0000)		/**< layout=44up */
#define XML_LAYOUT_45UP			(0x572D0000)		/**< layout=45up */
#define XML_LAYOUT_46UP			(0x572E0000)		/**< layout=46up */
#define XML_LAYOUT_47UP			(0x572F0000)		/**< layout=47up */
#define XML_LAYOUT_48UP			(0x57300000)		/**< layout=48up */
#define XML_LAYOUT_49UP			(0x57310000)		/**< layout=49up */
#define XML_LAYOUT_50UP			(0x57320000)		/**< layout=50up */
#define XML_LAYOUT_51UP			(0x57330000)		/**< layout=51up */
#define XML_LAYOUT_52UP			(0x57340000)		/**< layout=52up */
#define XML_LAYOUT_53UP			(0x57350000)		/**< layout=53up */
#define XML_LAYOUT_54UP			(0x57360000)		/**< layout=54up */
#define XML_LAYOUT_55UP			(0x57370000)		/**< layout=55up */
#define XML_LAYOUT_56UP			(0x57380000)		/**< layout=56up */
#define XML_LAYOUT_57UP			(0x57390000)		/**< layout=57up */
#define XML_LAYOUT_58UP			(0x573A0000)		/**< layout=58up */
#define XML_LAYOUT_59UP			(0x573B0000)		/**< layout=59up */
#define XML_LAYOUT_60UP			(0x573C0000)		/**< layout=60up */
#define XML_LAYOUT_61UP			(0x573D0000)		/**< layout=61up */
#define XML_LAYOUT_62UP			(0x573E0000)		/**< layout=62up */
#define XML_LAYOUT_63UP			(0x573F0000)		/**< layout=63up */
#define XML_LAYOUT_64UP			(0x57400000)		/**< layout=64up */
#define XML_LAYOUT_65UP			(0x57410000)		/**< layout=65up */
#define XML_LAYOUT_66UP			(0x57420000)		/**< layout=66up */
#define XML_LAYOUT_67UP			(0x57430000)		/**< layout=67up */
#define XML_LAYOUT_68UP			(0x57440000)		/**< layout=68up */
#define XML_LAYOUT_69UP			(0x57450000)		/**< layout=69up */
#define XML_LAYOUT_70UP			(0x57460000)		/**< layout=70up */
#define XML_LAYOUT_71UP			(0x57470000)		/**< layout=71up */
#define XML_LAYOUT_72UP			(0x57480000)		/**< layout=72up */
#define XML_LAYOUT_73UP			(0x57490000)		/**< layout=73up */
#define XML_LAYOUT_74UP			(0x574A0000)		/**< layout=74up */
#define XML_LAYOUT_75UP			(0x574B0000)		/**< layout=75up */
#define XML_LAYOUT_76UP			(0x574C0000)		/**< layout=76up */
#define XML_LAYOUT_77UP			(0x574D0000)		/**< layout=77up */
#define XML_LAYOUT_78UP			(0x574E0000)		/**< layout=78up */
#define XML_LAYOUT_79UP			(0x574F0000)		/**< layout=79up */
#define XML_LAYOUT_80UP			(0x57500000)		/**< layout=80up */
#define XML_LAYOUT_81UP			(0x57510000)		/**< layout=81up */
#define XML_LAYOUT_82UP			(0x57520000)		/**< layout=82up */
#define XML_LAYOUT_83UP			(0x57530000)		/**< layout=83up */
#define XML_LAYOUT_84UP			(0x57540000)		/**< layout=84up */
#define XML_LAYOUT_85UP			(0x57550000)		/**< layout=85up */
#define XML_LAYOUT_86UP			(0x57560000)		/**< layout=86up */
#define XML_LAYOUT_87UP			(0x57570000)		/**< layout=87up */
#define XML_LAYOUT_88UP			(0x57580000)		/**< layout=88up */
#define XML_LAYOUT_89UP			(0x57590000)		/**< layout=89up */
#define XML_LAYOUT_90UP			(0x575A0000)		/**< layout=90up */
#define XML_LAYOUT_91UP			(0x575B0000)		/**< layout=91up */
#define XML_LAYOUT_92UP			(0x575C0000)		/**< layout=92up */
#define XML_LAYOUT_93UP			(0x575D0000)		/**< layout=93up */
#define XML_LAYOUT_94UP			(0x575E0000)		/**< layout=94up */
#define XML_LAYOUT_95UP			(0x575F0000)		/**< layout=95up */
#define XML_LAYOUT_96UP			(0x57600000)		/**< layout=96up */
#define XML_LAYOUT_97UP			(0x57610000)		/**< layout=97up */
#define XML_LAYOUT_98UP			(0x57620000)		/**< layout=98up */
#define XML_LAYOUT_99UP			(0x57630000)		/**< layout=99up */
#define XML_LAYOUT_100UP		(0x57640000)		/**< layout=100up */
#define XML_LAYOUT_101UP		(0x57650000)		/**< layout=101up */
#define XML_LAYOUT_102UP		(0x57660000)		/**< layout=102up */
#define XML_LAYOUT_103UP		(0x57670000)		/**< layout=103up */
#define XML_LAYOUT_104UP		(0x57680000)		/**< layout=104up */
#define XML_LAYOUT_105UP		(0x57690000)		/**< layout=105up */
#define XML_LAYOUT_106UP		(0x576A0000)		/**< layout=106up */
#define XML_LAYOUT_107UP		(0x576B0000)		/**< layout=107up */
#define XML_LAYOUT_108UP		(0x576C0000)		/**< layout=108up */
#define XML_LAYOUT_109UP		(0x576D0000)		/**< layout=109up */
#define XML_LAYOUT_110UP		(0x576E0000)		/**< layout=110up */
#define XML_LAYOUT_111UP		(0x576F0000)		/**< layout=111up */
#define XML_LAYOUT_112UP		(0x57700000)		/**< layout=112up */
#define XML_LAYOUT_113UP		(0x57710000)		/**< layout=113up */
#define XML_LAYOUT_114UP		(0x57720000)		/**< layout=114up */
#define XML_LAYOUT_115UP		(0x57730000)		/**< layout=115up */
#define XML_LAYOUT_116UP		(0x57740000)		/**< layout=116up */
#define XML_LAYOUT_117UP		(0x57750000)		/**< layout=117up */
#define XML_LAYOUT_118UP		(0x57760000)		/**< layout=118up */
#define XML_LAYOUT_119UP		(0x57770000)		/**< layout=119up */
#define XML_LAYOUT_120UP		(0x57780000)		/**< layout=120up */
#define XML_LAYOUT_121UP		(0x57790000)		/**< layout=121up */
#define XML_LAYOUT_122UP		(0x577A0000)		/**< layout=122up */
#define XML_LAYOUT_123UP		(0x577B0000)		/**< layout=123up */
#define XML_LAYOUT_124UP		(0x577C0000)		/**< layout=124up */
#define XML_LAYOUT_125UP		(0x577D0000)		/**< layout=125up */
#define XML_LAYOUT_126UP		(0x577E0000)		/**< layout=126up */
#define XML_LAYOUT_127UP		(0x577F0000)		/**< layout=127up */
#define XML_LAYOUT_128UP		(0x57800000)		/**< layout=128up */
#define XML_LAYOUT_129UP		(0x57810000)		/**< layout=129up */
#define XML_LAYOUT_130UP		(0x57820000)		/**< layout=130up */
#define XML_LAYOUT_131UP		(0x57830000)		/**< layout=131up */
#define XML_LAYOUT_132UP		(0x57840000)		/**< layout=132up */
#define XML_LAYOUT_133UP		(0x57850000)		/**< layout=133up */
#define XML_LAYOUT_134UP		(0x57860000)		/**< layout=134up */
#define XML_LAYOUT_135UP		(0x57870000)		/**< layout=135up */
#define XML_LAYOUT_136UP		(0x57880000)		/**< layout=136up */
#define XML_LAYOUT_137UP		(0x57890000)		/**< layout=137up */
#define XML_LAYOUT_138UP		(0x578A0000)		/**< layout=138up */
#define XML_LAYOUT_139UP		(0x578B0000)		/**< layout=139up */
#define XML_LAYOUT_140UP		(0x578C0000)		/**< layout=140up */
#define XML_LAYOUT_141UP		(0x578D0000)		/**< layout=141up */
#define XML_LAYOUT_142UP		(0x578E0000)		/**< layout=142up */
#define XML_LAYOUT_143UP		(0x578F0000)		/**< layout=143up */
#define XML_LAYOUT_144UP		(0x57900000)		/**< layout=144up */
#define XML_LAYOUT_145UP		(0x57910000)		/**< layout=145up */
#define XML_LAYOUT_146UP		(0x57920000)		/**< layout=146up */
#define XML_LAYOUT_147UP		(0x57930000)		/**< layout=147up */
#define XML_LAYOUT_148UP		(0x57940000)		/**< layout=148up */
#define XML_LAYOUT_149UP		(0x57950000)		/**< layout=149up */
#define XML_LAYOUT_150UP		(0x57960000)		/**< layout=150up */
#define XML_LAYOUT_151UP		(0x57970000)		/**< layout=151up */
#define XML_LAYOUT_152UP		(0x57980000)		/**< layout=152up */
#define XML_LAYOUT_153UP		(0x57990000)		/**< layout=153up */
#define XML_LAYOUT_154UP		(0x579A0000)		/**< layout=154up */
#define XML_LAYOUT_155UP		(0x579B0000)		/**< layout=155up */
#define XML_LAYOUT_156UP		(0x579C0000)		/**< layout=156up */
#define XML_LAYOUT_157UP		(0x579D0000)		/**< layout=157up */
#define XML_LAYOUT_158UP		(0x579E0000)		/**< layout=158up */
#define XML_LAYOUT_159UP		(0x579F0000)		/**< layout=159up */
#define XML_LAYOUT_160UP		(0x57A00000)		/**< layout=160up */
#define XML_LAYOUT_161UP		(0x57A10000)		/**< layout=161up */
#define XML_LAYOUT_162UP		(0x57A20000)		/**< layout=162up */
#define XML_LAYOUT_163UP		(0x57A30000)		/**< layout=163up */
#define XML_LAYOUT_164UP		(0x57A40000)		/**< layout=164up */
#define XML_LAYOUT_165UP		(0x57A50000)		/**< layout=165up */
#define XML_LAYOUT_166UP		(0x57A60000)		/**< layout=166up */
#define XML_LAYOUT_167UP		(0x57A70000)		/**< layout=167up */
#define XML_LAYOUT_168UP		(0x57A80000)		/**< layout=168up */
#define XML_LAYOUT_169UP		(0x57A90000)		/**< layout=169up */
#define XML_LAYOUT_170UP		(0x57AA0000)		/**< layout=170up */
#define XML_LAYOUT_171UP		(0x57AB0000)		/**< layout=171up */
#define XML_LAYOUT_172UP		(0x57AC0000)		/**< layout=172up */
#define XML_LAYOUT_173UP		(0x57AD0000)		/**< layout=173up */
#define XML_LAYOUT_174UP		(0x57AE0000)		/**< layout=174up */
#define XML_LAYOUT_175UP		(0x57AF0000)		/**< layout=175up */
#define XML_LAYOUT_176UP		(0x57B00000)		/**< layout=176up */
#define XML_LAYOUT_177UP		(0x57B10000)		/**< layout=177up */
#define XML_LAYOUT_178UP		(0x57B20000)		/**< layout=178up */
#define XML_LAYOUT_179UP		(0x57B30000)		/**< layout=179up */
#define XML_LAYOUT_180UP		(0x57B40000)		/**< layout=180up */
#define XML_LAYOUT_181UP		(0x57B50000)		/**< layout=181up */
#define XML_LAYOUT_182UP		(0x57B60000)		/**< layout=182up */
#define XML_LAYOUT_183UP		(0x57B70000)		/**< layout=183up */
#define XML_LAYOUT_184UP		(0x57B80000)		/**< layout=184up */
#define XML_LAYOUT_185UP		(0x57B90000)		/**< layout=185up */
#define XML_LAYOUT_186UP		(0x57BA0000)		/**< layout=186up */
#define XML_LAYOUT_187UP		(0x57BB0000)		/**< layout=187up */
#define XML_LAYOUT_188UP		(0x57BC0000)		/**< layout=188up */
#define XML_LAYOUT_189UP		(0x57BD0000)		/**< layout=189up */
#define XML_LAYOUT_190UP		(0x57BE0000)		/**< layout=190up */
#define XML_LAYOUT_191UP		(0x57BF0000)		/**< layout=191up */
#define XML_LAYOUT_192UP		(0x57C00000)		/**< layout=192up */
#define XML_LAYOUT_193UP		(0x57C10000)		/**< layout=193up */
#define XML_LAYOUT_194UP		(0x57C20000)		/**< layout=194up */
#define XML_LAYOUT_195UP		(0x57C30000)		/**< layout=195up */
#define XML_LAYOUT_196UP		(0x57C40000)		/**< layout=196up */
#define XML_LAYOUT_197UP		(0x57C50000)		/**< layout=197up */
#define XML_LAYOUT_198UP		(0x57C60000)		/**< layout=198up */
#define XML_LAYOUT_199UP		(0x57C70000)		/**< layout=199up */
#define XML_LAYOUT_200UP		(0x57C80000)		/**< layout=200up */
#define XML_LAYOUT_201UP		(0x57C90000)		/**< layout=201up */
#define XML_LAYOUT_202UP		(0x57CA0000)		/**< layout=202up */
#define XML_LAYOUT_203UP		(0x57CB0000)		/**< layout=203up */
#define XML_LAYOUT_204UP		(0x57CC0000)		/**< layout=204up */
#define XML_LAYOUT_205UP		(0x57CD0000)		/**< layout=205up */
#define XML_LAYOUT_206UP		(0x57CE0000)		/**< layout=206up */
#define XML_LAYOUT_207UP		(0x57CF0000)		/**< layout=207up */
#define XML_LAYOUT_208UP		(0x57D00000)		/**< layout=208up */
#define XML_LAYOUT_209UP		(0x57D10000)		/**< layout=209up */
#define XML_LAYOUT_210UP		(0x57D20000)		/**< layout=210up */
#define XML_LAYOUT_211UP		(0x57D30000)		/**< layout=211up */
#define XML_LAYOUT_212UP		(0x57D40000)		/**< layout=212up */
#define XML_LAYOUT_213UP		(0x57D50000)		/**< layout=213up */
#define XML_LAYOUT_214UP		(0x57D60000)		/**< layout=214up */
#define XML_LAYOUT_215UP		(0x57D70000)		/**< layout=215up */
#define XML_LAYOUT_216UP		(0x57D80000)		/**< layout=216up */
#define XML_LAYOUT_217UP		(0x57D90000)		/**< layout=217up */
#define XML_LAYOUT_218UP		(0x57DA0000)		/**< layout=218up */
#define XML_LAYOUT_219UP		(0x57DB0000)		/**< layout=219up */
#define XML_LAYOUT_220UP		(0x57DC0000)		/**< layout=220up */
#define XML_LAYOUT_221UP		(0x57DD0000)		/**< layout=221up */
#define XML_LAYOUT_222UP		(0x57DE0000)		/**< layout=222up */
#define XML_LAYOUT_223UP		(0x57DF0000)		/**< layout=223up */
#define XML_LAYOUT_224UP		(0x57E00000)		/**< layout=224up */
#define XML_LAYOUT_225UP		(0x57E10000)		/**< layout=225up */
#define XML_LAYOUT_226UP		(0x57E20000)		/**< layout=226up */
#define XML_LAYOUT_227UP		(0x57E30000)		/**< layout=227up */
#define XML_LAYOUT_228UP		(0x57E40000)		/**< layout=228up */
#define XML_LAYOUT_229UP		(0x57E50000)		/**< layout=229up */
#define XML_LAYOUT_230UP		(0x57E60000)		/**< layout=230up */
#define XML_LAYOUT_231UP		(0x57E70000)		/**< layout=231up */
#define XML_LAYOUT_232UP		(0x57E80000)		/**< layout=232up */
#define XML_LAYOUT_233UP		(0x57E90000)		/**< layout=233up */
#define XML_LAYOUT_234UP		(0x57EA0000)		/**< layout=234up */
#define XML_LAYOUT_235UP		(0x57EB0000)		/**< layout=235up */
#define XML_LAYOUT_236UP		(0x57EC0000)		/**< layout=236up */
#define XML_LAYOUT_237UP		(0x57ED0000)		/**< layout=237up */
#define XML_LAYOUT_238UP		(0x57EE0000)		/**< layout=238up */
#define XML_LAYOUT_239UP		(0x57EF0000)		/**< layout=239up */
#define XML_LAYOUT_240UP		(0x57F00000)		/**< layout=240up */
#define XML_LAYOUT_241UP		(0x57F10000)		/**< layout=241up */
#define XML_LAYOUT_242UP		(0x57F20000)		/**< layout=242up */
#define XML_LAYOUT_243UP		(0x57F30000)		/**< layout=243up */
#define XML_LAYOUT_244UP		(0x57F40000)		/**< layout=244up */
#define XML_LAYOUT_245UP		(0x57F50000)		/**< layout=245up */
#define XML_LAYOUT_246UP		(0x57F60000)		/**< layout=246up */
#define XML_LAYOUT_247UP		(0x57F70000)		/**< layout=247up */
#define XML_LAYOUT_248UP		(0x57F80000)		/**< layout=248up */
#define XML_LAYOUT_249UP		(0x57F90000)		/**< layout=249up */
#define XML_LAYOUT_250UP		(0x57FA0000)		/**< layout=250up */
#define XML_LAYOUT_INDEX		(0x57FE0000)		/**< layout=index type */
#define XML_LAYOUT_BORDERLESS	(0x57FF0000)		/**< layout=borderless type */

//fixedSize
#define XML_FIXEDSIZE_DEFAULT	(0x58000000)		/**< fixedsize : default */
#define XML_FIXEDSIZE_2_5X3_25	(0x58010000)		/**< fixedsize : 2_5X3_25 */
#define XML_FIXEDSIZE_3_5X5		(0x58020000)		/**< fixedsize : 3_5X5 */
#define XML_FIXEDSIZE_4X6		(0x58030000)		/**< fixedsize : 4X6 */
#define XML_FIXEDSIZE_5X7		(0x58040000)		/**< fixedsize : 5X7 */
#define XML_FIXEDSIZE_8X10		(0x58050000)		/**< fixedsize : 8X10 */
#define XML_FIXEDSIZE_254X178	(0x58060000)		/**< fixedsize : 254X178 */
#define XML_FIXEDSIZE_110X74	(0x58070000)		/**< fixedsize : 110X74 */
#define XML_FIXEDSIZE_89X55		(0x58080000)		/**< fixedsize : 89X55 */
#define XML_FIXEDSIZE_100X148	(0x58090000)		/**< fixedsize : 100X148 */
#define XML_FIXEDSIZE_6X8		(0x580A0000)		/**< fixedsize : 6X8 */
#define XML_FIXEDSIZE_7X10		(0x580B0000)		/**< fixedsize : 7X10 */
#define XML_FIXEDSIZE_9X13		(0x580C0000)		/**< fixedsize : 9X13 */
#define XML_FIXEDSIZE_10X15		(0x580D0000)		/**< fixedsize : 10X15 */
#define XML_FIXEDSIZE_13X18		(0x580E0000)		/**< fixedsize : 13X18 */
#define XML_FIXEDSIZE_15X21		(0x580F0000)		/**< fixedsize : 15X21 */
#define XML_FIXEDSIZE_18X24		(0x58100000)		/**< fixedsize : 18X24 */
#define XML_FIXEDSIZE_A4		(0x58110000)		/**< fixedsize : A4 */
#define XML_FIXEDSIZE_LETTER	(0x58120000)		/**< fixedsize : LETTER */

//cropping
#define XML_CROPPING_DEFAULT	(0x59000000)		/**< cropping: default */
#define XML_CROPPING_OFF		(0x59010000)		/**< cropping: OFF */
#define XML_CROPPING_ON			(0x59020000)		/**< cropping: ON */

//dpsPrintServiceStatus
#define XML_PRINT_SVC_STATUS_PRINTING	(0x700000)		/**< dps print service : printing */
#define XML_PRINT_SVC_STATUS_IDLE		(0x700100)		/**< dps print service : idle */
#define XML_PRINT_SVC_STATUS_PAUSED		(0x700200)		/**< dps print service : paused */

//JobEndReason
#define XML_JOBEND_REASON_NOT_ENDED		(0x710000)		/**< job end reason : not ended */
#define XML_JOBEND_REASON_NORMALLY		(0x710100)		/**< job end reason : normally */
#define XML_JOBEND_REASON_ABORT9000		(0x710200)		/**< job end reason : abort 9000 */
#define XML_JOBEND_REASON_ABORT9001		(0x710300)		/**< job end reason : abort 9001 */
#define XML_JOBEND_REASON_OTHERS		(0x710400)		/**< job end reason : others */

/**
errorStatus
 */
#define XML_ERROR_STATUS_NONE			(0x720000)		/**< error statue : none */
#define XML_ERROR_STATUS_WARNING		(0x720100)		/**< error statue : warnnng */
#define XML_ERROR_STATUS_ERROR			(0x720200)		/**< error statue : error */

//errorReason
#define XML_ERROR_REASON_NONE			(0x730000)		/**< error reason : none */
#define XML_ERROR_REASON_PAPER			(0x730100)		/**< error reason : paper */
#define XML_ERROR_REASON_INK			(0x730200)		/**< error reason : ink */
#define XML_ERROR_REASON_HARDWARE		(0x730300)		/**< error reason : hardware */
#define XML_ERROR_REASON_FILE			(0x730400)		/**< error reason : file */

//disconnectEnable
#define XML_DISCONNECTENABLE_FALSE		(0x740000)		/**< disconnect enable : FALSE */
#define XML_DISCONNECTENABLE_TRUE		(0x740100)		/**< disconnect enable : TRUE */

//capabilityChange
#define XML_CAPABILITYCHANGE_FALSE		(0x750000)		/**< capability change : FALSE */
#define XML_CAPABILITYCHANGE_TRUE		(0x750100)		/**< capability change : TRUE */

//newJOBOK
#define XML_NEWJOBOK_FALSE	 			(0x760000)		/**< new job ok : FALSE */
#define XML_NEWJOBOK_TRUE				(0x760100)		/**< new job ok : TRUE  */

//Print option for each image
#define FJ_PICT_PRINT_OPTION_NONE				(0)			/**< None */
#define FJ_PICT_PRINT_OPTION_DATE				(0x01)		/**< print date & time */
#define FJ_PICT_PRINT_OPTION_FNAME				(0x02)		/**< print file name */
#define FJ_PICT_PRINT_OPTION_DATE_AND_FNAME		(0x03)		/**< print date, time and file name */
#define FJ_PICT_PRINT_OPTION_DUPLICATE_FILEID	(0x04)		/**< Duplicate FileID in place of copies */
/* @} */	// fj_pictbridge_definition group

/** @weakgroup fj_pictbridge_message
@{*/	// fj_pictbridge_message group

/**
[Outline] Print Message
 */
#define FJ_HM_DPS_MSG											(FJ_TITLE_PICTBRIDGE)

/**
[Outline] The message indicate that the PictBridge service is configured
 */
#define FJ_PICT_HOST_MSG_PRINTER_CONFIGURED					(0x01)

/**
[Argument] Bit[23~12]-total number of print images, Bit[11~0]-sequential number of print images<br>
[Outline] The sequential and total number of print images.
 */
#define FJ_PICT_HOST_MSG_PRINTING_PROGRESS					(0x02)

/**
[Argument] FJ_PRINTER_ERROR_REASON(Refer to PictBridge header file)<br>
[Outline] This message that printer occur some fatal error
*/
#define FJ_PICT_HOST_MSG_JOB_COMPLETE_FATAL_ERROR			(0x03)

/**
[Argument] FJ_PRINTER_ERROR_REASON(Refer to PictBridge header file)<br>
[Outline] This message that printer occur some revoverable error
*/
#define FJ_PICT_HOST_MSG_JOB_COMPLETE_RECOVERABLE_ERROR		(0x04)

/**
[Outline] This message thant printer finish job success
*/
#define FJ_PICT_HOST_MSG_JOB_COMPLETE_OK					(0x05)

/**
[Outline] This message thant printer status from pause to resume
*/
#define FJ_PICT_HOST_MSG_RESUMED							(0x06)

/**
[Outline] This message that printer encounter some error
*/
#define FJ_PICT_HOST_MSG_EXCEPTION							(0x07)

/**
[Outline] This message that printer erro was cleared
*/
#define FJ_PICT_HOST_MSG_ERROR_CLEARED						(0x08)

/**
[Outline] This message that printer enter idle status , and error are cleared<br>
*/
#define FJ_PICT_HOST_MSG_IDLE_NO_ERROR						(0x09)

/**
[Outline] unrecognized message is send<br>
*/
#define FJ_PICT_HOST_MSG_UNRECOGNIZED_OPERATION				(0x0A)

/**
[Argument] This message that printer service status value(Refer to PictBridge header file)<br>
[Outline] printer service status value 
*/
#define FJ_PICT_DBG_SERVICE_STATUS							(0x0B)

/**
[Argument] status value(Refer to PictBridge header file)<br>
[Outline] current error status<br>
*/
#define FJ_PICT_DBG_ERROR_STATUS							(0x0C)

/**
[Argument] reason value(Refer to PictBridge header file)<br>
[Outline] current error reason<br>
*/
#define FJ_PICT_DBG_ERROR_REASON							(0x0D)

/**
[Argument] reason value(Refer to PictBridge header file)<br>
[Outline] This message whether or not the print job is end and the end reason<br>
*/
#define FJ_PICT_DBG_JOB_END_REASON							(0x0E)

/**
[Argument] 0x740100 can<br>
                  0x740000 can not<br>
[Outline] This message whether or not the DSC and printer can disconnect current
*/
#define FJ_PICT_DBG_DISCONNECT_ENABLE						(0x0F)

/**
[Argument] 0x750100 changed<br>
                  0x750000 not changed<br>
[Outline] This message wheter or not printer's capability can change
*/
#define FJ_PICT_DBG_CAPABILITY_CHANGED						(0x10)

/**
[Argument] 0x760100 can<br>
                  0x760000 can not<br>
[Outline] This message wheter or not printer can receive new job from DSC
*/
#define FJ_PICT_DBG_NEW_JOB_OK								(0x11)

/**
[Argument] Upper two bytess: current index of printing jobs.<BR>
                 Lower two bytes: total number of printing jobs.<BR>
[Outline] This message indicates current and total prtinting pages to host

*/
#define FJ_PICT_HOST_MSG_PRINTING_TOTAL						(0x12)

/**
[Outline] This message indicates the PTP mode configuration is complete.
*/
#define FJ_PICT_HOST_MSG_PTP_CONFIGURED						(0x13)

/**
[Outline] This message indicates the session in PTP mode is closed with Host.
*/
#define FJ_PICT_HOST_MSG_PTP_SESSION_CLOSED					(0x14)

/**
[Outline] This message indicates the PTP layer receives unexpected packets from Host.
*/
#define FJ_PICT_HOST_MSG_PRINTER_ABNORMAL_RESPONSE			(0x15)
/* @} */	// fj_pictbridge_message group

/** @weakgroup fj_pictbridge_definition
@{*/	// fj_pictbridge_definition group

/**
DPS msg definition with FJ firmware
 */
#define kPictBridgeSubCmd_LowerBound	(14000)		/**< pictbridge sub command lower bound */
#define kPictBridgeSubCmd_UpperBound	(14999)		/**< pictbridge sub command upper bound */

#define kPictBridgeSubState_LowerBound	(15000)		/**< pictbridge sub state lower bound */
#define kPictBridgeSubState_UpperBound	(15999)		/**< pictbridge sub state upper bound */

/******************************************************************************
 Enumeration
******************************************************************************/

/**
pictbridge sub command enum
*/
enum {
	ePictBridgeSubCmd_First = kPictBridgeSubCmd_LowerBound,	/**< pictbridge sub command lower bound */
	ePictBridgeSubCmd_ConfigCapability,						/**< config capability */
	ePictBridgeSubCmd_SetPrintAction,						/**< set print action */
	ePictBridgeSubCmd_SetPrintImage,						/**< set print image */
	ePictBridgeSubCmd_GetDeviceStatus,						/**< get device status */
	ePictBridgeSubCmd_GetCapability,						/**< get capability */
	ePictBridgeSubCmd_Close,								/**< close */
	ePictBridgeSubCmd_ProcessDpsEvent,						/**< process pictbridge event */
	ePictBridgeSubCmd_End =kPictBridgeSubCmd_UpperBound,	/**< pictbridge sub command upper bound */
};

/**
pictbridge capability param
*/
typedef enum
{
	FJ_PICT_PARAM_QUALITY,          /**< 0x50000000 ~ 0x50030000 */
	FJ_PICT_PARAM_PAPER_SIZE,       /**< 0x51000000 ~ 0x51940000 */
	FJ_PICT_PARAM_PAPER_TYPE,       /**< 0x52000000 ~ 0x52030000 */
	FJ_PICT_PARAM_FILE_TYPE,        /**< 0x53000000 ~ 0x531E0000 */
	FJ_PICT_PARAM_DATE_PRINT,       /**< 0x54000000 ~ 0x54020000 */
	FJ_PICT_PARAM_FILENAME_PRINT,   /**< 0x55000000 ~ 0x55020000 */
	FJ_PICT_PARAM_IMAGE_OPTIMIZE,   /**< 0x56000000 ~ 0x56020000 */
	FJ_PICT_PARAM_LAYOUT,			/**< 0x57000000 ~ 0x57FF0000 */
	FJ_PICT_PARAM_FIXED_SIZE,		/**< 0x58000000 ~ 0x58120000 */
	FJ_PICT_PARAM_CROPPING,         /**< 0x59000000 ~ 0x59020000 */
	FJ_PICT_PARAM_TIMEOUT,			/**< timeout */
	FJ_PICT_PARAM_DATE_FORMAT,		/**< date format */
} FJ_PICT_CAPABILITY_PARAM;

/**
pictbridge action enum
*/
typedef enum
{
	FJ_PICT_ACTION_START,			/**< action start */
	FJ_PICT_ACTION_ABORT,			/**< action abort */
	FJ_PICT_ACTION_CONTINUE,		/**< action continue */
	FJ_PICT_ACTION_START_DPOF,		/**< action start DPOF */
} FJ_PICT_ACTION;

/**
printer error reason enum
*/
typedef enum
{
	PRINTER_NO_ERROR						= 0x00000,	/**< no error */	
	PRINTER_PAPERRELATED_ERROR				= 0x10000,	/**< paper related  error */
	PRINTER_PAPEREMPTY_ERROR				= 0x10100,	/**< paper empty  error */
	PRINTER_PAPERLOADED_ERROR				= 0x10200,	/**< paper loaded  error */
	PRINTER_PAPEREJECTED_ERROR				= 0x10300,	/**< paper ejected  error */
	PRINTER_PAPERMEDIA_ERROR				= 0x10400,	/**< paper media error */
	PRINTER_PAPERJAM_ERROR					= 0x10500,	/**< paper jam error */
	PRINTER_PAPERNEARLYEMPTY_ERROR			= 0x10600,	/**< paper nearly empty error */
	PRINTER_PAPERTYPESIZENOTSUPPORT_ERROR	= 0x10700,	/**< paper size type not support error */
	PRINTER_INKRELATED_ERROR				= 0x20000,	/**< ink related error */
	PRINTER_INKEMPTY_ERROR					= 0x20100,	/**< ink empty error */
	PRINTER_INKLOW_ERROR					= 0x20200,	/**< ink low error */
	PRINTER_INKWASTE_ERROR					= 0x20300,	/**< ink waste error */ 
	PRINTER_HWRELATED_ERROR					= 0x30000,	/**< hardware related error */
	PRINTER_HWFATAL_ERROR					= 0x30100,	/**< hardware fatal error */
	PRINTER_HWSERVICECALL_ERROR				= 0x30200,	/**< hardware service call error */
	PRINTER_HWPRINTERUNAVAILABLE_ERROR		= 0x30300,	/**< hardware printer unavailable error */
	PRINTER_HWPRINTERBUSY_ERROR				= 0x30400,	/**< hardware printer busy error */
	PRINTER_HWLEVEL_ERROR					= 0x30500,	/**< hardware level error */
	PRINTER_HWCOVEROPEN_ERROR				= 0x30600,	/**< hardware cover open error */
	PRINTER_HWNOMARKINGAGENT_ERROR			= 0x30700,	/**< hardware no marking agent error */
	PRINTER_HWINKCOVEROPEN_ERROR			= 0x30800,	/**< hardware ink cover open error */
	PRINTER_HWNOINKCARTRIDGE_ERROR			= 0x30900,	/**< hardware no ink cartridge error */
	PRINTER_FILERELATED_ERROR				= 0x40000,	/**< file related error */
	PRINTER_FILEPRINTINFO_ERROR				= 0x40100,	/**< file printinfo error */
	PRINTER_FILEDECODE_ERROR				= 0x40200,	/**< file decode error */
} FJ_PRINTER_ERROR_REASON;

/**
prmid PTP mode string enum
*/
typedef enum {
	FJ_PRMID_PTP_MODEL_STRING = 0,		/**< model string */
	FJ_PRMID_PTP_SERIAL_NUMBER,			/**< serial number */
	FJ_PRMID_PTP_VENDOR_NAME_STRING,	/**< vendor name string */
} FJ_PRMID_PTP_MODESTRING;

/**
prmid PictBridge configure string enum
*/
typedef enum {
	FJ_PRMID_PICT_CONFIG_VENDOR_NAME_STRING = 0,		/**< name string */
	FJ_PRMID_PICT_CONFIG_VENDOR_SVERSION_STRING,		/**< sversion string */
	FJ_PRMID_PICT_CONFIG_PRODUCT_STRING,				/**< product string */
} FJ_PRMID_PICT_CONFIGSTRING;

/**
dps date format type enum
*/
typedef enum {
	FJ_DPS_DATE_FORMAT_TYPE0 = 0,	/**< YY/MM/DD */
	FJ_DPS_DATE_FORMAT_TYPE1,		/**< YYYY/MM/DD */
	FJ_DPS_DATE_FORMAT_TYPE2,		/**< DD hh:mm:ss */
	FJ_DPS_DATE_FORMAT_TYPE3,		/**< YYYY/MM/DD hh:mm:ss */
	FJ_DPS_DATE_FORMAT_TYPE4,		/**< DD/MM/YY */
	FJ_DPS_DATE_FORMAT_TYPE5,		/**< DD/MM/YYYY */
	FJ_DPS_DATE_FORMAT_TYPE6,		/**< DD/MM/YYYY hh:mm:ss */
	FJ_DPS_DATE_FORMAT_TYPE7,		/**< MM/DD/YY */
	FJ_DPS_DATE_FORMAT_TYPE8,		/**< MM/DD/YYYY */
	FJ_DPS_DATE_FORMAT_TYPE9		/**< MM/DD/YYYY hh:mm:ss */
} FJ_DPS_DATE_FORMAT;

/**
dps callback function id enum
*/
typedef enum {
	FJ_DPS_CB_FUNCTION_DATETIME = 0,	/**<YY/MM/DD */
	FJ_DPS_CB_FUNCTION_SIZE			/**< size */
} FJ_DPS_CB_FUNCTION_ID;

/**
PictBridge sub state enum
*/
enum {
	ePictBridgeSubState_First = kPictBridgeSubState_LowerBound,	/**< pictbridge sub state lower bound */
	ePictBridgeSubState_Printer_Connected,						/**< printer connected */
	ePictBridgeSubState_Printer_Progress,						/**< printer progress */
	ePictBridgeSubState_Printer_Job_Complete_Ok,				/**< printer job complete ok */
	ePictBridgeSubState_Printer_Job_Continue,					/**< printer job continue */
	ePictBridgeSubState_Printer_Status,							/**< printer status */
	ePictBridgeSubState_Error_Cleared,							/**< error cleared */
	ePictBridgeSubState_Unrecognized_Operation,					/**< unrecognized operation */
	ePictBridgeSubState_Printer_Fatal_Error,					/**< printer fatal error */
	ePictBridgeSubState_Printer_Recoverable_Error,				/**< printer recoverable error */
	ePictBridgeSubState_Close,									/**< close */
	ePictBridgeSubState_Ptp_Connected,							/**< ptp connected */
	ePictBridgeSubState_Ptp_SessionClosed,						/**< ptp session closed */
	ePictBridgeSubState_Printer_Abnormal_Response,				/**< printer abnormal response */
	ePictBridgeSubState_End =kPictBridgeSubState_UpperBound,	/**< pictbridge sub state upper bound */
};      

typedef INT32(*lpfnDpsCbFuncEntry)(void*, void*);		/**< pictbridge callback function entry */

/******************************************************************************
 Structure
******************************************************************************/
/**	
@brief	DPS device status
 */
typedef struct {
	ULONG	ServiceStatus;		/**< service status */
	ULONG	JobEndReadon;		/**< job end read on */
	ULONG	ErrorStatus;		/**< error status */
	ULONG	ErrorReason;		/**< error reason */
	ULONG	DisconnectEnable;	/**< disconnect enable */
	ULONG	CapabilityChanged;	/**< capability changed */
	ULONG	NewJobOK;			/**< new job ok */
} FJ_DPS_DEVICE_STATUS;

/**	
@brief	capacity query
 */
typedef struct _CAPABILITY_QUERY_
{
    FJ_PICT_CAPABILITY_PARAM CapabilityType;	/**< capability type */
    ULONG ulPrinterPaperSize; /**< 0x51000000 ~ 0x51940000 the field only uses when CapabilityType = FJ_PICT_PARAM_LAYOUT */
} FJ_CAPABILITY_QUERY;

/**	
@brief	capability property
 */
typedef struct _CAPABILITY_PROPERTY_
{
	FJ_PICT_CAPABILITY_PARAM CapabilityType;	/**< capability type */
	ULONG ulCapabilityValue;					/**< capability value */
} FJ_CAPABILITY_PROPERTY;

/**	
@brief	print image property
 */
typedef struct _PRINT_IMAGE_PROPERTY_
{
	ULONG  ulFileID;				/**< file id */
	USHORT usCropCoordinateX;		/**< crop X-coordinate */
	USHORT usCropCoordinateY;		/**< crop Y-coordinate */
	USHORT usCropWidth;				/**< crop width */
	USHORT usCropHeight;			/**< crop height */
	USHORT usCopies;				/**< copies */
	UCHAR  ucPrintOption; 
	/**< 	bit 2 : indicate the fileID would be instead of copies to be filled in Start_Print script file<br>
			bit 1 : indicate the filename of the image to be printed, <br>
			bit 0 : indicate the date and time of the image to be printed <br>
	*/
} FJ_PRINT_IMAGE_PROPERTY;     
/* @} */	// fj_pictbridge_definition group

/******************************************************************************
 ProtoType
******************************************************************************/

/** @weakgroup fj_pictbridge_api */
/* @{ */	// fj_pictbridge_api group

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Query printer capability.
@param[in]	uCapabilityId	= Capability ID.
@param[in]	Buffer			= Data Store Area. Element 0 is Num. of elements.
@param[in]	uPaperSize		= Paper Size
@retval		FJ_ERR_OK		OK.
@retval		FJ_ERR_NG		Error.
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
@remarks 	None
*/
FJ_ERR_CODE	FJ_PictGetPrinterCapability( FJ_PICT_CAPABILITY_PARAM uCapabilityId, INT32 *Buffer, INT32 uPaperSize );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Setting the supprot capability for printing.
@param[in]	uCapabilityId		= Capability ID.
@param[in]	uiCapabilityValue	= Capability Value.
@retval		FJ_ERR_OK		OK.
@retval		FJ_ERR_NG		Error.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE	FJ_PictConfig( FJ_PICT_CAPABILITY_PARAM uCapabilityId, INT32 uiCapabilityValue );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Print action.
@param[in]	uCommand		= Print action value.
@retval		FJ_ERR_OK		OK.
@retval		FJ_ERR_NG		Error.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE	FJ_PictAction( FJ_PICT_ACTION uCommand );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Add image for printing.
@param[in]	FileID	= Dcf index of image(not 0).
@param[in]	uCropX	= Upper left Horizontal coordinate(X).
@param[in]	uCropY	= Upper left Vertical coordinate(Y).
@param[in]	uCropW	= Width(W).
@param[in]	uCropH	= Height(H).0
@param[in]	Copies	= Number of copies to be printed.
@param[in]	Flag		= Option flag.
@retval		FJ_ERR_OK	OK.
@retval		FJ_ERR_NG	Error.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE	FJ_PictAddImage( INT32 FileID, INT32 uCropX, INT32 uCropY, INT32 uCropW, INT32 uCropH, INT32 Copies, INT32 Flag);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Get the printer's status.
@param[out]	Status	= This array retun the value of printer status .
@retval		FJ_ERR_OK		OK.
@retval		FJ_ERR_NG		Error.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE	FJ_PICTGetDeviceStatus( FJ_DPS_DEVICE_STATUS* Status );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
Synchronize PictBridge File DB with DCF File Syatem.
@retval		FJ_ERR_OK		OK.
@retval		FJ_ERR_NG		Error.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE	FJ_PICTUpdateFileDB( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set PictBridge Callback Function Pointer Table.
@param[in]	uCbFuncID		    = FJ_DPS_CB_FUNCTION_ID.
@param[in]	fnDpsCbFuncEntry	= Function Pointer Entry.
@retval		FJ_ERR_OK		OK.
@retval		FJ_ERR_NG		Error.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE	FJ_PICTSetCBFunctionEntry(FJ_DPS_CB_FUNCTION_ID uCbFuncID, lpfnDpsCbFuncEntry fnDpsCbFuncEntry);

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Set PICT specific string.
@param[in]	wParmID	=	String type ID<br>
			FJ_PRMID_PICT_CONFIG_VENDOR_NAME_STRING : name string<br>
			FJ_PRMID_PICT_CONFIG_VENDOR_SVERSION_STRING : sversion string<br>
			FJ_PRMID_PICT_CONFIG_PRODUCT_STRING : product string<br>
@param[in]	bBuffer		= Input string.
@param[in]	dwSize		= String length.
@retval		FJ_ERR_OK		OK.
@retval		FJ_ERR_NG		Error.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE   FJ_PICTSetConfigString(FJ_PRMID_PICT_CONFIGSTRING wParmID, UCHAR* bBuffer, UINT32 dwSize );
//FJ_ERR_CODE	FJ_SetWDMDebugValue( INT32* DEBUG_ADDRESS );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Store PTP Model name string.
@param[in]	wParmID	=	String type ID<br>
			FJ_PRMID_PTP_MODEL_STRING : Model name<br>
			FJ_PRMID_PTP_SERIAL_NUMBER : Serial number<br>
			FJ_PRMID_PTP_VENDOR_NAME_STRING : Vendor name<br>
@param[in]	bBuffer		= 	Model name string.
@param[in]	dwSize		= 	String length.
@retval		FJ_ERR_OK		OK.
@retval		FJ_ERR_NG		Error.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE	FJ_SetPTPModeString(UINT16 wParmID, UCHAR* bBuffer, UINT32 dwSize );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
Initialize playback thumbnail data cache area.
@retval		FJ_ERR_OK		OK.
@retval		FJ_ERR_NG		Error.
@remarks 	None
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
FJ_ERR_CODE	FJ_PICTInitThumbnailCache( VOID );

/**
<table><tr><td>Task-independent portion</td><td>o</td></tr></table>
@attention  Mode information
<table>
	<tr><th>IDLE</th><th>MOVIE VIEW</th><th>MOVIE REC</th></tr>
	<tr><td>o</td><td>o</td><td>o</td></tr>
</table>
o: OK (Mode Check OK.)<br>x: NG (Mode Check NG. This API returns FJ_ERR_NG.)
*/
void    FJ_PictBridgeSendHostMessage(MSG Message);
extern  VOID PictIF_SetCommandTimeout(INT32 value);
extern  INT32 PictIF_GetCommandTimeout(VOID);
/* @} */	// fj_pictbridge_api group
#endif
