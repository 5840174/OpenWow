#pragma once

#include "M2_Part_Camera.h"

// FORWARD BEGIN
class CM2;
class CM2_Base_Instance;
// FORWARD END

class ZN_API CM2CameraComponent
	: public ICameraComponent3D
	, public CSceneNodeComponentBase
{
public:
	CM2CameraComponent(const CM2_Base_Instance& OwnerNode, const std::shared_ptr<const CM2_Part_Camera>& M2Camera);
	virtual ~CM2CameraComponent();

	void SetOrigin(glm::vec3 Position, float Orientation);

	// ICameraComponent3D
	void DoMoveFront(float Value);
	void DoMoveBack(float Value);
	void DoMoveLeft(float Value);
	void DoMoveRight(float Value);

	void SetPosition(glm::vec3 Position) override;
	glm::vec3 GetPosition() const override;
	void SetDirection(glm::vec3 Direction) override;
	glm::vec3 GetDirection() const override;

	void SetYaw(float Yaw) override;
	void AddYaw(float Yaw) override;
	float GetYaw() const override;
	void SetPitch(float Pitch) override;
	void AddPitch(float Pitch) override;
	float GetPitch() const override;

	void SetPerspectiveProjection(float fovy, float aspect, float zNear, float zFar) override;
	void SetOrthographicProjection(float aspect, float left, float right, float top, float bottom, float zNear, float zFar) override;

	const glm::mat4& GetViewMatrix() const override;
	const glm::mat4& GetInverseViewMatrix() const override;
	const glm::mat4& GetProjectionMatrix() const override;
	const glm::mat4& GetInverseProjectionMatrix() const override;

	const Frustum& GetFrustum() const;

	// ISceneNodeComponent
	void OnMessage(const ISceneNodeComponent* Component, ComponentMessageType Message) override;
	void Update(const UpdateEventArgs& e) override;

protected:
	const CM2_Base_Instance& GetM2OwnerNode() const;

private:
	std::shared_ptr<const CM2_Part_Camera>          m_M2Camera;

	glm::vec3                                       m_OriginPosition;
	float                                           m_OriginOrientation;
	glm::mat4                                       m_OriginMatrix;

	glm::vec3                                       m_Position;
	glm::vec3                                       m_Direction;

	glm::mat4                                       m_View;
	glm::mat4                                       m_Inverse_View;

	float                                           m_Aspect;
	glm::mat4                                       m_Projection;
	glm::mat4                                       m_Inverse_Projection;

	Frustum                                         m_Frustum;
};