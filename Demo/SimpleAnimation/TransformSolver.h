#pragma once

#include <vector>
#include <Windows.h>
#include <Nena\Math.h>

#define __TransformSolver_AllowSSE2 1 // ??

#if __TransformSolver_AllowSSE2 // yeeep
#define __TransformSolver_CallConv __vectorcall
#else
#define __TransformSolver_CallConv
#endif

namespace Demo
{
	struct TransformSolver
	{
		enum Result
		{
			kUndefined = -2,
			kNotFound = -1,
			kNoReal = 0,
			k3Real = 1,
			k2Real = 2,
			k3RealEq = 3,
			k2RealEq = 4,
			k1Real = 5,
		};

		//! \brief Is \c good solution.
		inline static ::BOOL Good(Result const &result) { return result > 0; }
		//! \brief Is \c bad solution.
		inline static ::BOOL Bad(Result const &result) { return result <= 0; }

		/** \brief Solves y-value for the animation key pair.     
		* \remark A bit complex and CPU-time-consuming function. One probably dont need it when animation tracks  
		* \remark are baked or the intervals are small or the segments are linear-like or he doesnt want super-exact solution.
		* \param [in] p0 Left point 
		* \param [in] p1 Right point  
		* \param [in] m0 Input tangent 
		* \param [in] m1 Output tangent 
		* \param [in] x Total elapsed time (not 0..1-ranged)
		* \param [out] y Output channel value 
		**/
		static TransformSolver::Result __TransformSolver_CallConv SolveSegment(
			_In_::Nena::Vector2 p0, _In_::Nena::Vector2 m0, // left
			_In_::Nena::Vector2 p1, _In_::Nena::Vector2 m1, // right
			_In_::FLOAT x, _Out_::FLOAT &y
			);

		/** \brief Estimates y-value for the animation key pair.  
		* \remark The returned value is an approximated solution. If your animation curves are complex, then use the function above.  
		* \remark Use this one if your animation is baked, the segments have small time intervals or are linear or small errors are inrelevant.
		* \param [in] p0 Left point 
		* \param [in] p1 Right point  
		* \param [in] m0 Input tangent 
		* \param [in] m1 Output tangent 
		* \param [in] x Total elapsed time (not 0..1-ranged)
		* \param [out] y Output channel value 
		**/
		static TransformSolver::Result __TransformSolver_CallConv SolveSegmentEst(
			_In_::Nena::Vector2 p0, _In_::Nena::Vector2 m0, // left
			_In_::Nena::Vector2 p1, _In_::Nena::Vector2 m1, // right
			_In_::FLOAT x, _Out_::FLOAT &y
			);

	protected:

		//! \brief Solves \c hermite segment.
		static ::FLOAT __TransformSolver_CallConv SolveHermiteSegment(
			_In_::FLOAT p0, _In_::FLOAT m0, // left
			_In_::FLOAT p1, _In_::FLOAT m1, // right
			_In_::FLOAT t
			);
		//! \brief Solves \c square equation.
		static TransformSolver::Result __TransformSolver_CallConv Solve2(
			_In_::FLOAT a, _In_::FLOAT b, _In_::FLOAT c, // coeff
			_Out_::FLOAT &x, _Out_::FLOAT &y // vars
			);
		//! \brief Solves \c cubic equation.
		static TransformSolver::Result __TransformSolver_CallConv Solve3(
			_In_::FLOAT a, _In_::FLOAT b, _In_::FLOAT c, _In_::FLOAT d, // coeff
			_Out_::FLOAT &x, _Out_::FLOAT &y, _Out_::FLOAT &z // vars
			);

	private:

		//! \brief Returns the max value from the provided.
		static ::FLOAT __TransformSolver_CallConv Max(
			_In_::FLOAT x, _In_::FLOAT y
			);
		//! \brief Returns the max value from the provided.
		static ::FLOAT __TransformSolver_CallConv Max(
			_In_::FLOAT x, _In_::FLOAT y, _In_::FLOAT z
			);
		//! \remark IEEE standard workaround. Can only slower the code, so dont use.
		static ::FLOAT __TransformSolver_CallConv Power(
			_In_::FLOAT x, _In_::FLOAT p
			);
		//! \brief Finds the value that is in range 0..1. If its not found, returns zero, otherwise nonzero.
		static ::BOOL __TransformSolver_CallConv FindHermiteTime(
			_In_::FLOAT x, _In_::FLOAT y, _In_::FLOAT z,
			_Out_::FLOAT &t
			);

	};
}