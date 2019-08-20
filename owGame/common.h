#pragma once

#include <sockets\\StdoutLog.h>
#include <sockets\\TcpSocket.h>
#include <sockets\\SocketThread.h>
#include <sockets\\SocketHandler.h>
#include <sockets\\SocketHandlerThread.h>
#pragma comment(lib, "sockets.lib")

// =================================================================================================
// Build settings
// =================================================================================================

#define MDX_PARTICLES_ENABLE

// Interfaces
#include "SkyManager.h"
#include "Managers.h"

// Wow types
#include "WowTypes.h"
#include "WowConsts.h"
#include "WowTime.h"
#include "WowChunkUtils.h"