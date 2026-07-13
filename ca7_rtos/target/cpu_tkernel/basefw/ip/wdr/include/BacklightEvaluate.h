int BacklightEvaluate(int sd1, int sd2, int md, int hl1, int hl2);

/*--------------*/
/*  パラメータ	*/
/*--------------*/
//５区分ヒストグラム凹凸判定結果
#define G1A 10
#define G1B 8
#define G1C 5
#define G1D 6
//３区分ヒストグラム凹凸判定結果
#define G2A_MAX 8
#define G2A_MIN 0
#define G2B_MAX 6
#define G2B_MIN 0
#define G2C_MAX 3
#define G2C_MIN 0
//ハイライト量判定結果
#define G3A_MAX 10
#define G3A_MIN 0
#define G3B_MAX 10
#define G3B_MIN 7
//ハイライト量判定閾値
#define HL1 1000
#define HL2 300
//判定処理ゲタ調整用
#define PARAM1_1 0
#define PARAM1_2 0
#define PARAM1_3 0
#define PARAM1_4 0
#define PARAM1_5 0
#define PARAM1_6 0
#define PARAM1_7 0
#define PARAM1_8 0
#define PARAM1_9 0
#define PARAM2_1 0
#define PARAM2_2 0
#define PARAM2_3 0
#define PARAM2_4 0
#define PARAM2_5 0
#define PARAM2_6 0
//想定縮小画像サイズ
#define SUMPIX 19200
#define SUMPIX_HALF 9600
//判定結果重み付け
#define OMOMI_PIX1 400
#define OMOMI_MAX1 6
#define OMOMI_MIN1 2
#define OMOMI_PIX2 2000
#define OMOMI_MAX2 4
#define OMOMI_MIN2 0
#define OMOMI_PIX3 100
#define OMOMI_PIX4 100
#define OMOMI_PIX5 100
