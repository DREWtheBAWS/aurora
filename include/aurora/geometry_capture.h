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
} AuroraGxCaptureDraw;

typedef void (*AuroraGeometryCaptureCallback)(const AuroraGxCaptureDraw* draw, void* userdata);

void aurora_set_geometry_capture(AuroraGeometryCaptureCallback callback, void* userdata);

#ifdef __cplusplus
}
#endif
