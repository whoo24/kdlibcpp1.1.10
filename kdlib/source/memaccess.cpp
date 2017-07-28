#include "stdafx.h"

#include <vector>

#include "kdlib/memaccess.h"
#include "kdlib/exceptions.h"

namespace kdlib {


///////////////////////////////////////////////////////////////////////////////

template<typename T>
T
readPtr( MEMOFFSET_64 offset )
{
    T val = 0;
    readMemory( offset, &val, sizeof(T), false );
    return val;
}

///////////////////////////////////////////////////////////////////////////////

unsigned char ptrByte( MEMOFFSET_64 offset )
{
    return readPtr<unsigned char>(offset);
}

///////////////////////////////////////////////////////////////////////////////

unsigned short ptrWord( MEMOFFSET_64 offset )
{
    return readPtr<unsigned short>(offset);
}

///////////////////////////////////////////////////////////////////////////////

unsigned long  ptrDWord( MEMOFFSET_64 offset )
{
    return readPtr<unsigned long>(offset);
}

///////////////////////////////////////////////////////////////////////////////

unsigned long long  ptrQWord( MEMOFFSET_64 offset )
{
    return readPtr<unsigned long long>(offset);
}

///////////////////////////////////////////////////////////////////////////////

unsigned long long ptrMWord( MEMOFFSET_64 offset )
{
    return ptrSize() == 8 ? ptrQWord( offset ) : ptrDWord( offset );
}

///////////////////////////////////////////////////////////////////////////////

char ptrSignByte( MEMOFFSET_64 offset )
{
    return readPtr<char>(offset);
}

///////////////////////////////////////////////////////////////////////////////

short ptrSignWord( MEMOFFSET_64 offset )
{
    return readPtr<short>(offset);
}

///////////////////////////////////////////////////////////////////////////////

long ptrSignDWord( MEMOFFSET_64 offset )
{
    return readPtr<long>(offset);
}

///////////////////////////////////////////////////////////////////////////////

long long ptrSignQWord( MEMOFFSET_64 offset )
{
    return readPtr<long long>(offset);
}

///////////////////////////////////////////////////////////////////////////////

long long ptrSignMWord( MEMOFFSET_64 offset )
{
    return ptrSize() == 8 ? ptrSignQWord( offset ) : ptrSignDWord( offset );
}

///////////////////////////////////////////////////////////////////////////////

float ptrSingleFloat( MEMOFFSET_64 offset )
{
    return readPtr<float>(offset);
}

///////////////////////////////////////////////////////////////////////////////

double ptrDoubleFloat( MEMOFFSET_64 offset )
{
    return readPtr<double>(offset);
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void setImpl( MEMOFFSET_64 offset, T value )
{
    writeMemory( offset, &value, sizeof(value), false );
}

///////////////////////////////////////////////////////////////////////////////

void setByte( MEMOFFSET_64 offset, unsigned char value )
{
    setImpl(offset, value);
}

///////////////////////////////////////////////////////////////////////////////

void setWord( MEMOFFSET_64 offset, unsigned short value )
{
    setImpl(offset, value);
}

///////////////////////////////////////////////////////////////////////////////


void setDWord( MEMOFFSET_64 offset, unsigned long value )
{
    setImpl(offset, value);
}

///////////////////////////////////////////////////////////////////////////////


void setQWord( MEMOFFSET_64 offset, unsigned long long value )
{
    setImpl(offset, value);
}

///////////////////////////////////////////////////////////////////////////////


void setSignByte( MEMOFFSET_64 offset, char value )
{
    setImpl(offset, value);
}

///////////////////////////////////////////////////////////////////////////////


void setSignWord( MEMOFFSET_64 offset, short value )
{
    setImpl(offset, value);
}

///////////////////////////////////////////////////////////////////////////////


void setSignDWord( MEMOFFSET_64 offset, long value )
{
    setImpl(offset, value);
}

///////////////////////////////////////////////////////////////////////////////


void setSignQWord( MEMOFFSET_64 offset, long long value )
{
    setImpl(offset, value);
}

///////////////////////////////////////////////////////////////////////////////


void setSingleFloat( MEMOFFSET_64 offset, float value )
{
    setImpl(offset, value);
}

///////////////////////////////////////////////////////////////////////////////


void setDoubleFloat( MEMOFFSET_64 offset, double value )
{
    setImpl(offset, value);
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
std::vector<T>
loadArray( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    if (!phyAddr && !isVaRegionValid(offset, number*sizeof(T)))
        throw MemoryException(offset);

    std::vector<T>   buffer(number);

    if (number)
        readMemory( offset, &buffer[0], number*sizeof(T), phyAddr );

    return buffer;
}

/////////////////////////////////////////////////////////////////////////////////////

std::vector<unsigned char> loadBytes( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    return loadArray<unsigned char>( offset, number, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

std::vector<unsigned short> loadWords( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    return loadArray<unsigned short>( offset, number, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////

std::vector<unsigned long> loadDWords( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    return loadArray<unsigned long>( offset, number, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////

std::vector<unsigned long long> loadQWords( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    return loadArray<unsigned long long>( offset, number, phyAddr );
}

/////////////////////////////////////////////////////////////////////////////////////

std::vector<char> loadSignBytes( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    return loadArray<char>( offset, number, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////

std::vector<short> loadSignWords( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    return loadArray<short>( offset, number, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////

std::vector<long> loadSignDWords( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    return loadArray<long>( offset, number, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////

std::vector<long long> loadSignQWords( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    return loadArray<long long>( offset, number, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////

std::vector<float> loadFloats( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    return loadArray<float>( offset, number, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////

std::vector<double> loadDoubles( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    return loadArray<double>( offset, number, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////

template<typename T>
void writeArray( MEMOFFSET_64 offset, const std::vector<T> &values, bool phyAddr )
{
    if (values.empty())
        return;

    const auto length = values.size() * sizeof(T);

    if (!phyAddr && !isVaRegionValid(offset, length))
        throw MemoryException(offset);

    writeMemory( offset, &values[0], length, phyAddr );
}

///////////////////////////////////////////////////////////////////////////////

void writeBytes(MEMOFFSET_64 offset, const std::vector<unsigned char> &values, bool phyAddr )
{
    writeArray(offset, values, phyAddr);
}

///////////////////////////////////////////////////////////////////////////////

void writeWords( MEMOFFSET_64 offset, const std::vector<unsigned short> &values, bool phyAddr )
{
    writeArray(offset, values, phyAddr);
}

///////////////////////////////////////////////////////////////////////////////

void writeDWords( MEMOFFSET_64 offset, const std::vector<unsigned long> &values, bool phyAddr )
{
    writeArray(offset, values, phyAddr);
}

///////////////////////////////////////////////////////////////////////////////


void writeQWords( MEMOFFSET_64 offset, std::vector<unsigned long long> const &values, bool phyAddr )
{
    writeArray(offset, values, phyAddr);
}

///////////////////////////////////////////////////////////////////////////////


void writeSignBytes( MEMOFFSET_64 offset, const std::vector<char> &values, bool phyAddr )
{
    writeArray(offset, values, phyAddr);
}

///////////////////////////////////////////////////////////////////////////////


void writeSignWords( MEMOFFSET_64 offset, const std::vector<short> &values, bool phyAddr )
{
    writeArray(offset, values, phyAddr);
}

///////////////////////////////////////////////////////////////////////////////


void writeSignDWords( MEMOFFSET_64 offset, const std::vector<long> &values, bool phyAddr )
{
    writeArray(offset, values, phyAddr);
}

///////////////////////////////////////////////////////////////////////////////


void writeSignQWords( MEMOFFSET_64 offset, std::vector<long long> const &values, bool phyAddr )
{
    writeArray(offset, values, phyAddr);
}

///////////////////////////////////////////////////////////////////////////////


void writeFloats( MEMOFFSET_64 offset, const std::vector<float> &values, bool phyAddr )
{
    writeArray(offset, values, phyAddr);
}

///////////////////////////////////////////////////////////////////////////////


void writeDoubles( MEMOFFSET_64 offset, const std::vector<double> &values, bool phyAddr )
{
    writeArray(offset, values, phyAddr);
}

///////////////////////////////////////////////////////////////////////////////

std::string loadChars( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    unsigned long  bufferSize = (unsigned long)(sizeof(std::vector<char>::value_type)*number);

    if (!phyAddr && !isVaRegionValid(offset, bufferSize))
        throw MemoryException(offset);

    std::vector<char>   buffer(number);
    
    if (number)
        readMemory( offset, &buffer[0], bufferSize, phyAddr );

    return std::string( buffer.begin(), buffer.end() );
}

///////////////////////////////////////////////////////////////////////////////

std::wstring loadWChars( MEMOFFSET_64 offset, unsigned long number, bool phyAddr )
{
    unsigned long   bufferSize = (unsigned long)(sizeof(std::vector<wchar_t>::value_type)*number);

    if (!phyAddr && !isVaRegionValid(offset, bufferSize))
        throw MemoryException(offset);

    std::vector<wchar_t>   buffer(number);

    if (number)
        readMemory( offset, &buffer[0], bufferSize, phyAddr );

    return std::wstring( buffer.begin(), buffer.end() );
}

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 ptrPtr( MEMOFFSET_64 offset, size_t psize)
{
    psize = psize == 0 ? ptrSize() : psize;

    if ( psize == 4 )
        return addr64(ptrDWord(offset) );

    if ( psize == 8 )
        return addr64(ptrQWord(offset) );

    throw DbgException("unknown pointer size");
}

///////////////////////////////////////////////////////////////////////////////

std::vector<MEMOFFSET_64> loadPtrs( MEMOFFSET_64 offset, unsigned long number, size_t psize )
{
    offset = addr64( offset );

    psize = psize == 0 ? ptrSize() : psize;

    if (!isVaRegionValid(offset, psize*number))
        throw MemoryException(offset);

    std::vector<MEMOFFSET_64>   ptrs(number);

    for ( unsigned long i = 0; i < number; ++i )
        ptrs[i] =ptrPtr( offset + i*psize, psize );

    return ptrs;
}

///////////////////////////////////////////////////////////////////////////////

std::vector<MEMOFFSET_64> loadPtrList( MEMOFFSET_64 offset, size_t psize )
{
    offset = addr64( offset );

    psize = psize == 0 ? ptrSize() : psize;

    std::vector<MEMOFFSET_64>   ptrs;
    ptrs.reserve(100);

    MEMOFFSET_64     entryAddress = 0;
    
    for( entryAddress = ptrPtr( offset, psize ); entryAddress != offset && entryAddress != 0; entryAddress = ptrPtr( entryAddress, psize ) )
       ptrs.push_back( entryAddress );
    
    return ptrs;
}

///////////////////////////////////////////////////////////////////////////////

void setPtr(MEMOFFSET_64 offset, MEMOFFSET_64 value, size_t psize)
{
    psize = psize == 0 ? ptrSize() : psize;

    if ( psize == 4 )
        return setDWord( offset, value & std::numeric_limits<MEMOFFSET_32>::max() );

    if ( psize == 8 )
        return setQWord( offset, value );

    throw DbgException("unknown pointer size");
}

///////////////////////////////////////////////////////////////////////////////

bool compareMemory( MEMOFFSET_64 addr1, MEMOFFSET_64 addr2, size_t length, bool phyAddr )
{
    bool        result = false;

    addr1 = addr64( addr1 );
    addr2 = addr64( addr2 );

    std::vector<char>   m1(length);
    std::vector<char>   m2(length);

    readMemory( addr1, &m1[0], length, phyAddr );
    readMemory( addr2, &m2[0], length, phyAddr );

    return std::equal( m1.begin(), m1.end(), m2.begin() );
}

///////////////////////////////////////////////////////////////////////////////

} // kdlib namespace end
