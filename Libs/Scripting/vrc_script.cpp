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

#include "vrc_script.h"

namespace vrc
{

Params::Params()
{
}

Params::~Params()
{
    Params::clear();
}

void Params::clear()
{
    // delete parameters
    iterator p_beg = begin(), p_end = end();
    for ( ; p_beg != p_end; ++p_beg )
        delete ( *p_beg );

    std::vector< BaseParam* >::clear();
}

Params& Params::operator = ( const Params& params )
{
    Params::const_iterator p_beg = params.begin(), p_end = params.end();
    for ( ; p_beg != p_end; ++p_beg )
        push_back( ( *p_beg )->clone() );
    
    return *this;
}

} // namespace vrc
