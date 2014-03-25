#if _MSC_VER
 #pragma once
#endif

#ifndef __REIGN_TYPEDEVENT_INCLUDED__
 #define __REIGN_TYPEDEVENT_INCLUDED__

 #include "ReignTypedEventHandler.hxx"

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

template <class TSender, class TEventArgs> class TypedEvent
{
	typedef TypedEventHandler<TSender, TEventArgs> Handler; // Event handler typew shortcut
	bool m_bAssigned; // Indicates whether this event has a handler 
	Handler *m_pHandler; // Event handler 

public:

	TypedEvent() : m_pHandler(nullptr), m_bAssigned(false)
	{
	}

	~TypedEvent()
	{
		if ( m_bAssigned )
		{
			// Safely delete member event handler 
			Safe_Delete( m_pHandler );  // Free used memory resources  
			// Avoid memory access violations 
		}
	}

	inline void operator()( _In_ TSender *SenderPtr, _In_ TEventArgs *ArgsPtr )
	{
		Invoke( SenderPtr, ArgsPtr );
	}

	inline void operator +=( _In_ Handler *EventHandlerPtr )
	{
		return Attach( EventHandlerPtr );
	}

	inline void Invoke( _In_ TSender *SenderPtr, _In_ TEventArgs *ArgsPtr )
	{
		if ( m_bAssigned ) // Assign to next delegate
		{
			m_pHandler->Invoke( SenderPtr, ArgsPtr );
		}

		// Ensure we wont have memory leaks. 
		Safe_Delete( ArgsPtr );
	}

	inline void Attach( _In_ Handler *EventHandlerPtr )
	{
		if ( EventHandlerPtr ) // We dont need empty handlers 
		{
			if ( m_bAssigned ) // Assign to next delegate
			{
				m_pHandler->Attach( EventHandlerPtr );
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

}; // Typed event

}; // Foundation
}; // Reign

#endif // !__REIGN_TYPEDEVENT_INCLUDED__
