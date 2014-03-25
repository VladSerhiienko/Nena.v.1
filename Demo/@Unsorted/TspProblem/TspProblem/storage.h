#ifndef TSP_STORAGE_H
#define TSP_STORAGE_H
#include "bsctypes.h"
using namespace Juniherz;

class TspStorage
{
public:
	// Initialize storage.
	TspStorage( _In_ uint32 nToSend, _In_ uint32 nToRecive );
	virtual ~TspStorage();

public:	
	// Sefloat32 price
	void SetPrice( _In_ uint32 from, _In_ uint32 to, _In_ float32 value );
	float32** GetPrices( );
	
public:	
	// Sefloat32 sender
	void SetSender( _In_ uint32 i, _In_ float32 value );
	float32 GetSender( _In_ uint32 x );
	float32* GetSenders( );
	
public:	
	// Recievers
	void SetReciver( _In_ uint32 i, _In_ float32 value);
	float32 GetReciver( _In_ uint32 x );
	float32* SetRecivers( );

public:		
	void Normalize( );

private:
	uint32 nSenders;			// Senders` count.
	uint32 nRecivers;			// Recievers` count.

	matrix *m_vtSenders;		// Senders` indices.
	matrix *m_vtRecivers;	// Recievers` indices.
	matrix *m_mtPrices;		// Prices` values.
};

#endif