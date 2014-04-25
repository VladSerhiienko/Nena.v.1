
#define _Szx 640
#define _Szy 480

//#define _Szx 1280
//#define _Szy  960

/*#define _Szx 1000
#define _Szy 1000*/

namespace PerCall
{
	Texture2D<float4> InputImage : register (t0);
	RWTexture2D<float4> OutputImage : register (u0);

	static const float Filter[7] = 
	{
		0.030078323, // 0
		0.104983664, // 1
		0.222250419, // 2
		0.285375187, // 3
		0.222250419, // 2
		0.104983664, // 1
		0.030078323  // 0
	};

	/*static const uint3 m3x = uint3(-3, 0, 0);
	static const uint3 p3x = uint3(+3, 0, 0);
	static const uint3 m2x = uint3(-2, 0, 0);
	static const uint3 p2x = uint3(+2, 0, 0);
	static const uint3 m1x = uint3(-1, 0, 0);
	static const uint3 p1x = uint3(+1, 0, 0);
	static const uint3 m3y = uint3(0, -3, 0);
	static const uint3 p3y = uint3(0, +3, 0);
	static const uint3 m2y = uint3(0, -2, 0);
	static const uint3 p2y = uint3(0, +2, 0);
	static const uint3 m1y = uint3(0, -1, 0);
	static const uint3 p1y = uint3(0, +1, 0);*/
};

#define _input PerCall::InputImage
#define _output PerCall::OutputImage
#define _filter(i) PerCall::Filter[i]
#define _inload(r, i) r = _input.Load(i)
#define _outstore(i, v) _output[i] = v

