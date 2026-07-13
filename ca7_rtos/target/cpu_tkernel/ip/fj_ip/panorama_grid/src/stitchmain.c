#include "stitchmain.h"
#include "stitchmain_dist.h"
#include "grid.h"
#include "image.h"
#include "affine.h"
#include "camera.h"
#include "coordsys.h"
#include "dist.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include "param.h"
#include "stitch.h"
#include "mem.h"
#include "accel2pose.h"

#if defined(ARM_TRIAL_TRIGONO_TABLE)
#include "mathtbl.h"
MathSin  *mathSin;
MathCos  *mathCos;
MathAsin *mathAsin;
MathAcos *mathAcos;
MathAtan *mathAtan;
MathSqrt *mathSqrt;
#endif

void Stitch_Main(int stitch_width, Stitch *stitch, StitchGrids *stitchin0, StitchGrids *stitchin1)
{
#if defined(ARM_TRIAL_TRIGONO_TABLE)
    mathSin  = malloc(sizeof(MathSin ));
    mathCos  = malloc(sizeof(MathCos ));
    mathAsin = malloc(sizeof(MathAsin));
    mathAcos = malloc(sizeof(MathAcos));
    mathAtan = malloc(sizeof(MathAtan));
    mathSqrt = malloc(sizeof(MathSqrt));

    mathSinInit (mathSin);
    mathCosInit (mathCos);
    mathAsinInit(mathAsin);
    mathAcosInit(mathAcos);
    mathAtanInit(mathAtan);
    mathSqrtInit(mathSqrt);
#endif

    int stitchmargin = STITCH_MARGIN;    // 228 pixels
    int stitchwidth  = stitch_width;     // 320 pixels = fov 30 degs for 3840x1920 output
                                         //  64 pixels = fov  6 degs for 3840x1920 output
    int cellw = 64;
    int cellh = 32;

    int cellingroupw = 4;
    int cellingrouph = 4;

    int imageinw = 1920;
    int imageinh = 1920;

    int imageoutw = 3840;
    int imageouth = 1920;

    XXXX_FLOAT  center0x = 958;
    XXXX_FLOAT  center0y = 956;

    XXXX_FLOAT  center1x = 964;
    XXXX_FLOAT  center1y = 954;

    XXXX_FLOAT  dist0scalepixel = 844;
    XXXX_FLOAT  dist0scalesensor = 1.570796f;
    XXXX_FLOAT  sensor2pix0 = dist0scalepixel / dist0scalesensor;
    XXXX_FLOAT  dist1scalepixel = dist0scalepixel;
//  XXXX_FLOAT  dist1scalesensor = dist0scalesensor;
    XXXX_FLOAT  sensor2pix1 = sensor2pix0;

    XXXX_FLOAT  *dist0in  = gdist0in;
    XXXX_FLOAT  *dist0out = gdist0out;
    XXXX_FLOAT  *dist1in  = gdist1in;
    XXXX_FLOAT  *dist1out = gdist1out;
    int         dist0len = DIST_LENGTH;
    int         dist1len = dist0len;

#if defined(ARM_TRIAL_GRID_SPARSE_X1_Y2)
    int gridstepx = 1;
    int gridstepy = 2;
#elif defined(ARM_TRIAL_GRID_SPARSE_X2_Y2)
    int gridstepx = 2;
    int gridstepy = 2;
#else
    int gridstepx = 1;
    int gridstepy = 1;
#endif

    static StitchParam param;

    param.dstx          = 0;
    param.dsty          = 0;
    param.dstw          = imageoutw;
    param.dsth          = imageouth;

    param.dstobjdistance    = 3000;

    param.cellw         = cellw;
    param.cellh         = cellh;
    param.cellingroupw  = cellingroupw;
    param.cellingrouph  = cellingrouph;
    param.gridstepx     = gridstepx;
    param.gridstepy     = gridstepy;
    param.stitchw       = stitchwidth;
    param.stitchmargin  = stitchmargin;

    {
        int w = imageinw;
        int h = imageinh;
        XXXX_FLOAT centerx = center0x;
        XXXX_FLOAT centery = center0y;
        XXXX_FLOAT scalepixw = dist0scalepixel;
        XXXX_FLOAT scaledegw = RAD2DEG(dist0scalepixel);

        ParamCamera_ProjType projtype = ParamCamera_ProjType_Lens;
        ParamDist paramdist;
        ParamDist_Init(&paramdist, dist0in, DEG2RAD(1), dist0out, sensor2pix0, dist0len);

        PoseAxis front = PoseAxis_X;
        Affine poseinworld;
        Affine_Init_Identity(&poseinworld);

        ParamCamera_Init(&param.camerasrc[0], w, h, centerx, centery, scalepixw, scaledegw, projtype, &paramdist, front, &poseinworld);
    }

    {
        int w = imageinw;
        int h = imageinh;
        XXXX_FLOAT centerx = center1x;
        XXXX_FLOAT centery = center1y;
        XXXX_FLOAT scalepixw = dist1scalepixel;
        XXXX_FLOAT scaledegw = RAD2DEG(dist1scalepixel);

        ParamCamera_ProjType projtype = ParamCamera_ProjType_Lens;
        ParamDist paramdist;
        ParamDist_Init(&paramdist, dist1in, DEG2RAD(1), dist1out, sensor2pix1, dist1len);

        PoseAxis front = param.camerasrc[0].front;
        Affine poseinworld;
        Affine_Init_RotZ(&poseinworld, DEG2RAD(180), 0, 0, 0);

        ParamCamera_Init(&param.camerasrc[1], w, h, centerx, centery, scalepixw, scaledegw, projtype, &paramdist, front, &poseinworld);
    }

    {
        int w = imageoutw;
        int h = imageouth;
        XXXX_FLOAT centerx = imageoutw / 2.0f;
        XXXX_FLOAT centery = imageouth / 2.0f;
        XXXX_FLOAT scalepixw = (XXXX_FLOAT)imageoutw;
        XXXX_FLOAT scaledegw = 360.0f;

        ParamCamera_ProjType projtype = ParamCamera_ProjType_Equirectangular;

        PoseAxis front = param.camerasrc[0].front;  // front = same as camerasrc0
        Affine poseinworld;                         // orientation = same as camerasrc0
        Vec3 t;                                     // location = midpoint of camerasrc0 and camerasrc1
        Vec3_Average(&param.camerasrc[0].poseinworld.t, &param.camerasrc[1].poseinworld.t, &t);
        Affine_CopyMatVec(&poseinworld, &param.camerasrc[0].poseinworld.m, &t);

        ParamCamera_Init(&param.cameradst, w, h, centerx, centery, scalepixw, scaledegw, projtype, NULL, front, &poseinworld);
    }

    Stitch_Init(stitch, &param);
    Stitch_Compute_StitchIn(stitch, stitchin0, stitchin1);
#if 0
	for ( int i = 0; i < stitchin0->count; i++ ) {
		printf( "width      = %d\n", stitchin0->grid[i].nodew );
		printf( "height     = %d\n", stitchin0->grid[i].nodeh );
		printf( "cellWidth  = %d\n", stitchin0->grid[i].cellw );
		printf( "cellHeight = %d\n", stitchin0->grid[i].cellh );
		printf( "outputX    = %d\n", stitchin0->grid[i].outputx );
		printf( "outputY    = %d\n", stitchin0->grid[i].outputy );
		printf( "nodes = (\n" );
		for ( int j = 0; j < 25; j++ ) {
			printf( "    ( %f, %f )\n", (stitchin0->grid[i].nodes+j*4)->x, (stitchin0->grid[i].nodes+j*4)->y );
		}
		printf( ");\n" );
	}

	for ( int i = 0; i < stitchin1->count; i++ ) {
		printf( "width      = %d\n", stitchin1->grid[i].nodew );
		printf( "height     = %d\n", stitchin1->grid[i].nodeh );
		printf( "cellWidth  = %d\n", stitchin1->grid[i].cellw );
		printf( "cellHeight = %d\n", stitchin1->grid[i].cellh );
		printf( "outputX    = %d\n", stitchin1->grid[i].outputx );
		printf( "outputY    = %d\n", stitchin1->grid[i].outputy );
		printf( "nodes = (\n" );
		for ( int j = 0; j < 25; j++ ) {
			printf( "    ( %f, %f )\n", (stitchin1->grid[i].nodes+j*4)->x, (stitchin1->grid[i].nodes+j*4)->y );
		}
		printf( ");\n" );
	}
#endif

#if defined(ARM_TRIAL_TRIGONO_TABLE)
    free(mathSin );
    free(mathCos );
    free(mathAsin);
    free(mathAcos);
    free(mathAtan);
    free(mathSqrt);
#endif
}

void Stitch_Final(float ax, float ay, float az, float ax0, float ay0, float az0, Stitch *stitch, StitchGrids *stitchfinal)
{
#if 0
    // calculate pose from gyro information
    Affine poseinworld;
    static int rotdeg = 0;
    XXXX_FLOAT rotrad = DEG2RAD(rotdeg++);
    Affine_Init_RotX(&poseinworld, rotrad, 0, 0, 0);
#else
    Affine poseinworld;
//  Affine_Init_Identity(&poseinworld);
    accel2pose( ax, ay, az, ax0, ay0, az0, poseinworld.m.e );
    poseinworld.t.v[0] = 0.0f;
    poseinworld.t.v[1] = 0.0f;
    poseinworld.t.v[2] = 0.0f;
#endif

    Stitch_Compute_StitchFinal(stitch, &poseinworld, stitchfinal);

#if 0
	printf( "poseinworld.m.e[0][0] = %f\n", poseinworld.m.e[0][0] );
	printf( "poseinworld.m.e[0][1] = %f\n", poseinworld.m.e[0][1] );
	printf( "poseinworld.m.e[0][2] = %f\n", poseinworld.m.e[0][2] );
	printf( "poseinworld.m.e[1][0] = %f\n", poseinworld.m.e[1][0] );
	printf( "poseinworld.m.e[1][1] = %f\n", poseinworld.m.e[1][1] );
	printf( "poseinworld.m.e[1][2] = %f\n", poseinworld.m.e[1][2] );
	printf( "poseinworld.m.e[2][0] = %f\n", poseinworld.m.e[2][0] );
	printf( "poseinworld.m.e[2][1] = %f\n", poseinworld.m.e[2][1] );
	printf( "poseinworld.m.e[2][2] = %f\n\n", poseinworld.m.e[2][2] );
	printf( "poseinworld.t.v[0] = %f\n", poseinworld.t.v[0] );
	printf( "poseinworld.t.v[1] = %f\n", poseinworld.t.v[1] );
	printf( "poseinworld.t.v[2] = %f\n\n", poseinworld.t.v[2] );
#endif

#if 0
	for ( int i = 0; i < stitchfinal->count; i++ ) {
		printf( "width      = %d\n", stitchfinal->grid[i].nodew );
		printf( "height     = %d\n", stitchfinal->grid[i].nodeh );
		printf( "cellWidth  = %d\n", stitchfinal->grid[i].cellw );
		printf( "cellHeight = %d\n", stitchfinal->grid[i].cellh );
		printf( "outputX    = %d\n", stitchfinal->grid[i].outputx );
		printf( "outputY    = %d\n", stitchfinal->grid[i].outputy );
		printf( "nodes = (\n" );
		for ( int j = 0; j < 25; j++ ) {
			printf( "    ( %f, %f )\n", (stitchfinal->grid[i].nodes+j*4)->x, (stitchfinal->grid[i].nodes+j*4)->y );
		}
		printf( ");\n" );
	}
#endif
}
