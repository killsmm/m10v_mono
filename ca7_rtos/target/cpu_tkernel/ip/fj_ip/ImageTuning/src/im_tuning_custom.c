/**************************************************************************/
/*  <Contents>   Main function                                            */
/*  <Filename>   Custom_Setting.c                                                   */
/*  <Enviroment> RVDS4 for ARM946                                         */
/*  <Creation Date/author>                                                */
/*               2018/08/01   RT                                          */
/*                                                                        */
/*                        Fujitsu API layer                               */
/*                                COPYRIGHT(C) SOCIONEXT LIMITED 2018     */
/**************************************************************************/

// ****************************************************************************
//  Include
// ****************************************************************************
#include "user.h"
#include "../include/im_tuning_buffer.h"
#include "../include/im_tuning_define.h"

// ****************************************************************************
//  Definition
// ****************************************************************************
#define BAUDRATE	115200//921600  // M12MO can go 921600 max but however..
// ****************************************************************************
//  Global Function Definition
// ****************************************************************************
//extern T_IQ_BIN	IQ_BIN;

_GetResource gGetResource[] =       {   //Name[8], ID, mem_ptr , Maxsize , Cursize
                                        {"IQBIN0",    0,0,0,0},
                                        {"IQBIN1",    1,0,0,0},
                                        {"IQBIN2",    2,0,0,0},
                                        {"IQBIN3",    3,0,0,0},
                                        {"IQBIN4",    4,0,0,0},
                                        {"IQBIN5",    5,0,0,0},
                                        {"IQBIN6",    6,0,0,0},
                                        {"IQBIN7",    7,0,0,0},
                                        {"IQBIN8",    8,0,0,0},
                                        {"EVPMAP",    9,0,0,0},
                                        {"FSHD0",    10,0,0,0},
                                        {"FSHD1",    11,0,0,0},
                                        {"FSHD2",    12,0,0,0},
                                        {"FSHD3",    13,0,0,0},
                                        {"FSHD4",    14,0,0,0},
                                        {"FSHD5",    15,0,0,0},
                                        {"FSHD6",    16,0,0,0},
                                        {"FSHD7",    17,0,0,0},
                                        {"FSHD8",    18,0,0,0},
                                        {"RGBBIN",   19,0,0,0},
                                        {"LDCBIN",   20,0,0,0},
                                        {"",0xFF, 0,0,0}, // Please don't edit this line.

                            };




_GetResource* MIL_Get_Resource_List()
{
    return gGetResource;
}

unsigned long Mil_Get_BaudRate(void)
{
    return BAUDRATE;
}

void Mil_Init_Resource(void)
{

}

