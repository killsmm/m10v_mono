#/*
#    Copyright (C) 2005 By eSOL Co.,Ltd. Tokyo, Japan
#
#    This software is protected by the law and the agreement concerning
#    a Japanese country copyright method, an international agreement,
#    and other intellectual property right and may be used and copied
#    only in accordance with the terms of such license and with the inclusion
#    of the above copyright notice.
#
#    This software or any other copies thereof may not be provided
#    or otherwise made available to any other person.  No title to
#    and ownership of the software is hereby transferred.
#
#    The information in this software is subject to change without
#    notice and should not be construed as a commitment by eSOL Co.,Ltd.
#*/
#/****************************************************************************
#[ config_prfile2.mk ] - PrFILE2 MAKE configurations
#****************************************************************************/
ifndef $(CONFIG_PRF2_MK)

CONFIG_PRF2_MK = 1

include $(USER_HOME)/config/config.mk

ifneq ($(USER_PRFILE2),0)
####
#### PrFILE2 option package selections
####
#### 0 = not use
#### 1 = use
####
#### use PrFILE2 Unicode Interface
USER_PRFILE2_UIF    = 0

#### use PrFILE2 exFAT
USER_PRFILE2_EXFAT  = 0

endif # ifneq ($(USER_PRFILE2),0)

endif # ifndef $(CONFIG_PRF2_MK)
