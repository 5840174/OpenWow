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
	CWoWClient(IBaseManager& BaseManager, IRenderDevice& RenderDevice, IScene& Scene, const std::string& AuthServerHost, uint16 AuthServerPort = 3724);
	virtual ~CWoWClient();

	void Update(UpdateEventArgs& e);

    // CWoWClient
	void BeginConnect(const std::string& Username, const std::string& Password);

	void OnRealmListSelected(const RealmInfo& SelectedRealm, BigNumber Key);
	
	void OnCharacterSelected(const CInet_CharacterTemplate& SelectedCharacter);

	CWoWWorld& GetWorld() const;

private:
	std::string                     m_Host;
	port_t                          m_Port;
	std::string                     m_Login;

private:
    // Sockets
	std::unique_ptr<CAuthSocket>    m_AuthSocket;
	std::shared_ptr<CWorldSocket>   m_WorldSocket;

	// World & General
	IBaseManager&                   m_BaseManager;
	IRenderDevice&                  m_RenderDevice;
	IScene&                         m_Scene;

	std::unique_ptr<CWoWClientCharactedSelection> m_CharacterSelection;
	std::unique_ptr<CWoWWorld>       m_World;
};

#endif