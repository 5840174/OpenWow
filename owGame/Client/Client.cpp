#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "Client.h"

CWoWClient::CWoWClient(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene& Scene, const std::string& AuthServerHost, uint16 AuthServerPort)
	: m_BaseManager(BaseManager)
	, m_RenderDevice(RenderDevice)
	, m_Scene(Scene)
{
	m_Host = AuthServerHost;
	m_Port = AuthServerPort;

    //m_SocketsHandler = std::make_shared<sockets::SocketHandler>();
	//m_SocketsHandlerThread = new sockets::SocketHandlerThread(*m_SocketsHandler);
	//m_SocketsHandlerThread->SetDeleteOnExit();
	//m_SocketsHandlerThread->Start();
	//m_SocketsHandlerThread->Wait();

	// Wait for thread initialized
	//while (&m_SocketsHandlerThread->Handler() == nullptr)
	//	Sleep(1);

	//m_SocketsHandlerThread->Handler().RegStdLog(&m_SocketLog);
}

CWoWClient::~CWoWClient()
{
	//delete m_SocketsHandlerThread;
	//m_SocketsHandler.reset();
	//m_SocketsHandlerThread->Stop();
	//while (m_SocketsHandlerThread->Handler().GetCount() != 0);
	//m_World.reset();
}

void CWoWClient::Update(UpdateEventArgs & e)
{
	if (m_AuthSocket)
		m_AuthSocket->Update();

	if (m_WorldSocket)
		m_WorldSocket->Update();
}



//
// CWoWClient
//
void CWoWClient::BeginConnect(const std::string& Username, const std::string& Password)
{
	m_Login = Utils::ToUpper(Username);
	std::string password = Utils::ToUpper(Password);

    m_AuthSocket = new CAuthSocket(*this, m_Login, password);
    m_AuthSocket->Open(m_Host, m_Port);
	//m_AuthSocket->SetDeleteByHandler();
	//m_SocketsHandlerThread->Handler().Add(m_AuthSocket);
}

void CWoWClient::AddRealm(RealmInfo& _realm)
{
	m_Realms.push_back(_realm);
	_realm.print();
}

void CWoWClient::OnSuccessConnect(BigNumber Key)
{
    RealmInfo* currRealm = &(m_Realms[0]);
	Log::Green("CWoWClient::OnSuccessConnect: Realm name '%s'.", currRealm->Name.c_str());

	m_WorldSocket = std::make_shared<CWorldSocket>(m_Login, Key);
	m_WorldSocket->Open(currRealm->getIP(), currRealm->getPort());
	//m_WorldSocket->SetDeleteByHandler();

	//m_SocketsHandlerThread->Handler().Add(m_WorldSocket.get());

	m_World = std::make_unique<WoWWorld>(m_Scene, m_WorldSocket);
}

WoWWorld& CWoWClient::GetWorld() const
{
	_ASSERT(m_World != nullptr);
	return *m_World;
}

#endif
