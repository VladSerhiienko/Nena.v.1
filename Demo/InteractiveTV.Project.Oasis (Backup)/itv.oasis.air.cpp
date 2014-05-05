#include "app.precompiled.h"
#include "itv.oasis.air.h"

struct SrwLockExclusiveGuard
{
	::SRWLOCK &m_lock;
	SrwLockExclusiveGuard( ::SRWLOCK &lock ) : m_lock( lock ) { AcquireSRWLockExclusive( &m_lock ); }
	~SrwLockExclusiveGuard( ) { ReleaseSRWLockExclusive( &m_lock ); }
};

InteractiveTV::Project::Oasis::Air::Air( )
{
	InitializeSRWLock( &msg_collection_guard );
}

InteractiveTV::Project::Oasis::Air::~Air( )
{
}

InteractiveTV::Project::Oasis::Air *InteractiveTV::Project::Oasis::Air::GetForCurrentThread()
{
	static Air s_air; return &s_air;
}

void InteractiveTV::Project::Oasis::Air::Grab(
	Air::String msg
	)
{
	return Grab(msg, Message::kInfo);
}

void InteractiveTV::Project::Oasis::Air::Grab(
	Air::Message::Type type, 
	Air::String msg
	)
{
	return Grab(msg, type);
}

void InteractiveTV::Project::Oasis::Air::Grab(
	Air::String msg, 
	Air::Message::Type type
	)
{
	return Grab(nullptr, msg, type);
}

void InteractiveTV::Project::Oasis::Air::Grab(
	Oasis::Object *obj, Air::String msg
	)
{
	return Grab(obj, msg, Message::kInfo);
}

void InteractiveTV::Project::Oasis::Air::Grab(
	Oasis::Object *obj, 
	Air::Message::Type type, 
	Air::String msg
	)
{
	return Grab( obj, msg, type );
}

void InteractiveTV::Project::Oasis::Air::Grab(
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

InteractiveTV::Project::Oasis::Air::StringDeque::const_iterator 
InteractiveTV::Project::Oasis::Air::Begin()
{
	return msg_collection.cbegin();
}

InteractiveTV::Project::Oasis::Air::StringDeque::const_iterator 
InteractiveTV::Project::Oasis::Air::End()
{
	return msg_collection.cend();
}

InteractiveTV::Project::Oasis::Air::StringDeque::size_type 
InteractiveTV::Project::Oasis::Air::Size()
{
	return msg_collection.size();
}

