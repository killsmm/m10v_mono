void MakeLutPoint(int ,int ,int ,int ,int ,unsigned char *,unsigned char *,unsigned char *,
				 unsigned char *,unsigned char *, unsigned char *, unsigned char *, unsigned char *,
				 unsigned char , unsigned char *);

/*--------------*/
/*  パラメータ	*/
/*--------------*/
//X座標初期値


#define X1ORG 172
#define X2ORG 84
#define X3ORG 214
#define X4ORG 42
//X座標調整用
#define X1ADJ 0                   //adjustment value of X1
#define X2ADJ 0                   //adjustment value of X2
#define X3ADJ 0                   //adjustment value of X3
#define X4ADJ 0                   //adjustment value of X4

// Low level
//Y_min,max調整用
#define Y1MAXADJ_0 0                //adjustment value of the maximum value of Y1
#define Y1MINADJ_0 0                //adjustment value of the minimum value of Y1
#define Y2MAXADJ_0 0                //adjustment value of the maximum value of Y2
#define Y2MINADJ_0 0                //adjustment value of the minimum value of Y2
#define Y3MAXADJ_0 0                //adjustment value of the maximum value of Y3
#define Y3MINADJ_0 0                //adjustment value of the minimum value of Y3
#define Y4MAXADJ_0 0                //adjustment value of the maximum value of Y4
#define Y4MINADJ_0 0                //adjustment value of the minimum value of Y4
//Y1-Y2閾値
#define Y1TOY2_0 50

// Middle level
//Y_min,max調整用
#define Y1MAXADJ_1 -5              	//adjustment value of the maximum value of Y1
#define Y1MINADJ_1 0               	//adjustment value of the minimum value of Y1
#define Y2MAXADJ_1 0               	//adjustment value of the maximum value of Y2
#define Y2MINADJ_1 5               	//adjustment value of the minimum value of Y2
#define Y3MAXADJ_1 -10              //adjustment value of the maximum value of Y3
#define Y3MINADJ_1 0                //adjustment value of the minimum value of Y3
#define Y4MAXADJ_1 5                //adjustment value of the maximum value of Y4
#define Y4MINADJ_1 5                //adjustment value of the minimum value of Y4
//Y1-Y2閾値
#define Y1TOY2_1 45

// High level
//Y_min,max調整用
#define Y1MAXADJ_2 -10              //adjustment value of the maximum value of Y1
#define Y1MINADJ_2 0                //adjustment value of the minimum value of Y1
#define Y2MAXADJ_2 15               //adjustment value of the maximum value of Y2
#define Y2MINADJ_2 15               //adjustment value of the minimum value of Y2
#define Y3MAXADJ_2 -15              //adjustment value of the maximum value of Y3
#define Y3MINADJ_2 0                //adjustment value of the minimum value of Y3
#define Y4MAXADJ_2 10               //adjustment value of the maximum value of Y4
#define Y4MINADJ_2 10               //adjustment value of the minimum value of Y4
//Y1-Y2閾値
#define Y1TOY2_2 35

//Y3-Y1閾値
#define Y3TOY1 20
#define Y3TOY1PLUS 20





