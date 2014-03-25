#include "App.h"
#include "AppConverter.h"

#include <string>
#include <sstream>
#include <ppl.h>
#include <ppltasks.h>
#include <unordered_map>
#include <concurrent_unordered_map.h>

#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>
#include <rapidxml_print.hpp>

#ifndef __NENA_APPVARS_INCLUDED__
#define __NENA_APPVARS_INCLUDED__

#define _Nena_AppVars_DefaultPath8 "app.vars.xml"
#define _Nena_AppVars_DefaultPath16 L"app.vars.xml"

namespace Nena
{
	struct App::Vars
	{
		friend App;

		typedef SRWLOCK Lock;
		typedef HRESULT HResultSync;
		typedef std::string String;
		typedef std::wstring String16;
		typedef rapidxml::xml_document<> Doc;
		typedef rapidxml::xml_node<> Node;
		typedef rapidxml::xml_attribute<> NodeAttribute;
		typedef concurrency::task<void> ActionAsync;
		typedef concurrency::task<HRESULT> HResultAsync;
		typedef concurrency::task<String> StringAsync;
		typedef concurrency::task<String16> String16Async;
		typedef std::unordered_map<String, String> Records1;
		typedef concurrency::concurrent_unordered_map<String, String> Records2;
		typedef Records2 Records;
		typedef Records::size_type RecordsSizeType;

		Vars();
		~Vars();

		HResultSync Read(BOOL clearRecs = FALSE, String file = _Nena_AppVars_DefaultPath8);
		HResultSync Flush(BOOL clearRecs = FALSE, String file = _Nena_AppVars_DefaultPath8);
		HResultAsync ReadAsync(BOOL clearRecs = FALSE, String file = _Nena_AppVars_DefaultPath8);
		HResultAsync FlushAsync(BOOL clearRecs = FALSE, String file = _Nena_AppVars_DefaultPath8);
		HResultSync HasRecord(String const &rec) const;
		HResultAsync HasRecordAsync(String rec) const;
		inline Records::iterator Begin() { return Recs.begin(); }
		inline Records::iterator End() { return Recs.end(); }

	public:

		template <typename _Ty = String> inline void StoreRecord(String const &key, _Ty const &value)
		{
			String stringValue;
			Converter::ToString(stringValue, value);
			StoreRecord(key, stringValue);
		}
		template <> inline void StoreRecord<>(String const &key, String const &value)
		{
			Recs[key] = value;
		}
		template <> inline void StoreRecord<>(String const &key, String16 const &value)
		{
			Recs[key] = Converter::String16To8(value);
		}

		template <typename _Ty = String> inline ActionAsync StoreRecordAsync(String key, _Ty value)
		{
			return concurrency::create_task([this, key, value]
			{
				String stringValue;
				Converter::ToString(stringValue, value);
				return stringValue;
			}).then([this, key](String value)
			{
				return StoreRecordAsync(key, value);
			});
		}
		template <> inline ActionAsync StoreRecordAsync<>(String key, String value)
		{
			return concurrency::create_task([this, key, value]
			{
				return StoreRecord(key, value);
			});
		}
		template <> inline ActionAsync StoreRecordAsync<>(String key, String16 value)
		{
			return concurrency::create_task([this, key, value]
			{
				return StoreRecord(key, value);
			});
		}

		template <typename _Ty = String> inline _Ty GetRecord(String const &key)
		{
			_Ty value;
			Converter::ToNumber(Recs[key], value);
			return value;
		}
		template <> inline String GetRecord<>(String const &key)
		{
			return Recs[key];
		}
		template <> inline String16 GetRecord<>(String const &key)
		{
			return Converter::String8To16(Recs[key]);
		}

		template <typename _Ty = String, typename _TyAsync = concurrency::task<_Ty>> inline _TyAsync GetRecordAsync(String key)
		{
			return concurrency::create_task([this, key]
			{
				return GetRecordAsync<String>(key)
			}).then([this](String stringValue)
			{
				_Ty value; Converter::ToNumber(stringValue, value);
				return value;
			});
		}
		template <> inline StringAsync GetRecordAsync<String, StringAsync>(String key)
		{
			return concurrency::create_task([this, key] { return GetRecord<String>(key); });
		}
		template <> inline String16Async GetRecordAsync<String16, String16Async>(String key)
		{
			return concurrency::create_task([this, key] { return GetRecord<String16>(key); });
		}

		void EraseRecord(String const &key);
		RecordsSizeType GetRecordsCount();

	public:

		static Vars *GetForCurrentThread();

	protected:

		Records Recs;
		Lock RecsLock;
	};
}

#endif