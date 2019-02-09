#pragma once

class IMesh;
class Camera;
class Render3DEventArgs;
class IVisitor;

class SceneNode3D : public Object, public ILoadable, public std::enable_shared_from_this<SceneNode3D>
{
	typedef Object base;
public:
	typedef std::vector<std::shared_ptr<SceneNode3D>> NodeList;
	typedef std::multimap<std::string, std::shared_ptr<SceneNode3D>> NodeNameMap;
	typedef std::vector<std::shared_ptr<IMesh>> MeshList;

public:
	explicit SceneNode3D(cmat4 localTransform = mat4(1.0f));
	virtual ~SceneNode3D();

	/**
	 * Assign a name to this scene node so that it can be searched for later.
	 */
	virtual cstring GetName() const;
	virtual void SetName(cstring name);

	// Translate
	void SetTranslate(cvec3 _translate);
	cvec3 GetTranslation() const;

	// Rotate
	void SetRotation(cvec3 _rotate);
	cvec3 GetRotation() const;

	// Rotate Quaternion
	void SetRotationQuaternion(cquat _rotate);
	cquat GetRotationQuaternion() const;

	// Scale
	void SetScale(cvec3 _scale);
	cvec3 GetScale() const;

	// Bounds
	void SetBounds(BoundingBox _bbox);
	cbbox GetBounds() const;


	/**
	 * Gets the scene node's local transform (relative to it's parent world transform).
	 */
	mat4 GetLocalTransform() const;
	/**
	 * Gets the inverse of the local transform (relative to it's parent world transform).
	 */
	mat4 GetInverseLocalTransform() const;
	void SetLocalTransform(cmat4 localTransform);



	/**
	 * Gets the scene node's world transform (concatenated with parents world transform)
	 * This function should be used sparingly as it is computed every time
	 * it is requested.
	 */
	mat4 GetWorldTransfom() const;
	/**
	 * Gets the inverse world transform of this scene node.
	 * Use this function sparingly as it is computed every time it is requested.
	 */
	mat4 GetInverseWorldTransform() const;
	void SetWorldTransform(cmat4 worldTransform);



	/**
	 * Add a child node to this node.
	 * NOTE: Circular references are not checked!
	 * A scene node takes ownership of it's children.
	 * If you delete the parent node, all of its children will also be deleted.
	 */
	virtual void AddChild(std::shared_ptr<SceneNode3D> childNode);
	virtual void RemoveChild(std::shared_ptr<SceneNode3D> childNode);
	virtual void SetParent(std::weak_ptr<SceneNode3D> parentNode);
	virtual NodeList GetChilds();

	/**
	 * Add a mesh to this scene node.
	 * The scene node does not take ownership of a mesh that is set on a mesh
	 * as it is possible that the same mesh is added to multiple scene nodes.
	 * Deleting the scene node does not delete the meshes associated with it.
	 */
	virtual void AddMesh(std::shared_ptr<IMesh> mesh);
	virtual void RemoveMesh(std::shared_ptr<IMesh> mesh);
	virtual const MeshList&  GetMeshes();

	/**
	 * Called before all others calls
	 */
	virtual void UpdateCamera(const Camera* camera);

	/**
	 * Allow a visitor to visit this node.
	 */
	virtual bool Accept(IVisitor& visitor);

	/**
	 * Updatable
	 */
	virtual void OnUpdate(UpdateEventArgs& e);

	/**
	 * Useful for culling
	 */
	bool checkFrustum(const Camera* _camera) const;
	bool checkDistance2D(cvec3 _camPos, float _distance) const;
	bool checkDistance(cvec3 _camPos, float _distance) const;

	// ILoadableObject
	virtual bool PreLoad() override;
	virtual bool Load() override;
	virtual bool Delete() override;

	void setLoadingBegin() override;
	bool isLoadingBegin() const override;
	void setLoaded() override;
	bool isLoaded() const override;

	virtual uint32 getPriority() const override;

protected:
	virtual mat4 GetParentWorldTransform() const;

	virtual void UpdateLocalTransform();
	virtual void UpdateWorldTransform();
	virtual void UpdateBounds();

private:
	std::string         m_Name;
	SceneNodeTypes      m_Type;

	// Transforms node from parent's space to world space for rendering.
	mat4                m_LocalTransform;
	mat4                m_InverseLocalTransform;	// This is the inverse of the local -> world transform.

	mat4                m_WorldTransform;
	mat4                m_InverseWorldTransform;

	vec3                m_Translate;
	vec3				m_Rotate;
	quat				m_RotateQuat;
	bool                m_IsRotateQuat;
	vec3                m_Scale;
	BoundingBox         m_Bounds;
	
	

	std::weak_ptr<SceneNode3D>  m_pParentNode;
	NodeList                  m_Children;
	NodeNameMap               m_ChildrenByName;
	std::mutex                m_ChildMutex;
	MeshList                  m_Meshes;
	std::mutex                m_MeshMutex;

private:
	std::atomic<bool>         m_IsLoadingBegin;
	std::atomic<bool>         m_IsLoaded;
};