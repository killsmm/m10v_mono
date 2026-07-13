/****************************************************************************/
/*  <Contents>   ホワイトマップ用ヘッダファイル                             */
/*  <Filename>   SAwbWhiteMap.h                                             */
/*  <Enviroment>                                                            */
/*                                                                          */
/*                                      Copyright 2015 Socionext Inc.       */
/*                                                SOCIONEXT Confidential    */
/****************************************************************************/

#ifndef SAWBWHITEMAP_H_INC
#define SAWBWHITEMAP_H_INC

/*========================================================================*/
/* 定数定義                                                               */
/*========================================================================*/
#define WM_X_ARRAY_SZ				(30)
#define WM_Y_ARRAY_SZ				(100)

#ifdef  SAWBWHITEMAP_H_DEF

#else 
	extern const unsigned char def_saucOutXyMap[3][87][2];
	extern const unsigned char def_saucWhiteMap[2][WM_Y_ARRAY_SZ][WM_X_ARRAY_SZ];
	extern unsigned char saucOutXyMap[3][87][2];
	extern unsigned char saucWhiteMap[2][WM_Y_ARRAY_SZ][WM_X_ARRAY_SZ];
#endif /* SAWBWHITEMAP_H_DEF */


#endif /* SAWBWHITEMAP_H_INC */
