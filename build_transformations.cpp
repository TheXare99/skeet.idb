#include "../include_cheat.h"

void __fastcall hooks::build_transformations(void* ecx, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& camera_transform, const int32_t bone_mask, byte* bone_computed)
{
    auto player = static_cast<C_CSPlayer*>(ecx);
	*reinterpret_cast<int*>(uintptr_t(player) + 0x292C) = 0;
    return orig_build_transformations(ecx, hdr, pos, q, camera_transform, bone_mask, bone_computed);
}