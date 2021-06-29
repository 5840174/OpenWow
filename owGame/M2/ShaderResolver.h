#pragma once

#ifdef USE_M2_MODELS

uint32 M2GetPixelShaderID(uint32 op_count, uint16 shader_id);
const char* M2GetPixelShaderName(uint32 op_count, uint16 shader_id);

uint16 GetMyShaderID(uint32);

void* GetEffect(SM2_SkinBatch* batch);

int32 GetPixel(const SM2_SkinBatch& batch);

#endif
