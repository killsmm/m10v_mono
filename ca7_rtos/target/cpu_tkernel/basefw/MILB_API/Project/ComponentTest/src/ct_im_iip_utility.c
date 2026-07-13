/**
 * @file		ct_im_iip_utility.c
 * @brief		IIP Utility ct code
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

// CT header
#include "ct_im_iip_utility.h"
#include "ct_im_iip.h"

// im_iip header
#include "im_iip.h"


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#ifndef CO_CT_IM_IIP_DISABLE

#if 1
#include <stdio.h>
#undef Ddim_Print
#define Ddim_Print(arg)		{DDIM_User_Printf arg;}		/**< printf function inside DDIM */
#endif



/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special


/*----------------------------------------------------------------------*/
/* Local Method															*/
/*----------------------------------------------------------------------*/

#ifndef CO_CT_IM_IIP_14_3_1_DISABLE_LUT
static const USHORT g_ct_im_iip_lutram_a_8bit[D_IM_IIP_LUT_MAXNUM] = {
	 255,  253,  251,  249,  247,  245,  243,  241,  239,  237,  235,  233,  231,  229,  227,  225,	// 0
	 223,  221,  219,  217,  215,  213,  211,  209,  207,  205,  203,  201,  199,  197,  195,  193,	// 32
	 191,  189,  187,  185,  183,  181,  179,  177,  175,  173,  171,  169,  167,  165,  163,  161,	// 64
	 159,  157,  155,  153,  151,  149,  147,  145,  143,  141,  139,  137,  135,  133,  131,  129,	// 96
	 127,  125,  123,  121,  119,  117,  115,  113,  111,  109,  107,  105,  103,  101,   99,   97,	// 128
	  95,   93,   91,   89,   87,   85,   83,   81,   79,   77,   75,   73,   71,   69,   67,   65,	// 160
	  63,   61,   59,   57,   55,   53,   51,   49,   47,   45,   43,   41,   39,   37,   35,   33,	// 192
	  31,   29,   27,   25,   23,   21,   19,   17,   15,   13,   11,    9,    7,    5,    3,    1,	// 224
};
static const USHORT g_ct_im_iip_lutram_d_8bit[D_IM_IIP_LUT_MAXNUM] = {
	 254,  252,  250,  248,  246,  244,  242,  240,  238,  236,  234,  232,  230,  228,  226,  224,	// 1
	 222,  220,  218,  216,  214,  212,  210,  208,  206,  204,  202,  200,  198,  196,  194,  192,	// 33
	 190,  188,  186,  184,  182,  180,  178,  176,  174,  172,  170,  168,  166,  164,  162,  160,	// 65
	 158,  156,  154,  152,  150,  148,  146,  144,  142,  140,  138,  136,  134,  132,  130,  128,	// 97
	 126,  124,  122,  120,  118,  116,  114,  112,  110,  108,  106,  104,  102,  100,   98,   96,	// 129
	  94,   92,   90,   88,   86,   84,   82,   80,   78,   76,   74,   72,   70,   68,   66,   64,	// 161
	  62,   60,   58,   56,   54,   52,   50,   48,   46,   44,   42,   40,   38,   36,   34,   32,	// 193
	  30,   28,   26,   24,   22,   20,   18,   16,   14,   12,   10,    8,    6,    4,    2,    0,	// 225
};
#endif //!CO_CT_IM_IIP_14_3_2_DISABLE_LUT

#ifndef CO_CT_IM_IIP_14_3_2_DISABLE_LUT
static const USHORT g_ct_im_iip_lutram_a_11bit[D_IM_IIP_LUT_MAXNUM] = {
	4095, 4090, 4086, 4082, 4078, 4074, 4070, 4066, 4062, 4058, 4054, 4050, 4046, 4042, 4038, 4034,	// 0
	4030, 4026, 4022, 4018, 4014, 4010, 4006, 4002, 3998, 3994, 3990, 3986, 3982, 3978, 3974, 3970,	// 32
	3966, 3962, 3958, 3954, 3950, 3946, 3942, 3938, 3934, 3930, 3926, 3922, 3918, 3914, 3910, 3906,	// 64
	3902, 3898, 3894, 3890, 3886, 3882, 3878, 3874, 3870, 3866, 3862, 3858, 3854, 3850, 3846, 3842,	// 96
	3838, 3834, 3830, 3826, 3822, 3818, 3814, 3810, 3806, 3802, 3798, 3794, 3790, 3786, 3782, 3778,	// 128
	3774, 3770, 3766, 3762, 3758, 3754, 3750, 3746, 3742, 3738, 3734, 3730, 3726, 3722, 3718, 3714,	// 160
	3710, 3706, 3702, 3698, 3694, 3690, 3686, 3682, 3678, 3674, 3670, 3666, 3662, 3658, 3654, 3650,	// 192
	3646, 3642, 3638, 3634, 3630, 3626, 3622, 3618, 3614, 3610, 3606, 3602, 3598, 3594, 3590, 3586,	// 224
	3582, 3578, 3574, 3570, 3566, 3562, 3558, 3554, 3550, 3546, 3542, 3538, 3534, 3530, 3526, 3522,	// 256
	3518, 3514, 3510, 3506, 3502, 3498, 3494, 3490, 3486, 3482, 3478, 3474, 3470, 3466, 3462, 3458,	// 288
	3454, 3450, 3446, 3442, 3438, 3434, 3430, 3426, 3422, 3418, 3414, 3410, 3406, 3402, 3398, 3394,	// 320
	3390, 3386, 3382, 3378, 3374, 3370, 3366, 3362, 3358, 3354, 3350, 3346, 3342, 3338, 3334, 3330,	// 352
	3326, 3322, 3318, 3314, 3310, 3306, 3302, 3298, 3294, 3290, 3286, 3282, 3278, 3274, 3270, 3266,	// 384
	3262, 3258, 3254, 3250, 3246, 3242, 3238, 3234, 3230, 3226, 3222, 3218, 3214, 3210, 3206, 3202,	// 416
	3198, 3194, 3190, 3186, 3182, 3178, 3174, 3170, 3166, 3162, 3158, 3154, 3150, 3146, 3142, 3138,	// 448
	3134, 3130, 3126, 3122, 3118, 3114, 3110, 3106, 3102, 3098, 3094, 3090, 3086, 3082, 3078, 3074,	// 480
	3070, 3066, 3062, 3058, 3054, 3050, 3046, 3042, 3038, 3034, 3030, 3026, 3022, 3018, 3014, 3010,	// 512
	3006, 3002, 2998, 2994, 2990, 2986, 2982, 2978, 2974, 2970, 2966, 2962, 2958, 2954, 2950, 2946,	// 544
	2942, 2938, 2934, 2930, 2926, 2922, 2918, 2914, 2910, 2906, 2902, 2898, 2894, 2890, 2886, 2882,	// 576
	2878, 2874, 2870, 2866, 2862, 2858, 2854, 2850, 2846, 2842, 2838, 2834, 2830, 2826, 2822, 2818,	// 608
	2814, 2810, 2806, 2802, 2798, 2794, 2790, 2786, 2782, 2778, 2774, 2770, 2766, 2762, 2758, 2754,	// 640
	2750, 2746, 2742, 2738, 2734, 2730, 2726, 2722, 2718, 2714, 2710, 2706, 2702, 2698, 2694, 2690,	// 672
	2686, 2682, 2678, 2674, 2670, 2666, 2662, 2658, 2654, 2650, 2646, 2642, 2638, 2634, 2630, 2626,	// 704
	2622, 2618, 2614, 2610, 2606, 2602, 2598, 2594, 2590, 2586, 2582, 2578, 2574, 2570, 2566, 2562,	// 736
	2558, 2554, 2550, 2546, 2542, 2538, 2534, 2530, 2526, 2522, 2518, 2514, 2510, 2506, 2502, 2498,	// 768
	2494, 2490, 2486, 2482, 2478, 2474, 2470, 2466, 2462, 2458, 2454, 2450, 2446, 2442, 2438, 2434,	// 800
	2430, 2426, 2422, 2418, 2414, 2410, 2406, 2402, 2398, 2394, 2390, 2386, 2382, 2378, 2374, 2370,	// 832
	2366, 2362, 2358, 2354, 2350, 2346, 2342, 2338, 2334, 2330, 2326, 2322, 2318, 2314, 2310, 2306,	// 864
	2302, 2298, 2294, 2290, 2286, 2282, 2278, 2274, 2270, 2266, 2262, 2258, 2254, 2250, 2246, 2242,	// 896
	2238, 2234, 2230, 2226, 2222, 2218, 2214, 2210, 2206, 2202, 2198, 2194, 2190, 2186, 2182, 2178,	// 928
	2174, 2170, 2166, 2162, 2158, 2154, 2150, 2146, 2142, 2138, 2134, 2130, 2126, 2122, 2118, 2114,	// 960
	2110, 2106, 2102, 2098, 2094, 2090, 2086, 2082, 2078, 2074, 2070, 2066, 2062, 2058, 2054, 2050,	// 992
	2046, 2042, 2038, 2034, 2030, 2026, 2022, 2018, 2014, 2010, 2006, 2002, 1998, 1994, 1990, 1986,	// 1024
	1982, 1978, 1974, 1970, 1966, 1962, 1958, 1954, 1950, 1946, 1942, 1938, 1934, 1930, 1926, 1922,	// 1056
	1918, 1914, 1910, 1906, 1902, 1898, 1894, 1890, 1886, 1882, 1878, 1874, 1870, 1866, 1862, 1858,	// 1088
	1854, 1850, 1846, 1842, 1838, 1834, 1830, 1826, 1822, 1818, 1814, 1810, 1806, 1802, 1798, 1794,	// 1120
	1790, 1786, 1782, 1778, 1774, 1770, 1766, 1762, 1758, 1754, 1750, 1746, 1742, 1738, 1734, 1730,	// 1152
	1726, 1722, 1718, 1714, 1710, 1706, 1702, 1698, 1694, 1690, 1686, 1682, 1678, 1674, 1670, 1666,	// 1184
	1662, 1658, 1654, 1650, 1646, 1642, 1638, 1634, 1630, 1626, 1622, 1618, 1614, 1610, 1606, 1602,	// 1216
	1598, 1594, 1590, 1586, 1582, 1578, 1574, 1570, 1566, 1562, 1558, 1554, 1550, 1546, 1542, 1538,	// 1248
	1534, 1530, 1526, 1522, 1518, 1514, 1510, 1506, 1502, 1498, 1494, 1490, 1486, 1482, 1478, 1474,	// 1280
	1470, 1466, 1462, 1458, 1454, 1450, 1446, 1442, 1438, 1434, 1430, 1426, 1422, 1418, 1414, 1410,	// 1312
	1406, 1402, 1398, 1394, 1390, 1386, 1382, 1378, 1374, 1370, 1366, 1362, 1358, 1354, 1350, 1346,	// 1344
	1342, 1338, 1334, 1330, 1326, 1322, 1318, 1314, 1310, 1306, 1302, 1298, 1294, 1290, 1286, 1282,	// 1376
	1278, 1274, 1270, 1266, 1262, 1258, 1254, 1250, 1246, 1242, 1238, 1234, 1230, 1226, 1222, 1218,	// 1408
	1214, 1210, 1206, 1202, 1198, 1194, 1190, 1186, 1182, 1178, 1174, 1170, 1166, 1162, 1158, 1154,	// 1440
	1150, 1146, 1142, 1138, 1134, 1130, 1126, 1122, 1118, 1114, 1110, 1106, 1102, 1098, 1094, 1090,	// 1472
	1086, 1082, 1078, 1074, 1070, 1066, 1062, 1058, 1054, 1050, 1046, 1042, 1038, 1034, 1030, 1026,	// 1504
	1022, 1018, 1014, 1010, 1006, 1002,  998,  994,  990,  986,  982,  978,  974,  970,  966,  962,	// 1536
	 958,  954,  950,  946,  942,  938,  934,  930,  926,  922,  918,  914,  910,  906,  902,  898,	// 1568
	 894,  890,  886,  882,  878,  874,  870,  866,  862,  858,  854,  850,  846,  842,  838,  834,	// 1600
	 830,  826,  822,  818,  814,  810,  806,  802,  798,  794,  790,  786,  782,  778,  774,  770,	// 1632
	 766,  762,  758,  754,  750,  746,  742,  738,  734,  730,  726,  722,  718,  714,  710,  706,	// 1664
	 702,  698,  694,  690,  686,  682,  678,  674,  670,  666,  662,  658,  654,  650,  646,  642,	// 1696
	 638,  634,  630,  626,  622,  618,  614,  610,  606,  602,  598,  594,  590,  586,  582,  578,	// 1728
	 574,  570,  566,  562,  558,  554,  550,  546,  542,  538,  534,  530,  526,  522,  518,  514,	// 1760
	 510,  506,  502,  498,  494,  490,  486,  482,  478,  474,  470,  466,  462,  458,  454,  450,	// 1792
	 446,  442,  438,  434,  430,  426,  422,  418,  414,  410,  406,  402,  398,  394,  390,  386,	// 1824
	 382,  378,  374,  370,  366,  362,  358,  354,  350,  346,  342,  338,  334,  330,  326,  322,	// 1856
	 318,  314,  310,  306,  302,  298,  294,  290,  286,  282,  278,  274,  270,  266,  262,  258,	// 1888
	 254,  250,  246,  242,  238,  234,  230,  226,  222,  218,  214,  210,  206,  202,  198,  194,	// 1920
	 190,  186,  182,  178,  174,  170,  166,  162,  158,  154,  150,  146,  142,  138,  134,  130,	// 1952
	 126,  122,  118,  114,  110,  106,  102,   98,   94,   90,   86,   82,   78,   74,   70,   66,	// 1984
	  62,   58,   54,   50,   46,   42,   38,   34,   30,   26,   22,   18,   14,   10,    6,    2,	// 2016
};
static const USHORT g_ct_im_iip_lutram_d_11bit[D_IM_IIP_LUT_MAXNUM] = {
	4092, 4088, 4084, 4080, 4076, 4072, 4068, 4064, 4060, 4056, 4052, 4048, 4044, 4040, 4036, 4032,	// 1
	4028, 4024, 4020, 4016, 4012, 4008, 4004, 4000, 3996, 3992, 3988, 3984, 3980, 3976, 3972, 3968,	// 33
	3964, 3960, 3956, 3952, 3948, 3944, 3940, 3936, 3932, 3928, 3924, 3920, 3916, 3912, 3908, 3904,	// 65
	3900, 3896, 3892, 3888, 3884, 3880, 3876, 3872, 3868, 3864, 3860, 3856, 3852, 3848, 3844, 3840,	// 97
	3836, 3832, 3828, 3824, 3820, 3816, 3812, 3808, 3804, 3800, 3796, 3792, 3788, 3784, 3780, 3776,	// 129
	3772, 3768, 3764, 3760, 3756, 3752, 3748, 3744, 3740, 3736, 3732, 3728, 3724, 3720, 3716, 3712,	// 161
	3708, 3704, 3700, 3696, 3692, 3688, 3684, 3680, 3676, 3672, 3668, 3664, 3660, 3656, 3652, 3648,	// 193
	3644, 3640, 3636, 3632, 3628, 3624, 3620, 3616, 3612, 3608, 3604, 3600, 3596, 3592, 3588, 3584,	// 225
	3580, 3576, 3572, 3568, 3564, 3560, 3556, 3552, 3548, 3544, 3540, 3536, 3532, 3528, 3524, 3520,	// 257
	3516, 3512, 3508, 3504, 3500, 3496, 3492, 3488, 3484, 3480, 3476, 3472, 3468, 3464, 3460, 3456,	// 289
	3452, 3448, 3444, 3440, 3436, 3432, 3428, 3424, 3420, 3416, 3412, 3408, 3404, 3400, 3396, 3392,	// 321
	3388, 3384, 3380, 3376, 3372, 3368, 3364, 3360, 3356, 3352, 3348, 3344, 3340, 3336, 3332, 3328,	// 353
	3324, 3320, 3316, 3312, 3308, 3304, 3300, 3296, 3292, 3288, 3284, 3280, 3276, 3272, 3268, 3264,	// 385
	3260, 3256, 3252, 3248, 3244, 3240, 3236, 3232, 3228, 3224, 3220, 3216, 3212, 3208, 3204, 3200,	// 417
	3196, 3192, 3188, 3184, 3180, 3176, 3172, 3168, 3164, 3160, 3156, 3152, 3148, 3144, 3140, 3136,	// 449
	3132, 3128, 3124, 3120, 3116, 3112, 3108, 3104, 3100, 3096, 3092, 3088, 3084, 3080, 3076, 3072,	// 481
	3068, 3064, 3060, 3056, 3052, 3048, 3044, 3040, 3036, 3032, 3028, 3024, 3020, 3016, 3012, 3008,	// 513
	3004, 3000, 2996, 2992, 2988, 2984, 2980, 2976, 2972, 2968, 2964, 2960, 2956, 2952, 2948, 2944,	// 545
	2940, 2936, 2932, 2928, 2924, 2920, 2916, 2912, 2908, 2904, 2900, 2896, 2892, 2888, 2884, 2880,	// 577
	2876, 2872, 2868, 2864, 2860, 2856, 2852, 2848, 2844, 2840, 2836, 2832, 2828, 2824, 2820, 2816,	// 609
	2812, 2808, 2804, 2800, 2796, 2792, 2788, 2784, 2780, 2776, 2772, 2768, 2764, 2760, 2756, 2752,	// 641
	2748, 2744, 2740, 2736, 2732, 2728, 2724, 2720, 2716, 2712, 2708, 2704, 2700, 2696, 2692, 2688,	// 673
	2684, 2680, 2676, 2672, 2668, 2664, 2660, 2656, 2652, 2648, 2644, 2640, 2636, 2632, 2628, 2624,	// 705
	2620, 2616, 2612, 2608, 2604, 2600, 2596, 2592, 2588, 2584, 2580, 2576, 2572, 2568, 2564, 2560,	// 737
	2556, 2552, 2548, 2544, 2540, 2536, 2532, 2528, 2524, 2520, 2516, 2512, 2508, 2504, 2500, 2496,	// 769
	2492, 2488, 2484, 2480, 2476, 2472, 2468, 2464, 2460, 2456, 2452, 2448, 2444, 2440, 2436, 2432,	// 801
	2428, 2424, 2420, 2416, 2412, 2408, 2404, 2400, 2396, 2392, 2388, 2384, 2380, 2376, 2372, 2368,	// 833
	2364, 2360, 2356, 2352, 2348, 2344, 2340, 2336, 2332, 2328, 2324, 2320, 2316, 2312, 2308, 2304,	// 865
	2300, 2296, 2292, 2288, 2284, 2280, 2276, 2272, 2268, 2264, 2260, 2256, 2252, 2248, 2244, 2240,	// 897
	2236, 2232, 2228, 2224, 2220, 2216, 2212, 2208, 2204, 2200, 2196, 2192, 2188, 2184, 2180, 2176,	// 929
	2172, 2168, 2164, 2160, 2156, 2152, 2148, 2144, 2140, 2136, 2132, 2128, 2124, 2120, 2116, 2112,	// 961
	2108, 2104, 2100, 2096, 2092, 2088, 2084, 2080, 2076, 2072, 2068, 2064, 2060, 2056, 2052, 2048,	// 993
	2044, 2040, 2036, 2032, 2028, 2024, 2020, 2016, 2012, 2008, 2004, 2000, 1996, 1992, 1988, 1984,	// 1025
	1980, 1976, 1972, 1968, 1964, 1960, 1956, 1952, 1948, 1944, 1940, 1936, 1932, 1928, 1924, 1920,	// 1057
	1916, 1912, 1908, 1904, 1900, 1896, 1892, 1888, 1884, 1880, 1876, 1872, 1868, 1864, 1860, 1856,	// 1089
	1852, 1848, 1844, 1840, 1836, 1832, 1828, 1824, 1820, 1816, 1812, 1808, 1804, 1800, 1796, 1792,	// 1121
	1788, 1784, 1780, 1776, 1772, 1768, 1764, 1760, 1756, 1752, 1748, 1744, 1740, 1736, 1732, 1728,	// 1153
	1724, 1720, 1716, 1712, 1708, 1704, 1700, 1696, 1692, 1688, 1684, 1680, 1676, 1672, 1668, 1664,	// 1185
	1660, 1656, 1652, 1648, 1644, 1640, 1636, 1632, 1628, 1624, 1620, 1616, 1612, 1608, 1604, 1600,	// 1217
	1596, 1592, 1588, 1584, 1580, 1576, 1572, 1568, 1564, 1560, 1556, 1552, 1548, 1544, 1540, 1536,	// 1249
	1532, 1528, 1524, 1520, 1516, 1512, 1508, 1504, 1500, 1496, 1492, 1488, 1484, 1480, 1476, 1472,	// 1281
	1468, 1464, 1460, 1456, 1452, 1448, 1444, 1440, 1436, 1432, 1428, 1424, 1420, 1416, 1412, 1408,	// 1313
	1404, 1400, 1396, 1392, 1388, 1384, 1380, 1376, 1372, 1368, 1364, 1360, 1356, 1352, 1348, 1344,	// 1345
	1340, 1336, 1332, 1328, 1324, 1320, 1316, 1312, 1308, 1304, 1300, 1296, 1292, 1288, 1284, 1280,	// 1377
	1276, 1272, 1268, 1264, 1260, 1256, 1252, 1248, 1244, 1240, 1236, 1232, 1228, 1224, 1220, 1216,	// 1409
	1212, 1208, 1204, 1200, 1196, 1192, 1188, 1184, 1180, 1176, 1172, 1168, 1164, 1160, 1156, 1152,	// 1441
	1148, 1144, 1140, 1136, 1132, 1128, 1124, 1120, 1116, 1112, 1108, 1104, 1100, 1096, 1092, 1088,	// 1473
	1084, 1080, 1076, 1072, 1068, 1064, 1060, 1056, 1052, 1048, 1044, 1040, 1036, 1032, 1028, 1024,	// 1505
	1020, 1016, 1012, 1008, 1004, 1000,  996,  992,  988,  984,  980,  976,  972,  968,  964,  960,	// 1537
	 956,  952,  948,  944,  940,  936,  932,  928,  924,  920,  916,  912,  908,  904,  900,  896,	// 1569
	 892,  888,  884,  880,  876,  872,  868,  864,  860,  856,  852,  848,  844,  840,  836,  832,	// 1601
	 828,  824,  820,  816,  812,  808,  804,  800,  796,  792,  788,  784,  780,  776,  772,  768,	// 1633
	 764,  760,  756,  752,  748,  744,  740,  736,  732,  728,  724,  720,  716,  712,  708,  704,	// 1665
	 700,  696,  692,  688,  684,  680,  676,  672,  668,  664,  660,  656,  652,  648,  644,  640,	// 1697
	 636,  632,  628,  624,  620,  616,  612,  608,  604,  600,  596,  592,  588,  584,  580,  576,	// 1729
	 572,  568,  564,  560,  556,  552,  548,  544,  540,  536,  532,  528,  524,  520,  516,  512,	// 1761
	 508,  504,  500,  496,  492,  488,  484,  480,  476,  472,  468,  464,  460,  456,  452,  448,	// 1793
	 444,  440,  436,  432,  428,  424,  420,  416,  412,  408,  404,  400,  396,  392,  388,  384,	// 1825
	 380,  376,  372,  368,  364,  360,  356,  352,  348,  344,  340,  336,  332,  328,  324,  320,	// 1857
	 316,  312,  308,  304,  300,  296,  292,  288,  284,  280,  276,  272,  268,  264,  260,  256,	// 1889
	 252,  248,  244,  240,  236,  232,  228,  224,  220,  216,  212,  208,  204,  200,  196,  192,	// 1921
	 188,  184,  180,  176,  172,  168,  164,  160,  156,  152,  148,  144,  140,  136,  132,  128,	// 1953
	 124,  120,  116,  112,  108,  104,  100,   96,   92,   88,   84,   80,   76,   72,   68,   64,	// 1985
	  60,   56,   52,   48,   44,   40,   36,   32,   28,   24,   20,   16,   12,    8,    4,    0,	// 2017
};
#endif //!CO_CT_IM_IIP_14_3_2_DISABLE_LUT


/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/

// Im_IIP_Utility_ResizeRotate 14-1-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_1_1: "
INT32 CT_Im_IIP_14_1_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_RR iip_util_rr0 = {
		.afn_unitid = E_IM_IIP_UNIT_ID_AFN1,		// UNITID of AFN
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,			// UNITID of SL
		.src_pixid = E_IM_IIP_PIXID_1,				// Source image PIXID
		.dst_pixid = E_IM_IIP_PIXID_2,				// Destination image PIXID
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 320,							// Width
				.lines = 240,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC420,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = 320,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 240,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.afn_cache_select = E_IM_IIP_PARAM_CSEL_0,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,			// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_000,		// Angle of rotation
		.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR,// Resize mode
	};
	T_IM_IIP_UTIL_RR iip_util_rr1 = {
		.afn_unitid = E_IM_IIP_UNIT_ID_AFN0,		// UNITID of AFN
		.sl_unitid = E_IM_IIP_UNIT_ID_SL2,			// UNITID of SL
		.src_pixid = E_IM_IIP_PIXID_4,				// Source image PIXID
		.dst_pixid = E_IM_IIP_PIXID_5,				// Destination image PIXID
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_LINES,			// Width
				.lines = D_IM_IIP_VGA_WIDTH,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 480,
					.Cb_B  = 480,
					.Cr_R  = 480,
					.Alpha = 480,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.afn_cache_select = E_IM_IIP_PARAM_CSEL_0,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,			// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_090,		// Angle of rotation
		.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR,// Resize mode
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES *2];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 60;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_ResizeRotate( &iip_util_rr0,
										&iip_util_rr1,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Utility_ResizeRotate 14-1-2
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_1_2: "
INT32 CT_Im_IIP_14_1_2( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_RR iip_util_rr0 = {
		.afn_unitid = E_IM_IIP_UNIT_ID_AFN0,		// UNITID of AFN
		.sl_unitid = E_IM_IIP_UNIT_ID_SL2,			// UNITID of SL
		.src_pixid = E_IM_IIP_PIXID_4,				// Source image PIXID
		.dst_pixid = E_IM_IIP_PIXID_5,				// Destination image PIXID
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_LINES,			// Width
				.lines = D_IM_IIP_VGA_WIDTH,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 480,
					.Cb_B  = 480,
					.Cr_R  = 480,
					.Alpha = 480,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.afn_cache_select = E_IM_IIP_PARAM_CSEL_0,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,			// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_270,		// Angle of rotation
		.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR,// Resize mode
	};
	T_IM_IIP_UTIL_RR iip_util_rr1 = {
		.afn_unitid = E_IM_IIP_UNIT_ID_AFN1,		// UNITID of AFN
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,			// UNITID of SL
		.src_pixid = E_IM_IIP_PIXID_1,				// Source image PIXID
		.dst_pixid = E_IM_IIP_PIXID_2,				// Destination image PIXID
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_2_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.afn_cache_select = E_IM_IIP_PARAM_CSEL_0,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,			// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_180,		// Angle of rotation
		.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR,// Resize mode
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES *2];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 60;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_ResizeRotate( &iip_util_rr0,
										&iip_util_rr1,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Utility_ResizeRotate 14-1-3
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_1_3: "
INT32 CT_Im_IIP_14_1_3( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_RR iip_util_rr0 = {
		.afn_unitid = E_IM_IIP_UNIT_ID_AFN0,		// UNITID of AFN
		.sl_unitid = E_IM_IIP_UNIT_ID_SL2,			// UNITID of SL
		.src_pixid = E_IM_IIP_PIXID_4,				// Source image PIXID
		.dst_pixid = E_IM_IIP_PIXID_5,				// Destination image PIXID
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_HD_4_3_WIDTH,			// Width
				.lines = D_IM_IIP_HD_4_3_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_HD_4_3_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_HD_4_3_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_HD_4_3_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_HD_4_3_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_HD_4_3_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_HD_4_3_YCC422_U8_A_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_HD_4_3_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_1_HD_4_3_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_1_HD_4_3_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_HD_4_3_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.afn_cache_select = E_IM_IIP_PARAM_CSEL_0,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,			// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_000,		// Angle of rotation
		.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR,// Resize mode
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 60;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_ResizeRotate( &iip_util_rr0,
										NULL,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Utility_ResizeRotate 14-1-4
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_1_4: "
INT32 CT_Im_IIP_14_1_4( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_RR iip_util_rr0 = {
		.afn_unitid = E_IM_IIP_UNIT_ID_AFN1,		// UNITID of AFN
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,			// UNITID of SL
		.src_pixid = E_IM_IIP_PIXID_1,				// Source image PIXID
		.dst_pixid = E_IM_IIP_PIXID_2,				// Destination image PIXID
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U16_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U16_A_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U16_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U16_Y_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.afn_cache_select = E_IM_IIP_PARAM_CSEL_0,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_16BITS,		// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_000,		// Angle of rotation
		.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR,// Resize mode
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 60;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_ResizeRotate( &iip_util_rr0,
										NULL,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Utility_ResizeRotate 14-1-5
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_1_5: "
INT32 CT_Im_IIP_14_1_5( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_RR iip_util_rr0 = {
		.afn_unitid = E_IM_IIP_UNIT_ID_AFN1,		// UNITID of AFN
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,			// UNITID of SL
		.src_pixid = E_IM_IIP_PIXID_1,				// Source image PIXID
		.dst_pixid = E_IM_IIP_PIXID_2,				// Destination image PIXID
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH /2,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_CHUNKY,		// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_CBCR,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// dummy
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// dummy
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH /2,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 0,						// dummy
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH *2,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH *2,
					.Alpha = 0,						// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = (ULONG)NULL,			// dummy
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Alpha = (ULONG)NULL,			// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH /4,			// Width
				.lines = D_IM_IIP_VGA_LINES /2,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_CHUNKY,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_CBCR,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// dummy
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// dummy
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH /4,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES /2,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 0,						// dummy
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH *2,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH *2,
					.Alpha = 0,						// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = (ULONG)NULL,			// dummy
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C,
					.Alpha = (ULONG)NULL,			// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.afn_cache_select = E_IM_IIP_PARAM_CSEL_0,	// AFN cache select.
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,			// PIXFMTTBL.PDEPTH[2:0] <br><br> Depth of pixel.
		.rotation = E_IM_IIP_UTIL_ROTATE_000,		// Angle of rotation
		.resize_mode = E_IM_IIP_UTIL_RESIZE_MODE_BILINEAR,// Resize mode
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_RR_BUF_BYTES];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 60;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_ResizeRotate( &iip_util_rr0,
										NULL,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Utility_ResizeRotate 14-1-X
INT32 CT_Im_IIP_Run_14_1( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-14-1-1
		case 1:
			return CT_Im_IIP_14_1_1();

		// Test-14-1-2
		case 2:
			return CT_Im_IIP_14_1_2();

		// Test-14-1-3
		case 3:
			return CT_Im_IIP_14_1_3();

		// Test-14-1-4
		case 4:
			return CT_Im_IIP_14_1_4();

		// Test-14-1-5
		case 5:
			return CT_Im_IIP_14_1_5();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// Im_IIP_Utility_CSC 14-2-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_2_1: "
INT32 CT_Im_IIP_14_2_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_CSC iip_util_csc0 = {
		.ld_unitid = E_IM_IIP_UNIT_ID_LD0,
		.csc_unitid = E_IM_IIP_UNIT_ID_CSC1,
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,
		.src_pixid = E_IM_IIP_PIXID_1,
		.dst_pixid = E_IM_IIP_PIXID_2,
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_RGBA4444,		// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_RGBA4444_GLOBAL_WIDTH,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_RGBA4444,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_RGBA4444,		// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_RGBA4444_GLOBAL_WIDTH,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_RGBA4444,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select = E_IM_IIP_PARAM_CSEL_0,
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.csck = {
			//    G      B      R      A
			{ 0x000, 0x000, 0x400, 0x000 },		// G
			{ 0x400, 0x000, 0x000, 0x000 },		// B
			{ 0x000, 0x000, 0x000, 0x400 },		// R
			{ 0x000, 0x400, 0x000, 0x000 },		// A
		},
		.csck_mode_sel = 2,		// bit format: Sxx_xx.xx_xxxx_xxxx
		.alpha_in_sel = E_IM_IIP_PARAM_ALPSEL_DIRECT,
		.alpha_out_sel = E_IM_IIP_PARAM_ALOSEL_CALCULATED,
		.alpha_val = 0,	// dummy
		.sl_sftval = 0,
	};
	T_IM_IIP_UTIL_CSC iip_util_csc1 = {
		.ld_unitid = E_IM_IIP_UNIT_ID_LD1,
		.csc_unitid = E_IM_IIP_UNIT_ID_CSC0,
		.sl_unitid = E_IM_IIP_UNIT_ID_SL4,
		.src_pixid = E_IM_IIP_PIXID_3,
		.dst_pixid = E_IM_IIP_PIXID_4,
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_RGBA4444,		// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_RGBA4444_GLOBAL_WIDTH,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_RGBA4444,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_RGBA4444,		// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_RGBA4444_GLOBAL_WIDTH,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_2_RGBA4444,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select = E_IM_IIP_PARAM_CSEL_0,
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.csck = {
			//    G      B      R      A
			{ 0x400, 0x000, 0x000, 0x000 },		// G
			{ 0x000, 0x400, 0x000, 0x000 },		// B
			{ 0x000, 0x000, 0x400, 0x000 },		// R
			{ 0x000, 0x000, 0x000, 0x400 },		// A
		},
		.csck_mode_sel = 2,		// bit format: Sxx_xx.xx_xxxx_xxxx
		.alpha_in_sel = E_IM_IIP_PARAM_ALPSEL_DIRECT,
		.alpha_out_sel = E_IM_IIP_PARAM_ALOSEL_CALCULATED,
		.alpha_val = 0,	// dummy
		.sl_sftval = 0,
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_CSC_BUF_BYTES *2];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 30;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_CSC( &iip_util_csc0,
							   &iip_util_csc1,
							   param_master_IF,
							   param_buffer_addr,
							   wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Utility_CSC 14-2-2
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_2_2: "
INT32 CT_Im_IIP_14_2_2( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_CSC iip_util_csc0 = {
		.ld_unitid = E_IM_IIP_UNIT_ID_LD0,
		.csc_unitid = E_IM_IIP_UNIT_ID_CSC1,
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,
		.src_pixid = E_IM_IIP_PIXID_1,
		.dst_pixid = E_IM_IIP_PIXID_2,
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 320,							// Width
				.lines = 240,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_RGBA4444,		// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = 320,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 240,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 320 *2,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_RGBA4444,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = 320,							// Width
				.lines = 240,							// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_RGBA4444,		// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = 320,								// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = 240,								// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = 320 *2,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_RGBA4444,
					.Cb_B  = 0,	// dummy
					.Cr_R  = 0,	// dummy
					.Alpha = 0,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select = E_IM_IIP_PARAM_CSEL_0,
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.csck = {
			//    G      B      R      A
			{ 0x400, 0x000, 0x000, 0x400 },		// G
			{ 0x000, 0x400, 0x000, 0x400 },		// B
			{ 0x000, 0x000, 0x400, 0x400 },		// R
			{ 0x000, 0x000, 0x000, 0x000 },		// A
		},
		.csck_mode_sel = 2,		// bit format: Sxx_xx.xx_xxxx_xxxx
		.alpha_in_sel = E_IM_IIP_PARAM_ALPSEL_DIRECT,
		.alpha_out_sel = E_IM_IIP_PARAM_ALOSEL_CALCULATED,
		.alpha_val = 0,	// dummy
		.sl_sftval = 0,
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_CSC_BUF_BYTES];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 30;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_CSC( &iip_util_csc0,
							   NULL,
							   param_master_IF,
							   param_buffer_addr,
							   wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// Im_IIP_Utility_CSC 14-2-X
INT32 CT_Im_IIP_Run_14_2( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-14-2-1
		case 1:
			return CT_Im_IIP_14_2_1();

		// Test-14-2-2
		case 2:
			return CT_Im_IIP_14_2_2();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// Im_IIP_Utility_LUT 14-3-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_3_1: "
INT32 CT_Im_IIP_14_3_1( VOID )
{
#ifndef CO_CT_IM_IIP_14_3_1_DISABLE_LUT
	INT32							ercd;
	T_IM_IIP_UTIL_LUT iip_util_lut = {
		.ld_unitid = E_IM_IIP_UNIT_ID_LD0,
		.lut_unitid = E_IM_IIP_UNIT_ID_LUT,
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,
		.src_pixid = E_IM_IIP_PIXID_1,
		.dst_pixid = E_IM_IIP_PIXID_2,
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,			// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select = E_IM_IIP_PARAM_CSEL_0,
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.lut_pix_depth = {
			8, 8, 8, 8
		},
		.splbit = {
			0, 0, 0, 0
		},
		.lutram = {
			.buffer_bytes = {
				[E_IM_IIP_LUTRAM_TYPE_A] = sizeof(g_ct_im_iip_lutram_a_8bit),
				[E_IM_IIP_LUTRAM_TYPE_B] = sizeof(g_ct_im_iip_lutram_a_8bit),
				[E_IM_IIP_LUTRAM_TYPE_C] = sizeof(g_ct_im_iip_lutram_a_8bit),
				[E_IM_IIP_LUTRAM_TYPE_D] = sizeof(g_ct_im_iip_lutram_d_8bit),
				[E_IM_IIP_LUTRAM_TYPE_E] = sizeof(g_ct_im_iip_lutram_d_8bit),
				[E_IM_IIP_LUTRAM_TYPE_F] = sizeof(g_ct_im_iip_lutram_d_8bit),
			},
			.buffer_addr = {
				[E_IM_IIP_LUTRAM_TYPE_A] = g_ct_im_iip_lutram_a_8bit,
				[E_IM_IIP_LUTRAM_TYPE_B] = g_ct_im_iip_lutram_a_8bit,
				[E_IM_IIP_LUTRAM_TYPE_C] = g_ct_im_iip_lutram_a_8bit,
				[E_IM_IIP_LUTRAM_TYPE_D] = g_ct_im_iip_lutram_d_8bit,
				[E_IM_IIP_LUTRAM_TYPE_E] = g_ct_im_iip_lutram_d_8bit,
				[E_IM_IIP_LUTRAM_TYPE_F] = g_ct_im_iip_lutram_d_8bit,
			},
		},
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_LUT_BUF_BYTES];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 30;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_LUT( &iip_util_lut,
							   param_master_IF,
							   param_buffer_addr,
							   wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

#endif //!CO_CT_IM_IIP_14_3_1_DISABLE_LUT
	return D_DDIM_OK;
}


// Im_IIP_Utility_LUT 14-3-2
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_3_2: "
INT32 CT_Im_IIP_14_3_2( VOID )
{
#ifndef CO_CT_IM_IIP_14_3_2_DISABLE_LUT
	INT32							ercd;
	T_IM_IIP_UTIL_LUT iip_util_lut = {
		.ld_unitid = E_IM_IIP_UNIT_ID_LD0,
		.lut_unitid = E_IM_IIP_UNIT_ID_LUT,
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,
		.src_pixid = E_IM_IIP_PIXID_1,
		.dst_pixid = E_IM_IIP_PIXID_2,
		.src = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U12_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U12_CB_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U12_CR_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U12_A_GLOBAL_WIDTH,	// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U12_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U12_CB,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U12_CR,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U12_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U12_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U12_CB_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U12_CR_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U12_A_GLOBAL_WIDTH,	// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U12_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U12_CB,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U12_CR,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U12_A,	// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select = E_IM_IIP_PARAM_CSEL_0,
		.pix_depth = D_IM_IIP_PDEPTH_12BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.lut_pix_depth = {
			12, 12, 12, 12
		},
		.splbit = {
			1, 1, 1, 1
		},
		.lutram = {
			.buffer_bytes = {
				[E_IM_IIP_LUTRAM_TYPE_A] = sizeof(g_ct_im_iip_lutram_a_11bit),
				[E_IM_IIP_LUTRAM_TYPE_B] = sizeof(g_ct_im_iip_lutram_a_11bit),
				[E_IM_IIP_LUTRAM_TYPE_C] = sizeof(g_ct_im_iip_lutram_a_11bit),
				[E_IM_IIP_LUTRAM_TYPE_D] = sizeof(g_ct_im_iip_lutram_d_11bit),
				[E_IM_IIP_LUTRAM_TYPE_E] = sizeof(g_ct_im_iip_lutram_d_11bit),
				[E_IM_IIP_LUTRAM_TYPE_F] = sizeof(g_ct_im_iip_lutram_d_11bit),
			},
			.buffer_addr = {
				[E_IM_IIP_LUTRAM_TYPE_A] = g_ct_im_iip_lutram_a_11bit,
				[E_IM_IIP_LUTRAM_TYPE_B] = g_ct_im_iip_lutram_a_11bit,
				[E_IM_IIP_LUTRAM_TYPE_C] = g_ct_im_iip_lutram_a_11bit,
				[E_IM_IIP_LUTRAM_TYPE_D] = g_ct_im_iip_lutram_d_11bit,
				[E_IM_IIP_LUTRAM_TYPE_E] = g_ct_im_iip_lutram_d_11bit,
				[E_IM_IIP_LUTRAM_TYPE_F] = g_ct_im_iip_lutram_d_11bit,
			},
		},
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_LUT_BUF_BYTES];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 30;


	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_LUT( &iip_util_lut,
							   param_master_IF,
							   param_buffer_addr,
							   wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

#endif //!CO_CT_IM_IIP_14_3_2_DISABLE_LUT
	return D_DDIM_OK;
}


// Im_IIP_Utility_LUT 14-3-X
INT32 CT_Im_IIP_Run_14_3( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-14-3-1
		case 1:
			return CT_Im_IIP_14_3_1();

		// Test-14-3-2
		case 2:
			return CT_Im_IIP_14_3_2();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


// IM_IIP_Utility_AlphaBlend 14-4-1
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_4_1: "
INT32 CT_Im_IIP_14_4_1( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_ALPHABLEND iip_util_blend = {
		.ld_unitid[0] = E_IM_IIP_UNIT_ID_LD0,
		.ld_unitid[1] = E_IM_IIP_UNIT_ID_LD1,
		.blend_unitid = E_IM_IIP_UNIT_ID_BLEND0,
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,
		.src_pixid[0] = E_IM_IIP_PIXID_0,
		.src_pixid[1] = E_IM_IIP_PIXID_1,
		.dst_pixid = E_IM_IIP_PIXID_2,
		.src[0] = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U8_A,
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.src[1] = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U8_A,
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U8_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U8_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U8_A_GLOBAL_WIDTH,
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U8_A,
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select[0] = E_IM_IIP_PARAM_CSEL_0,
		.ld_cache_select[1] = E_IM_IIP_PARAM_CSEL_0,
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.alpha = {
			.step = E_IM_IIP_PARAM_ALPHA_STEP_8BITS,
			.sel = E_IM_IIP_PARAM_ALPHA_SEL_P0_ALPHA,
			.out = E_IM_IIP_PARAM_ALPHA_OUT_P0_ALPHA,
			.ALPHA_VAL[0] = 0,
			.ALPHA_VAL[1] = 0,
			.ALPHA_VAL[2] = 0,
			.ALPHA_VAL[3] = 0,
		},
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_ALPHABLEND_BUF_BYTES];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 30;

#if 0
//	iip_util_blend.src[0].gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_blend.src[1].gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_blend.dst.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
//	iip_util_blend.alpha.sel = E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL;
	iip_util_blend.alpha.sel = E_IM_IIP_PARAM_ALPHA_SEL_P0_ALPHA;
//	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_FIXED_VAL;
//	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_P0_ALPHA;
	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_NONE;
#endif

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_AlphaBlend( &iip_util_blend,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// IM_IIP_Utility_AlphaBlend 14-4-2
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_4_2: "
INT32 CT_Im_IIP_14_4_2( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_ALPHABLEND iip_util_blend = {
		.ld_unitid[0] = E_IM_IIP_UNIT_ID_LD0,
		.ld_unitid[1] = E_IM_IIP_UNIT_ID_LD1,
		.blend_unitid = E_IM_IIP_UNIT_ID_BLEND0,
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,
		.src_pixid[0] = E_IM_IIP_PIXID_0,
		.src_pixid[1] = E_IM_IIP_PIXID_1,
		.dst_pixid = E_IM_IIP_PIXID_2,
		.src[0] = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_CHUNKY,		// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC444,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_RGBA8888_GLOBAL_WIDTH,
					.Cb_B  = 0,		// dummy
					.Cr_R  = 0,		// dummy
					.Alpha = 0,		// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_RGBA8888,
					.Cb_B  = 0,		// dummy
					.Cr_R  = 0,		// dummy
					.Alpha = 0,		// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.src[1] = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_CHUNKY,		// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC444,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_RGBA8888_GLOBAL_WIDTH,
					.Cb_B  = 0,		// dummy
					.Cr_R  = 0,		// dummy
					.Alpha = 0,		// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_4_RGBA8888,
					.Cb_B  = 0,		// dummy
					.Cr_R  = 0,		// dummy
					.Alpha = 0,		// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_CHUNKY,		// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC444,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_RGBA8888_GLOBAL_WIDTH,
					.Cb_B  = 0,		// dummy
					.Cr_R  = 0,		// dummy
					.Alpha = 0,		// dummy
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_RGBA8888,
					.Cb_B  = 0,		// dummy
					.Cr_R  = 0,		// dummy
					.Alpha = 0,		// dummy
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select[0] = E_IM_IIP_PARAM_CSEL_0,
		.ld_cache_select[1] = E_IM_IIP_PARAM_CSEL_0,
		.pix_depth = D_IM_IIP_PDEPTH_8BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.alpha = {
			.step = E_IM_IIP_PARAM_ALPHA_STEP_8BITS,
			.sel = E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL,
			.out = E_IM_IIP_PARAM_ALPHA_OUT_P1_ALPHA,
			.ALPHA_VAL[0] = 0,
			.ALPHA_VAL[1] = 0,
			.ALPHA_VAL[2] = 0,
			.ALPHA_VAL[3] = 0,
		},
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_ALPHABLEND_BUF_BYTES];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 30;

#if 0
//	iip_util_blend.src[0].gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_blend.src[1].gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_blend.dst.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
//	iip_util_blend.alpha.sel = E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL;
	iip_util_blend.alpha.sel = E_IM_IIP_PARAM_ALPHA_SEL_P0_ALPHA;
//	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_FIXED_VAL;
//	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_P0_ALPHA;
	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_NONE;
#endif

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_AlphaBlend( &iip_util_blend,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// IM_IIP_Utility_AlphaBlend 14-4-3
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_4_3: "
INT32 CT_Im_IIP_14_4_3( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_ALPHABLEND iip_util_blend = {
		.ld_unitid[0] = E_IM_IIP_UNIT_ID_LD0,
		.ld_unitid[1] = E_IM_IIP_UNIT_ID_LD1,
		.blend_unitid = E_IM_IIP_UNIT_ID_BLEND0,
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,
		.src_pixid[0] = E_IM_IIP_PIXID_0,
		.src_pixid[1] = E_IM_IIP_PIXID_1,
		.dst_pixid = E_IM_IIP_PIXID_2,
		.src[0] = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC444,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC444_U16_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC444_U16_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC444_U16_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC444_U16_A_GLOBAL_WIDTH,
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC444_U16_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC444_U16_CB,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC444_U16_CR,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC444_U16_A,
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.src[1] = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC444,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC444_U16_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC444_U16_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC444_U16_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC444_U16_A_GLOBAL_WIDTH,
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_4_YCC444_U16_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_4_YCC444_U16_CB,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_4_YCC444_U16_CR,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_4_YCC444_U16_A,
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_PLANE,			// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC444,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC444_U16_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC444_U16_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC444_U16_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC444_U16_A_GLOBAL_WIDTH,
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_YCC444_U16_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_1_YCC444_U16_CB,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_1_YCC444_U16_CR,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC444_U16_A,
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select[0] = E_IM_IIP_PARAM_CSEL_0,
		.ld_cache_select[1] = E_IM_IIP_PARAM_CSEL_0,
		.pix_depth = D_IM_IIP_PDEPTH_16BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.alpha = {
			.step = E_IM_IIP_PARAM_ALPHA_STEP_8BITS,
			.sel = E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL,
			.out = E_IM_IIP_PARAM_ALPHA_OUT_NONE,
			.ALPHA_VAL[0] = 127,
			.ALPHA_VAL[1] = 127,
			.ALPHA_VAL[2] = 127,
			.ALPHA_VAL[3] = 127,
		},
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_ALPHABLEND_BUF_BYTES];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 30;

#if 0
	iip_util_blend.src[0].gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_blend.src[1].gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_blend.dst.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_blend.alpha.sel = E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL;
//	iip_util_blend.alpha.sel = E_IM_IIP_PARAM_ALPHA_SEL_P0_ALPHA;
//	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_FIXED_VAL;
//	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_P0_ALPHA;
	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_NONE;
#endif

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_AlphaBlend( &iip_util_blend,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// IM_IIP_Utility_AlphaBlend 14-4-4
#undef D_IM_IIP_FUNC_NAME
#define D_IM_IIP_FUNC_NAME "CT_Im_IIP_14_4_4: "
INT32 CT_Im_IIP_14_4_4( VOID )
{
	INT32							ercd;
	T_IM_IIP_UTIL_ALPHABLEND iip_util_blend = {
		.ld_unitid[0] = E_IM_IIP_UNIT_ID_LD0,
		.ld_unitid[1] = E_IM_IIP_UNIT_ID_LD1,
		.blend_unitid = E_IM_IIP_UNIT_ID_BLEND0,
		.sl_unitid = E_IM_IIP_UNIT_ID_SL3,
		.src_pixid[0] = E_IM_IIP_PIXID_0,
		.src_pixid[1] = E_IM_IIP_PIXID_1,
		.dst_pixid = E_IM_IIP_PIXID_2,
		.src[0] = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U16_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U16_A_GLOBAL_WIDTH,
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_0_YCC422_U16_A,
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.src[1] = {								// Source image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_FALSE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U16_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U16_A_GLOBAL_WIDTH,
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U16_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U16_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U16_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_4_YCC422_U16_A,
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF0,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.dst = {								// Destination image info.
			.rect = {								// Rectangle info. (Source image area)
				.top = 0.0,								// Vertical position of top-left pixel
				.left = 0.0,							// Horizontal position of top-left pixel
				.width = D_IM_IIP_VGA_WIDTH,			// Width
				.lines = D_IM_IIP_VGA_LINES,			// Lines
			},
			.gbl = {								// Global image info.
				.alpha = D_IM_IIP_ALPHA_TRUE,				// PIXFMTTBL.ALPHA <br><br> Whether image has alpha or not.
				.frame_type = E_IM_IIP_FTYPE_Y_PACKED_CBCR,	// PIXFMTTBL.FTYPE[2:0] <br><br> Frame format.
				.pix_format = E_IM_IIP_PFMT_YCC422,			// PIXFMTTBL.PFMT[3:0] <br><br> Pixel format.
				.chunky_color = {							// PIXFMTTBL.CHKYORDR[7:0] <br><br> Chunky component.
					.component0 = D_IM_IIP_CHUNKY_COLOR_Y0_G,	// PIXFMTTBL.CHKYORDR[1:0] <br><br> Chunky component (offset +0).
					.component1 = D_IM_IIP_CHUNKY_COLOR_CB_B,	// PIXFMTTBL.CHKYORDR[3:2] <br><br> Chunky component (offset +1).
					.component2 = D_IM_IIP_CHUNKY_COLOR_CR_R,	// PIXFMTTBL.CHKYORDR[5:4] <br><br> Chunky component (offset +2).
					.component3 = D_IM_IIP_CHUNKY_COLOR_Y1_A,	// PIXFMTTBL.CHKYORDR[7:6] <br><br> Chunky component (offset +3).
				},
				.sign_Y_G = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGYG <br><br> Whether Y(G) image has sign bit or not.
				.sign_Cb_B = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGB <br><br> Whether Cb(B) image has sign bit or not.
				.sign_Cr_R = D_IM_IIP_UNSIGNED_DATA,		// PIXFMTTBL.SGR <br><br> Whether Cr(R) image has sign bit or not.
				.sign_D3 = D_IM_IIP_UNSIGNED_DATA,			// PIXFMTTBL.SGA <br><br> Whether the fourth color which CSC uses has sign bit or not.
				.width = D_IM_IIP_VGA_WIDTH,				// PIXFMTTBL.PICHSZ[14:0]	<br><br> The number of horizontal pixel. [16~16383]
				.lines = D_IM_IIP_VGA_LINES,				// PIXFMTTBL.PICVSZ[13:0]	<br><br> The number of vertical pixel. [2~16383]
				.line_bytes = {								// The size of byte of Global image 1 line.
					.Y_G   = D_IM_IIP_VGA_YCC422_U16_Y_GLOBAL_WIDTH,
					.Cb_B  = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH,
					.Cr_R  = D_IM_IIP_VGA_YCC422_U16_C_GLOBAL_WIDTH,
					.Alpha = D_IM_IIP_VGA_YCC422_U16_A_GLOBAL_WIDTH,
				},
				.addr = {									// Beginning address of Global image.
					.Y_G   = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_Y,
					.Cb_B  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_C,
					.Cr_R  = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_C,
					.Alpha = D_IM_IIP_IMG_MEM_ADDR_1_YCC422_U16_A,
				},
				.masterIF_Y_G = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFYG <br><br> Whether the location of Y(G) image is master I/F 0 or 1.
				.masterIF_Cb_B = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFB <br><br> Whether the location of Cb(B) image is master I/F 0 or 1.
				.masterIF_Cr_R = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFR <br><br> Whether the location of Cr(R) image is master I/F 0 or 1.
				.masterIF_Alpha = D_IM_IIP_MASTER_IF1,		// PIXFMTTBL.MTIFA <br><br> Whether the location of Alpha image is master I/F 0 or 1.
			},
		},
		.ld_cache_select[0] = E_IM_IIP_PARAM_CSEL_0,
		.ld_cache_select[1] = E_IM_IIP_PARAM_CSEL_0,
		.pix_depth = D_IM_IIP_PDEPTH_16BITS,
		.alpha_depth = E_IM_IIP_ADEPTH_8BITS,
		.alpha_subsampling = D_IM_IIP_ALPHA_NO_SUBSAMP,
		.alpha = {
			.step = E_IM_IIP_PARAM_ALPHA_STEP_8BITS,
			.sel = E_IM_IIP_PARAM_ALPHA_SEL_P0_ALPHA,
			.out = E_IM_IIP_PARAM_ALPHA_OUT_P0_ALPHA,
			.ALPHA_VAL[0] = 0,
			.ALPHA_VAL[1] = 0,
			.ALPHA_VAL[2] = 0,
			.ALPHA_VAL[3] = 0,
		},
	};
	UCHAR param_master_IF = D_IM_IIP_MASTER_IF0;
	UCHAR param_buffer[D_IM_IIP_UTIL_ALPHABLEND_BUF_BYTES];
	ULONG param_buffer_addr = ct_im_iip_roundup_8( (ULONG)param_buffer );	// Convet 8bytes align
	INT32 wait_time = 30;

#if 0
	iip_util_blend.src[0].gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_blend.src[1].gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_blend.dst.gbl.alpha = D_IM_IIP_ALPHA_FALSE;
	iip_util_blend.alpha.sel = E_IM_IIP_PARAM_ALPHA_SEL_FIXED_VAL;
//	iip_util_blend.alpha.sel = E_IM_IIP_PARAM_ALPHA_SEL_P0_ALPHA;
//	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_FIXED_VAL;
//	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_P0_ALPHA;
	iip_util_blend.alpha.out = E_IM_IIP_PARAM_ALPHA_OUT_NONE;
#endif

	Ddim_Print(( D_IM_IIP_FUNC_NAME "\n" ));

	ercd = IM_IIP_Utility_AlphaBlend( &iip_util_blend,
										param_master_IF,
										param_buffer_addr,
										wait_time );
	Ddim_Print(( D_IM_IIP_FUNC_NAME "result = 0x%x.\n", ercd ));

	return D_DDIM_OK;
}


// IM_IIP_Utility_AlphaBlend 14-4-X
INT32 CT_Im_IIP_Run_14_4( const UINT32 ct_no_3rd )
{
	switch( ct_no_3rd ) {
		// Test-14-4-1
		case 1:
			return CT_Im_IIP_14_4_1();

		// Test-14-4-2
		case 2:
			return CT_Im_IIP_14_4_2();

		// Test-14-4-3
		case 3:
			return CT_Im_IIP_14_4_3();

		// Test-14-4-4
		case 4:
			return CT_Im_IIP_14_4_4();

		default:
			Ddim_Print(("Error: Unknown command.\n"));
			break;
	}

	return D_DDIM_INPUT_PARAM_ERROR;
}


#endif //CO_CT_IM_IIP_DISABLE

