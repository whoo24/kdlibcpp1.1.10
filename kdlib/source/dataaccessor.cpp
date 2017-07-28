#include "stdafx.h"

#include "dataaccessorimpl.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

DataAccessorPtr  getMemoryAccessor( MEMOFFSET_64  offset, size_t length) 
{
    return DataAccessorPtr( new MemoryAccessor(offset, length) );
}

///////////////////////////////////////////////////////////////////////////////

DataAccessorPtr  getEmptyAccessor()
{
    return DataAccessorPtr( new EmptyAccessor() );
}

///////////////////////////////////////////////////////////////////////////////

DataAccessorPtr  getRegisterAccessor(const std::wstring& registerName)
{
    return DataAccessorPtr(new RegisterAccessor(registerName));
}

///////////////////////////////////////////////////////////////////////////////

DataAccessorPtr getCacheAccessor(size_t bufferSize, const std::wstring&  location)
{
    return DataAccessorPtr(new CacheAccessor(bufferSize, location) );
}

///////////////////////////////////////////////////////////////////////////////

DataAccessorPtr getCacheAccessor(const std::vector<char>& buffer, const std::wstring&  location)
{
    return DataAccessorPtr(new CacheAccessor(buffer, location) );
}

///////////////////////////////////////////////////////////////////////////////

DataAccessorPtr getCacheAccessor(const void* rawBuffer, size_t bufferSize, const std::wstring&  location)
{
    std::vector<char>  buffer( reinterpret_cast<const char*>(rawBuffer), reinterpret_cast<const char*>(rawBuffer) + bufferSize);
    return getCacheAccessor(buffer, location);
}

///////////////////////////////////////////////////////////////////////////////

DataAccessorPtr getCacheAccessor(const NumVariant& var, const std::wstring&  location)
{
    return DataAccessorPtr(new CacheAccessor(var, location));
}

///////////////////////////////////////////////////////////////////////////////

}
