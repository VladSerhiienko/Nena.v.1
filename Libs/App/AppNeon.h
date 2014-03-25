


#ifndef __NENA_APPNEON_INCLUDED__
#define __NENA_APPNEON_INCLUDED__


namespace Nena
{
	namespace Utilities
	{
		namespace Neon
		{
			typedef union __declspec(intrin_type) __declspec(align(8)) __n64
			{
				unsigned __int64 u64[1];
				unsigned __int32 u32[2];
				unsigned __int16 u16[4];
				unsigned __int8 u8[8];
				__int64 i64[1];
				__int32 i32[2];
				__int16 i16[4];
				__int8 i8[8];
				float f32[2];
			} __n64, n64, N64;

			typedef union __declspec(intrin_type) __declspec(align(8)) __n128
			{
				unsigned __int64 u64[2];
				unsigned __int32 u32[4];
				unsigned __int16 u16[8];
				unsigned __int8 u8[16];
				__int64 i64[2];
				__int32 i32[4];
				__int16 i16[8];
				__int8 i8[16];
				float f32[4];

				struct
				{
					__n64 n128_n64[2];
				};

			} __n128, n128, N128;

			typedef struct __n64x2
			{
				__n64 val[2];
			} __n64x2, n64x2, N64x2;

			typedef struct __n64x3
			{
				__n64 val[3];
			} __n64x3, n64x3, N64x3;

			typedef struct __n64x4
			{
				__n64 val[4];
			} __n64x4, n64x4, N64x4;

			typedef struct __n128x2
			{
				__n128 val[2];
			} __n128x2, n128x2, N128x2;

			typedef struct __n128x3
			{
				__n128 val[3];
			} __n128x3, n128x3, N128x3;

			typedef struct __n128x4
			{
				__n128 val[4];
			} __n128x4, n128x4, N128x4;

			typedef unsigned __int8  poly8_t;
			typedef unsigned __int16 poly16_t;
			typedef __n64    float32x2_t;
			typedef __n64x2  float32x2x2_t;
			typedef __n64x3  float32x2x3_t;
			typedef __n64x4  float32x2x4_t;
			typedef __n64    int8x8_t;
			typedef __n64x2  int8x8x2_t;
			typedef __n64x3  int8x8x3_t;
			typedef __n64x4  int8x8x4_t;
			typedef __n64    int16x4_t;
			typedef __n64x2  int16x4x2_t;
			typedef __n64x3  int16x4x3_t;
			typedef __n64x4  int16x4x4_t;
			typedef __n64    int32x2_t;
			typedef __n64x2  int32x2x2_t;
			typedef __n64x3  int32x2x3_t;
			typedef __n64x4  int32x2x4_t;
			typedef __n64    int64x1_t;
			typedef __n64x2  int64x1x2_t;
			typedef __n64x3  int64x1x3_t;
			typedef __n64x4  int64x1x4_t;
			typedef __n64    poly8x8_t;
			typedef __n64x2  poly8x8x2_t;
			typedef __n64x3  poly8x8x3_t;
			typedef __n64x4  poly8x8x4_t;
			typedef __n64    poly16x4_t;
			typedef __n64x2  poly16x4x2_t;
			typedef __n64x3  poly16x4x3_t;
			typedef __n64x4  poly16x4x4_t;
			typedef __n64    uint8x8_t;
			typedef __n64x2  uint8x8x2_t;
			typedef __n64x3  uint8x8x3_t;
			typedef __n64x4  uint8x8x4_t;
			typedef __n64    uint16x4_t;
			typedef __n64x2  uint16x4x2_t;
			typedef __n64x3  uint16x4x3_t;
			typedef __n64x4  uint16x4x4_t;
			typedef __n64    uint32x2_t;
			typedef __n64x2  uint32x2x2_t;
			typedef __n64x3  uint32x2x3_t;
			typedef __n64x4  uint32x2x4_t;
			typedef __n64    uint64x1_t;
			typedef __n64x2  uint64x1x2_t;
			typedef __n64x3  uint64x1x3_t;
			typedef __n64x4  uint64x1x4_t;
			typedef __n128   float32x4_t;
			typedef __n128x2 float32x4x2_t;
			typedef __n128x3 float32x4x3_t;
			typedef __n128x4 float32x4x4_t;
			typedef __n128   int8x16_t;
			typedef __n128x2 int8x16x2_t;
			typedef __n128x3 int8x16x3_t;
			typedef __n128x4 int8x16x4_t;
			typedef __n128   int16x8_t;
			typedef __n128x2 int16x8x2_t;
			typedef __n128x3 int16x8x3_t;
			typedef __n128x4 int16x8x4_t;
			typedef __n128   int32x4_t;
			typedef __n128x2 int32x4x2_t;
			typedef __n128x3 int32x4x3_t;
			typedef __n128x4 int32x4x4_t;
			typedef __n128   int64x2_t;
			typedef __n128x2 int64x2x2_t;
			typedef __n128x3 int64x2x3_t;
			typedef __n128x4 int64x2x4_t;
			typedef __n128   poly8x16_t;
			typedef __n128x2 poly8x16x2_t;
			typedef __n128x3 poly8x16x3_t;
			typedef __n128x4 poly8x16x4_t;
			typedef __n128   poly16x8_t;
			typedef __n128x2 poly16x8x2_t;
			typedef __n128x3 poly16x8x3_t;
			typedef __n128x4 poly16x8x4_t;
			typedef __n128   uint8x16_t;
			typedef __n128x2 uint8x16x2_t;
			typedef __n128x3 uint8x16x3_t;
			typedef __n128x4 uint8x16x4_t;
			typedef __n128   uint16x8_t;
			typedef __n128x2 uint16x8x2_t;
			typedef __n128x3 uint16x8x3_t;
			typedef __n128x4 uint16x8x4_t;
			typedef __n128   uint32x4_t;
			typedef __n128x2 uint32x4x2_t;
			typedef __n128x3 uint32x4x3_t;
			typedef __n128x4 uint32x4x4_t;
			typedef __n128   uint64x2_t;
			typedef __n128x2 uint64x2x2_t;
			typedef __n128x3 uint64x2x3_t;
			typedef __n128x4 uint64x2x4_t;
		};
	}
}

#endif