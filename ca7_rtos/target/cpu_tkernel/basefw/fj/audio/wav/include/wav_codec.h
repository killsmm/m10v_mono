/**
 * @file		wav_codec.h
 * @brief		api for WAV file making
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _WAV_CODEC_H
#define _WAV_CODEC_H

//// PCM /////

/* Conversion from a strereo to mono. */
/* Mono is constructed in left channel data of stereo. */
/* Stereo(Lch/Rch) -> Mono(Lch) */
extern void pcm_Stereo_to_Mono(unsigned short *pcm, unsigned long sample_cnt);

/* Conversion from a mono to stereo. */
/* Stereo is constructed in left chl data and Right ch data. */
/* Data same as Left ch in Right ch. */
/* Mono(Lch) -> Stereo(Lch/Rch(=Lch)) */
extern void pcm_Mono_to_Stereo(unsigned short *pcm, unsigned long sample_cnt);

//// u-law /////
extern void ULAW_Enc(unsigned char* law, short *pcm, unsigned long smpl_cnt, unsigned char channel);
extern void ULAW_Dec(short *pcm, unsigned char *law, unsigned long smpl_cnt, unsigned char channel);

//// a-law /////
extern void ALAW_Enc(unsigned char* law, short *pcm, unsigned long smpl_cnt, unsigned char channel);
extern void ALAW_Dec(short *pcm, unsigned char *law, unsigned long smpl_cnt, unsigned char channel);

#endif
