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
#define _VRC_SCRIPT_H_

#include <vector>
#include <string>
#include <stdexcept>

// Lua headers
extern "C"
{
#include <Nena\Lua\lua.h>
#include <Nena\Lua\lstate.h>
#include <Nena\Lua\lualib.h>
#include <Nena\Lua\lauxlib.h>
}

namespace vrc
{

//! Macro for getting and setting parameter values in exposed methods
/**
* 'params' is a Params object.
* 'index'  is the index in 'params' object for desired value.
* 'type'   is the type of parameter at specified index. Supported types are: float, double, int, std::string
* 'value'  is the value to be set at given index in params object.
*
* Note: there are no typechecks provided. Make sure that you use these macros -- in particular 'index' and 'type' -- according to 
*       specifications of your exposed methods.
*/
#define GET_SCRIPT_PARAMVALUE( params, index, type  )           const_cast< Params& >( params ).getValue< type >( index )
#define SET_SCRIPT_PARAMVALUE( params, index, type, value )     const_cast< Params& >( params ).setValue< type >( index, value )


//! Class for scripting related exceptions
class ScriptingException : public std::runtime_error
{
    public:
                                                ScriptingException( const std::string& reason ) :
                                                 std::runtime_error( reason )
                                                {
                                                }

        virtual                                 ~ScriptingException() throw() {}

                                                ScriptingException( const ScriptingException& e ) :
                                                 std::runtime_error( e )
                                                {
                                                }

    //protected:

                                                ScriptingException();

        ScriptingException&                     operator = ( const ScriptingException& );
};

//! Class describing one single parameter for an exposed method
class BaseParam
{
    public:

                                                BaseParam() {}

        virtual                                 ~BaseParam() {}

        //! Get parameter type
        virtual const std::type_info&           getTypeInfo() const = 0;

        //! Clone parameter
        virtual BaseParam*                      clone() = 0;
};

//! Parameter class supporting following types: float, double, int, std::string
template< typename TypeT >
class Param : public BaseParam
{
    public:

                                                Param() {}

                                                Param( TypeT value ) : 
                                                 _value( value )
                                                {}

        virtual                                 ~Param() {}

        //! Set value
        void                                    setValue( const TypeT& value )
                                                {
                                                    _value = value;
                                                }

        //! Get value
        const TypeT&                            getValue()
                                                {
                                                    return _value;
                                                }

        //! Get value type
        const std::type_info&                   getTypeInfo() const { return typeid( TypeT ); }

        //! Clone Param
        BaseParam*                              clone();

    //protected:

        //! Param value
        TypeT                                   _value;
};

template< class T > class BaseScript;

//! Container used for defining parameters and return values of a Lua interface method.
class Params: public std::vector< BaseParam* >
{
    public:

                                                Params();

        virtual                                 ~Params();

        //! Add a parameter of type TypeT ( used for built-in types )
        template< typename TypeT >
        void                                    add( TypeT param );

        //! Get value of param at given index
        template< typename TypeT >
        const TypeT&                            getValue( unsigned int index );

        //! Set value of param at given index
        template< typename TypeT >
        void                                    setValue( unsigned int index, TypeT value );

        //! Get type info of param at index 'index'
        inline const type_info&                 getTypeInfo( unsigned int index );

        //! Get count of parameters
        inline std::size_t                      size();

        //! Clear params
        void                                    clear();

        //! Assignment operator
        Params&                                 operator = ( const Params& params );

    //friend class BaseScript;
};

//! Base class for scripts
template< class T >
class BaseScript
{
    public:

        //! Lua libs which should be set up during creation
        //! Note: Lua 'base' lib is always loaded!
        enum
        {
            LOADLIB   = 0x02,
            STRING    = 0x04,
            TABLE     = 0x08,
            DEBUG     = 0x10,
            MATH      = 0x20,
            IO        = 0x40,

            ALL       = 0x7F,
            DEFAULT   = 0x6D  // all libs except DEBUG and LOADLIB
        };

        //! Define a method pointer type for exposed methods
        /**
        * Exposed method must have following signature
        *
        *  void T::methodname( const Params& arguments, Params& returnvalues )
        *
        */
        typedef void( T::*MethodPtr )( const Params&, Params& );

                                                BaseScript();

        virtual                                 ~BaseScript();

        //! The script can access entity's exposed interface using 'luaModuleName.'
        /**
        * Create a Script object given the script as string. 
        * The script can access entity's exposed interface using 'luaModuleName.'
        * Set appropriate flags for 'usedlibs' in order to get those Lua libs set up during creation.
        */
        void                                    loadScript( const std::string& luaModuleName, const std::string& scriptfile, unsigned int usedlibs = BaseScript::DEFAULT ) throw ( ScriptingException );

        //! Expose a method from C++ to Lua script
        /**
        * Fill the method 'arguments' in right order into 'arguments'.
        * If 'returnvalues' has no Params then the method will have no return values.
        */
        void                                    exposeMethod( const std::string& name, MethodPtr method, const Params& arguments, const Params& returnvalues = Params() );

        //! Execute script, call this after loading a script and once when all methods are exposed
        void                                    execute();

        //! Call script function 'fcnname' with given arguments 'p_arguments', let 'p_returnvalues' be NULL for having no return values expected.
        /**
        * Use this method after calling 'execute'.
        */
        void                                    callScriptFunction( const std::string& fcnname, Params* const p_arguments, Params* p_returnvalues = NULL );

        //! Close a loaded script and clean up associated resources.
        void                                    closeScript();

        //! Returns the script file name used during creation
        const std::string&                      getScriptFileName();

        //! Proxy function for all exposed methods
        /**
        * This method is used internally, do not use it!
        */
        static int                              exposedMethodProxy( lua_State* p_state );

    //protected:

        //! Setup specified Lua libraries, used during creation
        void                                    setupLuaLibs( lua_State* p_state, unsigned int usedlibs );

        //! Print parameter list info ( used for debugging scripts )
        void                                    printParamsInfo( const Params& params );

        //! Lua state
        lua_State*                              _p_state;

        //! Class for holding a method description used for exposing
        class MethodDescriptor
        {
            public:

                                                    MethodDescriptor() :
                                                     _p_instance( NULL ),
                                                     _ptrMethod( NULL )
                                                    {
                                                        _p_arguments   = new Params;
                                                        _p_returnValues = new Params;
                                                    }

            virtual                                 ~MethodDescriptor()
                                                    {
                                                        delete _p_arguments;
                                                        delete _p_returnValues;
                                                    }

            //! Method name
            std::string                             _methodName;

            //! Method arguments in right order
            Params*                                 _p_arguments;
            
            //! Method's return values in right order
            Params*                                 _p_returnValues;

            //! Pointer to class instance
            T*                                      _p_instance;

            //! Method pointer
            MethodPtr                               _ptrMethod;
        };

        //! Method list used for cleanup on object destruction
        std::vector< MethodDescriptor* >        _methods;

        //! Script file name
        std::string                             _scriptFile;

        //! Set during creating a script
        bool                                    _valid;

        //! Lua table index containing closures
        int                                     _methodTableIndex;

    //private:

        //! Avoid copy constructor
                                                BaseScript( const BaseScript& );

        //! Avoid assignment operator
        BaseScript&                             operator = ( const BaseScript& );
};

#include "vrc_script.inl"

}

#endif _VRC_SCRIPT_H_
