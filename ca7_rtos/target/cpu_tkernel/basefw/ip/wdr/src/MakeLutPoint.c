
/*-----------------------------------*/
/* セクション定義					 */
/*-----------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MakeLutPoint.h"

// Adjustment value structure
typedef struct{
	signed char	Y1MaxAdj;	// the maximum value of Y1
	signed char	Y1MinAdj;	// the minimum value of Y1
	signed char	Y2MaxAdj;	// the maximum value of Y2
	signed char	Y2MinAdj;	// the minimum value of Y2
	signed char	Y3MaxAdj;	// the maximum value of Y3
	signed char	Y3MinAdj;	// the minimum value of Y3
	signed char	Y4MaxAdj;	// the maximum value of Y4
	signed char	Y4MinAdj;	// the minimum value of Y4
	signed char	Y1Toy2;		// Y1-Y2閾値
	signed char	Y2PixMul;	// Y2算出時Pixel換算係数
}T_MLP_ADJ;

/*-------------------------------------------------------------------------------*/
/*関数名：MakeLutPoint()                                                         */
/*　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 */
/*概要： 補正カーブの曲点を算出                                                  */
/*                                                                               */
/*引数：　int NLf1,NLf2,NMid,NRt1,NRt2    五本のヒストグラム　　　　　　　　　　 */
/*        unsigned char *X1,*X2,*X3,*X4   曲点の横座標           　　　　        */
/*        unsigned char *Y1,*Y2,*Y3,*Y4   曲点の縦座標          　 　　　        */
/*																	             */
/*復帰値：なし　　　　　　　　　　　　　　　　　　　　　　 　　　 　　　　　　　 */
/*                                                                               */
/*備考：　																		 */
/*-------------------------------------------------------------------------------*/

void MakeLutPoint(int LfWeight1, int LfWeight2, int MidWeight, int RtWeight1, int RtWeight2, 
				 unsigned char *X1,unsigned char *X2, unsigned char *X3, unsigned char *X4,
				 unsigned char *Y1, unsigned char *Y2, unsigned char *Y3, unsigned char *Y4,
				 unsigned char lvl, unsigned char *RGamma)
{   
	//unsigned char Y1Max, Y1Min, Y2Max,Y2Min, Y3Max, Y3Min, Y4Max, Y4Min;
	unsigned char Y2Min, Y3Max, Y3Min, Y4Max;
	//int temp;
	int Pix;
	unsigned char Y1Max, Y1Min, Y2Max, Y4Min;
	const T_MLP_ADJ AdjVal[3]={
		// Y1MaxAdj,   Y1MinAdj,   Y2MaxAdj,   Y2MinAdj,   Y3MaxAdj,   Y3MinAdj,   Y4MaxAdj,   Y4MinAdj,  Y1Toy2, Y2PixMul,
		{Y1MAXADJ_0, Y1MINADJ_0, Y2MAXADJ_0, Y2MINADJ_0, Y3MAXADJ_0, Y3MINADJ_0, Y4MAXADJ_0, Y4MINADJ_0, Y1TOY2_0,       2},
		{Y1MAXADJ_1, Y1MINADJ_1, Y2MAXADJ_1, Y2MINADJ_1, Y3MAXADJ_1, Y3MINADJ_1, Y4MAXADJ_1, Y4MINADJ_1, Y1TOY2_1,       3},
		{Y1MAXADJ_2, Y1MINADJ_2, Y2MAXADJ_2, Y2MINADJ_2, Y3MAXADJ_2, Y3MINADJ_2, Y4MAXADJ_2, Y4MINADJ_2, Y1TOY2_2,       3}
		};
//	printf("AdjVal[%d].Y1MaxAdj=%d\n",lvl,AdjVal[lvl].Y1MaxAdj);
    //３種類のパラメータをテストするため、下記をコメントした2009/02/04
//	lvl = (lvl > 1) ? 1 : lvl;
	Y1Max = X1ORG + (255-X1ORG)/3+AdjVal[lvl].Y1MaxAdj;		/***Y1の最大値***/
	Y1Min = X1ORG + AdjVal[lvl].Y1MinAdj;					/***Y1の最小値***/
	Y2Max = X2ORG + (255-X2ORG)/3 + AdjVal[lvl].Y2MaxAdj;	/***Y2の最大値***/
	//Y4Min = X4ORG + AdjVal[lvl].Y4MinAdj;					/***Y4の最小値***//***Y4の最小値→Y2の値より算出する***/
	
	Pix = LfWeight1 +  LfWeight2 + MidWeight + RtWeight1 + RtWeight2; //サムネイル画像の画素数

    /*明部と中間部との曲点の縦座標を算出:*/
	//Y1Max = X1ORG + (255-X1ORG)/3+Y1MAXADJ;   /***Y1の最大値→const宣言に移動***/ 
	//Y1Min = X1ORG + Y1MINADJ;                 /***Y1の最小値→const宣言に移動***/
	*Y1= 255-255*(RtWeight1+RtWeight2)/Pix;		/***Y1算出***/
	//printf("Y1=%d, Y1Min=%d,Y1Max=%d\n",*Y1,Y1Min,Y1Max);
	if (*Y1>Y1Max) *Y1=Y1Max;
	else if(*Y1<Y1Min) *Y1=Y1Min;   

	/***中間部と暗部との曲点の縦座標を算出*/
	//Y2Max = X2ORG + (255-X2ORG)/3 + Y2MAXADJ; /***Y2の最大値→const宣言に移動***/
	Y2Min = *Y1 - X2ORG + AdjVal[lvl].Y2MinAdj; /***Y2の最小値***/

	if((RtWeight1+RtWeight2+MidWeight)>3*Pix/4) *Y2=Y2Min;
	else {
   		  *Y2=*Y1-255*MidWeight/(Pix*AdjVal[lvl].Y2PixMul); /***Y2算出***/

		  if (*Y2>Y2Max) *Y2=Y2Max;
		  else if (*Y2<Y2Min) *Y2=Y2Min;
	}
	//printf("Y2=%d,Y2Min=%d,Y2Max=%d\n",*Y2,Y2Min,Y2Max);
	if (*Y1-*Y2<=AdjVal[lvl].Y1Toy2) {
	//	temp=*Y1-*Y2;
	//	*Y1=*Y1+(Y1TOY2-temp)/3;
	//	*Y2=*Y2-(Y1TOY2-temp)*2/3;  	/***中間部分の幅を制限する(小さい過ぎるを防止）***/
		*Y1=*Y2+AdjVal[lvl].Y1Toy2;
	}

	/***明部をさらに分割の曲点の縦座標Y3を算出***/
	Y3Max = *Y1 + (255-*Y1)*2/3 + AdjVal[lvl].Y3MaxAdj;  /***Y3の最大値***/
	Y3Min = *Y1 + (255-*Y1)/3 + AdjVal[lvl].Y3MinAdj;     /***Y3の最小値***/

	*Y3=*Y1+(255-*Y1)*RtWeight1/(RtWeight1+RtWeight2);   /***Y3算出***/
	if (*Y3>Y3Max) *Y3=Y3Max; 
	else if (*Y3<Y3Min) *Y3=Y3Min;

	if (*Y3-*Y1<Y3TOY1) *Y3=*Y1+Y3TOY1PLUS;  /*明部の左側の部分の幅を制限する(小さい過ぎるを防止)*/

	/***暗部をさらに分割の曲点の縦座標Y4を算出***/
	Y4Max=*Y2*2/3 + AdjVal[lvl].Y4MaxAdj;		/***Y4の最大値***/
	//Y4Min=X4ORG + Y4MINADJ;					/***Y4の最小値→const宣言に移動***/
	Y4Min=*Y2/2 + AdjVal[lvl].Y4MinAdj;			/***Y4の最小値***/

    *Y4=*Y2*LfWeight1/(LfWeight1+LfWeight2);   /***Y4算出***/
	if(*Y4>Y4Max) *Y4=Y4Max;
	else if (*Y4<Y4Min) *Y4=Y4Min;

  	*X1=X1ORG+X1ADJ;      /***明部と中間部との曲点の横座標X1を算出***/
    *X2=X2ORG+X2ADJ;      /***中間部と暗部との曲点の横座標X2を算出***/
	*X3=X3ORG+X3ADJ;      /***明部をさらに分割の曲点の横座標X3を算出***/
	*X4=X4ORG+X4ADJ;      /***暗部をさらに分割の曲点の横座標X4を算出***/  

	//ガンマ前の曲点
	//printf("ガンマ前の曲点\n");
	//printf("X1=%d,X2=%d,X3=%d,X4=%d\n",*X1,*X2,*X3,*X4);
	//printf("Y1=%d,Y2=%d,Y3=%d,Y4=%d\n",*Y1,*Y2,*Y3,*Y4);		

	*X1 = RGamma[*X1];
	*X2 = RGamma[*X2];
	*X3 = RGamma[*X3];
	*X4 = RGamma[*X4];
	*Y1 = RGamma[*Y1];
	*Y2 = RGamma[*Y2];
	*Y3 = RGamma[*Y3];
	*Y4 = RGamma[*Y4];

	//if (*Y1>=*X1*255>>7) *Y1=*X1*255>>7;
	//if (*Y2>=*X2*255>>7)  *Y2=*X2*255>>7;//*X2=(int)((double)(*Y2<<7)/255.+0.5);
	//if (*Y3>=*X3*255>>7) *Y3=*X3*255>>7;
	//if (*Y4>=*X4*255>>7) *Y4=*X4*255>>7;// *X4=(int)((double)(*Y4<<7)/255.+0.5);

	//ガンマ後の曲点
	//printf("ガンマ後の曲点\n");
	//printf("X1=%d,X2=%d,X3=%d,X4=%d\n",*X1,*X2,*X3,*X4);
	//printf("Y1=%d,Y2=%d,Y3=%d,Y4=%d\n",*Y1,*Y2,*Y3,*Y4);		
	//printf("Y1Max=%d,Y2Max=%d,Y3Max=%d,Y4Max=%d\n",Y1Max,Y2Max,Y3Max,Y4Max);
	//printf("Y1Min=%d,Y2Min=%d,Y3Min=%d,Y4Min=%d\n",Y1Min,Y2Min,Y3Min,Y4Min);

	return;
}
