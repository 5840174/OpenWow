#include "stdafx.h"

// General
#include "Client.h"

CWoWClient::CWoWClient(IBaseManager& BaseManager, IRenderDevice& RenderDevice, const std::string& AuthServerHost, uint16 AuthServerPort)
	: m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
{
	m_Host = AuthServerHost;
	m_Port = AuthServerPort;

    m_SocketsHandler = std::make_shared<sockets::SocketHandler>();
	m_SocketsHandlerThread = new sockets::SocketHandlerThread(*m_SocketsHandler);
	m_SocketsHandlerThread->SetDeleteOnExit();
	m_SocketsHandlerThread->Start();
	m_SocketsHandlerThread->Wait();

	// Wait for thread initialized
	//while (&m_SocketsHandlerThread->Handler() == nullptr)
	//	Sleep(1);

	m_SocketsHandlerThread->Handler().RegStdLog(&m_SocketLog);
}

CWoWClient::~CWoWClient()
{
	delete m_SocketsHandlerThread;
	m_SocketsHandler.reset();
	//m_SocketsHandlerThread->Stop();
	//while (m_SocketsHandlerThread->Handler().GetCount() != 0);
	//m_World.reset();
}



//
// CWoWClient
//
void CWoWClient::SetScene(const std::shared_ptr<IScene>& Scene)
{
	m_Scene = Scene.get();
}

void CWoWClient::BeginConnect(const std::string& Username, const std::string& Password)
{
	m_Login = Utils::ToUpper(Username);
	std::string password = Utils::ToUpper(Password);

    m_AuthSocket = std::make_shared<CAuthSocket>(m_SocketsHandlerThread->Handler(), *this, m_Login, password);
    m_AuthSocket->Open(m_Host, m_Port);
	m_AuthSocket->SetDeleteByHandler();
	m_SocketsHandlerThread->Handler().Add(m_AuthSocket.get());
}

void CWoWClient::AddRealm(RealmInfo& _realm)
{
	m_Realms.push_back(_realm);
	_realm.print();
}

void CWoWClient::OnSuccessConnect(BigNumber Key)
{
    RealmInfo* currRealm = &(m_Realms[0]);

    std::shared_ptr<CWorldSocket> worldSocket = std::make_shared<CWorldSocket>(m_SocketsHandlerThread->Handler(), m_Login, Key);
	worldSocket->Open(currRealm->getIP(), currRealm->getPort());
	worldSocket->SetDeleteByHandler();
	m_SocketsHandlerThread->Handler().Add(worldSocket.get());

	m_World = std::make_unique<WoWWorld>(m_BaseManager, m_RenderDevice, m_Scene, worldSocket);
}

WoWWorld& CWoWClient::GetWorld() const
{
	_ASSERT(m_World != nullptr);
	return *m_World;
}
