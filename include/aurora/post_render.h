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

#ifdef __cplusplus
}
#endif
