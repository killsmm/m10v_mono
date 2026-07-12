#ifndef H_RTSP_STREAM_COM_H
#define H_RTSP_STREAM_COM_H
#define STREMD_STRUCT_MAX (31)
#define RTSP_INFO_SOCK_NAME "/tmp/streamd_infor_rtsp_sock"
#define RTSP_ADD_SOCK_NAME "/tmp/streamd_add_rtsp_sock"
#define D_MAX_STREAM_NUM	(STREMD_STRUCT_MAX)
#define D_MEDIA_TYPE_VIDEO	(1)
#define D_MEDIA_TYPE_AUDIO	(2)
#define D_MEDIA_TYPE_META	(3)
#define RTSP_ON_OFF "/tmp/use_rtsp_func.def"

typedef enum {
	E_ENCODE_TYPE_H264 = 1,
	E_ENCODE_TYPE_HEVC,
	E_ENCODE_TYPE_MJPEG,
	E_ENCODE_TYPE_ACC,
	E_ENCODE_TYPE_YUV,
	E_ENCODE_TYPE_META,
}E_ENCODE_TYPE;
typedef enum {
	E_NETSEC_APL_ID_GET_STREAM =1001,
	E_NETSEC_APL_ID_RES_GET_STREAM,
	E_NETSEC_APL_ID_ADD_STREAM,
	E_NETSEC_APL_ID_RES_ADD_STREAM,
	E_NETSEC_APL_ID_RMV_STREAM,
	E_NETSEC_APL_ID_RES_RMV_STREAM,
	E_NETSEC_APL_ID_PLAY_STREAM,
	E_NETSEC_APL_ID_END,
} E_NETSEC_APL_ID;
typedef enum {
	E_NETSEC_APL_RESULT_OK = 0,
	E_NETSEC_APL_RESULT_NG,
} E_NETSEC_APL_RESULT;
typedef enum {
	E_SAMPLING_RGB =1,
	E_SAMPLING_YCC422,
	E_SAMPLING_YCC420,
	E_SAMPLING_YCC444,
}E_SAMPLING_TYPE;
typedef enum {
	E_COLORIMETRY_BT709 =1,
	E_COLORIMETRY_BT601,
	E_COLORIMETRY_SMPTE240M,
}E_COLORIMETRY;
typedef struct {
	E_NETSEC_APL_ID request_id;
	E_NETSEC_APL_RESULT result;
}T_RES_NETSEC_APL;

typedef struct {
	unsigned char stream_id;//From 0
	unsigned char media_type;//1:video. 2: Audio
	unsigned char encode_type;//1:H264, 2: H265, 3 MJpeg. 4: ACC, 5:YUV
	unsigned char payload_id;
	unsigned int clock_rate;
	unsigned int profile_level_id;
//	unsigned int packe_mode;
	unsigned char frame_rate;
	unsigned char sampleing;//1:RGB 2: YUV422 3:YUV420 4:YUV444
	unsigned char depth;//Bit num per pixel
	unsigned char colorimetry;//1:BT709-2 2:BT601-5 3:SMPTE240M
	unsigned short width;
	unsigned short height;

}T_STREAM_INFOR;
typedef struct {
	E_NETSEC_APL_ID request_id;//Must be E_NETSEC_APL_ID_RES_GET_STREAM
	unsigned int stream_num;//Must be less than D_MAX_STREAM_NUM
	T_STREAM_INFOR stream_infor[D_MAX_STREAM_NUM];
}T_RES_STREAM_INFOR;

typedef struct {
	E_NETSEC_APL_ID request_id;//Must be E_NETSEC_APL_ID_ADD_STREAM
	int stream_id;
	unsigned int src_port[2];
	unsigned int des_port[2];
	char des_ip[32];
}T_ADD_STREAM;
typedef struct {
	E_NETSEC_APL_ID request_id;//Must be E_NETSEC_APL_ID_RMV_STREAM
	unsigned int stream_id;
}T_REMOVE_STREAM;
typedef struct {
	E_NETSEC_APL_ID request_id;
	unsigned int result;
}T_NETSC_APL_RESULT;

#define MAX_ab(a,b)	((a)>(b) ? a : b)
#define D_BUFFER_SIZE (MAX_ab(sizeof(T_ADD_STREAM),sizeof(T_RES_STREAM_INFOR))/sizeof(unsigned int))
#endif

