#pragma once

#include "AuthSocket.h"
#include "WorldSocket.h"

#include "SHA1.h"
#include "RealmInfo.h"

class CWoWClient : public std::enable_shared_from_this<CWoWClient>
{
public:
	CWoWClient(const std::string& AuthServerHost, uint16 AuthServerPort = 3724);

    // CWoWClient
	void BeginConnect(const std::string& Username, const std::string& Password);
	void OnSuccessConnect(BigNumber Key);

    //
	void AddRealm(RealmInfo& _realm);

	// Getters
    
  
    const uint32 getClientBuild() const { return 5875; }


	const std::string& getHost() const { return m_Host; }
    port_t getPort() const { return m_Port; }


	const std::string& GetLogin() const { return m_Username; }
    const SHA1Hash&    GetLoginPasswordHash() const { return m_LoginPasswordHash; }

	BigNumber* getKey() { return &m_Key; }

private:
	std::string                     m_Host;
	port_t                          m_Port;

private: // Used login data. Don't keep fucking password in string. This is secure :)
	std::string                     m_Username;
    SHA1Hash                        m_LoginPasswordHash;


	BigNumber                       m_Key;

	std::vector<RealmInfo>          m_Realms;

    // Sockets controller
    StdoutLog                       m_SocketLog;
    std::shared_ptr<SocketHandler>  m_SocketsHandler;

    // Sockets
	std::shared_ptr<CAuthSocket>    m_AuthSocket;
    std::shared_ptr<CWorldSocket>   m_WorldSocket;

};