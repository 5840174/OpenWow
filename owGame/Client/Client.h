#pragma once

#ifdef ENABLE_WOW_CLIENT

#include "AuthSocket.h"
#include "WorldSocket.h"

#include "CharacterSelection.h"
#include "World.h"

#include "RealmInfo.h"
#include "Templates/CharacterTemplate.h"

class ZN_API CWoWClient 
	: public std::enable_shared_from_this<CWoWClient>
{
public:
	CWoWClient(IScene& Scene, const std::string& AuthServerHost, uint16 AuthServerPort = 3724);
	virtual ~CWoWClient();

	void Update(/*std::future<void> PromiseExiter*/);

    // CWoWClient
	void BeginConnect(const std::string& Username, const std::string& Password);

	void OnRealmListSelected(const RealmInfo& SelectedRealm, BigNumber Key);
	
	void OnCharacterSelected(const CInet_CharacterTemplate& SelectedCharacter);

private:
	std::string                                   m_Host;
	port_t                                        m_Port;
	std::string                                   m_Login;

private:
	std::thread                                   m_UpdateThread;
	std::promise<void>					          m_UpdateThreadExiter;

	std::unique_ptr<CAuthSocket>                  m_AuthSocket;
	std::shared_ptr<CWorldSocket>                 m_WorldSocket;

	IScene&                                       m_Scene;
	std::unique_ptr<CWoWClientCharactedSelection> m_CharacterSelection;
	std::unique_ptr<CWoWWorld>                    m_World;
};

#endif