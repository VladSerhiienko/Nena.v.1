#if _MSC_VER
 #pragma once
#endif

#ifndef __REIGN_TYPEDEVENTHANDLER_INCLUDED__
 #define __REIGN_TYPEDEVENTHANDLER_INCLUDED__
 #include "ReignTypeSelector.hxx"


// SUMMARY: Standard compliant and portable event handling mechanism. Two or three levels of indirections are
// supported. In addition, now its a binary representation of a member function pointer as an internal data 
// structure so that it becomes possible to compare it with others. 
// REMARKS: The size of the internal buffer can be customized by defining a proper macro or additional template
// parameter.
// NOTE: The compiler with a descent inline optimization can optimize it out and yield the code with 
// member function pointers as a non-typed template parameters. 

namespace Reign 
{ 
namespace Foundation 
{ 

template <class TSender, class TEventArgs> class TypedEventHandler
{
	// Static method pointer type (cdecl call version)
	typedef void (*MethodStub)(TypedEventHandler *DelegateRef, void *pObj, TSender *pSender, TEventArgs *pArgs); 
	typedef TypedEventHandler<TSender, TEventArgs> Handler; // Event handler typew shortcut
	typedef void *Object; // Generic object pointer

	enum EventHandlerTraits
	{
		// For non-virtual functions its 4 bytes for Win32, and 8 bytes for x64. Otherwise we should allocate more memory. 
		BufferSize = 0x08
	};

	Object m_pObject; // Object
	MethodStub m_pStub; // Object callback ( cdecl call )
	Handler *m_pHandler; // Event handler 

	union // Object function pointer force qword at least 
	{
		Object m_pObjectFunction; // Actual boxed function pointer 
		// Used for allocating the object function pointer. If pointer size is less or equals 8 bytes, memory will be allocated automatically 
		// after the delegate object is constructed. Then in FunctionPointerByValue::Initialize we should only replace the memory with the new 
		// construct function pointer object.  Otherwise the we utilize malloc function to allocate more then 8 bytes and replace the newly allocated 
		// memory with the constructed function pointer object. 
		unsigned char m_Buffer[BufferSize]; 
	};

	bool m_bAllocated; // We have used malloc to allocate additional memory to store function pointer 
	bool m_bAssigned; // We have already assigned this handler and can use then next member event handler 

	template <class TObject>
	struct DelegateStub
	{
		inline static void MethodStub( // Customly converts thiscall to cdecl
			_In_ TypedEventHandler *DelegatePtr, // This delegate pointer 
			_In_ Object ObjectPtr, // Boxed recevier object pointer 
			_In_ TSender *SenderPtr, // Sender object pointer 
			_In_ TEventArgs *ArgsPtr // Event arguments object pointer 
			)
		{
			TObject *pObj = static_cast<TObject*>(ObjectPtr); // Unbox recevier object pointer 
			return SelectFunctionPointerAllocator<TObject>::Type::Invoke( // Hook event receiver
				DelegatePtr, pObj, 
				SenderPtr, ArgsPtr
				);
		}
	};

	template <class TObject>
	struct FunctionPointerByValue
	{
		typedef void (TObject::*TObjectFunction)(TSender*, TEventArgs*);

		// Allocates function pointer by value (consuming buffer)
		inline static void Initialize(
			_In_ TypedEventHandler *DelegatePtr, // This delegate pointer 
			_In_ TObject *ObjectPtr, // Unboxed recevier object pointer
			// Unboxed recevier object pointer member function pointer 
			_In_ void (TObject::*ObjectFunctionPtr)(TSender*, TEventArgs*)
			)
		{
			DelegatePtr->m_bAllocated = false; // We did not use additional memory resources 
			new (DelegatePtr->m_Buffer) TObjectFunction(ObjectFunctionPtr); // Replace buffer memory 
		}

		// Invoke object callback function 
		inline static void Invoke(
			_In_ TypedEventHandler *DelegatePtr, // This delegate pointer
			_In_ TObject *ObjectPtr, // Unboxed recevier object pointer
			_In_ TSender *SenderPtr, // Sender object pointer 
			_In_ TEventArgs *ArgsPtr // Event arguments object pointer 
			)
		{
			// Unbox and get actual object member function ( thiscall )
			TObjectFunction const Method = *reinterpret_cast<TObjectFunction const *>(DelegatePtr->m_Buffer);
			return (ObjectPtr->*Method)(SenderPtr, ArgsPtr); // Fire
		}
	};

	template <class TObject>
	struct FunctionPointerByAlloc
	{
		typedef void (TObject::*TObjectFunction)(TSender*, TEventArgs*);

		// Allocates function pointer with malloc
		inline static void Initialize(
			_In_ TypedEventHandler *DelegatePtr, // This delegate pointer 
			_In_ TObject *ObjectPtr, // Unboxed recevier object pointer
			// Unboxed recevier object pointer member function pointer 
			_In_ void (TObject::*ObjectFunctionPtr)(TSender*, TEventArgs*)
			)
		{
			// Allocate more memory 
			_In_ DelegatePtr->m_pObjectFunction = ::malloc( sizeof(TObjectFunction) );
			// Notify delegate object, that it should free the allocated memory then. Otherwise 
			// the object function pointer buffer will be freed when the delegate is destructed. 
			_In_ DelegatePtr->m_bAllocated = true;
			// Replace the allocated memory with the constucted function pointer. 
			_In_ new ( DelegatePtr->m_pObjectFunction ) TObjectFunction( ObjectFunctionPtr );
		}

		// Invoke object callback function 
		inline static void Invoke(
			_In_ TypedEventHandler *DelegatePtr, // This delegate pointer
			_In_ TObject *ObjectPtr, // Unboxed recevier object pointer
			_In_ TSender *SenderPtr, // Sender object pointer 
			_In_ TEventArgs *ArgsPtr // Event arguments object pointer 
			)
		{
			// Unbox and get actual object member function ( thiscall )
			TObjectFunction const Method = *reinterpret_cast<TObjectFunction const *>( DelegatePtr->m_pObjectFunction );
			return ( ObjectPtr->*Method )( SenderPtr, ArgsPtr ); // Fire
		}
	};

	template <class TObject>
	struct SelectFunctionPointerAllocator
	{
		// Check whether we need to allocate more then 8 bytes. 
		enum { Condition = sizeof(void (TObject::*)(TSender*, TEventArgs*)) <= BufferSize };
		// No additional memory allocation required
		typedef FunctionPointerByValue<TObject> Then; 
		// Additional memory allocation required
		typedef FunctionPointerByAlloc<TObject> Else;  
		// Function pointer allocator type 
		typedef typename If <Condition, Then, Else>::Result Type; 
	};

public:

	inline TypedEventHandler() : 
		m_pObjectFunction(nullptr), 
		m_pHandler(nullptr), 
		m_bAllocated(false),
		m_bAssigned(false),
		m_pObject(nullptr), 
		m_pStub(nullptr)
	{
		ZeroMemory(m_Buffer, BufferSize);
	}

	template <class TObject>
	inline TypedEventHandler( _In_ TObject *ObjectPtr, _In_ void (TObject::*ObjectFunctionPtr)(TSender*, TEventArgs*) ) : 
		m_pObjectFunction(nullptr), 
		m_pHandler(nullptr), 
		m_bAllocated(false),
		m_bAssigned(true),
		m_pObject(nullptr), 
		m_pStub(nullptr)
	{
		ZeroMemory(m_Buffer, BufferSize);
		FromMethod( ObjectPtr, ObjectFunctionPtr );
	}

	// Free all the dynamically allocated memory resources
	~TypedEventHandler()
	{
		if (m_bAllocated) // The additional memory was allocated to store the function pointer 
		{
			m_bAllocated = false; // Its not allocated any more 
			::free(m_pObjectFunction); // 1> Free used memory resources 
			m_pObjectFunction = nullptr; // 2> Avoid memory access violations  
		}

		if (m_pHandler) // Check whether we have used more then one handler 
		{
			// Safely delete member event handler 
			delete m_pHandler;  // 1> Free used memory resources  
			m_pHandler = nullptr; // 2> Avoid memory access violations 
		}
	}

	template <class TObject>
	inline void FromMethod( _In_ TObject *ObjectPtr, _In_ void (TObject::*ObjectFunctionPtr)(TSender*, TEventArgs*)	)
	{
		SelectFunctionPointerAllocator<TObject>::Type::Initialize(
			this, ObjectPtr, 
			ObjectFunctionPtr
			);

		m_pObject = ObjectPtr;
		m_pStub = &DelegateStub<TObject>::MethodStub;
		m_bAssigned = true;
	}

	inline void operator()( _In_ TSender *SenderPtr, _In_ TEventArgs *ArgsPtr )
	{
		return Invoke( SenderPtr, ArgsPtr );
	}

	inline void operator +=( _In_ Handler *SisterHandler )
	{
		return Attach( SisterHandler );
	}

	inline void Invoke( _In_ TSender *SenderPtr, _In_ TEventArgs *ArgsPtr )
	{
		if ( m_bAssigned ) 
		{
			(*m_pStub)( this, m_pObject, SenderPtr, ArgsPtr );
		}

		if ( m_pHandler )
		{
			m_pHandler->Invoke( SenderPtr, ArgsPtr );
		}
	}

	inline void Attach( _In_ Handler *SisterHandler )
	{
		if ( SisterHandler ) // We dont need empty handlers 
		{
			if ( m_pHandler )
			{
				// Add event handler recursively 
				m_pHandler->Attach( SisterHandler );
			}
			else
			{
				// Assign to this delegate 
				m_pHandler = SisterHandler;
			}
		}
	}

}; // Typed event handler

}; // Foundation
}; // Reign

#endif // !__REIGN_TYPEDEVENTHANDLER_INCLUDED__



