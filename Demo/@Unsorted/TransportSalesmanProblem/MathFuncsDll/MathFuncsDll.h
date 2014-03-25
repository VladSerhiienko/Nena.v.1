#ifdef MATHFUNCSDLL_EXPORTS
#define MATHFUNCSDLL_API __declspec(dllexport)
#else
#define MATHFUNCSDLL_API __declspec(dllimport)
#endif


namespace MathFuncs
{
	class MyMathFuncs
	{
	public:
		static MATHFUNCSDLL_API double Add( double a, double b );
		static MATHFUNCSDLL_API double Subtract( double a, double b );
		static MATHFUNCSDLL_API double Multiply( double a, double b );
		static MATHFUNCSDLL_API double Divide( double a, double b );
	};
};