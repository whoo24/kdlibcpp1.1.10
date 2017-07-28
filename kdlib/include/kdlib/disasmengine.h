#pragma once

#include <string>

#include "kdlib/dbgtypedef.h"

namespace kdlib {

void disasmAssemblay( MEMOFFSET_64 offset, const std::wstring &instruction, MEMOFFSET_64 &nextOffset );
void disasmDisassembly( MEMOFFSET_64 offset, std::wstring &instruction, MEMOFFSET_64 &nextOffset, MEMOFFSET_64 &ea );
MEMOFFSET_64 getNearInstruction( MEMOFFSET_64 offset, MEMDISPLACEMENT delta );

} // end kdlib namespace 
