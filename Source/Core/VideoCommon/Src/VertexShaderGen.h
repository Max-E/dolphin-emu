// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2
// Refer to the license.txt file included.

#ifndef GCOGL_VERTEXSHADER_H
#define GCOGL_VERTEXSHADER_H

#include "XFMemory.h"
#include "VideoCommon.h"

// TODO should be reordered
#define SHADER_POSITION_ATTRIB  0
#define SHADER_POSMTX_ATTRIB    1
#define SHADER_NORM0_ATTRIB     2
#define SHADER_NORM1_ATTRIB     3
#define SHADER_NORM2_ATTRIB     4
#define SHADER_COLOR0_ATTRIB    5
#define SHADER_COLOR1_ATTRIB    6

#define SHADER_TEXTURE0_ATTRIB  8
#define SHADER_TEXTURE1_ATTRIB  9
#define SHADER_TEXTURE2_ATTRIB  10
#define SHADER_TEXTURE3_ATTRIB  11
#define SHADER_TEXTURE4_ATTRIB  12
#define SHADER_TEXTURE5_ATTRIB  13
#define SHADER_TEXTURE6_ATTRIB  14
#define SHADER_TEXTURE7_ATTRIB  15



// shader variables
#define I_POSNORMALMATRIX       "cpnmtx"
#define I_PROJECTION            "cproj"
#define I_MATERIALS             "cmtrl"
#define I_LIGHTS                "clights"
#define I_TEXMATRICES           "ctexmtx"
#define I_TRANSFORMMATRICES     "ctrmtx"
#define I_NORMALMATRICES        "cnmtx"
#define I_POSTTRANSFORMMATRICES "cpostmtx"
#define I_DEPTHPARAMS           "cDepth" // farZ, zRange, scaled viewport width, scaled viewport height

#define C_POSNORMALMATRIX        0
#define C_PROJECTION            (C_POSNORMALMATRIX + 6)
#define C_MATERIALS             (C_PROJECTION + 4)
#define C_LIGHTS                (C_MATERIALS + 4)
#define C_TEXMATRICES           (C_LIGHTS + 40)
#define C_TRANSFORMMATRICES     (C_TEXMATRICES + 24)
#define C_NORMALMATRICES        (C_TRANSFORMMATRICES + 64)
#define C_POSTTRANSFORMMATRICES (C_NORMALMATRICES + 32)
#define C_DEPTHPARAMS           (C_POSTTRANSFORMMATRICES + 64)
#define C_VENVCONST_END         (C_DEPTHPARAMS + 1)
const s_svar VSVar_Loc[] = {  {I_POSNORMALMATRIX, C_POSNORMALMATRIX, 6 },
						{I_PROJECTION , C_PROJECTION, 4  },
						{I_MATERIALS, C_MATERIALS, 4 },
						{I_LIGHTS, C_LIGHTS, 40 },
						{I_TEXMATRICES, C_TEXMATRICES, 24 },
						{I_TRANSFORMMATRICES , C_TRANSFORMMATRICES, 64  },
						{I_NORMALMATRICES , C_NORMALMATRICES, 32  },
						{I_POSTTRANSFORMMATRICES, C_POSTTRANSFORMMATRICES, 64 },
						{I_DEPTHPARAMS, C_DEPTHPARAMS, 1 },
						};
template<bool safe>
class _VERTEXSHADERUID
{
#define NUM_VSUID_VALUES_SAFE 25
public:
	u32 values[safe ? NUM_VSUID_VALUES_SAFE : 9];

	_VERTEXSHADERUID()
	{
	}

	_VERTEXSHADERUID(const _VERTEXSHADERUID& r)
	{
		for (size_t i = 0; i < sizeof(values) / sizeof(u32); ++i) 
			values[i] = r.values[i]; 
	}

	int GetNumValues() const 
	{
		if (safe) return NUM_VSUID_VALUES_SAFE;
		else return (((values[0] >> 23) & 0xf) * 3 + 3) / 4 + 3; // numTexGens*3/4+1
	}

	bool operator <(const _VERTEXSHADERUID& _Right) const
	{
		if (values[0] < _Right.values[0])
			return true;
		else if (values[0] > _Right.values[0])
			return false;

		int N = GetNumValues();
		for (int i = 1; i < N; ++i) 
		{
			if (values[i] < _Right.values[i])
				return true;
			else if (values[i] > _Right.values[i])
				return false;
		}

		return false;
	}

	bool operator ==(const _VERTEXSHADERUID& _Right) const
	{
		if (values[0] != _Right.values[0])
			return false;

		int N = GetNumValues();
		for (int i = 1; i < N; ++i)
		{
			if (values[i] != _Right.values[i])
				return false;
		}

		return true;
	}
};
typedef _VERTEXSHADERUID<false> VERTEXSHADERUID;
typedef _VERTEXSHADERUID<true> VERTEXSHADERUIDSAFE;


// components is included in the uid.
char* GenerateVSOutputStruct(char* p, u32 components, API_TYPE api_type);
const char *GenerateVertexShaderCode(u32 components, API_TYPE api_type);

void GetVertexShaderId(VERTEXSHADERUID *uid, u32 components);
void GetSafeVertexShaderId(VERTEXSHADERUIDSAFE *uid, u32 components);

// Used to make sure that our optimized vertex shader IDs don't lose any possible shader code changes
void ValidateVertexShaderIDs(API_TYPE api, VERTEXSHADERUIDSAFE old_id, const std::string& old_code, u32 components);

#endif // GCOGL_VERTEXSHADER_H
