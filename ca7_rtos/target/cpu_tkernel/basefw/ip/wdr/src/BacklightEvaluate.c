
/*-----------------------------------*/
/* セクション定義					 */
/*-----------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "BacklightEvaluate.h"

//	［改版履歴］
//		＝レベル＝	＝作者名＝	＝作成日＝	＝コメント＝
//		  V1.00		Toyoda  	2009.08.06	正規化処理修正版、４捨５入修正

/*********************************************************************
関数名：BacklightEvaluate()
概要　：５区分のヒストグラムから逆光度合いを判別する。
引数　：int	sd1		シャドー１の画素数（0 - 41）
        int	sd2		シャドー２の画素数（42 - 83）
		int	md		ミッドレンジの画素数（84 - 171）
		int	hl1		ハイライト１の画素数（172 - 213）
		int	hl2		ハイライト２の画素数（214 - 255）
復帰値：int	total	　逆光度合い（0～100）

*********************************************************************/
int BacklightEvaluate(int sd1, int sd2, int md, int hl1, int hl2)
{
	/*----------*/
	/* 変数宣言 */
	/*----------*/
	int sd,hl;
	int g1,g2,g3;
	int total = 0;
	int pix, ratio;
	int sa1,sa2,omomi1,omomi2;

	//ヒストグラムの母数を正規化（160ｘ120のサムネイルサイズへ）
	pix = sd1 + sd2 + md + hl1 + hl2;
	ratio = (pix <<8) / SUMPIX ;
	if(ratio < 1) ratio = 1;
	sd1 = (sd1 << 8) / ratio;
	sd2 = (sd2 << 8) / ratio;
	md  = (md  << 8) / ratio;
	hl1 = (hl1 << 8) / ratio;
	hl2 = (hl2 << 8) / ratio;
	//printf("%d,%d,%d,%d,%d,all=%d,ratio=%d\n",sd1,sd2,md,hl1,hl2,sd1 + sd2 + md + hl1 + hl2,ratio);

	sd = sd1 + sd2;
	hl = hl1 + hl2;

	//逆光判定処理
	//その①　５区分のヒストグラムの形状で判定　でこぼこしているかどうか
	//本処理では、逆光パターンを抽出。初期値0なので、どれかに引っ掛からないと逆光度合い0
	g1 = 0;
	if(sd1 > md+PARAM1_1){
		if(sd2 < md+PARAM1_2){
			//printf("逆光１(10)\n");
			//差分によって点数付け
			sa1 = sd1 - sd2;
			sa2 = md  - sd2;
			//printf("sa1=%d,sa2=%d\n",sa1,sa2);

			if(sa1 < sa2) {
				omomi1 = sa1 / OMOMI_PIX1;
				omomi2 = sa2 / OMOMI_PIX2;
			}else{
				omomi1 = sa2 / OMOMI_PIX1;
				omomi2 = sa1 / OMOMI_PIX2;
			}
			if(omomi1< OMOMI_MIN1)		omomi1 = OMOMI_MIN1;
			else if(omomi1 > OMOMI_MAX1)omomi1 = OMOMI_MAX1;
			if(omomi2< OMOMI_MIN2)		omomi2 = OMOMI_MIN2;
			else if(omomi2 > OMOMI_MAX2)omomi2 = OMOMI_MAX2;
			//printf("omomi1=%d.omomi2=%d\n",omomi1,omomi2);

			g1 = G1A * (omomi1+omomi2);
		}else if(md < hl1+PARAM1_3 || md < hl2+PARAM1_4){
			//printf("逆光２(8)\n");
			//差分によって点数付け
			sa1 = sd1 - md;
			if(hl1 > hl2) sa2 = hl1  - md;
			else          sa2 = hl2  - md;
			//printf("sa1=%d,sa2=%d\n",sa1,sa2);
			if(sa1 < sa2) {
				omomi1 = sa1 / OMOMI_PIX1;
				omomi2 = sa2 / OMOMI_PIX2;
			}else{
				omomi1 = sa2 / OMOMI_PIX1;
				omomi2 = sa1 / OMOMI_PIX2;
			}
			if(omomi1< OMOMI_MIN1)		omomi1 = OMOMI_MIN1;
			else if(omomi1 > OMOMI_MAX1)omomi1 = OMOMI_MAX1;
			if(omomi2< OMOMI_MIN2)		omomi2 = OMOMI_MIN2;
			else if(omomi2 > OMOMI_MAX2)omomi2 = OMOMI_MAX2;
			//printf("omomi1=%d.omomi2=%d\n",omomi1,omomi2);

			g1 = G1B * (omomi1+omomi2);
		}
	}else{
		if(sd2 < md+PARAM1_5 && sd2 < sd1+PARAM1_6){
			//printf("逆光３(5)\n");
			//差分によって点数付け
			sa1 = sd1 - sd2;
			sa2 = md  - sd2;
			//printf("sa1=%d,sa2=%d\n",sa1,sa2);
			if(sa1 < sa2) {
				omomi1 = sa1 / OMOMI_PIX1;
				omomi2 = sa2 / OMOMI_PIX2;
			}else{
				omomi1 = sa2 / OMOMI_PIX1;
				omomi2 = sa1 / OMOMI_PIX2;
			}
			if(omomi1< OMOMI_MIN1)		omomi1 = OMOMI_MIN1;
			else if(omomi1 > OMOMI_MAX1)omomi1 = OMOMI_MAX1;
			if(omomi2< OMOMI_MIN2)		omomi2 = OMOMI_MIN2;
			else if(omomi2 > OMOMI_MAX2)omomi2 = OMOMI_MAX2;
			//printf("omomi1=%d.omomi2=%d\n",omomi1,omomi2);

			g1 = G1C * (omomi1+omomi2);
		}
		else if(sd2 > md+PARAM1_7 ){
			if(md < hl1+PARAM1_8 || md < hl2+PARAM1_9){
				//printf("逆光４(6)\n");
				//差分によって点数付け
				sa1 = sd2 - md;
				if(hl1 > hl2) sa2 = hl1  - md;
				else          sa2 = hl2  - md;
				//printf("sa1=%d,sa2=%d\n",sa1,sa2);
				if(sa1 < sa2) {
					omomi1 = sa1 / OMOMI_PIX1;
					omomi2 = sa2 / OMOMI_PIX2;
				}else{
					omomi1 = sa2 / OMOMI_PIX1;
					omomi2 = sa1 / OMOMI_PIX2;
				}
				if(omomi1< OMOMI_MIN1)		omomi1 = OMOMI_MIN1;
				else if(omomi1 > OMOMI_MAX1)omomi1 = OMOMI_MAX1;
				if(omomi2< OMOMI_MIN2)		omomi2 = OMOMI_MIN2;
				else if(omomi2 > OMOMI_MAX2)omomi2 = OMOMI_MAX2;
				//printf("omomi1=%d.omomi2=%d\n",omomi1,omomi2);

				g1 = G1D * (omomi1+omomi2);
			}
		}
	}
	//その②　３区分のヒストグラム　中間調がへこんでいるかどうか
	//本処理では、逆光ではないパターンを抽出する。初期値10
	g2 = 10;
	if(hl > md+PARAM2_1 && md > sd+PARAM2_2){
		//printf("逆光ではない(2)\n");
		sa1 = hl - sd;
		omomi1 = sa1/OMOMI_PIX3;
		if(omomi1< G2A_MIN)		 omomi1 = G2A_MIN;
		else if(omomi1 > G2A_MAX)omomi1 = G2A_MAX;
		g2 = 10 - omomi1;
	}else if(md > sd+PARAM2_3 && md > hl+PARAM2_4){
		//printf("ほぼ逆光ではない(4)\n");
		sa1 = md - sd;
		sa2 = md - hl;
		if(sa1 < sa2) omomi1 = sa1 / OMOMI_PIX4;
		else		  omomi1 = sa2 / OMOMI_PIX4;
		if(omomi1< G2B_MIN)		 omomi1 = G2B_MIN;
		else if(omomi1 > G2B_MAX)omomi1 = G2B_MAX;
		g2 = 10 - omomi1;
	}else if(md > sd+PARAM2_5 || md > hl+PARAM2_6){
		//printf("たぶん逆光ではない(7)\n");
		sa1 = md - sd;
		sa2 = md - hl;
		if(sa1 > sa2) omomi1 = sa1 / OMOMI_PIX5;
		else		  omomi1 = sa2 / OMOMI_PIX5;
		if(omomi1< G2C_MIN)		 omomi1 = G2C_MIN;
		else if(omomi1 > G2C_MAX)omomi1 = G2C_MAX;
		g2 = 10 - omomi1;
	}

	//その③　ハイライトの量
	//本処理では、逆光ではないパターンを抽出する。初期値10
	g3 = 10;
	if(hl < HL1){ //<1000
		//printf("ハイライト無し\n");
		g3 = 10*hl/HL1;
		if(g3 < G3A_MIN)		g3 = G3A_MIN;
		else if(g3 > G3A_MAX)	g3 = G3A_MAX;
	}else if(hl2 < HL2){ //<300
		//printf("ハイエストライト不足で怪しい\n");
		g3 = 10*hl2/HL2;
		if(g3 < G3B_MIN)		g3 = G3B_MIN;
		else if(g3 > G3B_MAX)	g3 = G3B_MAX;
	}	

	//printf("g1=%d,g2=%d,g3=%d\n",g1,g2,g3);
	//①②③を掛け合わせて逆光度合い算出（g1が100,g2g3が10で逆光度合い100%になる）
	total = (g1 * g2 * g3 + 50) /100;
	//printf("逆光度合い==%d％\n",total);

	return total; 



}
