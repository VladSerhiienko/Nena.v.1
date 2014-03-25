#include <Windows.h>

#ifndef __NENA_TYPEDEVENT_INCLUDED__
#define __NENA_TYPEDEVENT_INCLUDED__

namespace Nena
{
	template <class TSender, class TEventArgs> class TypedEventHandler
	{
		// Static method pointer type (cdecl call version)
		typedef void(*MethodStub)(TypedEventHandler *DelegateRef, void *pObj, TSender *pSender, TEventArgs *pArgs);
		typedef TypedEventHandler<TSender, TEventArgs> Handler; // Event handler typew shortcut
		typedef void *Object; // Generic object pointer

		enum EventHandlerTraits : UINT32
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
			UCHAR m_Buffer[BufferSize];
		};

		template <typename Pointer>
		void Safe_Delete(Pointer pObj)
		{
			if (pObj) { delete pObj; pObj = nullptr; }
		}

		BOOL m_bAllocated; // We have used malloc to allocate additional memory to store function pointer 
		BOOL m_bAssigned; // We have already assigned this handler and can use then next member event handler 

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
				DelegatePtr->m_bAllocated = FALSE; // We did not use additional memory resources 
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
				_In_ DelegatePtr->m_pObjectFunction = ::malloc(sizeof(TObjectFunction));
				// Notify delegate object, that it should free the allocated memory then. Otherwise 
				// the object function pointer buffer will be freed when the delegate is destructed. 
				_In_ DelegatePtr->m_bAllocated = TRUE;
				// Replace the allocated memory with the constucted function pointer. 
				_In_ new (DelegatePtr->m_pObjectFunction) TObjectFunction(ObjectFunctionPtr);
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
				TObjectFunction const Method = *reinterpret_cast<TObjectFunction const *>(DelegatePtr->m_pObjectFunction);
				return (ObjectPtr->*Method)(SenderPtr, ArgsPtr); // Fire
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
			m_bAllocated(FALSE),
			m_bAssigned(FALSE),
			m_pObject(nullptr),
			m_pStub(nullptr)
		{
			ZeroMemory(m_Buffer, BufferSize);
		}

		template <class TObject>
		inline TypedEventHandler(_In_ TObject *ObjectPtr, _In_ void (TObject::*ObjectFunctionPtr)(TSender*, TEventArgs*)) :
			m_pObjectFunction(nullptr),
			m_pHandler(nullptr),
			m_bAllocated(FALSE),
			m_bAssigned(TRUE),
			m_pObject(nullptr),
			m_pStub(nullptr)
		{
			ZeroMemory(m_Buffer, BufferSize);
			FromMethod(ObjectPtr, ObjectFunctionPtr);
		}

		// Free all the dynamically allocated memory resources
		~TypedEventHandler()
		{
			if (m_bAllocated) // The additional memory was allocated to store the function pointer 
			{
				m_bAllocated = FALSE; // Its not allocated any more 
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
		inline void FromMethod(_In_ TObject *ObjectPtr, _In_ void (TObject::*ObjectFunctionPtr)(TSender*, TEventArgs*))
		{
			SelectFunctionPointerAllocator<TObject>::Type::Initialize(
				this, ObjectPtr,
				ObjectFunctionPtr
				);

			m_pObject = ObjectPtr;
			m_pStub = &DelegateStub<TObject>::MethodStub;
			m_bAssigned = TRUE;
		}

		inline void operator()(_In_ TSender *SenderPtr, _In_ TEventArgs *ArgsPtr)
		{
			return Invoke(SenderPtr, ArgsPtr);
		}

		inline void operator +=(_In_ Handler *SisterHandler)
		{
			return Attach(SisterHandler);
		}

		inline void Invoke(_In_ TSender *SenderPtr, _In_ TEventArgs *ArgsPtr)
		{
			if (m_bAssigned)
			{
				(*m_pStub)(this, m_pObject, SenderPtr, ArgsPtr);
			}

			if (m_pHandler)
			{
				m_pHandler->Invoke(SenderPtr, ArgsPtr);
			}
		}

		inline void Attach(_In_ Handler *SisterHandler)
		{
			if (SisterHandler) // We dont need empty handlers 
			{
				if (m_pHandler)
				{
					// Add event handler recursively 
					m_pHandler->Attach(SisterHandler);
				}
				else
				{
					// Assign to this delegate 
					m_pHandler = SisterHandler;
				}
			}
		}

	};

	template <class TSender, class TEventArgs> class TypedEvent
	{
		typedef TypedEventHandler<TSender, TEventArgs> Handler; // Event handler typew shortcut
		BOOL m_bAssigned; // Indicates whether this event has a handler 
		Handler *m_pHandler; // Event handler 

		template <typename Pointer>
		void Safe_Delete(Pointer pObj)
		{
			if (pObj) { delete pObj; pObj = nullptr; }
		}

	public:

		TypedEvent() : m_pHandler(nullptr), m_bAssigned(FALSE)
		{
		}

		~TypedEvent()
		{
			if (m_bAssigned)
			{
				// Safely delete member event handler 
				Safe_Delete(m_pHandler);  // Free used memory resources  
				// Avoid memory access violations 
			}
		}

		inline void operator()(_In_ TSender *SenderPtr, _In_ TEventArgs *ArgsPtr)
		{
			Invoke(SenderPtr, ArgsPtr);
		}

		inline void operator +=(_In_ Handler *EventHandlerPtr)
		{
			return Attach(EventHandlerPtr);
		}

		inline void Invoke(_In_ TSender *SenderPtr, _In_ TEventArgs *ArgsPtr)
		{
			if (m_bAssigned) // Assign to next delegate
			{
				m_pHandler->Invoke(SenderPtr, ArgsPtr);
			}

			// Ensure we wont have memory leaks. 
			Safe_Delete(ArgsPtr);
		}

		inline void Attach(_In_ Handler *EventHandlerPtr)
		{
			if (EventHandlerPtr) // We dont need empty handlers 
			{
				if (m_bAssigned) // Assign to next delegate
				{
					m_pHandler->Attach(EventHandlerPtr);
				}
				else
				{
					// Assign to this delegate 
					m_pHandler = EventHandlerPtr;
					m_bAssigned = true;
				}
			}
		}

	private:

		// Prevent copying 
		TypedEvent(TypedEvent const &);
		TypedEvent& operator= (TypedEvent const &);

	};

	// Type selector
	template <BOOL Condition, typename Then, typename Else> struct If;
	template <typename Then, typename Else> struct If <TRUE, Then, Else> { typedef Then Result; };
	template <typename Then, typename Else> struct If <FALSE, Then, Else> { typedef Else Result; };

	// Type selector
	template <BOOL Condition, typename Then, typename Else> struct If
	{
		template <BOOL InnerCondition> struct Selector;
		template <> struct Selector <TRUE> { typedef Then Result; };
		template <> struct Selector <FALSE> { typedef Else Result; };
		typedef typename Selector <Condition>::Result Result;
	};
}

#endif