#pragma once

class chams : public singleton<chams>
{
public:
	void draw_record( IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* custom_bone_to_world );
	bool draw_hst( lag_record_t& record, matrix3x4_t* out );
	bool draw_ch( IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld );
	void dark_mode() const;
};
