#pragma once
#include <webgpu/webgpu.h>

#ifdef __cplusplus
extern "C" {
#endif

// Fires once per frame after all GX rendering is complete and before the
// EFB→swapchain copy. The callback may record additional compute or render
// passes into `encoder`. Do NOT call wgpuQueueSubmit — Aurora submits the
// finished command buffer itself after the callback returns.
typedef void (*AuroraPostRenderCallback)(WGPUDevice device,
                                         WGPUCommandEncoder encoder,
                                         void* userdata);

void aurora_set_post_render_callback(AuroraPostRenderCallback cb, void* userdata);

// Fires once per frame just before the LAST perspective→orthographic GX render
// pass transition (i.e., just before the real UI/HUD draws begin).
// Using the last such boundary avoids misfiring on intermediate screen effects
// (e.g. sun haze, fade overlays) that happen to use orthographic projection
// mid-frame and would otherwise let subsequent 3D rendering overwrite the result.
// If GX never transitions from perspective to orthographic this frame the
// callback does not fire. Same signature as AuroraPostRenderCallback.
void aurora_set_pre_ui_callback(AuroraPostRenderCallback cb, void* userdata);

// The depth texture Aurora produced for the main GX render pass.
// Valid to bind as a texture inside a post-render callback.
// Becomes invalid after the frame ends — do not cache across frames.
WGPUTextureView aurora_get_depth_texture_view(void);

// The underlying WGPUTexture for the depth buffer. Use this to create a
// WGPUTextureAspect_DepthOnly view for shader binding (texture_depth_2d).
WGPUTexture aurora_get_depth_texture(void);

// The projection matrix GX used for the most recent draw call.
// Written row-major into out[16] (i.e. out[row*4 + col]).
void aurora_get_proj_matrix(float out[16]);

// The EFB color texture Aurora rendered the GX scene into (post MSAA-resolve).
// This is the texture that will be copied to the swapchain after the callback returns.
// Valid inside a post-render callback only; do not cache across frames.
WGPUTextureView aurora_get_color_texture_view(void);
WGPUTexture     aurora_get_color_texture(void);

// Returns Aurora's device queue (non-owning — do NOT call wgpuQueueRelease on it).
// Use this instead of wgpuDeviceGetQueue() inside post-render callbacks to avoid
// reference-counting issues introduced in newer Dawn versions.
WGPUQueue aurora_get_queue(void);

#ifdef __cplusplus
}
#endif
