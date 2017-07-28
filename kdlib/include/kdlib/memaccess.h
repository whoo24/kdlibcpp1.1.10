#pragma once

#include <string>
#include <vector>

#include "kdlib/dbgtypedef.h"
#include "kdlib/dbgengine.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

MEMOFFSET_64 addr64( MEMOFFSET_64 offset );
void readMemory( MEMOFFSET_64 offset, void* buffer, size_t length, bool phyAddr = false, unsigned long *readed = 0 );
bool readMemoryUnsafe( MEMOFFSET_64 offset, void* buffer, size_t length, bool phyAddr = false, unsigned long *readed = 0 );
bool isVaValid( MEMOFFSET_64 addr );
bool isVaRegionValid(MEMOFFSET_64 addr, size_t length);
bool compareMemory( MEMOFFSET_64 addr1, MEMOFFSET_64 addr2, size_t length, bool phyAddr = false );
MEMOFFSET_64 searchMemory( MEMOFFSET_64 beginOffset, unsigned long length, const std::vector<char>& pattern );
MEMOFFSET_64 findMemoryRegion( MEMOFFSET_64 beginOffset, MEMOFFSET_64& regionOffset, unsigned long long &regionLength );
kdlib::MemoryProtect getVaProtect( kdlib::MEMOFFSET_64 offset );

void writeMemory( MEMOFFSET_64 offset, const void* buffer, size_t length, bool phyAddr = false, unsigned long *written = 0 );

///////////////////////////////////////////////////////////////////////////////

unsigned char ptrByte( MEMOFFSET_64 offset );
unsigned short ptrWord( MEMOFFSET_64 offset );
unsigned long  ptrDWord( MEMOFFSET_64 offset );
unsigned long long  ptrQWord( MEMOFFSET_64 offset );
unsigned long long ptrMWord( MEMOFFSET_64 offset );
char ptrSignByte( MEMOFFSET_64 offset );
short ptrSignWord( MEMOFFSET_64 offset );
long ptrSignDWord( MEMOFFSET_64 offset );
long long ptrSignQWord( MEMOFFSET_64 offset );
long long ptrSignMWord( MEMOFFSET_64 offset );
float ptrSingleFloat( MEMOFFSET_64 offset );
double ptrDoubleFloat( MEMOFFSET_64 offset );

void setByte( MEMOFFSET_64 offset, unsigned char value );
void setWord( MEMOFFSET_64 offset, unsigned short value );
void setDWord( MEMOFFSET_64 offset, unsigned long value );
void setQWord( MEMOFFSET_64 offset, unsigned long long value );
void setSignByte( MEMOFFSET_64 offset, char value );
void setSignWord( MEMOFFSET_64 offset, short value );
void setSignDWord( MEMOFFSET_64 offset, long value );
void setSignQWord( MEMOFFSET_64 offset, long long value );
void setSingleFloat( MEMOFFSET_64 offset, float value );
void setDoubleFloat( MEMOFFSET_64 offset, double value );

std::vector<unsigned char> loadBytes( MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false );
std::vector<unsigned short> loadWords( MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false );
std::vector<unsigned long> loadDWords( MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false );
std::vector<unsigned long long> loadQWords( MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false );
std::vector<char> loadSignBytes( MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false );
std::vector<short> loadSignWords( MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false );
std::vector<long> loadSignDWords( MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false );
std::vector<long long> loadSignQWords( MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false );
std::vector<float> loadFloats( MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false );
std::vector<double> loadDoubles( MEMOFFSET_64 offset, unsigned long count, bool phyAddr = false );

void writeBytes( MEMOFFSET_64 offset, const std::vector<unsigned char> &values, bool phyAddr = false );
void writeWords( MEMOFFSET_64 offset, const std::vector<unsigned short> &values, bool phyAddr = false );
void writeDWords( MEMOFFSET_64 offset, const std::vector<unsigned long> &values, bool phyAddr = false );
void writeQWords( MEMOFFSET_64 offset, std::vector<unsigned long long> const &values, bool phyAddr = false );
void writeSignBytes( MEMOFFSET_64 offset, const std::vector<char> &values, bool phyAddr = false );
void writeSignWords( MEMOFFSET_64 offset, const std::vector<short> &values, bool phyAddr = false );
void writeSignDWords( MEMOFFSET_64 offset, const std::vector<long> &values, bool phyAddr = false );
void writeSignQWords( MEMOFFSET_64 offset, std::vector<long long> const &values, bool phyAddr = false );
void writeFloats( MEMOFFSET_64 offset, const std::vector<float> &values, bool phyAddr = false );
void writeDoubles( MEMOFFSET_64 offset, const std::vector<double> &values, bool phyAddr = false );

std::string loadChars( MEMOFFSET_64 offset, unsigned long number, bool phyAddr = false );
std::wstring loadWChars( MEMOFFSET_64 offset, unsigned long  number, bool phyAddr = false );
std::string loadCStr( MEMOFFSET_64 offset );
std::wstring loadWStr( MEMOFFSET_64 offset );

void writeCStr( MEMOFFSET_64 offset, const std::string& str);
void writeWStr( MEMOFFSET_64 offset, const std::wstring& str);

MEMOFFSET_64 ptrPtr( MEMOFFSET_64 offset, size_t psize = 0 );
std::vector<MEMOFFSET_64> loadPtrs( MEMOFFSET_64 offset, unsigned long count, size_t psize = 0 );
std::vector<MEMOFFSET_64> loadPtrList( MEMOFFSET_64 offset, size_t psize = 0 );

void setPtr( MEMOFFSET_64 offset, MEMOFFSET_64 value, size_t psize = 0 );

///////////////////////////////////////////////////////////////////////////////

} // kdlib namespace end
