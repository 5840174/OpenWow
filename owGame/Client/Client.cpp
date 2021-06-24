#include "stdafx.h"

#ifdef ENABLE_WOW_CLIENT

// General
#include "Client.h"

CWoWClient::CWoWClient(IScene& Scene, const std::string& AuthServerHost, uint16 AuthServerPort)
	: m_Scene(Scene)
{
	m_Host = AuthServerHost;
	m_Port = AuthServerPort;

	
	
	//std::future<void> futureObj = m_UpdateThreadExiter.get_future();
	//m_UpdateThread = std::thread(&CWoWClient::Update, this, std::move(futureObj));
	//m_UpdateThread.detach();
}

CWoWClient::~CWoWClient()
{
	//m_UpdateThreadExiter.set_value();
	//_ASSERT(m_UpdateThread.joinable());
	//m_UpdateThread.join();
}

void CWoWClient::Update(const UpdateEventArgs& e/*std::future<void> PromiseExiter*/)
{
	//while (PromiseExiter.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
	{
		if (m_AuthSocket && m_AuthSocket->GetStatus() == CSocket::Connected)
			m_AuthSocket->Update();

		if (m_WorldSocket && m_WorldSocket->GetStatus() == CSocket::Connected)
			m_WorldSocket->Update();

		if (m_World)
			m_World->Update(e);
	}
}



//
// CWoWClient
//
void CWoWClient::BeginConnect(const std::string& Username, const std::string& Password)
{
	m_Login = Utils::ToUpper(Username);
	std::string password = Utils::ToUpper(Password);

    m_AuthSocket = std::make_unique<CAuthSocket>(*this, m_Login, password);
    m_AuthSocket->Open(m_Host, m_Port);
}

void CWoWClient::OnRealmListSelected(const RealmInfo& SelectedRealm, BigNumber Key)
{
	Log::Green("CWoWClient::OnRealmListSelected: Realm name '%s'.", SelectedRealm.Name.c_str());

	m_AuthSocket->Disconnect();

	m_WorldSocket = std::make_shared<CWorldSocket>(m_Login, Key);
	m_WorldSocket->Open(SelectedRealm.getIP(), SelectedRealm.getPort());

	m_CharacterSelection = std::make_unique<CWoWClientCharactedSelection>(*this, m_Scene);
	m_WorldSocket->SetExternalHandler(std::bind(&CWoWClientCharactedSelection::ProcessHandler, m_CharacterSelection.get(), std::placeholders::_1, std::placeholders::_2));
}

void CWoWClient::OnCharacterSelected(const CInet_CharacterTemplate & SelectedCharacter)
{
	Log::Green("CWoWClient::OnCharacterSelected: Character name '%s'.", SelectedCharacter.Name.c_str());

	m_CharacterSelection.reset();

	m_World = std::make_unique<CWoWWorld>(m_Scene, m_WorldSocket);
	m_WorldSocket->SetExternalHandler(std::bind(&CWoWWorld::ProcessHandler, m_World.get(), std::placeholders::_1, std::placeholders::_2));

	m_World->EnterWorld(SelectedCharacter);
}

#endif
