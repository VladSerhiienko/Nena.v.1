#pragma once
#include "pch.h"

ref class BasicInputData
{
internal:
	BasicInputData(_In_ Platform::String^ mapping);
	BasicInputData(_In_ WCHAR* mapping);
	BasicInputData(_In_ WCHAR symbol, _In_ UINT32 frequency);

public:
	void Map(_In_ Platform::String^ mapping);

internal:
	void Map(_In_ WCHAR* mapping);
	void AddBit();
	void AddZero();

internal:
	property WCHAR Symbol { WCHAR get(); }
	property UINT32 Frequency { UINT32 get(); }

	property Platform::String^ Code 
	{ 
		Platform::String^ get(); 
		void set(Platform::String^ value); 
	}
	
private:
	WCHAR				m_symbol;
	UINT32				m_symbolFrequency;
	Platform::String^	m_codeString;

};