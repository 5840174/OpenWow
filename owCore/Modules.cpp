#include "stdafx.h"

// General
#include "Modules.h"

// Additional
#include "Log.h"
#include "Input.h"

// Static

bool Modules::Init()
{
    assert1(Log::Init());

	return true;
}

void Modules::Destroy()
{

}