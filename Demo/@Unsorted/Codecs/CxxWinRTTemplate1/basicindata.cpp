#include "basicindata.h"
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 100


BasicInputData::BasicInputData(_In_ Platform::String^ mapping)
{
	Map(mapping);
	m_codeString = ref new Platform::String();
}

BasicInputData::BasicInputData(_In_ WCHAR* mapping)
{
	Map(mapping);
	m_codeString = ref new Platform::String();
}

BasicInputData::BasicInputData(_In_ WCHAR symbol, _In_ UINT32 frequency) : 
	m_symbol(symbol), m_symbolFrequency(frequency)
{
	m_codeString = ref new Platform::String();
}

void BasicInputData::Map(_In_ Platform::String^ mapping)
{
	std::wstring wszdata(mapping->Data());

	WCHAR *freq = nullptr;
	WCHAR *symbol = wcstok_s(&wszdata[0], L"\t", &freq);

	size_t i = 0;
	char *dest = (char*)malloc((size_t)BUFFER_SIZE);

	int size = wcstombs_s(&i, dest, (size_t)BUFFER_SIZE, freq, (size_t)BUFFER_SIZE);

	m_symbol = *symbol;
	m_symbolFrequency = static_cast<UINT32>(atoi(dest));
}

void BasicInputData::Map(_In_ WCHAR* mapping)
{
	WCHAR *freq = nullptr;
	WCHAR *symbol = wcstok_s(mapping, L"\t", &freq);

	size_t i = 0;
	char *dest = (char*)malloc((size_t)BUFFER_SIZE);

	int size = wcstombs_s(&i, dest, (size_t)BUFFER_SIZE, freq, (size_t)BUFFER_SIZE);

	m_symbol = *symbol;
	m_symbolFrequency = static_cast<UINT32>(atoi(dest));
}

void BasicInputData::AddBit()
{
	m_codeString += L"1";
}

void BasicInputData::AddZero()
{
	m_codeString += L"0";
}

UINT32 BasicInputData::Frequency::get()
{
	return m_symbolFrequency;
}

WCHAR BasicInputData::Symbol::get()
{
	return m_symbol;
}

Platform::String^ BasicInputData::Code::get()
{
	return m_codeString;
}

void BasicInputData::Code::set(Platform::String^ value)
{
	m_codeString = value;
}