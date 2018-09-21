/*
 * configNet.h
 *
 *  Created on: 2011-4-15
 *      Author: Administrator
 */

#ifndef WDB_CONFIG_NET_H_
#define WDB_CONFIG_NET_H_

#include <vxWorks.h>
#include "wdbCommIfLib.h"
#include "End.h"
#include "wdbEndPktDrv.h"
#include "wdbLibP.h"
#include "wdbUdpLib.h"
#include "wdb.h"
#include "wdbOsLib.h"
#include "wdbMemLib.h"
#include "muxLib.h"

#define logMsg printf
#define WDB_MTU 1200
#define WDB_END_DEVICE_NAME
extern char wdbEndName[END_NAME_MAX];
extern UINT32 wdbEndDeviceUnit;
#endif /* WDB_INTERNEL_H_ */
