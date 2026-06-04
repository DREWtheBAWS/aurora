#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// All data is valid only for the duration of the callback.
// vertData, posArray, and indices point into Aurora's internal frame buffers.
typedef struct AuroraGxCaptureDraw {
  // Packed big-endian vertex bytes from the GX FIFO
  const uint8_t* vertData;
  uint32_t       vertCount;
  uint32_t       vertStride;

  // Position attribute descriptor
  uint32_t posOffset;       // byte offset of the pos field (or pos index) within each vertex
  uint8_t  posCompCnt;      // GXCompCnt: GX_POS_XY=0, GX_POS_XYZ=1
  uint8_t  posCompType;     // GXCompType: GX_U8=0,GX_S8=1,GX_U16=2,GX_S16=3,GX_F32=4
  uint8_t  posFrac;         // fixed-point fractional bits (0 for float)
  uint8_t  posAttrType;     // GXAttrType: GX_DIRECT=1, GX_INDEX8=2, GX_INDEX16=3

  // Indirect position array (only meaningful when posAttrType != GX_DIRECT)
  const uint8_t* posArray;
  uint32_t       posArrayStride;
  bool           posArrayLittleEndian;

  // Per-vertex matrix index (byte 0 of each vertex / 3 = matrix index)
  bool     hasPnMtxIdx;
  uint32_t currentPnMtx;   // used when hasPnMtxIdx == false

  // Position matrices: [matrixIndex][row][col], row-major, 3 rows x 4 cols
  float pnMtx[10][3][4];

  // Triangle index list (native endian u16), always a multiple of 3
  const uint16_t* indices;
  uint32_t        indexCount;

  // Render context — useful for filtering draws to specific passes
  uint8_t projType;        // GXProjectionType: GX_PERSPECTIVE=0, GX_ORTHOGRAPHIC=1
  float   viewportWidth;   // render viewport dimensions in pixels
  float   viewportHeight;

  // Full 4x4 GX projection matrix at draw time (row-major).
  // Reconstructed from the 6 XF projection params by Aurora.
  float projMtx[4][4];

  // Texture coordinate 0 attribute descriptor (same layout convention as pos).
  // tex0AttrType == GX_NONE means no tex coords present for this draw.
  uint32_t       tex0Offset;       // byte offset of tex0 within each vertex
  uint8_t        tex0CompCnt;      // GXCompCnt: GX_TEX_S=0, GX_TEX_ST=1
  uint8_t        tex0CompType;     // GXCompType: GX_U8=0,GX_S8=1,GX_U16=2,GX_S16=3,GX_F32=4
  uint8_t        tex0Frac;         // fixed-point fractional bits (0 for float)
  uint8_t        tex0AttrType;     // GXAttrType: GX_NONE=0,GX_DIRECT=1,GX_INDEX8=2,GX_INDEX16=3
  const uint8_t* tex0Array;        // indirect array (only when tex0AttrType != GX_DIRECT)
  uint32_t       tex0ArrayStride;

  // Currently-bound texture for GX_TEXMAP0 (raw WGPUTextureView, cast at call site).
  // Null when no texture is bound or the texture has no GPU-side handle yet.
  void* tex0View;
  bool  tex0HasAlpha;  // true when the GX texture format carries an alpha channel

  // Alpha compare state at draw time.  alphaComp0 == GX_ALWAYS means no alpha test.
  uint8_t alphaComp0;  // GXCompare for operand 0
  uint8_t alphaRef0;   // reference value 0 (0-255)
  uint8_t alphaComp1;  // GXCompare for operand 1
  uint8_t alphaRef1;   // reference value 1 (0-255)
  uint8_t alphaOp;     // GXAlphaOp combining comp0 and comp1
} AuroraGxCaptureDraw;

typedef void (*AuroraGeometryCaptureCallback)(const AuroraGxCaptureDraw* draw, void* userdata);

void aurora_set_geometry_capture(AuroraGeometryCaptureCallback callback, void* userdata);

#ifdef __cplusplus
}
#endif
