#include <utility>
#include "nts.hpp"
#include "variables.hpp"

using std::move;

namespace nts
{

//------------------------------------//
// VariableUse                        //
//------------------------------------//

VariableUse::VariableUse ( VariableReference & vref ) :
	user_type ( UserType::VariableReference )
{
	_var = nullptr;
	user_ptr.vref = & vref;
}

VariableUse::VariableUse ( ArrayWrite & awr ) :
	user_type ( UserType::ArrayWrite )
{
	_var = nullptr;
	user_ptr.arr_wr = & awr;
}

VariableUse::VariableUse ( CallTransitionRule & ctr ) :
	user_type ( UserType::CallTransitionRule )
{
	_var = nullptr;
	user_ptr.ctr = & ctr;
}

VariableUse::VariableUse ( UserType type, UserPtr ptr ) :
	user_type ( type ),
	user_ptr  ( ptr  )
{
	_var = nullptr;
}

VariableUse::VariableUse ( const VariableUse & orig ) :
	user_type ( orig.user_type )
{
	// set() can call release()
	_var = nullptr;
	set ( orig.get() );
}

VariableUse::VariableUse ( VariableUse && old ) :
	user_type ( old.user_type )
{
	_var = nullptr;
	set ( old.release() );
}

VariableUse::~VariableUse()
{
	release();
}

void VariableUse::set ( Variable * v )
{
	release();
	if ( v )
	{
		_pos = v->_uses.insert ( v->_uses.cend(), this );
		_var = v;
	}
}

Variable * VariableUse::release()
{
	Variable * v = _var;
	if ( _var )
	{

		_var->_uses.erase ( _pos );
		_var = nullptr;
	}
	return v;
}

VariableUse & VariableUse::operator= ( Variable * var )
{
	set ( var );
	return *this;
}

VariableUse & VariableUse::operator= ( const VariableUse & orig )
{
	set ( orig.get() );
	return *this;
}

VariableUse & VariableUse::operator= ( VariableUse && old )
{
	set ( old.release() );
	return *this;
}

//------------------------------------//
// VariableUseContainer               //
//------------------------------------//

VariableUseContainer :: VariableUseContainer ( VariableReference & vref )
{
	_type = VariableUse::UserType::VariableReference;
	_ptr.vref = & vref;
}

VariableUseContainer :: VariableUseContainer ( ArrayWrite & arr_wr )
{
	_type = VariableUse::UserType::ArrayWrite;
	_ptr.arr_wr = & arr_wr;
}

VariableUseContainer :: VariableUseContainer ( CallTransitionRule & ctr )
{
	_type = VariableUse::UserType::CallTransitionRule;
	_ptr.ctr = & ctr;
}

VariableUseContainer :: VariableUseContainer ( Havoc & hvc )
{
	_type = VariableUse::UserType::Havoc;
	_ptr.hvc = & hvc;
}

void VariableUseContainer :: push_back ( Variable * v )
{
	std::vector < VariableUse > :: push_back ( VariableUse ( _type, _ptr ) );
	std::vector < VariableUse > :: back() = v;
}

//------------------------------------//
// VariableContainer                  //
//------------------------------------//

VariableContainer::VariableContainer ( list < Variable * > l ) :
	list < Variable * > ( l )
{
	;
}

VariableContainer::~VariableContainer()
{
	for ( Variable * v : *this )
	{
		delete v;
	}
	clear();
}

VariableContainer & VariableContainer::operator= ( VariableContainer && old )
{
	list < Variable * > :: operator= ( move ( old ) );
	return *this;
}

}; // namespace nts;
