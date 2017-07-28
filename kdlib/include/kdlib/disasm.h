#pragma once

#include <string>
#include <vector>

#include "kdlib\dbgtypedef.h"

namespace kdlib {

/////////////////////////////////////////////////////////////////////////////////

class Disasm  {

public:

    Disasm( MEMOFFSET_64 offset = 0);

    std::wstring  disassemble() {
        m_currentOffset += m_length;
        doDisasm();
        return m_disasm;
    }

    std::wstring  jump(MEMOFFSET_64 offset) {
        m_currentOffset = offset;
        doDisasm();
        return m_disasm;
    }

    std::wstring jumprel(MEMDISPLACEMENT delta) {
        m_currentOffset = getNearInstruction(delta);
        doDisasm();
        return m_disasm;
    }

    MEMOFFSET_64 getNearInstruction( MEMDISPLACEMENT delta );

    std::wstring  reset() {
        m_currentOffset = m_beginOffset;
        doDisasm();
        return m_disasm;
    }

    std::wstring assembly( const std::wstring &instr );

    std::wstring instruction() const {
        return m_disasm;
    }

    std::wstring opmnemo() const; 

    std::vector<unsigned char> opcode() const;

    MEMOFFSET_64 begin() const {
        return m_beginOffset;
    }

    MEMOFFSET_64 current() const {
        return m_currentOffset;
    }

    size_t length() const {
        return m_length;
    }

    MEMOFFSET_64 ea() const {
        return m_ea;
    }

private:

    void doDisasm();

    MEMOFFSET_64    m_beginOffset;
    MEMOFFSET_64    m_currentOffset;
    MEMOFFSET_64    m_ea;
    size_t          m_length;
    std::wstring    m_disasm;
};

/////////////////////////////////////////////////////////////////////////////////

} ; // end pykd namespace

