#pragma once

#include "procfixture.h"
#include "kdlib/disasm.h"

using namespace kdlib;

class DisasmTest : public ProcessFixture
{
public:

    DisasmTest() : ProcessFixture(L"disasmtest") {}
};

TEST_F(DisasmTest, disasm)
{
    Disasm   dasm;

    for (int i = 0; i < 100; ++i)
    {
        std::wstring  instruction = dasm.instruction();
        EXPECT_TRUE(!instruction.empty());

        std::vector<unsigned char>  opcode = dasm.opcode();
        EXPECT_TRUE(!opcode.empty());

        size_t  length = dasm.length();
        EXPECT_TRUE(length > 0);

        std::wstring  opmnemo = dasm.opmnemo();
        EXPECT_TRUE(!opmnemo.empty());

        dasm.disassemble();
    }
}
