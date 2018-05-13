#include "stdafx.h"

// Include
#include "RenderDevice.h"


// General
#include "Technique.h"

// Additional

string ProcessShader(File& f)
{
	if (!f.Open())
	{
		Log::Error("Error open shader [%s].", f.Path_Name().c_str());
		return "";
	}

	string data = "";

	while (!f.IsEof())
	{
		string line = f.ReadLine();

		// Skip empty lines
		if (line.length() == 0)
		{
			continue;
		}

		// Find directive
		if (line[0] == '#' && line[1] == 'i' && line[2] == 'n' && line[3] == 'c' && line[4] == 'l')
		{
			uint32_t firstBracketPosition = line.find('"');
            assert1(firstBracketPosition != string::npos);

			uint32_t lastBracketPosition = line.find_last_of('"');
			assert1(firstBracketPosition != lastBracketPosition);

			string inludeFileName = line.substr(firstBracketPosition + 1, lastBracketPosition - firstBracketPosition - 1);
			data += ProcessShader(File(f.Path() + inludeFileName)) + '\n';

			continue;
		}

		data += line + '\n';
	}

	return data;
}

//

Technique::Technique(cstring _fileName)
{
	string shVS = ProcessShader(File(_fileName + ".vs"));
	string shFS = ProcessShader(File(_fileName + ".fs"));

    Process(_fileName, shVS.c_str(), shFS.c_str(), nullptr);

    InitBaseUniforms();
}

Technique::Technique(cstring _fileNameVS, cstring _fileNameFS)
{
	string shVS = ProcessShader(File(_fileNameVS));
	string shFS = ProcessShader(File(_fileNameFS));

    Process(_fileNameVS, shVS.c_str(), shFS.c_str(), nullptr);

    InitBaseUniforms();
}

Technique::Technique(cstring _fileNameVS, cstring _fileNameFS, cstring _fileNameGS)
{
    string shVS = ProcessShader(File(_fileNameVS));
    string shFS = ProcessShader(File(_fileNameFS));
    string shGS = ProcessShader(File(_fileNameGS));

    Process(_fileNameVS, shVS.c_str(), shFS.c_str(), shGS.c_str());

    InitBaseUniforms();
}

Technique::~Technique()
{

}

void Technique::Process(cstring fileName, const char* vertexShaderSrc, const char* fragmentShaderSrc, const char* geometryShaderSrc)
{
    shader = _Render->r->createShader(vertexShaderSrc, fragmentShaderSrc, geometryShaderSrc, nullptr, nullptr, nullptr);
    if (_Render->r->getShaderLog().empty())
    {
        Log::Green("Shader[%s]: Successfull. Id [%d].", fileName.c_str(), shader->oglProgramObj);
    }
    else
    {
        Log::Error("Shader[%s]: Error.", fileName.c_str());
        Log::Error(_Render->r->getShaderLog().c_str());
    }
}
