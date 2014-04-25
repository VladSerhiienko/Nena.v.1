/****************************************************************
 *  YAG2002 (http://yag2002.sourceforge.net)
 *  Copyright (C) 2005-2006, A. Botorabi
 *
 *  This program is free software; you can redistribute it and/or 
 *  modify it under the terms of the GNU Lesser General Public 
 *  License version 2.1 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public 
 *  License along with this program; if not, write to the Free 
 *  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 * 
 ****************************************************************/

/*###############################################################
 # class providing base mechanisms for loading and executing scripts
 #  currently Lua ( http://www.lua.org ) is supported as scripting 
 #  language.
 #
 #   date of creation:  04/15/2006
 #
 #   author:            ali botorabi (boto) 
 #      e-mail:         botorabi@gmx.net
 #
 ################################################################*/

#ifndef _VRC_SCRIPT_H_
    #error "do not include this file directly, include vrc_script.h instead"
#endif

// Template definitions of Param
template< typename TypeT >
BaseParam* Param< TypeT >::clone()
{
    Param< TypeT >* p_param = new Param< TypeT >( _value );
    return p_param;
}

// Template definitions of Params
template< typename TypeT >
void Params::add( TypeT param )
{
    typename Param< TypeT >* p_param = new Param< TypeT >( param );
    std::vector< BaseParam* >::push_back( p_param );
}

template< typename TypeT >
const TypeT& Params::getValue( unsigned int index )
{
    typename Param< TypeT >* p_param = static_cast< Param< TypeT >* >( at( index ) );
    return p_param->getValue();
}

template< typename TypeT >
void Params::setValue( unsigned int index, TypeT value )
{
    typename Param< TypeT >* p_param = static_cast< Param< TypeT >* >( at( index ) );
    p_param->setValue( value );
}

const type_info& Params::getTypeInfo( unsigned int index )
{
    return at( index )->getTypeInfo();
}

std::size_t Params::size()
{ 
    return std::vector< BaseParam* >::size(); 
}

// Template definitions of BaseScript
template< class T >
BaseScript< T >::BaseScript() :
_p_state( NULL ),
_methodTableIndex( 0 ),
_valid( false )
{
}

template< class T >
BaseScript< T >::~BaseScript()
{
    closeScript();
}

template< class T >
void BaseScript< T >::setupLuaLibs( lua_State* p_state, unsigned int usedlibs )
{
    luaopen_base( p_state );

    if ( usedlibs & BaseScript::LOADLIB )
    {
        luaopen_package( p_state );
        lua_settop( p_state, 0 );
    }
    if ( usedlibs & BaseScript::STRING )
    {
        luaopen_string( p_state );
        lua_settop( p_state, 0 );
    }
    if ( usedlibs & BaseScript::TABLE )
    {
        luaopen_table( p_state );
        lua_settop( p_state, 0 );
    }
    if ( usedlibs & BaseScript::DEBUG )
    {
        luaopen_debug( p_state );
        lua_settop( p_state, 0 );
    }
    if ( usedlibs & BaseScript::MATH )
    {
        luaopen_math( p_state );
        lua_settop( p_state, 0 );
    }
    if ( usedlibs & BaseScript::IO )
    {
        luaopen_io( p_state );
        lua_settop( p_state, 0 );
    }
}

template< class T >
void BaseScript< T >::loadScript( const std::string& luaModuleName, const std::string& scriptfile, unsigned int usedlibs ) throw ( ScriptingException )
{    
    assert( ( _p_state == NULL ) && "script file already created!" );

	_p_state = luaL_newstate();
    
    setupLuaLibs( _p_state, usedlibs );

	int status = luaL_dofile(_p_state, scriptfile.c_str());
	//int status = luaL_loadfile(_p_state, scriptfile.c_str());
    if ( status == LUA_ERRFILE )
        throw ScriptingException( std::string( "BaseScript: cannot open Lua file: " ) + scriptfile );
    else if ( status == LUA_ERRSYNTAX )
        throw ScriptingException( std::string( "BaseScript: syntax error in Lua file: " ) + scriptfile );
    else if ( status != 0 )
        throw ScriptingException( std::string( "BaseScript: error loading Lua file: " ) + scriptfile );

    lua_newtable( _p_state );
    _methodTableIndex = lua_gettop( _p_state );
    lua_pushstring( _p_state, luaModuleName.c_str() );
    lua_pushvalue( _p_state, _methodTableIndex );
	lua_settable(_p_state, -3);
	lua_pop(_p_state, 1);
    
    _valid = true;
    _scriptFile = scriptfile;
}

template< class T >
void BaseScript< T >::closeScript()
{
    // destroy lua state
    if ( _p_state )
    {
        lua_close( _p_state );
        _p_state = NULL;
    }

    // cleanup the method entries
    std::vector< MethodDescriptor* >::iterator p_beg = _methods.begin(), p_end = _methods.end();
    for ( ; p_beg != p_end; ++p_beg )
        delete ( *p_beg );

    _methods.clear();
}

template< class T >
const std::string& BaseScript< T >::getScriptFileName()
{
    return _scriptFile;
}

template< class T >
void BaseScript< T >::printParamsInfo( const Params& params )
{
    std::stringstream msg;
    Params::const_iterator p_beg = params.begin(), p_end = params.end();
    for ( ; p_beg != p_end; ++p_beg )
    {
        if ( ( *p_beg )->getTypeInfo() == typeid( float ) )
            msg << "float ";
        else if ( ( *p_beg )->getTypeInfo() == typeid( int ) )
            msg << "int ";
        else if ( ( *p_beg )->getTypeInfo() == typeid( double ) )
            msg << "double ";
        else if ( ( *p_beg )->getTypeInfo() == typeid( std::string ) )
            msg << "string ";
        else
            assert( NULL && "BaseScript< T >::printParamsInfo, unsupported parameter type" );
    }

    //log_debug << "( " << msg.str() << ")";
}

template< class T >
int BaseScript< T >::exposedMethodProxy( lua_State* p_state )
{
    // get number of arguments passed from script on function call
    int nargs = lua_gettop( p_state );

    int res = lua_islightuserdata( p_state, lua_upvalueindex( 1 ) );
    assert( res && "missing user data" );
    MethodDescriptor* p_entry = static_cast< MethodDescriptor* >( lua_touserdata( p_state, lua_upvalueindex( 1 ) ) );
    assert( p_entry && "could not find method in user data" );

    // get the instance
    T*      p_instance   = p_entry->_p_instance;
    Params& arguments    = *p_entry->_p_arguments;
    Params& returnvalues = *p_entry->_p_returnValues;

    // grab the arguments
    {
        if ( nargs != arguments.size() )
        {
            //log_error << "script error: exposed method called with wrong parameter count, file: " << p_instance->getScriptFileName() 
            //    << ", method name: " << p_entry->_methodName;

            p_instance->printParamsInfo( arguments );
            //log_error << std::endl;
            return 0;
        }
        for ( int cnt = 1; cnt <= nargs; cnt++ )
        {
            // get the param type
            const type_info& typeinfo = arguments.getTypeInfo( cnt - 1 );

            if ( lua_isnumber( p_state, cnt ) )
            {            
                if ( typeinfo == typeid( float ) )
                    arguments.setValue< float >( cnt - 1, lua_tonumber( p_state, cnt ) );
                else if ( typeinfo == typeid( int ) )
                    arguments.setValue< int >( cnt - 1, lua_tonumber( p_state, cnt ) );
                else if ( typeinfo == typeid( double ) )
                    arguments.setValue< double >( cnt - 1, lua_tonumber( p_state, cnt ) );
                else
                {
                    //log_error << "script error: exposed method called with unsupported or wrong parameter type, file: " << p_instance->getScriptFileName() << ", method name: " << p_entry->_methodName;
                    p_instance->printParamsInfo( arguments );
                    //log_error << std::endl;
                }
            }
            else if ( lua_isstring( p_state, cnt ) )
            {
                if ( typeinfo == typeid( std::string ) )
                    arguments.setValue< std::string >( cnt - 1, lua_tostring( p_state, cnt ) );
                else
                {
                    //log_error << "script error: exposed method called with unsupported or wrong parameter type, file: " << p_instance->getScriptFileName() << ", method name: " << p_entry->_methodName;
                    p_instance->printParamsInfo( arguments );
                    //log_error << std::endl;
                }
            }
        }
    }

    // call method
    ( p_instance->*( p_entry->_ptrMethod ) )( arguments, returnvalues );
    
    // push return values onto Lua stack
    {
        Params::iterator p_beg = returnvalues.begin(), p_end = returnvalues.end();
        for ( unsigned int index = 0; p_beg != p_end; ++p_beg, ++index )
        {
            const type_info& tinfo = ( *p_beg )->getTypeInfo();
            
            if ( tinfo == typeid( float ) )
                lua_pushnumber( p_instance->_p_state, GET_SCRIPT_PARAMVALUE( returnvalues, index, float ) );
            else if ( tinfo == typeid( int ) )
                lua_pushnumber( p_instance->_p_state, GET_SCRIPT_PARAMVALUE( returnvalues, index, int ) );
            else if ( tinfo == typeid( double ) )
                lua_pushnumber( p_instance->_p_state, GET_SCRIPT_PARAMVALUE( returnvalues, index, double ) );
            else if ( tinfo == typeid( std::string ) )
                lua_pushstring( p_instance->_p_state, ( GET_SCRIPT_PARAMVALUE( returnvalues, index, std::string ) ).c_str() );
        }
    }

    // return number of return values
    return returnvalues.size();
}

template< class T >
void BaseScript< T >::exposeMethod( const std::string& name, MethodPtr method, const Params& arguments, const Params& returnvalues )
{
    // create a method description
    MethodDescriptor* p_ms  = new MethodDescriptor;
    p_ms->_p_instance       = static_cast< T* >( this );
    p_ms->_methodName       = name;
    p_ms->_ptrMethod        = method;
    *p_ms->_p_arguments     = arguments;
    *p_ms->_p_returnValues  = returnvalues;
    _methods.push_back( p_ms );

    // register the exposed method in Lua
    lua_pushstring( _p_state, name.c_str() );
    lua_pushlightuserdata( _p_state, p_ms );
    lua_pushcclosure( _p_state, exposedMethodProxy, 1 );
    lua_settable( _p_state, _methodTableIndex );
}

template< class T >
void BaseScript< T >::callScriptFunction(const std::string& fcnname, Params* const p_arguments, Params* p_returnvalues)
{
    // get number of return values
    int numret = p_returnvalues ? p_returnvalues->size() : 0;

	if (fcnname.size())
    lua_getglobal( _p_state, fcnname.c_str() );

    // push arguments to Lua stack
    {
        Params::iterator p_beg = p_arguments->begin(), p_end = p_arguments->end();
        for ( unsigned int index = 0; p_beg != p_end; ++p_beg, ++index )
        {
            const type_info& tinfo = ( *p_beg )->getTypeInfo();
            
            if ( tinfo == typeid( float ) )
                lua_pushnumber( _p_state, GET_SCRIPT_PARAMVALUE( *p_arguments, index, float ) );
            else if ( tinfo == typeid( int ) )
                lua_pushnumber( _p_state, GET_SCRIPT_PARAMVALUE( *p_arguments, index, int ) );
            else if ( tinfo == typeid( double ) )
                lua_pushnumber( _p_state, GET_SCRIPT_PARAMVALUE( *p_arguments, index, double ) );
            else if ( tinfo == typeid( std::string ) )
                lua_pushstring( _p_state, ( GET_SCRIPT_PARAMVALUE( *p_arguments, index, std::string ) ).c_str() );
        }
    }

    // call the Lua function
    lua_call( _p_state, p_arguments->size(), numret );

    // get return values
    if ( numret > 0 )
    {        
        int sindex = -numret;
        for ( int cnt = 0; cnt < numret; ++cnt, ++sindex )
        {
            // get the param type
            const type_info& typeinfo = p_returnvalues->getTypeInfo( cnt );            
            if ( lua_isnumber( _p_state, sindex ) )
            {            
                if ( typeinfo == typeid( float ) )
                    p_returnvalues->setValue< float >( cnt, lua_tonumber( _p_state, sindex ) );
                else if ( typeinfo == typeid( int ) )
                    p_returnvalues->setValue< int >( cnt, lua_tonumber( _p_state, sindex ) );
                else if ( typeinfo == typeid( double ) )
                    p_returnvalues->setValue< double >( cnt, lua_tonumber( _p_state, sindex ) );
                else
                {
                    //log_error << "script error: return value(s) mismatch occured during calling script function: " << fcnname << std::endl;
                }
            }
            else if ( lua_isstring( _p_state, sindex ) )
            {
                if ( typeinfo == typeid( std::string ) )
                    p_returnvalues->setValue< std::string >( cnt, lua_tostring( _p_state, sindex ) );
                else
                {
                    //log_error << "script error: return value(s) mismatch occured during calling script function: " << fcnname << std::endl;
                }
            }
            //else
                //log_error << "script error: cannot retrieve return value for script function: " << fcnname << std::endl;
        }
    }

    // restore Lua stack
    lua_pop( _p_state, static_cast< int >( numret ) );
}

template< class T >
void BaseScript< T >::execute()
{
    if ( !_valid )
    {
        //log_error << "script error: cannot execute invalid script" << std::endl; 
        return;
    }

    lua_pop( _p_state, 1 );
    lua_call( _p_state, 0, 0 );
}
