#pragma once

#include "AuthSocket.h"
#include "WorldSocket.h"

#include "World.h"

#include "SHA1.h"
#include "RealmInfo.h"

class ZN_API CWoWClient 
	: public std::enable_shared_from_this<CWoWClient>
{
public:
	CWoWClient(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene* m_Scene, const std::string& AuthServerHost, uint16 AuthServerPort = 3724);

    // CWoWClient
	void BeginConnect(const std::string& Username, const std::string& Password);
	void AddRealm(RealmInfo& _realm);
	void OnSuccessConnect(BigNumber Key);
	
	WoWWorld& GetWorld() const;

private:
	std::string                     m_Host;
	port_t                          m_Port;
	std::string                     m_Login;

private:
	std::vector<RealmInfo>          m_Realms;

    // Sockets controller
    StdoutLog                       m_SocketLog;
    std::shared_ptr<SocketHandler>  m_SocketsHandler;
	std::shared_ptr<SocketHandlerThread> m_SocketsHandlerThread;

    // Sockets
	std::shared_ptr<CAuthSocket>    m_AuthSocket;

	// World & General
	IBaseManager& m_BaseManager;
	IRenderDevice& m_RenderDevice;
	IScene* m_Scene;
	std::unique_ptr<WoWWorld> m_World;
};