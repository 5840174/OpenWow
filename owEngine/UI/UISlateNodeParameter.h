#pragma once

// Common UI
#include "UIBaseNode.h"
#include "UIText.h"
#include "UITexture.h"

// Slate
#include "UISlateEditorInterfaces.h"
#include "UISlateConnectionPoint.h"

// FORWARD BEGIN
class CUISlateEditor;
class CUISlateNode;
// FORWARD END

class CUISlateNodeParameter : 
    public CUIBaseNode,
    public IUISlateConnectionable
{
	typedef CUIBaseNode base;
public:
	CUISlateNodeParameter(std::weak_ptr<CUISlateEditor> Editor);
	virtual ~CUISlateNodeParameter();

	// CUISlateNodeParameter
    void Initialize();
	void CreateDefault();

    void SetText(const std::string& Text);

    // IUISlateConnectionable
    vec2 GetConnectPoint() const;
    BoundingRect GetConnectRectangle() const;
    LineDefaultDirection GetConnectDirection() const;

	// CUIBaseNode
    virtual glm::vec2 GetSize() const override;
    virtual std::vector<std::shared_ptr<CUIBaseNode>> GetChilds() const override;

    // Input events
    bool OnMouseButtonPressed(MouseButtonEventArgs& e) override;
    void OnMouseButtonReleased(MouseButtonEventArgs& e) override;

protected:
    std::shared_ptr<CUITextureNode>     m_Background;
    
    // Weak refs
    std::weak_ptr<CUISlateEditor>       m_Editor;

private:
    std::shared_ptr<CUISlateConnectionPoint> m_LinePoint;

    bool                                m_IsLinePointOpened;
};