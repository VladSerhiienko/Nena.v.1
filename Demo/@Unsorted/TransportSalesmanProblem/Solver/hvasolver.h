#ifndef JUNIHERZ_HEURISTIC_VOGEL_APPROACH_H
#define JUNIHERZ_HEURISTIC_VOGEL_APPROACH_H
#include "bsctpsstorage.h"

#define MIN( x, y ) ((x > y) ? (y) : (x));

namespace Juniherz
{
	// Prototype.
	template <typename T>
	class HVASolver;

	// Type definitions.
	typedef HVASolver<float32> Solver32;
	typedef HVASolver<float64> Solver64;

	// Solver declaration.
	template <typename T>
	class HVASolver	
	{
	public:
		// Initialize solver.
		HVASolver( TpsStorage<T> storage );
		// Get the solution.
		void Solve( );
		// Release used inner resources.
		virtual void Release( );
		// Destroy.
		virtual ~HVASolver();

	public:
		void CalculateFines( );
		void GetSection( bool &section, uint32 index );
		bool IsAvailable( bool section, uint32 index );

	public:
		uint32 GetWidth( );
		uint32 GetHeight( );
		bool GetNextValues( uint32 &row, uint32 &column );

	protected:
		TpsStorage<T>* m_ptStorage;
		Matrix<T>* m_pvtFineRow;
		Matrix<T>* m_pvtFineColumn;
		Matrix<T>* m_pmtSolution;

	};

	// Implementation.
#include "hvasolver.inl"
};

#endif