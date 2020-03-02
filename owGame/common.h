#pragma once

#include <sockets\\StdoutLog.h>
#include <sockets\\TcpSocket.h>
#include <sockets\\SocketThread.h>
#include <sockets\\SocketHandler.h>
#include <sockets\\SocketHandlerThread.h>


//#define WOW_CLASSIC_1_12_1
#define WOW_BC_2_4_3


/**
  * Enable or disable M2 models support
*/
#define USE_M2_MODELS


// Interfaces
#include "Interfaces/ILiquid.h"
#include "Interfaces/SkyManager.h"
#include "Interfaces/Managers.h"

// Wow types
#include "WowTypes.h"
#include "WowConsts.h"
#include "WowTime.h"
#include "WowChunkUtils.h"
