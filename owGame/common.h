#pragma once

#include <sockets\\StdoutLog.h>
#include <sockets\\TcpSocket.h>
#include <sockets\\SocketThread.h>
#include <sockets\\SocketHandler.h>
#include <sockets\\SocketHandlerThread.h>
#pragma comment(lib, "sockets.lib")

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


static inline vec3 Fix_XZY(cvec3 _vec)
{
	return vec3(_vec.x, _vec.z, _vec.y);
}

static inline vec3 Fix_XZmY(cvec3 _vec)
{
	return vec3(_vec.x, _vec.z, -_vec.y);
}