/******************************************************************************/
/**
 *  @file   ipcu.h
 *  @brief  sample
 */
/*
 *  Copyright 2017 Socionext Inc.
 ******************************************************************************/
#ifndef __IPCU_H__
#define __IPCU_H__

/* Debug print */
#define IPCU_DEBUG 0

/******************************************************************** 
 *  Function prototype
 ********************************************************************/
int IPCU_Command_Send(T_CPUCMD_COMMAND_ACK* ack, UINT32 main, UINT32 sub, ...);
int Camera_If_Open(void);
void Camera_If_Close(void);

#endif /* __IPCU_H__ */
