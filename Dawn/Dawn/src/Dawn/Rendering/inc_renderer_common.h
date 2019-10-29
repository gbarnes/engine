#pragma once
#include "Core/Math.h"
#include "inc_common.h"

namespace Dawn
{
	enum class RenderFlags {
		MaterialId = 0x20,
		Depth = 0x30,
		NoShader = 0x40
	};

	// Maximum of 9 RenderLayers!
	enum class RenderLayer 
	{
		StaticGeom = 0x0,
		DynamicGeom = 0x1,
		Layer2 = 0x2,
		Layer3 = 0x3,
		HUD = 0x4,
		Layer5 = 0x5,
		Layer6 = 0x6,
		Layer7 = 0x7,
		Screen = 0x8
	};

	// these only work when not using 
	// any edge casses such as 0.0f or INFINITY
	// http://www.fox-toolkit.org/ftp/fasthalffloatconversion.pdf
	// https://www.gamedev.net/forums/topic/667485-c-cant-find-a-match-for-16-bit-float-and-how-to-convert-32-bit-float-to-16-bit-one/
	// https://cellperformance.beyond3d.com/articles/2006/07/branchfree_implementation_of_h_1.html
	// I decided to use the branch free version of mike acton see the last link for this
	// I still need to investigate a bit how the solution works exactly
	/*
	static s16 FloatToFloat16(float value)
	{
		union
		{
			f32 f;
			i32 i;
		};

		s16   fltInt16;
		f = value;

		// Explanation:
		// we first shift the entire exponent and mantissa
		// 13 bits backwards in order to cut the mantissa at the back
		// the first bit (sign) is being ignored.
		// Afterwards we subtract the mask 011 10... from the prior part
		// to remove unneccessary exponent parts.
		// At the end we combine this with the sign.
		fltInt16 = ((i & 0x7fffffff) >> 13) - (0x38000000 >> 13);
		fltInt16 |= ((i & 0x80000000) >> 16);

		return fltInt16;
	}

	static float Float16ToFloat(s16 fltInt16)
	{
		union
		{
			f32 f;
			i32 i;
		};

		i32 fltInt32 = ((fltInt16 & 0x8000) << 16);
		fltInt32 |= ((fltInt16 & 0x7fff) << 13) + 0x38000000;
		i = fltInt32;
		return f;
	}
	*/


	// 64 Bit Key
	//			Layer (3 Bits)
	//		  		  ^	       16 Bit                     32 Bit
	//		  		  |  [    Depth Id   ] [           Material ID           ]
	// 00000000 00000LLL 00000000 00000000 00000000 00000000 00000000 00000000
	// 
	// The depth should always be normalized within 0.0 and 1.0
	static u64 GenDrawKey64(bool InIsValid, u32 MaterialId, RenderLayer Layer, float Depth)
	{
		// todo --- add different kinds of key generations such as sorted by depth first or 
		//	        material by switching the positions in the bitmask.
		//		    add more methods to generate smaller keys for other buckets.

		union
		{
			float DepthToFloat;
			u32 DepthToInt;
		};

		DepthToFloat = Depth;

		// u32 MatId = u32((Key & 0x00000000FFFFFFFF));
		// s16 DepthExt = s16((Key & 0x0000FFFF00000000) >> 32);

		// Floats are represented by 32 Bits. 
		// Sign (+ or minus)
		// ^
		// 0000 0000 0000 0000 0000 0000 0000 0000
		//  [Exponent][       Mantissa           ]
		//
		// In order to be able to store the depth in the key we 
		// will convert it into a 16-bit integer value.
		u16 Depth16Bit = Math::half_from_float(DepthToInt);
		u64 Key = u64((u8)InIsValid) << 51 | u64((u8)Layer) << 48 | (u64(Depth16Bit) << 32) | u64(MaterialId);

		return Key;
	}


	struct DrawKey64
	{
		RenderLayer Layer;			//3 Bits (9 Layers)
		float Depth;				//16 Bits
		u32 MaterialId;				//32 Bits 
		bool bIsValid = false;		//1 Bit
		bool _pad[3];				// pad out the struct!
	};

	static DrawKey64 DecodeDrawKey64(u64 InKey)
	{
		union
		{
			float DepthToFloat;
			u32 DepthToInt;
		};

		DrawKey64 Key = {};

		Key.MaterialId = u32((InKey & 0x00000000FFFFFFFF));
		DepthToInt = Math::half_to_float(u16((InKey & 0x0000FFFF00000000) >> 32));
		Key.Depth = DepthToFloat;
		Key.Layer = (RenderLayer)(u8((InKey & 0x0007000000000000) >> 48));
		Key.bIsValid = (bool((InKey & 0x0008000000000000) >> 51));

		return Key;
	}
}


#define GEN_INVALID_DRAW_KEY() GenDrawKey(false, 0, RenderLayer::Layer0, 0.0f)
#define DECODE_DRAW_KEY_64(InKey, ResourceSystem) ResourceSystem->FindMaterial()