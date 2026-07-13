/**
 * @file		__jdspro_pg.h
 * @brief		Definition JDSPRO Macro I/O register
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _IO_PRO_PG_H_
#define _IO_PRO_PG_H_

/*  structure of PGTRG  (2840_8100h)    */
union io_pgtrg{
    unsigned long       word;
    struct {
        unsigned long   PGTRG   :2;
        unsigned long           :30;
    }bit;
};

/*  structure of PGCTL  (2840_8104h)    */
union io_pgctl{
    unsigned long       word;
    struct {
        unsigned long   PGMD    :1;
        unsigned long           :31;
    }bit;
};

/*  structure of PGVW   (2840_8108h)    */
union io_pgvw{
    unsigned long       word;
    struct {
        unsigned long   PGVW    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of PGHW   (2840_810Ch)    */
union io_pghw{
    unsigned long       word;
    struct {
        unsigned long   PGHW    :14;
        unsigned long           :18;
    }bit;
};

/*  structure of PGVBLNK    (2840_8110h)    */
union io_pgvblnk{
    unsigned long       word;
    struct {
        unsigned long   PGVBLNK :14;
        unsigned long           :18;
    }bit;
};

/*  structure of PGHBLNK    (2840_8114h)    */
union io_pghblnk{
    unsigned long       word;
    struct {
        unsigned long   PGHBLNK :14;
        unsigned long           :18;
    }bit;
};

/*  structure of PGVDHDBLNK (2840_8118h)    */
union io_pgvdhdblnk{
    unsigned long       word;
    struct {
        unsigned long   PGVHBLNK    :14;
        unsigned long               :18;
    }bit;
};

/* Define i/o mapping */
struct io_pg{
    /* JDSIMG */
    union  io_pgtrg       PGTRG;      /* 2840_(8100 - 8103h) */
    union  io_pgctl       PGCTL;      /* 2840_(8104 - 8107h) */
    union  io_pgvw        PGVW;       /* 2840_(8108 - 810Bh) */
    union  io_pghw        PGHW;       /* 2840_(810C - 810Fh) */
    union  io_pgvblnk     PGVBLNK;    /* 2840_(8110 - 8113h) */
    union  io_pghblnk     PGHBLNK;    /* 2840_(8114 - 8117h) */
    union  io_pgvdhdblnk  PGVDHDBLNK; /* 2840_(8118 - 811Bh) */

    unsigned char dmy_811C_81FF[0x8200-0x811C]; /* 2840_(811C - 81FFh) */

};

#endif	// _IO_PRO_PG_H_
