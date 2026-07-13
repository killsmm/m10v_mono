/**
 * @file		libmovf.h
 * @brief 		MOV File Format Library Header.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2016 Socionext Inc.</I></B>
 */

#ifndef _LIBMOVF_H_
#define _LIBMOVF_H_

//#define MP4F_SECTION_ON

#define MP4F_FORMAT_MOV					/* valid MP4F_FORMAT_MOV */
#ifdef  MP4F_FORMAT_MOV
	#define MP4F_FORMAT_MOV_ELEMENT		/* valid MP4F_FORMAT_MOV_ELEMENT */
#endif
#define MP4F_FS_convlseek				/* lseek -> read */

#define MP4F_LITTLE_ENDIAN				/* select MP4F_BIG_ENDIAN or MP4F_LITTLE_ENDIAN */

/* ----------------------------------------------------------------------------
	valid ATOM
 ---------------------------------------------------------------------------- */
#define MP4F_UUID_ATOM					/* valid MP4F_UUID_ATOM */
#define MP4F_FREE_ATOM					/* valid MP4F_FREE_ATOM */
#define MP4F_UDTA_ATOM					/* valid MP4F_UDTA_ATOM */

#ifdef MP4F_FREE_ATOM
#define MP4F_MULTIPUL_FREE_ATOM			/* valid multipul MP4F_FREE_ATOM */
#endif

#ifdef  MP4F_FORMAT_MOV
#define MOV_FTYP_ATOM					/* valid MOV_FTYP_ATOM */
#endif

#define MOV_USER_SET_ATOM_READ_HIGH		/* user setting atom reading speed-up */


//#define MP4F_TIME_CALL_START_TIMING			/* MP4F_time call timing changee */
/* ----------------------------------------------------------------------------
	The key word of "MP4F_FAST_FS" is given for the high-speed FS reading. 
 ---------------------------------------------------------------------------- */

typedef unsigned char					uInt8;
typedef unsigned short					uInt16;
typedef unsigned long					uInt32;

typedef void *							MP4FHANDLE;
typedef unsigned long					MP4FErr;


/* Error Code */
#define MP4F_OK							0x00000000
#define MP4F_ALREADY_OPEN_ERR			0x00010000
#define MP4F_ATOM_STRUCTURE_ERR			0x00020000
#define MP4F_BUFFER_ALREADY_SET_ERR		0x00030000
#define MP4F_CHUNK_BOUNDARY_ERR			0x00040000
#define MP4F_CHUNK_NOT_FOUND_ERR		0x00050000
#define MP4F_ENTRY_NOT_FOUND_ERR 		0x00060000
#define MP4F_FILE_TYPE_EXIST_ERR		0x00070000
#define MP4F_FILE_TYPE_NOT_EXIST_ERR	0x00080000
#define MP4F_MAX_COUNT_ERR				0x00090000
#define MP4F_MEDIA_DURATION_OVER_ERR	0x000A0000
#define MP4F_MEMORY_ALLOC_ERR			0x000B0000
#define MP4F_MEMORY_SIZE_ERR			0x000C0000
#define MP4F_MISMATCH_ERR				0x000D0000
#define MP4F_NULL_POINTER_ERR			0x000E0000
#define MP4F_OPEN_MODE_ERR				0x000F0000
#define MP4F_PARM_ERR					0x00100000
#define MP4F_READ_NOT_YET_ERR			0x00110000
#define MP4F_REREAD_ERR					0x00120000
#define MP4F_SAMPLE_DESCR_INVALID_ERR	0x00130000
#define MP4F_SAMPLE_NONE_ERR			0x00140000
#define MP4F_SAMPLE_NOT_FOUND_ERR		0x00150000
#define MP4F_SIZE_UNSUPPORTED_ERR		0x00160000
#define MP4F_TRACK_DURATION_OVER_ERR	0x00170000
#define MP4F_TRACK_ID_ALLOC_ERR			0x00180000
#define MP4F_TRACK_NONE_ERR				0x00190000
#define MP4F_TRACK_NOT_FOUND_ERR		0x001A0000
#define MP4F_TRACK_PARM_NOT_SET_ERR		0x001B0000
#define MP4F_VERSION_UNSUPPORTED_ERR	0x001D0000
#define MP4F_UNEXPECTED_ERR				0xFFFF0000

/* Macro definition used by argument of external API */
/* MP4F_OpenMemory, MP4F_OpenFile  */
#define MP4F_OPEN_MODE_READ				1
#define MP4F_OPEN_MODE_WRITE			2
#define MP4F_AT_CHUNK_UPDATE			1
#define MP4F_AT_WRITE_FUNC				2
#define MP4F_AFTER_MOOV					1
#define MP4F_BEFORE_MOOV				2
#define MP4F_CREATE_FULL				1
#define MP4F_CREATE_ONLY_HEADER			2
/* MP4F_CalculateSize, MP4F_WriteToMemBuffer  */
#define MP4F_WRITE_ALL					1
#define MP4F_WRITE_TO_MDATHDR			2
#define MP4F_WRITE_MOOV					3
#define MP4F_WRITE_MDAT 				4
#define MP4F_WRITE_BY_MIN_MEM 			5
#define MP4F_READ_ALL					6
#define MP4F_READ_RAWDATA				7
#define MP4F_WRITE_FROM_INITIAL			1
#define MP4F_WRITE_NEXT					2
#define MP4F_WRITE_COMPLETE				1
#define MP4F_WRITE_CONTINUE				2
/* MP4F_AddTrack, MP4F_GetTrackInformation */
#define MP4F_HANDLER_TYPE_VIDEO 		1
#define MP4F_HANDLER_TYPE_SOUND			2
#define MP4F_TRACK_ENABLE				1
#define MP4F_TRACK_IN_MOVIE				2
#define MP4F_TRACK_IN_PREVIEW			4
#ifdef MP4F_FORMAT_MOV_ELEMENT
#define MP4F_TRACK_IN_POSTER			8
#endif
/* MP4F_AddDataReference, MP4F_GetDataReference */
#define MP4F_DATA_TYPE_SELF_CONTAINED	1
#define MP4F_DATA_TYPE_URL				2
#define MP4F_DATA_TYPE_URN				3
/* MP4F_AddSampleDescription, MP4F_GetSampleDescription */
#define MP4F_SAMPLE_DESCR_TYPE_MP4A		1
#define MP4F_SAMPLE_DESCR_TYPE_MP4V		2
#define MP4F_SAMPLE_DESCR_TYPE_SAMR		3
#define MP4F_SAMPLE_DESCR_TYPE_SOWT		4	/* valid MP4F_FORMAT_MOV */
#define MP4F_SAMPLE_DESCR_TYPE_TWOS		5	/* valid MP4F_FORMAT_MOV */
#define MP4F_SAMPLE_DESCR_TYPE_ULAW		6	/* valid MP4F_FORMAT_MOV */
#define MP4F_SAMPLE_DESCR_TYPE_AVC1		7
#define MP4F_SAMPLE_DESCR_TYPE_HVC1		8
/* MP4F_AddSample, MP4F_GetSample etc */
#define MP4F_NO_SYNC_SAMPLE 			1
#define MP4F_SYNC_SAMPLE 				2
/* for TTR */
#define MP4F_NO_SAMPLE					0xFFFFFFFF
/* QuickTime Language Code Values */
typedef enum {
	MP4F_MOV_LANG_ENGLISH		= 0,
	MP4F_MOV_LANG_FRENCH		= 1,
	MP4F_MOV_LANG_GERMAN		= 2,
	MP4F_MOV_LANG_ITALIAN		= 3,
	MP4F_MOV_LANG_DUTCH			= 4,
	MP4F_MOV_LANG_SWEDISH		= 5,
	MP4F_MOV_LANG_SPANISH		= 6,
	MP4F_MOV_LANG_DANISH		= 7,
	MP4F_MOV_LANG_PORTUGUESE	= 8,
	MP4F_MOV_LANG_NORWEGIAN		= 9,
	MP4F_MOV_LANG_HEBREW		= 10,
	MP4F_MOV_LANG_JAPANESE		= 11,
	MP4F_MOV_LANG_ARABIC		= 12,
	MP4F_MOV_LANG_FINNISH		= 13,
	MP4F_MOV_LANG_GREEK			= 14,
	MP4F_MOV_LANG_ICELANDIC		= 15,
	MP4F_MOV_LANG_MALTESE		= 16,
	MP4F_MOV_LANG_TURKISH		= 17,
	MP4F_MOV_LANG_CROATIAN		= 18,
	MP4F_MOV_LANG_TRADITIONAL	= 19,
	MP4F_MOV_LANG_URDU			= 20,
	MP4F_MOV_LANG_HINDI			= 21,
	MP4F_MOV_LANG_THAI			= 22,
	MP4F_MOV_LANG_KOREAN		= 23,
	MP4F_MOV_LANG_LITHUANIAN	= 24,
	MP4F_MOV_LANG_POLISH		= 25,
	MP4F_MOV_LANG_HUNGARIAN		= 26,
	MP4F_MOV_LANG_ESTONIAN		= 27,
	MP4F_MOV_LANG_LETTISH		= 28,
	MP4F_MOV_LANG_LATVIAN		= 28,
	MP4F_MOV_LANG_SAAMISK		= 29,
	MP4F_MOV_LANG_LAPPISH		= 29,
	MP4F_MOV_LANG_FAEROESE		= 30,
	MP4F_MOV_LANG_FARSI			= 31,
	MP4F_MOV_LANG_RUSSIAN		= 32,
	MP4F_MOV_LANG_SIMPLIFIED	= 33,
	MP4F_MOV_LANG_FLEMISH		= 34,
	MP4F_MOV_LANG_IRISH			= 35,
	MP4F_MOV_LANG_ALBANIAN		= 36,
	MP4F_MOV_LANG_ROMANIAN		= 37,
	MP4F_MOV_LANG_CZECH			= 38,
	MP4F_MOV_LANG_SLOVAK		= 39,
	MP4F_MOV_LANG_SLOVENIAN		= 40,
	MP4F_MOV_LANG_YIDDISH		= 41,
	MP4F_MOV_LANG_SERBIAN		= 42,
	MP4F_MOV_LANG_MACEDONIAN	= 43,
	MP4F_MOV_LANG_BULGARIAN		= 44,
	MP4F_MOV_LANG_UKRAINIAN		= 45,
	MP4F_MOV_LANG_BYELORUSSIAN	= 46,
	MP4F_MOV_LANG_UZBEK			= 47,
	MP4F_MOV_LANG_KAZAKH		= 48,
	MP4F_MOV_LANG_AZERBAIJANI	= 49,
	MP4F_MOV_LANG_AZERBAIJANAR	= 50,
	MP4F_MOV_LANG_ARMENIAN		= 51,
	MP4F_MOV_LANG_GEORGIAN		= 52,
	MP4F_MOV_LANG_MOLDAVIAN		= 53,
	MP4F_MOV_LANG_KIRGHIZ		= 54,
	MP4F_MOV_LANG_TAJIKI		= 55,
	MP4F_MOV_LANG_TURKMEN		= 56,
	MP4F_MOV_LANG_MONGOLIAN		= 57,
	MP4F_MOV_LANG_MONGOLIANCYR	= 58,
	MP4F_MOV_LANG_PASHTO		= 59,
	MP4F_MOV_LANG_KURDISH		= 60,
	MP4F_MOV_LANG_KASHMIRI		= 61,
	MP4F_MOV_LANG_SINDHI		= 62,
	MP4F_MOV_LANG_TIBETAN		= 63,
	MP4F_MOV_LANG_NEPALI		= 64,
	MP4F_MOV_LANG_SANSKRIT		= 65,
	MP4F_MOV_LANG_MARATHI		= 66,
	MP4F_MOV_LANG_BENGALI		= 67,
	MP4F_MOV_LANG_ASSAMESE		= 68,
	MP4F_MOV_LANG_GUJARATI		= 69,
	MP4F_MOV_LANG_PUNJABI		= 70,
	MP4F_MOV_LANG_ORIYA			= 71,
	MP4F_MOV_LANG_MALAYALAM		= 72,
	MP4F_MOV_LANG_KANNADA		= 73,
	MP4F_MOV_LANG_TAMIL			= 74,
	MP4F_MOV_LANG_TELUGU		= 75,
	MP4F_MOV_LANG_SINHALESE		= 76,
	MP4F_MOV_LANG_BURMESE		= 77,
	MP4F_MOV_LANG_KHMER			= 78,
	MP4F_MOV_LANG_LAO			= 79,
	MP4F_MOV_LANG_VIETNAMESE	= 80,
	MP4F_MOV_LANG_INDONESIAN	= 81,
	MP4F_MOV_LANG_TAGALOG		= 82,
	MP4F_MOV_LANG_MALAYROMAN	= 83,
	MP4F_MOV_LANG_MALAYARABIC	= 84,
	MP4F_MOV_LANG_AMHARIC		= 85,
	MP4F_MOV_LANG_TIGRINYA		= 86,
	MP4F_MOV_LANG_GALLA			= 87,
	MP4F_MOV_LANG_OROMO			= 87,
	MP4F_MOV_LANG_SOMALI		= 88,
	MP4F_MOV_LANG_SWAHILI		= 89,
	MP4F_MOV_LANG_RUANDA		= 90,
	MP4F_MOV_LANG_RUNDI			= 91,
	MP4F_MOV_LANG_CHEWA			= 92,
	MP4F_MOV_LANG_MALAGASY		= 93,
	MP4F_MOV_LANG_ESPERANTO		= 94,
	MP4F_MOV_LANG_SEP1,
	MP4F_MOV_LANG_SEP2			= 127,
	MP4F_MOV_LANG_WELSH			= 128,
	MP4F_MOV_LANG_BASQUE		= 129,
	MP4F_MOV_LANG_CATALAN		= 130,
	MP4F_MOV_LANG_LATIN			= 131,
	MP4F_MOV_LANG_QUECHUA		= 132,
	MP4F_MOV_LANG_GUARANI		= 133,
	MP4F_MOV_LANG_AYMARA		= 134,
	MP4F_MOV_LANG_TATAR			= 135,
	MP4F_MOV_LANG_UIGHUR		= 136,
	MP4F_MOV_LANG_DZONGKHA		= 137,
	MP4F_MOV_LANG_JAVANESEROM	= 138,
	MP4F_MOV_LANG_MAX
} MP4F_MOV_LANG_CODE;

#ifdef MP4F_MULTIPUL_FREE_ATOM
/* Multipul Free Atom Place */
typedef enum{
	MP4F_FREE_PLACE_VIDEO_STSC = 0,	// Video Track (after stsc atom).
	MP4F_FREE_PLACE_VIDEO_STSZ,		// Video Track (after stsz atom).
	MP4F_FREE_PLACE_VIDEO_STCO,		// Video Track (after stco atom).
	MP4F_FREE_PLACE_VIDEO_STSS,		// Video Track (after stss atom).
	MP4F_FREE_PLACE_AUDIO_STSC,		// Audio Track (after stsc atom).
	MP4F_FREE_PLACE_AUDIO_STCO,		// Audio Track (after stco atom).
	MP4F_FREE_PLACE_MAX
} MP4F_FREE_PLACE;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* User-implemented API */
void *MP4F_malloc(unsigned long);
void *MP4F_realloc(void *, unsigned long);
void MP4F_free(void *);
#ifdef MP4F_TIME_CALL_START_TIMING
unsigned long MP4F_time(void);
#else
unsigned long MP4F_time(unsigned long *, unsigned long * );
#endif
signed long MP4F_lseek(signed int, unsigned long, signed int, unsigned long *);
signed long MP4F_read_header(signed int, void *, unsigned long);
signed long MP4F_read_sample(signed int, void *, void **, unsigned long);
signed long MP4F_write(signed int, const void *, unsigned long);

/* External API */
MP4FErr MP4F_OpenMemory(MP4FHANDLE *mp4fHandle, uInt32 openMode, uInt8 *mp4Address,
  uInt32 mp4Size, uInt32 mdatOrder, uInt32 mdatCreate, uInt32 movieTimescale);
MP4FErr MP4F_Close(MP4FHANDLE mp4fHandle);
MP4FErr MP4F_Read(MP4FHANDLE mp4fHandle);
MP4FErr MP4F_WriteToMemBuffer(MP4FHANDLE mp4fHandle, uInt32 writeAtomType,
  uInt8 *memBuffer, uInt32 *size, uInt32 *writeFlag);
MP4FErr MP4F_CalculateSize(MP4FHANDLE mp4fHandle, uInt32 type, uInt32 *size);
MP4FErr MP4F_GetMovieInformation(MP4FHANDLE mp4fHandle, uInt32 *movieTimescale,
  uInt32 *duration, uInt32 *trackCount, uInt32 **trackIdList, uInt32 *creationTime,
  uInt32 *modificationTime);
MP4FErr MP4F_SetFileType(MP4FHANDLE mp4fHandle, uInt32 majorBrand, uInt32 minorVersion,
  uInt32 compatibleBrandCount, uInt32 *compatibleBrandList);
MP4FErr MP4F_GetFileType(MP4FHANDLE mp4fHandle, uInt32 *majorBrand,
  uInt32 *minorVersion, uInt32 *compatibleBrandCount, uInt32 **compatibleBrandList);
#ifdef MP4F_FORMAT_MOV_ELEMENT
MP4FErr MP4F_AddTrack(MP4FHANDLE mp4fHandle, uInt32 *trackId, uInt32 handlerType,
  uInt32 trackFlags, uInt32 mediaTimescale, uInt32 matrix[9], uInt16 language, char *handlerName);
MP4FErr MP4F_GetTrackInformation(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *handlerType, uInt32 *trackFlags, uInt32 *durationByMovie,
  uInt32 *mediaTimescale, uInt32 *durationByMedia, uInt16 *language,
  char **handlerName);
#else
MP4FErr MP4F_AddTrack(MP4FHANDLE mp4fHandle, uInt32 *trackId, uInt32 handlerType,
  uInt32 trackFlags, uInt32 mediaTimescale, uInt32 matrix[9], uInt8 language[3], char *handlerName);
MP4FErr MP4F_GetTrackInformation(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *handlerType, uInt32 *trackFlags, uInt32 *durationByMovie,
  uInt32 *mediaTimescale, uInt32 *durationByMedia, uInt8 language[3],
  char **handlerName);
#endif
MP4FErr MP4F_SetWidthHeightToVideoTrack(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 width, uInt32 height);
MP4FErr MP4F_GetWidthHeightFromVideoTrack(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *width, uInt32 *height);
MP4FErr MP4F_AddDataReference(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 type,
  char *name, char *location, uInt32 *drefEntryId);
MP4FErr MP4F_GetDataReference(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *type,
  char **name, char **location, uInt32 drefEntryId);
MP4FErr MP4F_GetDataReferenceCount(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *drefEntryCount);
MP4FErr MP4F_AddSampleDescription(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 drefEntryId, uInt32 descrType, uInt32 mp4DecConfigParm[5],
  uInt32 decSpecificInfoSize, uInt8 *decSpecificInfo, uInt32 *stsdEntryId);
MP4FErr MP4F_GetSampleDescription(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *drefEntryId, uInt32 *descrType, uInt32 mp4DecConfigParm[5],
  uInt32 *decSpecificInfoSize, uInt8 **decSpecificInfo, uInt32 stsdEntryId);
MP4FErr MP4F_SetParmToMp4AudioSampleDescr(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 stsdEntryId, uInt16 channelCount, uInt16 sampleSize);
MP4FErr MP4F_GetParmFromMp4AudioSampleDescr(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 stsdEntryId, uInt16 *channelCount, uInt16 *sampleSize);
MP4FErr MP4F_SetParmToMp4VideoSampleDescr(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 stsdEntryId, uInt16 width, uInt16 height, uInt8 compressorname[32]);
MP4FErr MP4F_GetParmFromMp4VideoSampleDescr(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 stsdEntryId, uInt16 *width, uInt16 *height);
MP4FErr MP4F_GetSampleDescriptionCount(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *stsdEntryCount);
MP4FErr MP4F_AddSample(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 sampleSize,
  uInt32 sampleDuration, uInt32 sampleCTOffset, uInt32 syncSampleFlag, uInt8 *sample);
MP4FErr MP4F_EndCurrentChunk(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 stsdEntryId,uInt8* datap);
MP4FErr MP4F_AddChunk(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 stsdEntryId,
  uInt32 sampleCount, uInt32 *eachSampleSize, uInt32 *eachSampleDuration,
  uInt32 *eachSampleCTOffset, uInt32 *eachSampleSyncFlag, uInt8 *sample);
MP4FErr MP4F_GetNextSample(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *sampleId,
  uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration, uInt32 *sampleDT,
  uInt32 *sampleCTOffset, uInt32 *syncSampleFlag, uInt8 *sample, uInt8 **sample_adjustment);
MP4FErr MP4F_GetNextSampleOffset( MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *sampleId, uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration,
  uInt32 *sampleDT, uInt32 *sampleCTOffset, uInt32 *syncSampleFlag,
  uInt32 *sampleOffset);
MP4FErr MP4F_GetNextSyncSample(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *sampleId,
  uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration, uInt32 *sampleDT,
  uInt32 *sampleCTOffset, uInt8 *sample, uInt8 **sample_adjustment, uInt8 key_skip_num, uInt32 *calc_over_dt);
MP4FErr MP4F_GetNextSyncSampleOffset(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *sampleId, uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration,
  uInt32 *sampleDT, uInt32 *sampleCTOffset, uInt32 *sampleOffset, uInt8 key_skip_num);
MP4FErr MP4F_GetPrevSyncSample(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *sampleId,
  uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration, uInt32 *sampleDT,
  uInt32 *sampleCTOffset, uInt8 *sample, uInt8 **sample_adjustment, uInt8 key_skip_num);
MP4FErr MP4F_GetPrevSyncSampleOffset(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *sampleId, uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration,
  uInt32 *sampleDT, uInt32 *sampleCTOffset, uInt32 *sampleOffset, uInt8 key_skip_num);
MP4FErr MP4F_GetSample(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *sampleId,
  uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration, uInt32 *sampleDT,
  uInt32 *sampleCTOffset, uInt32 *syncSampleFlag, uInt8 *sample, uInt8 **sample_adjustment);
MP4FErr MP4F_GetSampleOffset(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *sampleId,
  uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration, uInt32 *sampleDT,
  uInt32 *sampleCTOffset, uInt32 *syncSampleFlag, uInt32 *sampleOffset);
MP4FErr MP4F_GetSyncSample(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *sampleId,
  uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration, uInt32 *sampleDT,
  uInt32 *sampleCTOffset, uInt8 *sample, uInt8 **sample_adjustment);
MP4FErr MP4F_GetSyncSampleOffset(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *sampleId, uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration,
  uInt32 *sampleDT, uInt32 *sampleCTOffset, uInt32 *sampleOffset);
MP4FErr MP4F_GetSyncSampleBefore(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *sampleId,
  uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration, uInt32 *sampleDT,
  uInt32 *sampleCTOffset, uInt8 *sample, uInt8 **sample_adjustment);
MP4FErr MP4F_GetSyncSampleOffsetBefore(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *sampleId, uInt32 *stsdEntryId, uInt32 *sampleSize, uInt32 *sampleDuration,
  uInt32 *sampleDT, uInt32 *sampleCTOffset, uInt32 *sampleOffset);
MP4FErr MP4F_GetSampleCount(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *sampleCount);
MP4FErr MP4F_GetSyncSampleCount(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 *syncSampleCount);
MP4FErr MP4F_GetNextChunkOffset(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *chunkId,
  uInt32 *stsdEntryId, uInt32 *sampleCount, uInt32 **eachSampleSize,
  uInt32 **eachSampleDuration, uInt32 **eachSampleDT, uInt32 **eachSampleCTOffset,
  uInt32 **eachSampleSyncFlag, uInt32 *chunkOffset,uInt32 *chunkSize);
MP4FErr MP4F_GetPrevChunkOffset(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *chunkId,
  uInt32 *stsdEntryId, uInt32 *sampleCount, uInt32 **eachSampleSize,
  uInt32 **eachSampleDuration, uInt32 **eachSampleDT, uInt32 **eachSampleCTOffset,
  uInt32 **eachSampleSyncFlag, uInt32 *chunkOffset,uInt32 *chunkSize);
MP4FErr MP4F_GetChunkOffset(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *chunkId,
  uInt32 searchSampleDT, uInt32 *stsdEntryId, uInt32 *sampleCount,
  uInt32 **eachSampleSize, uInt32 **eachSampleDuration, uInt32 **eachSampleDT,
  uInt32 **eachSampleCTOffset, uInt32 **eachSampleSyncFlag, uInt32 *chunkOffset,
  uInt32 *chunkSize);
MP4FErr MP4F_GetChunkCount(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *chunkCount);
MP4FErr MP4F_SetRawDataBuffer(MP4FHANDLE mp4fHandle, uInt8 *buffer, uInt32 bufferSize);
MP4FErr MP4F_GetVersion(uInt32 *version);
MP4FErr MP4F_GetTimeInfo(uInt32 fd, uInt32 *creationTime, uInt32 *modificationTime);
MP4FErr MP4F_OpenFile(MP4FHANDLE *mp4fHandle, uInt32 openMode, uInt32 fd, uInt32 mdatOrder,
  uInt32 writeMode, uInt32 movieTimescale);
MP4FErr MP4F_WriteToFile(MP4FHANDLE mp4fHandle);

#ifdef MP4F_FORMAT_MOV
MP4FErr MP4F_SetParmToQTVideoSampleDescr( MP4FHANDLE mp4fHandle, uInt32 trackId, 
  uInt32 stsdEntryId, uInt16 width, uInt16 height, uInt8 compressorname[32], uInt16 colrInfo[4]);
MP4FErr MP4F_GetParmFromQTVideoSampleDescr( MP4FHANDLE mp4fHandle, uInt32 trackId, 
  uInt32 stsdEntryId, uInt16 *width, uInt16 *height, uInt16 colrInfo[4]);
MP4FErr MP4F_SetParmToQTAudioSampleDescr(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 stsdEntryId, uInt16 channelCount, uInt16 sampleSize,
  uInt32 samplesPerPacket, uInt32 bytesPerPacket, uInt32 bytesPerFrame,
  uInt32 bytesPerSample);
MP4FErr MP4F_GetParmFromQTAudioSampleDescr(MP4FHANDLE mp4fHandle, uInt32 trackId,
  uInt32 stsdEntryId, uInt16 *channelCount, uInt16 *sampleSize,
  uInt32 *samplesPerPacket, uInt32 *bytesPerPacket, uInt32 *bytesPerFrame,
  uInt32 *bytesPerSample);
#endif

/* valid MP4F_FAST_FS */
MP4FErr MP4F_GetNextChunkOffsetLoad(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *chunkId,
  uInt32 *stsdEntryId, uInt32 *sampleCount, uInt32 **eachSampleSize,
  uInt32 **eachSampleDuration, uInt32 **eachSampleDT, uInt32 **eachSampleCTOffset,
  uInt32 **eachSampleSyncFlag, uInt32 *chunkOffset, uInt32 *chunkSize, uInt8 *sample, uInt8 **sample_adjustment, uInt32 *calc_over_dt);
/* valid MP4F_FAST_FS */
MP4FErr MP4F_GetPrevChunkOffsetLoad(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *chunkId,
  uInt32 *stsdEntryId, uInt32 *sampleCount, uInt32 **eachSampleSize,
  uInt32 **eachSampleDuration, uInt32 **eachSampleDT, uInt32 **eachSampleCTOffset,
  uInt32 **eachSampleSyncFlag, uInt32 *chunkOffset, uInt32 *chunkSize, uInt8 *sample, uInt8 **sample_adjustment);
MP4FErr MP4F_GetRate(MP4FHANDLE mp4fHandle, uInt32 trackId, float *rate);
MP4FErr MP4F_ResetTrackCounter(MP4FHANDLE mp4fHandle, uInt32 trackId);

#ifdef MP4F_UUID_ATOM
MP4FErr MP4F_AddUserExtension( MP4FHANDLE mp4fHandle, uInt32 trackId, uInt8 *data, uInt32 dataSize, uInt8 extendedType[16] );
MP4FErr MP4F_GetUserExtensionSize( MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *dataSize, uInt8 extendedType[16] );
MP4FErr MP4F_GetUserExtension( MP4FHANDLE mp4fHandle, uInt32 trackId, uInt8 *data );
#endif

#ifdef MP4F_FREE_ATOM
MP4FErr MP4F_AddFree(MP4FHANDLE mp4fHandle, uInt8 *data, uInt32 dataSize);
MP4FErr MP4F_GetFreeSize(MP4FHANDLE mp4fHandle, uInt32 *dataSize);
MP4FErr MP4F_GetFree(MP4FHANDLE mp4fHandle, uInt8 *data);
#ifdef MP4F_MULTIPUL_FREE_ATOM
MP4FErr MP4F_AddFree_Multi(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 place, uInt8 *data, uInt32 dataSize);
MP4FErr MP4F_GetFreeSize_Multi(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 place, uInt32 *dataSize);
MP4FErr MP4F_GetFree_Multi(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 place, uInt8 *data);
#endif
#endif

#ifdef MP4F_UDTA_ATOM
MP4FErr MP4F_AddUdta(MP4FHANDLE mp4fHandle, uInt8 *data, uInt32 dataSize);
MP4FErr MP4F_GetUdtaSize(MP4FHANDLE mp4fHandle, uInt32 *dataSize);
MP4FErr MP4F_GetUdta(MP4FHANDLE mp4fHandle, uInt8 *data);
MP4FErr MP4F_EditUdta(MP4FHANDLE mp4fHandle, uInt8 *data, uInt32 dataSize);
#endif

MP4FErr MP4F_AddEditList( MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 trackDuration, uInt32 mediaTime, uInt32 mediaRate);
MP4FErr MP4F_GetEditListCount(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 *listCount);
MP4FErr MP4F_GetEditList(MP4FHANDLE mp4fHandle, uInt32 trackId, uInt32 listId, uInt32 *trackDuration, uInt32 *mediaTime, uInt32 *mediaRate);


MP4FErr MP4F_SizeAdditionStart(MP4FHANDLE mp4fHandle, uInt32 max_media_size, uInt32 audio_1sec_size);
MP4FErr MP4F_SizeAdditionReset(MP4FHANDLE mp4fHandle);

#ifdef __cplusplus
}
#endif

#endif
