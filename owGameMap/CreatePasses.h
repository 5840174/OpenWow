#pragma once

void AddSkyPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene);
void AddWDLPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene);
void AddMCNKPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene);

// WMO
void AddWMOPasses(std::shared_ptr<IRenderDevice> device, std::shared_ptr<IRenderTarget> _renderTarget, RenderTechnique * technique, const Viewport * viewport, std::shared_ptr<Scene3D> scene);