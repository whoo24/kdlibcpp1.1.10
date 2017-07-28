#include "stdafx.h"

#include <sstream>

#include "kdlib/exceptions.h"

#include "udtfield.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

const TypeFieldPtr& FieldCollection::lookup(const std::wstring &name) const
{
    FieldList::const_reverse_iterator it;
    for ( it = m_fields.rbegin(); it !=  m_fields.rend(); ++it )
    {
        if ( (*it)->getName() == name )
            return *it;
    }

    std::wstringstream   sstr;
    sstr << L"field \"" << name << L" not found";

    throw TypeException( m_name, sstr.str() );
}

///////////////////////////////////////////////////////////////////////////////

TypeFieldPtr& FieldCollection::lookup(const std::wstring &name)
{
    const TypeFieldPtr &filedPtr = const_cast<const FieldCollection&>(*this).lookup(name);
    return const_cast<TypeFieldPtr&>(filedPtr);
}


///////////////////////////////////////////////////////////////////////////////

const TypeFieldPtr& FieldCollection::lookup(size_t index) const
{
    if ( index >= m_fields.size() )
        throw IndexException( index );

    return m_fields[index];
}

//////////////////////////////////////////////////////////////////////////////

TypeFieldPtr& FieldCollection::lookup(size_t index)
{
    const TypeFieldPtr &filedPtr = const_cast<const FieldCollection&>(*this).lookup(index);
    return const_cast<TypeFieldPtr&>(filedPtr);
}

//////////////////////////////////////////////////////////////////////////////

size_t FieldCollection::getIndex(const std::wstring &name) const
{
    size_t  index = 0;

    FieldList::const_iterator it;
    for ( it = m_fields.begin(); it != m_fields.end(); ++it, ++index )
    {
        if ( (*it)->getName() == name )
            return index;
    }

    std::wstringstream   sstr;
    sstr << L"field \"" << name << L" not found";

    throw TypeException( m_name, sstr.str() );
}

//////////////////////////////////////////////////////////////////////////////

TypeInfoPtr SymbolUdtField::getTypeInfo()
{
    return loadType(m_symbol);
}

///////////////////////////////////////////////////////////////////////////////

NumVariant EnumField::getValue() const
{
    NumVariant vr;
    m_symbol->getValue(vr);
    return vr;
}

///////////////////////////////////////////////////////////////////////////////

} // end kdlib namespace