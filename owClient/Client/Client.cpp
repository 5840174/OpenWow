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
	//m_UpdateThread = std::thread(&CWoWClient::UpdateFromThread, this, std::move(futureObj));
	//m_UpdateThread.detach();
}

CWoWClient::~CWoWClient()
{
	//m_UpdateThreadExiter.set_value();
	//_ASSERT(m_UpdateThread.joinable());
	//m_UpdateThread.join();
}

void CWoWClient::Update(const UpdateEventArgs& e)
{
	if (m_AuthSocket && m_AuthSocket->GetStatus() == CSocket::Connected)
		m_AuthSocket->Update();

	if (m_WorldSocket && m_WorldSocket->GetStatus() == CSocket::Connected)
		m_WorldSocket->Update();

	//std::lock_guard<std::mutex> lock(m_Scene.GetChildModifyLock());

	if (m_World)
	{
		m_World->Update(e);
	}
}

void CWoWClient::UpdateFromThread(std::future<void> PromiseExiter)
{
	while (PromiseExiter.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
	{
		UpdateEventArgs e(1.0f, 1.0f, 123);
		e.Camera = m_Scene.GetCameraController()->GetCamera().get();
		e.CameraForCulling = m_Scene.GetCameraController()->GetCamera().get();

		Update(e);
	}
}



//
// CWoWClient
//
void CWoWClient::Login(const std::string& Username, const std::string& Password)
{
	m_Login = Utils::ToUpper(Username);
	std::string password = Utils::ToUpper(Password);

    m_AuthSocket = std::make_unique<CAuthSocket>(*this, m_Login, password);
    m_AuthSocket->Open(m_Host, m_Port);
}

void CWoWClient::OnRealmsListObtained(const std::vector<SRealmInfo>& Realms, BigNumber Key)
{
	OnRealmSelected(Realms[9], Key);
}

void CWoWClient::OnRealmSelected(const SRealmInfo& SelectedRealm, BigNumber Key)
{
	Log::Green("CWoWClient::OnRealmListSelected: Realm name '%s'.", SelectedRealm.Name.c_str());

	m_AuthSocket->Disconnect();

	m_WorldSocket = std::make_shared<CWorldSocket>(m_Login, Key);
	m_WorldSocket->Open(SelectedRealm.getIP(), SelectedRealm.getPort());

	m_CharacterSelection = std::make_unique<CWoWClientCharactedSelection>(*this, m_Scene);
	m_WorldSocket->SetExternalHandler(std::bind(&CWoWClientCharactedSelection::ProcessPacket, m_CharacterSelection.get(), std::placeholders::_1));
}

void CWoWClient::OnCharactersListObtained(const std::vector<SCharacterTemplate>& Characters)
{
	OnCharacterSelected(Characters[0]);
}

void CWoWClient::OnCharacterSelected(const SCharacterTemplate & SelectedCharacter)
{
	Log::Green("CWoWClient::OnCharacterSelected: Character name '%s'.", SelectedCharacter.Name.c_str());

	m_CharacterSelection.reset();

	//SelectedCharacter.ToBase64String();

	//CWorldObjectCreator creator(m_Scene.GetBaseManager());
	//auto creature = creator.BuildCharacterFromTemplate(m_Scene.GetBaseManager().GetApplication().GetRenderDevice(), m_Scene, SelectedCharacter);

	m_World = std::make_unique<CWoWWorld>(m_Scene, m_WorldSocket);
	m_WorldSocket->SetExternalHandler(std::bind(&CWoWWorld::ProcessPacket, m_World.get(), std::placeholders::_1));

	m_World->EnterWorld(SelectedCharacter);
}

#endif
