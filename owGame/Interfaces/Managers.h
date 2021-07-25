#pragma once

ZN_INTERFACE ZN_API IWoWSceneNode
{
	virtual void UpdateWoW(const UpdateEventArgs& e) = 0;
};