#include "stdafx.h"

std::shared_ptr<CWoWClient> WoWClient;

void S_CharEnum(CByteBuffer& _buff)
{
    uint8 charCnt;
    _buff.readBytes(&charCnt, sizeof(uint8));

    /*CharacterTemplate* chars = new CharacterTemplate[charCnt];
    for (uint8 i = 0; i < charCnt; i++)
    {
        chars[i].TemplateFill(_buff);
        chars[i].TemplatePrint();
    }

    // Select character
    CharacterTemplate& charTemplate = chars[0];
    vec3 charPosition = fromGameToReal(vec3(charTemplate.Y, charTemplate.X, charTemplate.Z));

    CByteBuffer bb;
    bb << (uint64)charTemplate.GUID;
    m_authWorldController->getWorldSocket()->SendData(CMSG_PLAYER_LOGIN, bb);*/


    //Orc
    /*ByteBuffer bb;
    bb.Write("Txtyryer", 9);
    bb.Write((uint8)2);
    bb.Write((uint8)1);
    bb.Write((uint8)0);
    bb.Write((uint8)15);
    bb.Write((uint8)6);
    bb.Write((uint8)3);
    bb.Write((uint8)6);
    bb.Write((uint8)5);
    bb.Write((uint8)0);
    SendData(CMSG_CHAR_CREATE, bb);*/

}

int main(int argumentCount, char* arguments[])
{
	_BaseManager = std::make_shared<CBaseManager>();

	std::shared_ptr<CSettings> settings = std::make_shared<CSettings>();
	AddManager<ISettings>(settings);
	settings->AddDefaults();
		
	std::shared_ptr<CLog> log = std::make_shared<CLog>();
	AddManager<ILog>(log);

	std::shared_ptr<CConsole> console = std::make_shared<CConsole>();
	AddManager<IConsole>(console);
	console->AddCommonCommands();
		
	std::shared_ptr<IFilesManager> filesManager = std::make_shared<CFilesManager>();
	AddManager<IFilesManager>(filesManager);

	std::shared_ptr<IFilesStorage> localFilesGamedata = std::make_shared<CLocalFilesStorage>("D:\\_programming\\OpenWow\\_gamedata\\");
	filesManager->RegisterFilesStorage(localFilesGamedata);

    //WoWClient = std::make_shared<CWoWClient>("192.168.1.50", 3724);
    //WoWClient->BeginConnect("admin", "admin");

    //authWorldController->getWorldSocket()->AddHandler(SMSG_CHAR_ENUM, std::bind(&S_CharEnum, std::placeholders::_1));

    while (true)
    {
        Sleep(16);
    }

	return 0;
}
