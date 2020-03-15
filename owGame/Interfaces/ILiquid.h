#pragma once

// FORWARD BEGIN
class CLiquid;
// FORWARD END

ZN_INTERFACE ZN_API ILiquidInstanceInitializaton
{
	virtual ~ILiquidInstanceInitializaton() {}

	virtual void LiquidInitialize(std::shared_ptr<CLiquid> LiquidObject, glm::vec3 Position) = 0;
};