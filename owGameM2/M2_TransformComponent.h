#pragma once

// FORWARD BEGIN
class CM2_Base_Instance;
// FORWARD END

class
    __declspec(UUID_TransformComponent)
    CM2_TransformComponent : public CTransformComponent3D
{
    typedef CTransformComponent3D base;
public:
    CM2_TransformComponent(std::shared_ptr<SceneNode3D> OwnerNode);
    virtual ~CM2_TransformComponent();

protected:
    std::shared_ptr<CM2_Base_Instance> GetOwnerNode();

    // CTransformComponent
    virtual void UpdateLocalTransform() override;
};