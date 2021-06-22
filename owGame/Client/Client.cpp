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
}

CWoWClient::~CWoWClient()
{

}

void CWoWClient::Update(UpdateEventArgs & e)
{
	if (m_AuthSocket && m_AuthSocket->GetStatus() == CSocket::Connected)
		m_AuthSocket->Update();

	if (m_WorldSocket && m_WorldSocket->GetStatus() == CSocket::Connected)
		m_WorldSocket->Update();
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

	m_CharacterSelection = std::make_unique<CWoWClientCharactedSelection>(*this, m_Scene, m_WorldSocket);
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

CWoWWorld& CWoWClient::GetWorld() const
{
	_ASSERT(m_World != nullptr);
	return *m_World;
}

#endif
