#if _MSC_VER
#pragma once
#endif

#ifndef __REIGN_CORE_INCLUDED__
#define __REIGN_CORE_INCLUDED__
#include <Windows.h>
#include <strsafe.h>
#include <sstream>
#include <string>

#define DECLSPEC_INTRIN		__declspec(intrin_type)
#define DECLSPEC_ALIGN(x)	__declspec(align(x)) // Alignes type 

#define IfThenElse( Condition, Then, Else ) ((Condition) ? (Then) : (Else)) // Shortcut for <Condition> ? <Then> : <Else> structure. 
#define MaxOf( X, Y ) ((X) > (Y) ? (X) : (Y)) // Gets max value
#define MinOf( X, Y ) ((X) < (Y) ? (X) : (Y)) // Gets min value
#define AbsOf( X ) ((X) < 0 ? (-X) : (X)) // Gets absolute value 

#ifndef REIGN_DESKTOP

#define public_shared_access 			public
#define public_owned_access 			internal
#define protected_shared_access 		protected
#define protected_owned_access 			protected private
#define owned_access 					private
#define value_struct_decl 				value struct
#define value_class_decl 				value class
#define ref_struct_decl 				ref struct
#define ref_class_decl 					ref class
#define class_decl 						class
#define struct_decl 					struct
#define alloc_ref_new 					ref new
#define alloc_new 						new
#define public_dtor_access				private
#define private_dtor_access				private
#define protected_dtor_access			private
#define _Ptr_							^

#else

#define public_shared_access			public
#define public_owned_access				public
#define protected_shared_access			protected
#define protected_owned_access			protected
#define owned_access					private
#define value_struct_decl				struct
#define value_class_decl 				class
#define ref_struct_decl 				struct
#define ref_class_decl 					class
#define class_decl 						class
#define struct_decl 					struct
#define alloc_ref_new 					new
#define alloc_new						new
#define public_dtor_access				public
#define private_dtor_access				private
#define protected_dtor_access			protected
#define _Ptr_							*

#endif


#define null												nullptr
#define pure												PURE
#define Undefined_Slot										(-1)
#define REIGN_NOALIAS										__declspec(noalias)										
#define REIGN_ALIGN( Bytes )								__declspec(align(Bytes))	
#define REIGN_HINTINLINING									__inline										
#define REIGN_FORCEINLINING									__forceinline										
#define REIGN_METHODSTDCALLTYPE								_stdcall	
#define ReignMaxDebugNameLength (64)

#define NoFlags												(0U)
#define BitMask( Pos )										(1 << Pos)

#define IsFlagChecked( Flags, Flag )						( (Flags & Flag) == Flag )
#define GetUuidOf( T )										( __uuidof( T ) )
#define GetByteSizeOf( x )									( sizeof(x) )
#define GetBitSizeOf( x )									( GetByteSizeOf(x) * 8 )

#define Safe_Delete( x )									{ if ( x ) { delete (x); (x) = null; } } // Ptr
#define Safe_DeleteArray( x )								{ if ( x ) { delete[] (x); (x) = null; } } // Dynamically allocated Array
#define Safe_Release( x )									{ if ( x ) { (x)->Release(); (x) = null; } } // Com
#define Safe_Destroy( x )									{ if ( x ) { (x)->Destroy(); (x) = null; } } // Fbx

//#ifdef REIGN_DESKTOP // Properties

#define DeclGetSetPropOnly( Type, Name )  __declspec( property ( put = PropertySetterFor##Name, get = PropertyGetterFor##Name ) ) Type Name; typedef Type PropertyTypeOf##Name; 
#define DeclGetPropOnly( Type, Name ) __declspec( property (get = PropertyGetterFor##Name) ) Type Name; typedef Type PropertyTypeOf##Name; 
#define DeclSetPropOnly( Type, Name ) __declspec( property (put = PropertySetterFor##Name) ) Type Name; typedef Type PropertyTypeOf##Name;
#define DeclGetterFor( Name ) PropertyTypeOf##Name const PropertyGetterFor##Name( ) 
#define DeclTypedGetterFor( Type, Name ) Type PropertyGetterFor##Name( ) 
#define DeclConstTypedGetterFor( Type, Name ) Type PropertyGetterFor##Name( ) const
#define DeclSetterFor( Name ) void PropertySetterFor##Name( PropertyTypeOf##Name const &ValueRef )  
#define DeclTypedSetterFor( Type, Name ) void PropertySetterFor##Name( Type Value )  
#define DeclStaticGetterFor( Name ) static PropertyTypeOf##Name const PropertyGetterFor##Name( ) 
#define DeclStaticTypedGetterFor( Type, Name ) static Type PropertyGetterFor##Name( ) 
#define DeclStaticSetterFor( Name ) static void PropertySetterFor##Name( PropertyTypeOf##Name const &ValueRef )  
#define DeclStaticTypedSetterFor( Type, Name ) static void PropertySetterFor##Name( Type Value )  
#define DeclGetSetProp( Type, Name )  DeclGetSetPropOnly( Type, Name ); DeclGetterFor( Name );  DeclSetterFor( Name ); 
#define DeclGetProp( Type, Name ) DeclGetPropOnly( Type, Name ); DeclGetterFor( Name ); 
#define DeclSetProp( Type, Name ) DeclSetPropOnly( Type, Name ); DeclSetterFor( Name );  
#define DeclVirtualGetterFor( Name ) virtual PropertyTypeOf##Name const PropertyGetterFor##Name( ) 
#define DeclVirtualSetterFor( Name ) virtual void PropertySetterFor##Name( PropertyTypeOf##Name const &ValueRef)  
#define DeclPureVirtualGetterFor( Name ) virtual PropertyTypeOf##Name const PropertyGetterFor##Name( ) pure
#define DeclPureVirtualSetterFor( Name ) virtual void PropertySetterFor##Name( PropertyTypeOf##Name const &ValueRef) pure
#define DeclVirtualGetSetProp( Type, Name )  DeclGetSetPropOnly( Type, Name ); DeclVirtualGetterFor( Name );  DeclVirtualSetterFor( Name ); 
#define DeclVirtualGetProp( Type, Name ) DeclGetPropOnly( Type, Name ); DeclVirtualGetterFor( Name ); 
#define DeclVirtualSetProp( Type, Name ) DeclSetPropOnly( Type, Name ); DeclVirtualSetterFor( Name ); 

//#endif // !Properties


// Declares most common custom type usage definitions 
#define TinyTypeDecl(Type) \
	typedef Type *Type##Ptr; \
	typedef Type &Type##Ref; \
	typedef Type **Type##Address; \
	typedef Type *&Type##Ptr##Ref; \
	typedef Type const Type##Const; \
	typedef Type const *Type##Const##Ptr; \
	typedef Type const &Type##Const##Ref; \
	typedef Type *const Type##Ptr##Const; \
	typedef Type const *const Type##Const##Ptr##Const;

// Declares most common pod-type usage definitions 
#define TypeDecl(StandardTypeName, PredefinedTypeName, DesiredTypeName) \
	typedef StandardTypeName DesiredTypeName; \
	typedef StandardTypeName PredefinedTypeName##_##t; \
	typedef PredefinedTypeName##_##t PredefinedTypeName##_##t; \
	typedef PredefinedTypeName##_##t *PredefinedTypeName##_##ptr_##t, *DesiredTypeName##Ptr; \
	typedef PredefinedTypeName##_##t &PredefinedTypeName##_##ref_##t, &DesiredTypeName##Ref; \
	typedef PredefinedTypeName##_##t const PredefinedTypeName##_##const_##t, DesiredTypeName##Const; \
	typedef PredefinedTypeName##_##t **PredefinedTypeName##_##address_##t, **DesiredTypeName##Address; \
	typedef PredefinedTypeName##_##t *&PredefinedTypeName##_##ptr_##ref_##t, *&DesiredTypeName##PtrRef; \
	typedef PredefinedTypeName##_##t const *PredefinedTypeName##_##const_##ptr_##t, *DesiredTypeName##Const##Ptr; \
	typedef PredefinedTypeName##_##t const &PredefinedTypeName##_##const_##ref_##t, &DesiredTypeName##Const##Ref; \
	typedef PredefinedTypeName##_##t *const PredefinedTypeName##_##ptr_##const_##t, *const DesiredTypeName##Ptr##Const; \
	typedef PredefinedTypeName##_##t const *const PredefinedTypeName##_##const_##ptr_##const_##t, *const DesiredTypeName##Const##Ptr##Const;

#ifdef REIGN_DEBUG
#define PrintToDebugWindowOnly16( wszString ) { OutputDebugStringW( wszString ); OutputDebugStringW( L"\n" ); } // Does exactly what it says. 
#define PrintToDebugWindowOnly8( szString )  { OutputDebugStringA( szString ); OutputDebugStringA( "\n" ); } // Does exactly what it says. 
#if defined(_UNICODE) || defined(UNICODE)
#define PrintToDebugWindowOnly( wszString ) PrintToDebugWindowOnly16(wszString) // Does exactly what it says. 
#else
#define PrintToDebugWindowOnly( szString ) PrintToDebugWindowOnly8 // Does exactly what it says. 
#endif
#else // Noop in release version. 
#define PrintToDebugWindowOnly( szString ) 
#endif

#ifdef REIGN_DEBUG 
// We want to the know the returned value only when debugging. Take the evaluated expression in brackets. 
// Please ensure you dont use this variable when releasing the project. 
#define TypedDebugVariableWithCast( Type, Name ) Type Name = (Type)
// We want to the know the returned value only when debugging. 
// Please ensure you dont use this variable when releasing the project. 
#define TypedDebugVariable( Type, Name ) Type Name = 
// We want to the know the returned value only when debugging. 
// Please ensure you dont use this variable when releasing the project. 
#define AutoDebugVariable( Name ) auto Name = 
#else // Noops
#define TypedDebugVariableWithCast( Type, Name )
#define TypedDebugVariable( Type, Name )		
#define AutoDebugVariable( Name )				
#endif

#ifdef REIGN_DEBUG
namespace
{
	template <class TDeviceChild>
	inline void SetDebugName( _In_ TDeviceChild *pObj, _In_ const wchar_t *pstrName )
	{
		char szNameString[ 64 ];
		auto iNameStringLength = WideCharToMultiByte(
			CP_ACP, 0, pstrName, -1, szNameString,
			64, nullptr, nullptr
			);

		if ( pObj )
		{
			pObj->SetPrivateData(
				WKPDID_D3DDebugObjectName,
				iNameStringLength - 1,
				szNameString
				);
		}
	}

	template <class TDeviceChild>
	inline void SetDebugName( _In_ TDeviceChild *pObj, _In_ const char *pstrName )
	{
		if ( pObj )
		{
			Reign::Standard::String8 Str = pstrName;
			pObj->SetPrivateData(
				WKPDID_D3DDebugObjectName,
				(unsigned int) Str.size( ),
				pstrName
				);
		}
	}
};
#else
#define SetDebugName(Obj, Name)
#endif // !REIGN_DEBUG


// Sehnsucht Engine 
namespace Reign
{
	// Standard types
	namespace Standard
	{
		typedef void Void, Object, *VoidPtr, *const VoidPtrConst,
			*object_ptr_t, *handle_t,
			*ObjectPtr, *Handle;

		TypeDecl( HINSTANCE, hinstance, InstanceHandle );	// App instance handle
		TypeDecl( HRESULT, hresult, HResult );				// Window message first parameter
		TypeDecl( LRESULT, lresult, LResult );				// Window message first parameter
		TypeDecl( HWND, hwnd, WindowHandle );				// Window handle
		TypeDecl( WPARAM, wparam, WParam );					// Window message first parameter
		TypeDecl( LPARAM, lparam, LParam );					// Window message second parameter

		TypeDecl( double, __float64, Float64 );				// Double (8bytes)
		TypeDecl( float, __float32, Float32 );				// Single (4bytes)

		TypeDecl( bool, __bool, Bool );						// Boolean (4bytes)
		TypeDecl( char, __char8, Char8 );					// Char (1byte)
		TypeDecl( wchar_t, __char16, Char16 );				// WideChar (2bytes)
		TypeDecl( unsigned char, __uchar8, UChar8 );		// Unsigned Char (1byte)
		TypeDecl( unsigned short, __uchar16, UChar16 );		// Unsigned WideChar (2bytes)

		TypeDecl( __int64, __int64, Int64 );					// LongLong (8bytes)
		TypeDecl( __int32, __int32, Int32 );					// Int (4bytes)
		TypeDecl( __int16, __int16, Int16 );					// Short (2bytes)
		TypeDecl( __int8, __int8, Int8 );					// Char (1byte)

		TypeDecl( unsigned __int64, __uint64, UInt64 );		// QWORD (8bytes)
		TypeDecl( unsigned __int32, __uint32, UInt32 );		// DWORD (4bytes)
		TypeDecl( unsigned __int16, __uint16, UInt16 );		// WORD (2bytes)
		TypeDecl( unsigned __int8, __uint8, UInt8 );			// BYTE (1byte)

		TypeDecl( unsigned __int64, __qword, QWord );		// QWORD (8bytes)
		TypeDecl( unsigned __int32, __dword, DWord );		// DWORD (4bytes)
		TypeDecl( unsigned __int16, __word, Word );			// WORD (2bytes)
		TypeDecl( unsigned __int8, __byte, Byte );			// BYTE (1byte)

#ifdef REIGN_DESKTOP // To ensure the compability with the Windows Store Apps.
		typedef unsigned __int8 boolean;

		typedef unsigned __int64 uint64;
		typedef unsigned __int32 uint32;
		typedef unsigned __int16 uint16;
		typedef unsigned __int8 uint8;
		typedef __int64 int64;
		typedef __int32 int32;
		typedef __int16 int16;
		typedef __int8 int8;

		typedef wchar_t char16;
		typedef char char8;

		typedef double float64;
		typedef float float32;
#endif

		typedef std::stringstream	StringStream8;
		typedef std::wstringstream	StringStream16;
		typedef std::ofstream		OutputFileStream8;
		typedef std::wofstream		OutputFileStream16;

		TypeDecl( std::wstring, string16, String16 );				// String (2bytes)
		TypeDecl( std::string, string8, String8 );				// String (1byte)

#if defined(_UNICODE) || defined(UNICODE)
		typedef Char16 Char;
		typedef String16 String;
		typedef StringStream16 StringStream;
#else
		typedef Char8 Char; 
		typedef String8 String; 
		typedef StringStream8 StringStream; 
#endif

		// Forces the type to implement Release() method. 
		struct IReleasee
		{
			// Destroys internally allocated resources.
			virtual void Release( ) = 0;

		};


#ifdef REIGN_DEBUG

		ref_class_decl Named
		{
#ifdef REIGN_DESKTOP

			Standard::String16 m_wszName; // Debug name storage

		public_owned_access:

			// The name is better be set (i.e. "BufferPool0", "RenderParticlesVS", "IAPass1")
			// Consider setting the D3D COMs debug names, Sehnsucht::DirectXNative::SetDebugName(...). 
			Named( ) : m_wszName( L"Undefined" ) {}
			// The name is better be set (i.e. "BufferPool0", "RenderParticlesVS", "IAPass1")
			// Consider setting the D3D COMs debug names, Sehnsucht::DirectXNative::SetDebugName(...). 
			Named( String16ConstRef szName ) : m_wszName( szName ) {}
			// The name is better be set (i.e. "BufferPool0", "RenderParticlesVS", "IAPass1")
			// Consider setting the D3D COMs debug names, Sehnsucht::DirectXNative::SetDebugName(...). 
			Named( const Char16 *&szName ) : m_wszName( szName ) {}

		public_owned_access:

			DeclGetSetPropOnly( String16Ptr, DebugName ); // Debug name property.
			DeclTypedSetterFor( String16ConstRef, DebugName ) { m_wszName = Value; } // Gets instance debug name.
			DeclTypedGetterFor( String16ConstPtrConst, DebugName ) { return( &m_wszName ); } // Sets instance debug name.
			DeclConstTypedGetterFor( String16ConstPtrConst, DebugName ) { return( &m_wszName ); } // Sets instance debug name.

#else

			Platform::String^ m_wszName;

		internal:
			// The name is better be set (i.e. "BufferPool0", "RenderParticlesVS", "IAPass1")
			// Consider setting the D3D COMs debug names, Sehnsucht::DirectXNative::SetDebugName(...). 
			Named() : m_wszName( L"Undefined" ) { }
			// The name is better be set (i.e. "BufferPool0", "RenderParticlesVS", "IAPass1")
			// Consider setting the D3D COMs debug names, Sehnsucht::DirectXNative::SetDebugName(...). 
			Named( Platform::String^ wszName ) : m_wszName( wszName ) { } 

		internal:

			property Platform::String^ DebugName
			{
				Platform::String^ get() { return( m_wszName ); }
				Void set( Platform::String^ wszName) { m_wszName = wszName; };
			};

#endif

		};

#else

		/*Noop*/ ref class Named
		{
		internal:
			/*Noop*/ Named() { }
			template <typename TName> 
			/*Noop*/ Named( TName ) { }
		};

#endif

		template<class NativeDesc>
		// Shortcut wrapper class for derived config types 
		struct DescWrapper : public NativeDesc
		{
			// Set the default values 
			virtual void Reset( ) {};

			// Get the native value copy
			virtual NativeDesc AsNativeCopy( )
			{
				return( static_cast<NativeDesc>( *this ) );
			}

			// Get the address of the native value
			virtual NativeDesc *AsNativeAddress( )
			{
				return( dynamic_cast<NativeDesc*>( this ) );
			}

			// Get the native value reference
			virtual NativeDesc &AsNativeReference( )
			{
				return( *this );
			}
		};

		template<class U>
		// Zeros mem of the pod instance (shortcut for the ZeroMemory define). Can work for the array as well (use ZeroValueArray for pod pointers). 
		// \note Its not the delete or delete[]. In these cases consider using defines Safe_Delete, Safe_DeleteArray, MemoryPools etc. 
		inline void ZeroValue( _In_ U *pValue, _In_opt_ size_t Size = sizeof( U ) )
		{
			ZeroMemory( pValue, Size );
		}

		template<class U>
		// Zeros mem of the pod instances (params are pretty much obvious)
		// \note Its not delete[]. In these cases consider using define Safe_DeleteArray. 
		static inline void ZeroValueArray( _In_ U *pValue, _In_ size_t NumElemnts, _In_ size_t ElementSize = sizeof( U ) )
		{
			ZeroMemory( pValue, ( NumElemnts * ElementSize ) );
		}

		// \note Microsoft::WRL::ComPtr<T> can be utilized to avoid manual com pointer handling.
		// \note Safe_Release can be used as well 
		inline void Release( _In_ IUnknown *pValue )
		{
			if ( pValue )
			{
				pValue->Release( );
				pValue = 0;
			}
		}
	};

	// Neon types from the <arm_neon.h>
	namespace Neon
	{

		typedef union DECLSPEC_INTRIN DECLSPEC_ALIGN( 8 ) ___n64
		{
			unsigned __int64    n64_u64[ 1 ];
			unsigned __int32    n64_u32[ 2 ];
			unsigned __int16    n64_u16[ 4 ];
			unsigned __int8     n64_u8[ 8 ];
			__int64             n64_i64[ 1 ];
			__int32             n64_i32[ 2 ];
			__int16             n64_i16[ 4 ];
			__int8              n64_i8[ 8 ];
			float               n64_f32[ 2 ];
		} __n64;

		typedef union DECLSPEC_INTRIN DECLSPEC_ALIGN( 8 ) ___n128
		{
			unsigned __int64   n128_u64[ 2 ];
			unsigned __int32   n128_u32[ 4 ];
			unsigned __int16   n128_u16[ 8 ];
			unsigned __int8    n128_u8[ 16 ];
			__int64            n128_i64[ 2 ];
			__int32            n128_i32[ 4 ];
			__int16            n128_i16[ 8 ];
			__int8             n128_i8[ 16 ];
			float              n128_f32[ 4 ];

			struct
			{
				__n64  low64;
				__n64  high64;
			} n64_largeparts;

		} __n128;

		typedef struct __n64x2
		{
			__n64 val[ 2 ];
		} __n64x2;

		typedef struct __n64x3
		{
			__n64 val[ 3 ];
		} __n64x3;

		typedef struct __n64x4
		{
			__n64 val[ 4 ];
		} __n64x4;

		typedef struct __n128x2
		{
			__n128 val[ 2 ];
		} __n128x2;

		typedef struct __n128x3
		{
			__n128 val[ 3 ];
		} __n128x3;

		typedef struct __n128x4
		{
			__n128 val[ 4 ];
		} __n128x4;

		typedef unsigned __int8  poly8_t;
		typedef unsigned __int16 poly16_t;
		typedef __n64    float32x2_t;
		typedef __n64x2  float32x2x2_t;
		typedef __n64x3  float32x2x3_t;
		typedef __n64x4  float32x2x4_t;
		typedef __n64    int8x8_t;
		typedef __n64x2  int8x8x2_t;
		typedef __n64x3  int8x8x3_t;
		typedef __n64x4  int8x8x4_t;
		typedef __n64    int16x4_t;
		typedef __n64x2  int16x4x2_t;
		typedef __n64x3  int16x4x3_t;
		typedef __n64x4  int16x4x4_t;
		typedef __n64    int32x2_t;
		typedef __n64x2  int32x2x2_t;
		typedef __n64x3  int32x2x3_t;
		typedef __n64x4  int32x2x4_t;
		typedef __n64    int64x1_t;
		typedef __n64x2  int64x1x2_t;
		typedef __n64x3  int64x1x3_t;
		typedef __n64x4  int64x1x4_t;
		typedef __n64    poly8x8_t;
		typedef __n64x2  poly8x8x2_t;
		typedef __n64x3  poly8x8x3_t;
		typedef __n64x4  poly8x8x4_t;
		typedef __n64    poly16x4_t;
		typedef __n64x2  poly16x4x2_t;
		typedef __n64x3  poly16x4x3_t;
		typedef __n64x4  poly16x4x4_t;
		typedef __n64    uint8x8_t;
		typedef __n64x2  uint8x8x2_t;
		typedef __n64x3  uint8x8x3_t;
		typedef __n64x4  uint8x8x4_t;
		typedef __n64    uint16x4_t;
		typedef __n64x2  uint16x4x2_t;
		typedef __n64x3  uint16x4x3_t;
		typedef __n64x4  uint16x4x4_t;
		typedef __n64    uint32x2_t;
		typedef __n64x2  uint32x2x2_t;
		typedef __n64x3  uint32x2x3_t;
		typedef __n64x4  uint32x2x4_t;
		typedef __n64    uint64x1_t;
		typedef __n64x2  uint64x1x2_t;
		typedef __n64x3  uint64x1x3_t;
		typedef __n64x4  uint64x1x4_t;
		typedef __n128   float32x4_t;
		typedef __n128x2 float32x4x2_t;
		typedef __n128x3 float32x4x3_t;
		typedef __n128x4 float32x4x4_t;
		typedef __n128   int8x16_t;
		typedef __n128x2 int8x16x2_t;
		typedef __n128x3 int8x16x3_t;
		typedef __n128x4 int8x16x4_t;
		typedef __n128   int16x8_t;
		typedef __n128x2 int16x8x2_t;
		typedef __n128x3 int16x8x3_t;
		typedef __n128x4 int16x8x4_t;
		typedef __n128   int32x4_t;
		typedef __n128x2 int32x4x2_t;
		typedef __n128x3 int32x4x3_t;
		typedef __n128x4 int32x4x4_t;
		typedef __n128   int64x2_t;
		typedef __n128x2 int64x2x2_t;
		typedef __n128x3 int64x2x3_t;
		typedef __n128x4 int64x2x4_t;
		typedef __n128   poly8x16_t;
		typedef __n128x2 poly8x16x2_t;
		typedef __n128x3 poly8x16x3_t;
		typedef __n128x4 poly8x16x4_t;
		typedef __n128   poly16x8_t;
		typedef __n128x2 poly16x8x2_t;
		typedef __n128x3 poly16x8x3_t;
		typedef __n128x4 poly16x8x4_t;
		typedef __n128   uint8x16_t;
		typedef __n128x2 uint8x16x2_t;
		typedef __n128x3 uint8x16x3_t;
		typedef __n128x4 uint8x16x4_t;
		typedef __n128   uint16x8_t;
		typedef __n128x2 uint16x8x2_t;
		typedef __n128x3 uint16x8x3_t;
		typedef __n128x4 uint16x8x4_t;
		typedef __n128   uint32x4_t;
		typedef __n128x2 uint32x4x2_t;
		typedef __n128x3 uint32x4x3_t;
		typedef __n128x4 uint32x4x4_t;
		typedef __n128   uint64x2_t;
		typedef __n128x2 uint64x2x2_t;
		typedef __n128x3 uint64x2x3_t;
		typedef __n128x4 uint64x2x4_t;
	};
};

#if !defined(REIGN_METROARM) && !defined(REIGN_PHONEARM)
#define REIGN_NONARM 1
#else // !REIGN_NONARM
#define REIGN_ARM 1
#endif // !REIGN_ARM

#ifndef REIGN_DESKTOP
#define Throw throw ref new Platform::Exception(E_FAIL)
#define ThrowWithMsg(Msg) throw ref new Platform::Exception(Msg)
#define ThrowWithHResult(Hr) throw Platform::Exception::CreateException(Hr)
#define ThrowWithMsgAndHResult(Msg, Hr) throw ref new Platform::Exception(Msg, Hr)
#else
#define Throw throw std::exception("Unknown exception", E_FAIL)
#define ThrowWithMsg(Msg) throw std::exception(Msg)
#define ThrowWithHResult(Hr) throw std::exception("Unknown exception", Hr)
#define ThrowWithMsgAndHResult(Msg,Hr) throw std::exception(Msg, Hr)
#endif

#define _To_large_int_ptr	(LARGE_INTEGER*)
#define _To_single			(Reign::Standard::Float32)
#define _To_float 			(Reign::Standard::Float32)
#define _To_double 			(Float64)
#define _To_uint64 			(UInt64)
#define _To_int64 			(Int64)
#define _To_uint 			(UInt32)
#define _To_int 			(Int32)
#define _To_byte 			(Byte)
#define _To_short 			(Int16)
#define _To_ushort 			(UInt16)
#define _To_ptr_diff		(std::ptrdiff_t)

#define _As_large_int_ptr	static_cast<LARGE_INTEGER*>
#define _As_single			static_cast<Reign::Standard::Float32>
#define _As_float 			static_cast<Reign::Standard::Float32>
#define _As_double 			static_cast<Float64>
#define _As_uint64 			static_cast<UInt64>
#define _As_ushort 			static_cast<UInt16>
#define _As_int64 			static_cast<Int64>
#define _As_short 			static_cast<Int16>
#define _As_byte 			static_cast<Byte>
#define _As_uint 			static_cast<UInt32>
#define _As_int 			static_cast<Int32>

#define _Reinterpret_as_float_ptr	reinterpret_cast<Float32Ptr>
#define _Reinterpret_as_uint_ptr	reinterpret_cast<UInt32Ptr>
#define _Reinterpret_as_int_ptr		reinterpret_cast<Int32Ptr>
#define _Reinterpret_as_byte_ptr	reinterpret_cast<BytePtr>

#define _Size_of_as_uint _To_uint sizeof
#define _Byte_size_uint _Size_of_as_uint
#define _Auto_const auto const

#ifdef REIGN_DESKTOP_SUPPORT_WINDOWS7
#define _Reign_desktop_support_windows_7 1
#endif


// Inline properties suitable for value types like matrices or vectors.
// NOTE: Forces the compiler to use inline expansion
#define _Decl_value_prop_finl2_(PropMember, PropType, PropName, ActionsOnGetConst, ActionsOnGetCpy, ActionsOnEdit) \
	REIGN_FORCEINLINING PropType const *Get##PropName( ) const { ActionsOnGetConst; return &PropMember; } \
	REIGN_FORCEINLINING PropType Get##PropName##Cpy( ) const { ActionsOnGetCpy; return PropMember; } \
	REIGN_FORCEINLINING PropType *Edit##PropName( ) { ActionsOnEdit; return &PropMember; }

// Inline properties suitable for value types like matrices or vectors.
// NOTE: Hints the compiler to use inline expansion
#define _Decl_value_prop_hinl2_(PropMember, PropType, PropName, ActionsOnGetConst, ActionsOnGetCpy, ActionsOnEdit) \
	REIGN_HINTINLINING PropType const *Get##PropName( ) const { ActionsOnGetConst; return &PropMember; } \
	REIGN_HINTINLINING PropType Get##PropName##Cpy( ) const { ActionsOnGetCpy; return PropMember; } \
	REIGN_HINTINLINING PropType *Edit##PropName( ) { ActionsOnEdit; return &PropMember; }

// Inline properties suitable for scalar value types or simple pointer types.
#define _Decl_value_prop_finl1_(PropMember, PropType, PropName, ActionsOnGet, ActionsOnSet) \
	REIGN_FORCEINLINING PropType PropName( ) const { ActionsOnGet return PropMember; } \
	REIGN_FORCEINLINING void PropName( _In_ PropType value ) { ActionsOnSet; PropMember = value; }

// Inline properties suitable for scalar value types or simple pointer types.
// NOTE: Hints the compiler to use inline expansion
#define _Decl_value_prop_hinl1_(PropMember, PropType, PropName, ActionsOnGet, ActionsOnSet) \
	REIGN_HINTINLINING PropType PropName( ) const { ActionsOnGet; return PropMember; } \
	REIGN_HINTINLINING void PropName( _In_ PropType value ) { ActionsOnSet; PropMember = value; }


// Inline properties suitable for value types like matrices or vectors.
// NOTE: Forces the compiler to use inline expansion
#define _Decl_value_prop_finl2(PropMember, PropType, PropName) \
	REIGN_FORCEINLINING PropType const *Get##PropName( ) const { return &PropMember; } \
	REIGN_FORCEINLINING PropType Get##PropName##Cpy( ) const { return PropMember; } \
	REIGN_FORCEINLINING PropType *Edit##PropName( ) { return &PropMember; }

// Inline properties suitable for value types like matrices or vectors.
// NOTE: Hints the compiler to use inline expansion
#define _Decl_value_prop_hinl2(PropMember, PropType, PropName) \
	REIGN_HINTINLINING PropType const *Get##PropName( ) const { return &PropMember; } \
	REIGN_HINTINLINING PropType Get##PropName##Cpy( ) const { return PropMember; } \
	REIGN_HINTINLINING PropType *Edit##PropName( ) { return &PropMember; }

// Inline properties suitable for scalar value types or simple pointer types.
// NOTE: Forces the compiler to use inline expansion
#define _Decl_value_prop_finl1(PropMember, PropType, PropName) \
	REIGN_FORCEINLINING PropType PropName( ) const { return PropMember; } \
	REIGN_FORCEINLINING void PropName( _In_ PropType value ) { PropMember = value; }

// Inline properties suitable for scalar value types or simple pointer types.
// NOTE: Hints the compiler to use inline expansion
#define _Decl_value_prop_hinl1(PropMember, PropType, PropName) \
	REIGN_HINTINLINING PropType PropName( ) const { return PropMember; } \
	REIGN_HINTINLINING void PropName( _In_ PropType value ) { PropMember = value; }

#define _No_additional_property_actions (void) 0 
#define _No_actions _No_additional_property_actions
#define _No_args_ 

namespace Reign
{
	namespace Standard
	{
		typedef Char8 _Short_dbg_msg[ MAX_PATH ];
	};
};

#ifdef REIGN_DEBUG
inline void _Print_pointer( char *name, std::ptrdiff_t pointer )
{
	Reign::Standard::_Short_dbg_msg info;
	{
		sprintf_s( info, "Reign (Dbg). Pointer Name=%s Address=0x%X", name, pointer );
		PrintToDebugWindowOnly8( info );
	}
}
#endif

#define _Reign_apply_dx_cb_alignment REIGN_ALIGN(16)
#define _Delete_pointer_array Safe_DeleteArray
#define _Delete_pointer Safe_Delete

#endif // !__REIGN_CORE_INCLUDED__
