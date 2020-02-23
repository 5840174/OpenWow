#include "stdafx.h"

// General
#include "Client.h"

CWoWClient::CWoWClient(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene* Scene, const std::string& AuthServerHost, uint16 AuthServerPort)
	: m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_Scene(Scene)
{
	m_Host = AuthServerHost;
	m_Port = AuthServerPort;

	
    m_SocketsHandler = std::make_shared<SocketHandler>();
	m_SocketsHandlerThread = std::make_shared<SocketHandlerThread>(*m_SocketsHandler);
	m_SocketsHandlerThread->SetRelease(true);

	// Wait for thread initialized
	while (&m_SocketsHandlerThread->Handler() == nullptr)
		Sleep(1);

	m_SocketsHandlerThread->Handler().RegStdLog(&m_SocketLog);
}

void CWoWClient::BeginConnect(const std::string& Username, const std::string& Password)
{
	m_Login = Utils::ToUpper(Username);
	std::string password = Utils::ToUpper(Password);

    m_AuthSocket = std::make_shared<CAuthSocket>(m_SocketsHandlerThread->Handler(), *this, m_Login, password);
    m_AuthSocket->Open(m_Host, m_Port);
	m_SocketsHandlerThread->Handler().Add(m_AuthSocket);
}

void CWoWClient::AddRealm(RealmInfo& _realm)
{
	m_Realms.push_back(_realm);
	_realm.print();
}

void CWoWClient::OnSuccessConnect(BigNumber Key)
{
    m_AuthSocket->SetDeleteByHandler();

    RealmInfo* currRealm = &(m_Realms[0]);

    std::shared_ptr<CWorldSocket> worldSocket = std::make_shared<CWorldSocket>(m_SocketsHandlerThread->Handler(), m_Login, Key);
	worldSocket->Open(currRealm->getIP(), currRealm->getPort());
	m_SocketsHandlerThread->Handler().Add(worldSocket);

	m_World = std::make_unique<WoWWorld>(m_BaseManager, m_RenderDevice, m_Scene, worldSocket);
}


WoWWorld& CWoWClient::GetWorld() const
{
	_ASSERT(m_World != nullptr);
	return *m_World;
}
