#include "stdafx.h"

// General
#include "Client.h"

CWoWClient::CWoWClient(const std::string& AuthServerHost, uint16 AuthServerPort)
{
	m_Host = AuthServerHost;
	m_Port = AuthServerPort;

	
    m_SocketsHandler = std::make_shared<SocketHandler>();
	m_SocketsHandlerThread = std::make_shared<SocketHandlerThread>(*m_SocketsHandler);
	m_SocketsHandlerThread->SetRelease(true);

	while (&m_SocketsHandlerThread->Handler() == nullptr)
		Sleep(1);

	m_SocketsHandlerThread->Handler().RegStdLog(&m_SocketLog);
}

void CWoWClient::BeginConnect(const std::string& Username, const std::string& Password)
{
    m_Username = Utils::ToUpper(Username);

#pragma region Login and Password Sha1Hash
    char loginPasswordUpperCase[256];
    memset(loginPasswordUpperCase, 0x00, sizeof(loginPasswordUpperCase));

    sprintf_s(loginPasswordUpperCase, "%s:%s", Username.c_str(), Password.c_str());
    std::string loginPasswordUpperCaseString = Utils::ToUpper(loginPasswordUpperCase);

    m_LoginPasswordHash.Initialize();
    m_LoginPasswordHash.UpdateData((const uint8*)loginPasswordUpperCaseString.c_str(), loginPasswordUpperCaseString.size());
    m_LoginPasswordHash.Finalize();

    memset(loginPasswordUpperCase, 0x00, sizeof(loginPasswordUpperCase));
    loginPasswordUpperCaseString = "";
#pragma endregion

    m_AuthSocket = std::make_shared<CAuthSocket>(m_SocketsHandlerThread->Handler(), *this);
    m_AuthSocket->Open(getHost(), getPort());

	m_SocketsHandlerThread->Handler().Add(m_AuthSocket);


    //while (m_SocketsHandler->GetCount())
	//{
	//    m_SocketsHandler->Select(1, 0);
	//}
}

void CWoWClient::OnSuccessConnect(BigNumber Key)
{
	m_Key = Key;

    m_AuthSocket->SetDeleteByHandler(true);

    RealmInfo* currRealm = &(m_Realms[0]);

    m_WorldSocket = std::make_shared<CWorldSocket>(m_SocketsHandlerThread->Handler(), *this);
    m_WorldSocket->Open(currRealm->getIP(), currRealm->getPort());

	m_SocketsHandlerThread->Handler().Add(m_WorldSocket);
}

void CWoWClient::ProcessHandler(Opcodes Opcode, CServerPacket & BB)
{
	const auto& handler = m_Handlers.find(Opcode);
	if (handler != m_Handlers.end())
	{
		if (handler->second != nullptr)
		{
			(handler->second).operator()(BB);
		}
	}
}

void CWoWClient::AddWorldHandler(Opcodes Opcode, std::function<void(CServerPacket&)> Handler)
{
	_ASSERT(Handler != nullptr);
	m_Handlers.insert(std::make_pair(Opcode, Handler));
}

void CWoWClient::SendPacket(CClientPacket & Packet)
{
	m_WorldSocket->SendPacket(Packet);
}

void CWoWClient::AddRealm(RealmInfo& _realm)
{
	m_Realms.push_back(_realm);
	_realm.print();
}
