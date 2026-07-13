/**
 * @file		_jdsraw3a.h
 * @brief		Definition JDSRAW3A Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


/*	structure of RCTL0	(288A_0000h)	*/
union io_raw_rctl0{
	unsigned long		word;
	struct {
		unsigned long	SR	:1;
		unsigned long		:31;
	}bit;
};

/*	structure of RCTL1	(288A_0004h)	*/
union io_raw_rctl1{
	unsigned long		word;
	struct {
		unsigned long	RTRG	:2;
		unsigned long			:2;
		unsigned long	LUTEN	:1;
		unsigned long	DEFOP	:1;
		unsigned long	DEFFC	:1;
		unsigned long	BYTS	:1;
		unsigned long	RFMT	:2;
		unsigned long	OPMD	:1;
		unsigned long			:1;
		unsigned long	BITSEL	:1;
		unsigned long			:19;
	}bit;
};

/*	structure of COND	(288A_0008h)	*/
union io_raw_cond{
	unsigned long		word;
	struct {
		unsigned long	COND	:3;
		unsigned long			:29;
	}bit;
};

/*	structure of RINTF	(288A_000Ch)	*/
union io_raw_rintf{
	unsigned long		word;
	struct {
		unsigned long	__RF		:1;
		unsigned long				:3;
		unsigned long	__RAXERF	:1;
		unsigned long	__WAXERF	:1;
		unsigned long				:26;
	}bit;
};

/*	structure of RINTE	(288A_0040h)	*/
union io_raw_rinte{
	unsigned long		word;
	struct {
		unsigned long	RE		:1;
		unsigned long			:3;
		unsigned long	RAXERE	:1;
		unsigned long	WAXERE	:1;
		unsigned long			:26;
	}bit;
};

/*	structure of HSIZE	(288A_0044h)	*/
union io_raw_hsize{
	unsigned long		word;
	struct {
		unsigned long	HSIZE	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of VSIZE	(288A_0048h)	*/
union io_raw_vsize{
	unsigned long		word;
	struct {
		unsigned long	VSIZE	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of DSIZE	(288A_004Ch)	*/
union io_raw_dsize{
	unsigned long		word;
	struct {
		unsigned long	DSIZE	:24;
		unsigned long			:8;
	}bit;
};

/*	structure of MSA	(288A_0050h)	*/
union io_raw_msa{
	unsigned long		word;
	struct {
		unsigned long	MSA	:32;
	}bit;
};

/*	structure of MDA	(288A_0054h)	*/
union io_raw_mda{
	unsigned long		word;
	struct {
		unsigned long	MDA	:32;
	}bit;
};

/*	structure of DEFINIT	(288A_0058h)	*/
union io_raw_definit{
	unsigned long		word;
	struct {
		unsigned long	DEFINIT	:14;
		unsigned long			:18;
	}bit;
};

/*	structure of CLMT	(288A_005Ch)	*/
union io_raw_clmt{
	unsigned long		word;
	struct {
		unsigned long	CLMT	:29;
		unsigned long			:3;
	}bit;
};

/*	structure of CCNT	(288A_0060h)	*/
union io_raw_ccnt{
	unsigned long		word;
	struct {
		unsigned long	CCNT	:29;
		unsigned long			:3;
	}bit;
};

/*	structure of TBLAEN	(288A_0064h)	*/
union io_raw_tblaen{
	unsigned long		word;
	struct {
		unsigned long	TBLAEN	:1;
		unsigned long			:31;
	}bit;
};

/*	structure of MRAXCTL	(288A_0068h)	*/
union io_raw_mraxctl{
	unsigned long		word;
	struct {
		unsigned long	MRCACHE	:4;
		unsigned long	MRPROT	:3;
		unsigned long			:25;
	}bit;
};

/*	structure of MWAXCTL	(288A_006Ch)	*/
union io_raw_mwaxctl{
	unsigned long		word;
	struct {
		unsigned long	MWCACHE	:4;
		unsigned long	MWPROT	:3;
		unsigned long			:25;
	}bit;
};

/*	structure of MRAXSTS	(288A_0070h)	*/
union io_raw_mraxsts{
	unsigned long		word;
	struct {
		unsigned long	RRESP	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of MWAXSTS	(288A_0074h)	*/
union io_raw_mwaxsts{
	unsigned long		word;
	struct {
		unsigned long	BRESP	:2;
		unsigned long			:30;
	}bit;
};

/*	structure of CODE	(288A_0080h)	*/
union io_raw_code_1{
	unsigned long		word;
	struct {
		unsigned long	CODE0	:16;
		unsigned long	CODE1	:16;
	}bit;
};
union io_raw_code_2{
	unsigned long		word;
	struct {
		unsigned long	CODE2	:16;
		unsigned long	CODE3	:16;
	}bit;
};
union io_raw_code_3{
	unsigned long		word;
	struct {
		unsigned long	CODE4	:16;
		unsigned long	CODE5	:16;
	}bit;
};
union io_raw_code_4{
	unsigned long		word;
	struct {
		unsigned long	CODE6	:16;
		unsigned long	CODE7	:16;
	}bit;
};
union io_raw_code_5{
	unsigned long		word;
	struct {
		unsigned long	CODE8	:16;
		unsigned long	CODE9	:16;
	}bit;
};
union io_raw_code_6{
	unsigned long		word;
	struct {
		unsigned long	CODEA	:16;
		unsigned long	CODEB	:16;
	}bit;
};
union io_raw_code_7{
	unsigned long		word;
	struct {
		unsigned long	CODEC	:16;
		unsigned long	CODED	:16;
	}bit;
};
union io_raw_code_8{
	unsigned long		word;
	struct {
		unsigned long	CODEE	:16;
		unsigned long			:16;
	}bit;
};
struct io_raw_code{
	union io_raw_code_1	CODE1;
	union io_raw_code_2	CODE2;
	union io_raw_code_3	CODE3;
	union io_raw_code_4	CODE4;
	union io_raw_code_5	CODE5;
	union io_raw_code_6	CODE6;
	union io_raw_code_7	CODE7;
	union io_raw_code_8	CODE8;
};

/*	structure of LEN	(288A_00A0h)	*/
union io_raw_len_1{
	unsigned long		word;
	struct {
		unsigned long	LEN0	:16;
		unsigned long	LEN1	:16;
	}bit;
};
union io_raw_len_2{
	unsigned long		word;
	struct {
		unsigned long	LEN2	:16;
		unsigned long	LEN3	:16;
	}bit;
};
union io_raw_len_3{
	unsigned long		word;
	struct {
		unsigned long	LEN4	:16;
		unsigned long	LEN5	:16;
	}bit;
};
union io_raw_len_4{
	unsigned long		word;
	struct {
		unsigned long	LEN6	:16;
		unsigned long	LEN7	:16;
	}bit;
};
union io_raw_len_5{
	unsigned long		word;
	struct {
		unsigned long	LEN8	:16;
		unsigned long	LEN9	:16;
	}bit;
};
union io_raw_len_6{
	unsigned long		word;
	struct {
		unsigned long	LENA	:16;
		unsigned long	LENB	:16;
	}bit;
};
union io_raw_len_7{
	unsigned long		word;
	struct {
		unsigned long	LENC	:16;
		unsigned long	LEND	:16;
	}bit;
};
union io_raw_len_8{
	unsigned long		word;
	struct {
		unsigned long	LENE	:16;
		unsigned long			:16;
	}bit;
};
struct io_raw_len{
	union io_raw_len_1	LEN1;
	union io_raw_len_2	LEN2;
	union io_raw_len_3	LEN3;
	union io_raw_len_4	LEN4;
	union io_raw_len_5	LEN5;
	union io_raw_len_6	LEN6;
	union io_raw_len_7	LEN7;
	union io_raw_len_8	LEN8;
};

/*	structure of LUT	(288A_0100h)	*/
union io_raw_lut_1{
	unsigned long		word;
	struct {
		unsigned long	LUT_0	:8;
		unsigned long	LUT_1	:8;
		unsigned long	LUT_2	:8;
		unsigned long	LUT_3	:8;
	}bit;
};
union io_raw_lut_2{
	unsigned long		word;
	struct {
		unsigned long	LUT_4	:8;
		unsigned long	LUT_5	:8;
		unsigned long	LUT_6	:8;
		unsigned long	LUT_7	:8;
	}bit;
};
union io_raw_lut_3{
	unsigned long		word;
	struct {
		unsigned long	LUT_8	:8;
		unsigned long	LUT_9	:8;
		unsigned long	LUT_10	:8;
		unsigned long	LUT_11	:8;
	}bit;
};
union io_raw_lut_4{
	unsigned long		word;
	struct {
		unsigned long	LUT_12	:8;
		unsigned long	LUT_13	:8;
		unsigned long	LUT_14	:8;
		unsigned long	LUT_15	:8;
	}bit;
};
union io_raw_lut_5{
	unsigned long		word;
	struct {
		unsigned long	LUT_16	:8;
		unsigned long	LUT_17	:8;
		unsigned long	LUT_18	:8;
		unsigned long	LUT_19	:8;
	}bit;
};
union io_raw_lut_6{
	unsigned long		word;
	struct {
		unsigned long	LUT_20	:8;
		unsigned long	LUT_21	:8;
		unsigned long	LUT_22	:8;
		unsigned long	LUT_23	:8;
	}bit;
};
union io_raw_lut_7{
	unsigned long		word;
	struct {
		unsigned long	LUT_24	:8;
		unsigned long	LUT_25	:8;
		unsigned long	LUT_26	:8;
		unsigned long	LUT_27	:8;
	}bit;
};
union io_raw_lut_8{
	unsigned long		word;
	struct {
		unsigned long	LUT_28	:8;
		unsigned long	LUT_29	:8;
		unsigned long	LUT_30	:8;
		unsigned long	LUT_31	:8;
	}bit;
};
union io_raw_lut_9{
	unsigned long		word;
	struct {
		unsigned long	LUT_32	:8;
		unsigned long	LUT_33	:8;
		unsigned long	LUT_34	:8;
		unsigned long	LUT_35	:8;
	}bit;
};
union io_raw_lut_10{
	unsigned long		word;
	struct {
		unsigned long	LUT_36	:8;
		unsigned long	LUT_37	:8;
		unsigned long	LUT_38	:8;
		unsigned long	LUT_39	:8;
	}bit;
};
union io_raw_lut_11{
	unsigned long		word;
	struct {
		unsigned long	LUT_40	:8;
		unsigned long	LUT_41	:8;
		unsigned long	LUT_42	:8;
		unsigned long	LUT_43	:8;
	}bit;
};
union io_raw_lut_12{
	unsigned long		word;
	struct {
		unsigned long	LUT_44	:8;
		unsigned long	LUT_45	:8;
		unsigned long	LUT_46	:8;
		unsigned long	LUT_47	:8;
	}bit;
};
union io_raw_lut_13{
	unsigned long		word;
	struct {
		unsigned long	LUT_48	:8;
		unsigned long	LUT_49	:8;
		unsigned long	LUT_50	:8;
		unsigned long	LUT_51	:8;
	}bit;
};
union io_raw_lut_14{
	unsigned long		word;
	struct {
		unsigned long	LUT_52	:8;
		unsigned long	LUT_53	:8;
		unsigned long	LUT_54	:8;
		unsigned long	LUT_55	:8;
	}bit;
};
union io_raw_lut_15{
	unsigned long		word;
	struct {
		unsigned long	LUT_56	:8;
		unsigned long	LUT_57	:8;
		unsigned long	LUT_58	:8;
		unsigned long	LUT_59	:8;
	}bit;
};
union io_raw_lut_16{
	unsigned long		word;
	struct {
		unsigned long	LUT_60	:8;
		unsigned long	LUT_61	:8;
		unsigned long	LUT_62	:8;
		unsigned long	LUT_63	:8;
	}bit;
};
union io_raw_lut_17{
	unsigned long		word;
	struct {
		unsigned long	LUT_64	:8;
		unsigned long	LUT_65	:8;
		unsigned long	LUT_66	:8;
		unsigned long	LUT_67	:8;
	}bit;
};
union io_raw_lut_18{
	unsigned long		word;
	struct {
		unsigned long	LUT_68	:8;
		unsigned long	LUT_69	:8;
		unsigned long	LUT_70	:8;
		unsigned long	LUT_71	:8;
	}bit;
};
union io_raw_lut_19{
	unsigned long		word;
	struct {
		unsigned long	LUT_72	:8;
		unsigned long	LUT_73	:8;
		unsigned long	LUT_74	:8;
		unsigned long	LUT_75	:8;
	}bit;
};
union io_raw_lut_20{
	unsigned long		word;
	struct {
		unsigned long	LUT_76	:8;
		unsigned long	LUT_77	:8;
		unsigned long	LUT_78	:8;
		unsigned long	LUT_79	:8;
	}bit;
};
union io_raw_lut_21{
	unsigned long		word;
	struct {
		unsigned long	LUT_80	:8;
		unsigned long	LUT_81	:8;
		unsigned long	LUT_82	:8;
		unsigned long	LUT_83	:8;
	}bit;
};
union io_raw_lut_22{
	unsigned long		word;
	struct {
		unsigned long	LUT_84	:8;
		unsigned long	LUT_85	:8;
		unsigned long	LUT_86	:8;
		unsigned long	LUT_87	:8;
	}bit;
};
union io_raw_lut_23{
	unsigned long		word;
	struct {
		unsigned long	LUT_88	:8;
		unsigned long	LUT_89	:8;
		unsigned long	LUT_90	:8;
		unsigned long	LUT_91	:8;
	}bit;
};
union io_raw_lut_24{
	unsigned long		word;
	struct {
		unsigned long	LUT_92	:8;
		unsigned long	LUT_93	:8;
		unsigned long	LUT_94	:8;
		unsigned long	LUT_95	:8;
	}bit;
};
union io_raw_lut_25{
	unsigned long		word;
	struct {
		unsigned long	LUT_96	:8;
		unsigned long	LUT_97	:8;
		unsigned long	LUT_98	:8;
		unsigned long	LUT_99	:8;
	}bit;
};
union io_raw_lut_26{
	unsigned long		word;
	struct {
		unsigned long	LUT_100	:8;
		unsigned long	LUT_101	:8;
		unsigned long	LUT_102	:8;
		unsigned long	LUT_103	:8;
	}bit;
};
union io_raw_lut_27{
	unsigned long		word;
	struct {
		unsigned long	LUT_104	:8;
		unsigned long	LUT_105	:8;
		unsigned long	LUT_106	:8;
		unsigned long	LUT_107	:8;
	}bit;
};
union io_raw_lut_28{
	unsigned long		word;
	struct {
		unsigned long	LUT_108	:8;
		unsigned long	LUT_109	:8;
		unsigned long	LUT_110	:8;
		unsigned long	LUT_111	:8;
	}bit;
};
union io_raw_lut_29{
	unsigned long		word;
	struct {
		unsigned long	LUT_112	:8;
		unsigned long	LUT_113	:8;
		unsigned long	LUT_114	:8;
		unsigned long	LUT_115	:8;
	}bit;
};
union io_raw_lut_30{
	unsigned long		word;
	struct {
		unsigned long	LUT_116	:8;
		unsigned long	LUT_117	:8;
		unsigned long	LUT_118	:8;
		unsigned long	LUT_119	:8;
	}bit;
};
union io_raw_lut_31{
	unsigned long		word;
	struct {
		unsigned long	LUT_120	:8;
		unsigned long	LUT_121	:8;
		unsigned long	LUT_122	:8;
		unsigned long	LUT_123	:8;
	}bit;
};
union io_raw_lut_32{
	unsigned long		word;
	struct {
		unsigned long	LUT_124	:8;
		unsigned long	LUT_125	:8;
		unsigned long	LUT_126	:8;
		unsigned long	LUT_127	:8;
	}bit;
};
union io_raw_lut_33{
	unsigned long		word;
	struct {
		unsigned long	LUT_128	:8;
		unsigned long	LUT_129	:8;
		unsigned long	LUT_130	:8;
		unsigned long	LUT_131	:8;
	}bit;
};
union io_raw_lut_34{
	unsigned long		word;
	struct {
		unsigned long	LUT_132	:8;
		unsigned long	LUT_133	:8;
		unsigned long	LUT_134	:8;
		unsigned long	LUT_135	:8;
	}bit;
};
union io_raw_lut_35{
	unsigned long		word;
	struct {
		unsigned long	LUT_136	:8;
		unsigned long	LUT_137	:8;
		unsigned long	LUT_138	:8;
		unsigned long	LUT_139	:8;
	}bit;
};
union io_raw_lut_36{
	unsigned long		word;
	struct {
		unsigned long	LUT_140	:8;
		unsigned long	LUT_141	:8;
		unsigned long	LUT_142	:8;
		unsigned long	LUT_143	:8;
	}bit;
};
union io_raw_lut_37{
	unsigned long		word;
	struct {
		unsigned long	LUT_144	:8;
		unsigned long	LUT_145	:8;
		unsigned long	LUT_146	:8;
		unsigned long	LUT_147	:8;
	}bit;
};
union io_raw_lut_38{
	unsigned long		word;
	struct {
		unsigned long	LUT_148	:8;
		unsigned long	LUT_149	:8;
		unsigned long	LUT_150	:8;
		unsigned long	LUT_151	:8;
	}bit;
};
union io_raw_lut_39{
	unsigned long		word;
	struct {
		unsigned long	LUT_152	:8;
		unsigned long	LUT_153	:8;
		unsigned long	LUT_154	:8;
		unsigned long	LUT_155	:8;
	}bit;
};
union io_raw_lut_40{
	unsigned long		word;
	struct {
		unsigned long	LUT_156	:8;
		unsigned long	LUT_157	:8;
		unsigned long	LUT_158	:8;
		unsigned long	LUT_159	:8;
	}bit;
};
union io_raw_lut_41{
	unsigned long		word;
	struct {
		unsigned long	LUT_160	:8;
		unsigned long	LUT_161	:8;
		unsigned long	LUT_162	:8;
		unsigned long	LUT_163	:8;
	}bit;
};
union io_raw_lut_42{
	unsigned long		word;
	struct {
		unsigned long	LUT_164	:8;
		unsigned long	LUT_165	:8;
		unsigned long	LUT_166	:8;
		unsigned long	LUT_167	:8;
	}bit;
};
union io_raw_lut_43{
	unsigned long		word;
	struct {
		unsigned long	LUT_168	:8;
		unsigned long	LUT_169	:8;
		unsigned long	LUT_170	:8;
		unsigned long	LUT_171	:8;
	}bit;
};
union io_raw_lut_44{
	unsigned long		word;
	struct {
		unsigned long	LUT_172	:8;
		unsigned long	LUT_173	:8;
		unsigned long	LUT_174	:8;
		unsigned long	LUT_175	:8;
	}bit;
};
union io_raw_lut_45{
	unsigned long		word;
	struct {
		unsigned long	LUT_176	:8;
		unsigned long	LUT_177	:8;
		unsigned long	LUT_178	:8;
		unsigned long	LUT_179	:8;
	}bit;
};
union io_raw_lut_46{
	unsigned long		word;
	struct {
		unsigned long	LUT_180	:8;
		unsigned long	LUT_181	:8;
		unsigned long	LUT_182	:8;
		unsigned long	LUT_183	:8;
	}bit;
};
union io_raw_lut_47{
	unsigned long		word;
	struct {
		unsigned long	LUT_184	:8;
		unsigned long	LUT_185	:8;
		unsigned long	LUT_186	:8;
		unsigned long	LUT_187	:8;
	}bit;
};
union io_raw_lut_48{
	unsigned long		word;
	struct {
		unsigned long	LUT_188	:8;
		unsigned long	LUT_189	:8;
		unsigned long	LUT_190	:8;
		unsigned long	LUT_191	:8;
	}bit;
};
union io_raw_lut_49{
	unsigned long		word;
	struct {
		unsigned long	LUT_192	:8;
		unsigned long	LUT_193	:8;
		unsigned long	LUT_194	:8;
		unsigned long	LUT_195	:8;
	}bit;
};
union io_raw_lut_50{
	unsigned long		word;
	struct {
		unsigned long	LUT_196	:8;
		unsigned long	LUT_197	:8;
		unsigned long	LUT_198	:8;
		unsigned long	LUT_199	:8;
	}bit;
};
union io_raw_lut_51{
	unsigned long		word;
	struct {
		unsigned long	LUT_200	:8;
		unsigned long	LUT_201	:8;
		unsigned long	LUT_202	:8;
		unsigned long	LUT_203	:8;
	}bit;
};
union io_raw_lut_52{
	unsigned long		word;
	struct {
		unsigned long	LUT_204	:8;
		unsigned long	LUT_205	:8;
		unsigned long	LUT_206	:8;
		unsigned long	LUT_207	:8;
	}bit;
};
union io_raw_lut_53{
	unsigned long		word;
	struct {
		unsigned long	LUT_208	:8;
		unsigned long	LUT_209	:8;
		unsigned long	LUT_210	:8;
		unsigned long	LUT_211	:8;
	}bit;
};
union io_raw_lut_54{
	unsigned long		word;
	struct {
		unsigned long	LUT_212	:8;
		unsigned long	LUT_213	:8;
		unsigned long	LUT_214	:8;
		unsigned long	LUT_215	:8;
	}bit;
};
union io_raw_lut_55{
	unsigned long		word;
	struct {
		unsigned long	LUT_216	:8;
		unsigned long	LUT_217	:8;
		unsigned long	LUT_218	:8;
		unsigned long	LUT_219	:8;
	}bit;
};
union io_raw_lut_56{
	unsigned long		word;
	struct {
		unsigned long	LUT_220	:8;
		unsigned long	LUT_221	:8;
		unsigned long	LUT_222	:8;
		unsigned long	LUT_223	:8;
	}bit;
};
union io_raw_lut_57{
	unsigned long		word;
	struct {
		unsigned long	LUT_224	:8;
		unsigned long	LUT_225	:8;
		unsigned long	LUT_226	:8;
		unsigned long	LUT_227	:8;
	}bit;
};
union io_raw_lut_58{
	unsigned long		word;
	struct {
		unsigned long	LUT_228	:8;
		unsigned long	LUT_229	:8;
		unsigned long	LUT_230	:8;
		unsigned long	LUT_231	:8;
	}bit;
};
union io_raw_lut_59{
	unsigned long		word;
	struct {
		unsigned long	LUT_232	:8;
		unsigned long	LUT_233	:8;
		unsigned long	LUT_234	:8;
		unsigned long	LUT_235	:8;
	}bit;
};
union io_raw_lut_60{
	unsigned long		word;
	struct {
		unsigned long	LUT_236	:8;
		unsigned long	LUT_237	:8;
		unsigned long	LUT_238	:8;
		unsigned long	LUT_239	:8;
	}bit;
};
union io_raw_lut_61{
	unsigned long		word;
	struct {
		unsigned long	LUT_240	:8;
		unsigned long	LUT_241	:8;
		unsigned long	LUT_242	:8;
		unsigned long	LUT_243	:8;
	}bit;
};
union io_raw_lut_62{
	unsigned long		word;
	struct {
		unsigned long	LUT_244	:8;
		unsigned long	LUT_245	:8;
		unsigned long	LUT_246	:8;
		unsigned long	LUT_247	:8;
	}bit;
};
union io_raw_lut_63{
	unsigned long		word;
	struct {
		unsigned long	LUT_248	:8;
		unsigned long	LUT_249	:8;
		unsigned long	LUT_250	:8;
		unsigned long	LUT_251	:8;
	}bit;
};
union io_raw_lut_64{
	unsigned long		word;
	struct {
		unsigned long	LUT_252	:8;
		unsigned long	LUT_253	:8;
		unsigned long	LUT_254	:8;
		unsigned long	LUT_255	:8;
	}bit;
};
struct io_raw_lut{
	union io_raw_lut_1		LUT1;
	union io_raw_lut_2		LUT2;
	union io_raw_lut_3		LUT3;
	union io_raw_lut_4		LUT4;
	union io_raw_lut_5		LUT5;
	union io_raw_lut_6		LUT6;
	union io_raw_lut_7		LUT7;
	union io_raw_lut_8		LUT8;
	union io_raw_lut_9		LUT9;
	union io_raw_lut_10		LUT10;
	union io_raw_lut_11		LUT11;
	union io_raw_lut_12		LUT12;
	union io_raw_lut_13		LUT13;
	union io_raw_lut_14		LUT14;
	union io_raw_lut_15		LUT15;
	union io_raw_lut_16		LUT16;
	union io_raw_lut_17		LUT17;
	union io_raw_lut_18		LUT18;
	union io_raw_lut_19		LUT19;
	union io_raw_lut_20		LUT20;
	union io_raw_lut_21		LUT21;
	union io_raw_lut_22		LUT22;
	union io_raw_lut_23		LUT23;
	union io_raw_lut_24		LUT24;
	union io_raw_lut_25		LUT25;
	union io_raw_lut_26		LUT26;
	union io_raw_lut_27		LUT27;
	union io_raw_lut_28		LUT28;
	union io_raw_lut_29		LUT29;
	union io_raw_lut_30		LUT30;
	union io_raw_lut_31		LUT31;
	union io_raw_lut_32		LUT32;
	union io_raw_lut_33		LUT33;
	union io_raw_lut_34		LUT34;
	union io_raw_lut_35		LUT35;
	union io_raw_lut_36		LUT36;
	union io_raw_lut_37		LUT37;
	union io_raw_lut_38		LUT38;
	union io_raw_lut_39		LUT39;
	union io_raw_lut_40		LUT40;
	union io_raw_lut_41		LUT41;
	union io_raw_lut_42		LUT42;
	union io_raw_lut_43		LUT43;
	union io_raw_lut_44		LUT44;
	union io_raw_lut_45		LUT45;
	union io_raw_lut_46		LUT46;
	union io_raw_lut_47		LUT47;
	union io_raw_lut_48		LUT48;
	union io_raw_lut_49		LUT49;
	union io_raw_lut_50		LUT50;
	union io_raw_lut_51		LUT51;
	union io_raw_lut_52		LUT52;
	union io_raw_lut_53		LUT53;
	union io_raw_lut_54		LUT54;
	union io_raw_lut_55		LUT55;
	union io_raw_lut_56		LUT56;
	union io_raw_lut_57		LUT57;
	union io_raw_lut_58		LUT58;
	union io_raw_lut_59		LUT59;
	union io_raw_lut_60		LUT60;
	union io_raw_lut_61		LUT61;
	union io_raw_lut_62		LUT62;
	union io_raw_lut_63		LUT63;
	union io_raw_lut_64		LUT64;
};

/* Define i/o mapping */
struct io_jdsraw{
    /* JDSIMG */
	union  io_raw_rctl0		RCTL0;		/* 288A_(0000 - 0003h) */
	union  io_raw_rctl1		RCTL1;		/* 288A_(0004 - 0007h) */
	union  io_raw_cond		COND;		/* 288A_(0008 - 000Bh) */
	union  io_raw_rintf		RINTF;		/* 288A_(000C - 000Fh) */

	unsigned char dmy_0010_003F[0x0040-0x0010];	/* 288A_(0010 - 003Fh) */

	union  io_raw_rinte		RINTE;		/* 288A_(0040 - 0043h) */
	union  io_raw_hsize		HSIZE;		/* 288A_(0044 - 0047h) */
	union  io_raw_vsize		VSIZE;		/* 288A_(0048 - 004Bh) */
	union  io_raw_dsize		DSIZE;		/* 288A_(004C - 004Fh) */
	union  io_raw_msa		MSA;		/* 288A_(0050 - 0053h) */
	union  io_raw_mda		MDA;		/* 288A_(0054 - 0057h) */
	union  io_raw_definit	DEFINIT;	/* 288A_(0058 - 005Bh) */
	union  io_raw_clmt		CLMT;		/* 288A_(005C - 005Fh) */
	union  io_raw_ccnt		CCNT;		/* 288A_(0060 - 0063h) */
	union  io_raw_tblaen	TBLAEN;		/* 288A_(0064 - 0067h) */
	union  io_raw_mraxctl	MRAXCTL;	/* 288A_(0068 - 006Bh) */
	union  io_raw_mwaxctl	MWAXCTL;	/* 288A_(006C - 006Fh) */
	union  io_raw_mraxsts	MRAXSTS;	/* 288A_(0070 - 0073h) */
	union  io_raw_mwaxsts	MWAXSTS;	/* 288A_(0074 - 0077h) */

	unsigned char dmy_0078_007F[0x0080-0x0078];	/* 288A_(0078 - 007Fh) */

	struct io_raw_code		CODE;		/* 288A_(0080 - 009Fh) */
	struct io_raw_len		LEN;		/* 288A_(00A0 - 00BFh) */

	unsigned char dmy_00C0_00FF[0x0100-0x00C0];	/* 288A_(00C0 - 00FFh) */

	struct io_raw_lut		LUT;		/* 288A_(0100 - 01FFh) */

	unsigned char dmy_288A0200_288AFFFF[0x288B0000-0x288A0200];	/* 288A_0200 - 288A_FFFFh */

};

extern volatile struct io_jdsraw		IO_RAW;
