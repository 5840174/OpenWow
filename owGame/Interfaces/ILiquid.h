#pragma once

// FORWARD BEGIN
class CLiquid;
// FORWARD END

ZN_INTERFACE ZN_API ILiquidInstanceInitializaton
{
	virtual ~ILiquidInstanceInitializaton() {}

	virtual void Initialize(std::shared_ptr<CLiquid> LiquidObject, vec3 Position) = 0;
};