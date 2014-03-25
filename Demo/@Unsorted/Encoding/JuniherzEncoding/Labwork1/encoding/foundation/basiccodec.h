#pragma once
#include "pch.h"
#include "basicalphabet.h"

ref class Codec
{
internal:
	Codec(BasicAlphabet^ alphabet)
	{
		m_alphabet = alphabet;
	}

	Platform::String^ Encode(Platform::String^ decoded)
	{
		Platform::String^ encoded = L" ";

		for (UINT32 i = 0; i < decoded->Length(); i++)
		{
			encoded += m_alphabet->Encode(decoded->Data()[i]);
		}

		return encoded;
	}

	Platform::String^ Decode(Platform::String^ encoded)
	{
		std::wstring wcszDecoded; //(pwcDecoded);

		for (UINT32 i = 0; i < encoded->Length(); /*  */)
		{
			std::wstring wcszTempEncoded;
			wcszTempEncoded += encoded->Data()[i];

			UINT32 j (0);

			for (j = i + 1; j < encoded->Length() + 1; j++)
			{
				WCHAR symbol = m_alphabet->Decode(ref new Platform::String(wcszTempEncoded.data()));

				if (symbol == (L'\0'))
				{
					wcszTempEncoded += encoded->Data()[j];
				}
				else
				{
					wcszDecoded += symbol;
					break;
				}
			}

			i = j;
		}

		return ref new Platform::String(wcszDecoded.data());
	}

private:
	BasicAlphabet^ m_alphabet;
};