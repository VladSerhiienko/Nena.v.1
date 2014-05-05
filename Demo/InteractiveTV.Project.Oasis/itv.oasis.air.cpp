#include "app.precompiled.h"
#include "itv.oasis.air.h"

struct SrwLockExclusiveGuard
{
	::SRWLOCK &m_lock;
	SrwLockExclusiveGuard( ::SRWLOCK &lock ) : m_lock( lock ) { AcquireSRWLockExclusive( &m_lock ); }
	~SrwLockExclusiveGuard( ) { ReleaseSRWLockExclusive( &m_lock ); }
};

InteractiveTV::Oasis::Air::Air( )
{
	InitializeSRWLock( &msg_collection_guard );
}

InteractiveTV::Oasis::Air::~Air( )
{
}

InteractiveTV::Oasis::Air *InteractiveTV::Oasis::Air::GetForCurrentThread()
{
	static Air s_air; return &s_air;
}

void InteractiveTV::Oasis::Air::Grab(
	Air::String msg
	)
{
	return Grab(msg, Message::kInfo);
}

void InteractiveTV::Oasis::Air::Grab(
	Air::Message::Type type, 
	Air::String msg
	)
{
	return Grab(msg, type);
}

void InteractiveTV::Oasis::Air::Grab(
	Air::String msg, 
	Air::Message::Type type
	)
{
	return Grab(nullptr, msg, type);
}

void InteractiveTV::Oasis::Air::Grab(
	Oasis::Object *obj, Air::String msg
	)
{
	return Grab(obj, msg, Message::kInfo);
}

void InteractiveTV::Oasis::Air::Grab(
	Oasis::Object *obj, 
	Air::Message::Type type, 
	Air::String msg
	)
{
	return Grab( obj, msg, type );
}

void InteractiveTV::Oasis::Air::Grab(
	Oasis::Object *obj, 
	Air::String msg, 
	Air::Message::Type type
	)
{
	if (msg.size() > MaxMessageSize) msg = msg.substr(0, MaxMessageSize - 3) + "...";
	if (obj) msg.insert(0, obj->GetName() + "[");
	else msg.insert(0, _Oasis_origin "{::}/[");

	switch (type)
	{
	case Message::Type::kInfo:		msg.insert(0, "[!]"); break;
	case Message::Type::kWarning:	msg.insert(0, "[@]"); break;
	case Message::Type::kError:		msg.insert(0, "[#]"); break;
	case Message::Type::kCritical:	msg.insert(0, "[*]"); break;
	case Message::Type::kFatal:		msg.insert(0, "[~]"); break;
	}

	msg += ']';

	{
		SrwLockExclusiveGuard guard( 
			msg_collection_guard 
			);

		msg_collection.push_back( { msg, type } );
		if ( msg_collection.size( ) > MaxMessageCount )
			msg_collection.pop_front( );

		std::cout << msg << std::endl;
		OutputDebugStringA( msg.c_str( ) );
		OutputDebugStringA( "\n" );
	}
}

InteractiveTV::Oasis::Air::StringDeque::const_iterator 
InteractiveTV::Oasis::Air::Begin()
{
	return msg_collection.cbegin();
}

InteractiveTV::Oasis::Air::StringDeque::const_iterator 
InteractiveTV::Oasis::Air::End()
{
	return msg_collection.cend();
}

InteractiveTV::Oasis::Air::StringDeque::size_type 
InteractiveTV::Oasis::Air::Size()
{
	return msg_collection.size();
}

