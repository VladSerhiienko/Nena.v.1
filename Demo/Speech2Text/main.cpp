#pragma warning(disable: 4996)

#include <Windows.h>

#include <wrl.h>
#include <sapi.h>
#include <sphelper.h>

#include <wchar.h>
#include <iostream>

int main(int argc, char* argv[])
{
	if (FAILED(::CoInitialize(NULL)))
		return FALSE;

	HRESULT result = S_OK;

	GUID formatGUID;
	WAVEFORMATEX *waveFormat;
	WCHAR *recoProfileName;
	SPRECOSTATE recoState;
	SPCATEGORYTYPE categoryType;
	SPRECOGNIZERSTATUS recoStatus;
	SPCONTEXTSTATE recoContextState;
	SPRECOCONTEXTSTATUS recoContextStatus;
	SPGRAMMARSTATE recoGrammarState;
	SPGRAMMAROPTIONS recoGrammarOptions;
	SPAUDIOOPTIONS audioOptions = SPAO_NONE;

	Microsoft::WRL::ComPtr<ISpObjectToken> objectToken;
	Microsoft::WRL::ComPtr<ISpObjectToken> audioObjectToken;
	Microsoft::WRL::ComPtr<ISpRecognizer3> recognizer3;
	Microsoft::WRL::ComPtr<ISpRecognizer> recognizer;
	Microsoft::WRL::ComPtr<ISpRecoContext> recoContext;
	Microsoft::WRL::ComPtr<ISpRecoContext2> recoContext2;
	Microsoft::WRL::ComPtr<ISpRecoGrammar> recoGrammar;
	Microsoft::WRL::ComPtr<ISpRecoGrammar2> recoGrammar2;
	Microsoft::WRL::ComPtr<ISpRecoResult> recoResult;
	Microsoft::WRL::ComPtr<ISpRecoCategory> category;
	Microsoft::WRL::ComPtr<ISpAudio> audio;
	Microsoft::WRL::ComPtr<ISpSREngine2> engine;
	Microsoft::WRL::ComPtr<ISpStream> stream;
	CSpEvent speechEvent;

	SPSTREAMFORMAT speechStreamFormat = SPSF_22kHz8BitMono;
	CSpStreamFormat streamFormat(speechStreamFormat, &result);


	result = CoCreateInstance(
		CLSID_SpSharedRecognizer, NULL, CLSCTX_ALL, IID_ISpRecognizer3,
		//CLSID_SpInprocRecognizer, NULL, CLSCTX_ALL, IID_ISpRecognizer3,
		(void **) recognizer3.ReleaseAndGetAddressOf()
		); 
	result = recognizer3.As(&recognizer);

	result = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, audio.ReleaseAndGetAddressOf());
	result = recognizer->SetInput(audio.Get(), TRUE);

	result = recognizer->CreateRecoContext(recoContext.ReleaseAndGetAddressOf());
	result = recoContext.As(&recoContext2);

	result = recoContext->CreateGrammar(1, recoGrammar.ReleaseAndGetAddressOf());
	result = recoGrammar.As(&recoGrammar2);

	/*result = recognizer->GetRecoState(&recoState);
	result = recognizer->GetStatus(&recoStatus);
	result = recognizer->GetRecoProfile(objectToken.ReleaseAndGetAddressOf());
	result = objectToken->GetStringValue(nullptr, &recoProfileName);

	result = recognizer->GetFormat(SPSTREAMFORMATTYPE::SPWF_INPUT, &formatGUID, &waveFormat);
	result = recognizer->GetFormat(SPSTREAMFORMATTYPE::SPWF_SRENGINE, &formatGUID, &waveFormat);

	result = recoContext->GetContextState(&recoContextState);
	result = recoContext->GetStatus(&recoContextStatus);*/

	/* 
	XML Approximation:
	<RULE NAME="Travel" TOPLEVEL="ACTIVE">
		<RULEREF NAME="Method"/>
		<RULEREF NAME="Dest"/>
	</RULE>
	<RULE NAME="Method">
		<LIST>
			<PHRASE>fly to</PHRASE>
			<PHRASE>drive to</PHRASE>
			<PHRASE>take train to</PHRASE>
		</LIST>
	</RULE>
	<RULE NAME="Dest" DYNAMIC="TRUE">
		<LIST>
			<PHRASE>Seattle</PHRASE>
			<PHRASE>New York</PHRASE>
			<PHRASE>Washington DC</PHRASE>
		</LIST>
	</RULE>
	*/

	/*

	SPSTATEHANDLE hStateTravel = nullptr;
	SPSTATEHANDLE hStateTravel_Second = nullptr;
	SPSTATEHANDLE hStateMethod = nullptr;
	SPSTATEHANDLE hStateDest = nullptr;

	result = recoGrammar->GetRule(L"Travel", 0, SPRAF_TopLevel | SPRAF_Active, TRUE, &hStateTravel);

	result = recoGrammar->GetRule(L"Method", 0, 0, TRUE, &hStateMethod);
	result = recoGrammar->GetRule(L"Dest", 0, SPRAF_Dynamic, TRUE, &hStateDest);
	
	result = recoGrammar->CreateNewState(hStateTravel, &hStateTravel_Second);
	result = recoGrammar->AddRuleTransition(hStateTravel, hStateTravel_Second, hStateMethod, 1, NULL);
	result = recoGrammar->AddRuleTransition(hStateTravel_Second, NULL, hStateDest, 1, NULL);

	result = recoGrammar->AddWordTransition(hStateMethod, NULL, L"fly to", L" ", SPWT_LEXICAL, 1, NULL);
	result = recoGrammar->AddWordTransition(hStateMethod, NULL, L"drive to", L" ", SPWT_LEXICAL, 1, NULL);
	result = recoGrammar->AddWordTransition(hStateMethod, NULL, L"take train to", L" ", SPWT_LEXICAL, 1, NULL);

	result = recoGrammar->AddWordTransition(hStateDest, NULL, L"Seattle", L" ", SPWT_LEXICAL, 1, NULL);
	result = recoGrammar->AddWordTransition(hStateDest, NULL, L"New York", L" ", SPWT_LEXICAL, 1, NULL);
	result = recoGrammar->AddWordTransition(hStateDest, NULL, L"Washington DC", L" ", SPWT_LEXICAL, 1, NULL);

	result = recoGrammar->Commit(0);
	*/

	result = recoGrammar->LoadDictation(
		nullptr, 
		SPLOADOPTIONS::SPLO_STATIC
		);
	result = recoContext->SetAudioOptions(
		SPAO_RETAIN_AUDIO, 
		&streamFormat.FormatId(), 
		streamFormat.WaveFormatExPtr()
		);
	result = recoGrammar->SetDictationState(
		SPRULESTATE::SPRS_ACTIVE
		);

	//result = recoGrammar->GetGrammarState(&recoGrammarState);

	BOOL close = FALSE;
	while (!close)
	{
		result = recoContext->WaitForNotifyEvent(15000);

		if (SUCCEEDED(result))
		{
			result = speechEvent.GetFrom(recoContext.Get());
			if (SUCCEEDED(result))
			{
				if (SPEI_RECOGNITION == speechEvent.eEventId)
				{
					recoResult = speechEvent.RecoResult();

					SPPHRASE *phrase;
					result = recoResult->GetPhrase(&phrase);

					WCHAR *pwszText;
					result = recoResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE, &pwszText, NULL);
					std::wcout << pwszText << std::endl;

					WCHAR *pwszWord;
					result = recoResult->GetText(0, 1, TRUE, &pwszWord, NULL);

					if (std::wstring(L"close") == std::wstring(pwszWord))
						close = TRUE;

					speechEvent.Clear();
				}
			}
		}
	}

	result = recoGrammar->SetDictationState(
		SPRULESTATE::SPRS_INACTIVE
		);
	result = recoGrammar->UnloadDictation(
		);

	/*if (recoResult.Get())
	{
		result = recoResult->GetAudio(0, 0, (ISpStreamFormat**) &streamFormat);
	}*/

	//{
	//	result = recognizer3->GetActiveCategory(
	//		category.ReleaseAndGetAddressOf()
	//		);
	//	if (SUCCEEDED(result) && category.Get())
	//		result = category->GetType(
	//			&categoryType
	//			);
	//}



	::CoUninitialize();
	return EXIT_SUCCESS;
}