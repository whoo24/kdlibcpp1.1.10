#include "stdafx.h"
#include "diawrapper.h"

namespace kdlib {

///////////////////////////////////////////////////////////////////////////////

#define _DEF_BASIC_TYPE(x)  DiaSymbol::ValueNameEntry(bt##x, L#x)
const DiaSymbol::ValueNameEntry DiaSymbol::basicTypeName[] = {
    _DEF_BASIC_TYPE(NoType),
    _DEF_BASIC_TYPE(Void),
    _DEF_BASIC_TYPE(Char),
    _DEF_BASIC_TYPE(WChar),
    _DEF_BASIC_TYPE(Int),
    _DEF_BASIC_TYPE(UInt),
    _DEF_BASIC_TYPE(Float),
    _DEF_BASIC_TYPE(BCD),
    _DEF_BASIC_TYPE(Bool),
    _DEF_BASIC_TYPE(Long),
    _DEF_BASIC_TYPE(ULong),
    _DEF_BASIC_TYPE(Currency),
    _DEF_BASIC_TYPE(Date),
    _DEF_BASIC_TYPE(Variant),
    _DEF_BASIC_TYPE(Complex),
    _DEF_BASIC_TYPE(Bit),
    _DEF_BASIC_TYPE(BSTR),
    _DEF_BASIC_TYPE(Hresult)
};
#undef _DEF_BASIC_TYPE

const size_t DiaSymbol::cntBasicTypeName = _countof(DiaSymbol::basicTypeName);

///////////////////////////////////////////////////////////////////////////////

} //kdlib namespace end




















//
//#include "diawrapper.h"
//#include "utils.h"
//
//namespace pyDia {
//
//////////////////////////////////////////////////////////////////////////////////
//
//#define _DEF_SYM_TAG_VAL(x) Symbol::ValueNameEntry(SymTag##x, #x)
//const Symbol::ValueNameEntry Symbol::symTagName[SymTagMax] = {
//    _DEF_SYM_TAG_VAL(Null),
//    _DEF_SYM_TAG_VAL(Exe),
//    _DEF_SYM_TAG_VAL(Compiland),
//    _DEF_SYM_TAG_VAL(CompilandDetails),
//    _DEF_SYM_TAG_VAL(CompilandEnv),
//    _DEF_SYM_TAG_VAL(Function),
//    _DEF_SYM_TAG_VAL(Block),
//    _DEF_SYM_TAG_VAL(Data),
//    _DEF_SYM_TAG_VAL(Annotation),
//    _DEF_SYM_TAG_VAL(Label),
//    _DEF_SYM_TAG_VAL(PublicSymbol),
//    _DEF_SYM_TAG_VAL(UDT),
//    _DEF_SYM_TAG_VAL(Enum),
//    _DEF_SYM_TAG_VAL(FunctionType),
//    _DEF_SYM_TAG_VAL(PointerType),
//    _DEF_SYM_TAG_VAL(ArrayType),
//    _DEF_SYM_TAG_VAL(BaseType),
//    _DEF_SYM_TAG_VAL(Typedef),
//    _DEF_SYM_TAG_VAL(BaseClass),
//    _DEF_SYM_TAG_VAL(Friend),
//    _DEF_SYM_TAG_VAL(FunctionArgType),
//    _DEF_SYM_TAG_VAL(FuncDebugStart),
//    _DEF_SYM_TAG_VAL(FuncDebugEnd),
//    _DEF_SYM_TAG_VAL(UsingNamespace),
//    _DEF_SYM_TAG_VAL(VTableShape),
//    _DEF_SYM_TAG_VAL(VTable),
//    _DEF_SYM_TAG_VAL(Custom),
//    _DEF_SYM_TAG_VAL(Thunk),
//    _DEF_SYM_TAG_VAL(CustomType),
//    _DEF_SYM_TAG_VAL(ManagedType),
//    _DEF_SYM_TAG_VAL(Dimension)
//};
//#undef _DEF_SYM_TAG_VAL
//
//////////////////////////////////////////////////////////////////////////////////
//
//#define _DEF_DATA_KIND_VAL(x) Symbol::ValueNameEntry(DataIs##x, #x)
//const Symbol::ValueNameEntry Symbol::dataKindName[DataIsConstant + 1] = {
//    _DEF_DATA_KIND_VAL(Unknown),
//    _DEF_DATA_KIND_VAL(Local),
//    _DEF_DATA_KIND_VAL(StaticLocal),
//    _DEF_DATA_KIND_VAL(Param),
//    _DEF_DATA_KIND_VAL(ObjectPtr),
//    _DEF_DATA_KIND_VAL(FileStatic),
//    _DEF_DATA_KIND_VAL(Global),
//    _DEF_DATA_KIND_VAL(Member),
//    _DEF_DATA_KIND_VAL(StaticMember),
//    _DEF_DATA_KIND_VAL(Constant)
//};
//#undef _DEF_DATA_KIND_VAL
//
//
//////////////////////////////////////////////////////////////////////////////////
//
//#define _DEF_LOC_TYPE(x)    Symbol::ValueNameEntry(LocIs##x, #x)
//const Symbol::ValueNameEntry Symbol::locTypeName[LocTypeMax] = {
//    _DEF_LOC_TYPE(Null),
//    _DEF_LOC_TYPE(Static),
//    _DEF_LOC_TYPE(TLS),
//    _DEF_LOC_TYPE(RegRel),
//    _DEF_LOC_TYPE(ThisRel),
//    _DEF_LOC_TYPE(Enregistered),
//    _DEF_LOC_TYPE(BitField),
//    _DEF_LOC_TYPE(Slot),
//    _DEF_LOC_TYPE(IlRel),
//    Symbol::ValueNameEntry(LocInMetaData, "InMetaData"),
//    _DEF_LOC_TYPE(Constant)
//};
//#undef _DEF_LOC_TYPE
//
//////////////////////////////////////////////////////////////////////////////////
//
//#define _DEF_BASIC_TYPE(x)  Symbol::ValueNameEntry(bt##x, #x)
//const Symbol::ValueNameEntry Symbol::basicTypeName[] = {
//    _DEF_BASIC_TYPE(NoType),
//    _DEF_BASIC_TYPE(Void),
//    _DEF_BASIC_TYPE(Char),
//    _DEF_BASIC_TYPE(WChar),
//    _DEF_BASIC_TYPE(Int),
//    _DEF_BASIC_TYPE(UInt),
//    _DEF_BASIC_TYPE(Float),
//    _DEF_BASIC_TYPE(BCD),
//    _DEF_BASIC_TYPE(Bool),
//    _DEF_BASIC_TYPE(Long),
//    _DEF_BASIC_TYPE(ULong),
//    _DEF_BASIC_TYPE(Currency),
//    _DEF_BASIC_TYPE(Date),
//    _DEF_BASIC_TYPE(Variant),
//    _DEF_BASIC_TYPE(Complex),
//    _DEF_BASIC_TYPE(Bit),
//    _DEF_BASIC_TYPE(BSTR),
//    _DEF_BASIC_TYPE(Hresult)
//};
//#undef _DEF_BASIC_TYPE
//
//const size_t Symbol::cntBasicTypeName = _countof(Symbol::basicTypeName);
//
//std::string Symbol::getBasicTypeName( ULONG basicType )
//{
//    for ( size_t i = 0; i < Symbol::cntBasicTypeName; ++i )
//    {
//        if ( basicType == Symbol::basicTypeName[i].first )
//            return std::string( Symbol::basicTypeName[i].second );
//    }
//
//    std::stringstream   sstr;
//
//    sstr << "faild to find basic type with index %d" << basicType;
//
//    throw Exception( sstr.str() );
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//#define _DEF_UDT_KIND(x)    Symbol::ValueNameEntry(Udt##x, #x)
//const Symbol::ValueNameEntry Symbol::udtKindName[] = {
//    _DEF_UDT_KIND(Struct),
//    _DEF_UDT_KIND(Class),
//    _DEF_UDT_KIND(Union)
//};
//#undef  _DEF_UDT_KIND
//
//const size_t Symbol::cntUdtKindName = _countof(udtKindName);
//
//////////////////////////////////////////////////////////////////////////////////
//
//#define _DEF_I386_REG(x)    Symbol::ValueNameEntry(CV_REG_##x, #x)
//const Symbol::ValueNameEntry Symbol::i386RegName[] = {
//    _DEF_I386_REG(AL),
//    _DEF_I386_REG(CL),
//    _DEF_I386_REG(DL),
//    _DEF_I386_REG(BL),
//    _DEF_I386_REG(AH),
//    _DEF_I386_REG(CH),
//    _DEF_I386_REG(DH),
//    _DEF_I386_REG(BH),
//    _DEF_I386_REG(AX),
//    _DEF_I386_REG(CX),
//    _DEF_I386_REG(DX),
//    _DEF_I386_REG(BX),
//    _DEF_I386_REG(SP),
//    _DEF_I386_REG(BP),
//    _DEF_I386_REG(SI),
//    _DEF_I386_REG(DI),
//    _DEF_I386_REG(EAX),
//    _DEF_I386_REG(ECX),
//    _DEF_I386_REG(EDX),
//    _DEF_I386_REG(EBX),
//    _DEF_I386_REG(ESP),
//    _DEF_I386_REG(EBP),
//    _DEF_I386_REG(ESI),
//    _DEF_I386_REG(EDI),
//    _DEF_I386_REG(ES),
//    _DEF_I386_REG(CS),
//    _DEF_I386_REG(SS),
//    _DEF_I386_REG(DS),
//    _DEF_I386_REG(FS),
//    _DEF_I386_REG(GS),
//    _DEF_I386_REG(IP),
//    _DEF_I386_REG(FLAGS ),
//    _DEF_I386_REG(EIP),
//    _DEF_I386_REG(EFLAGS),
//    _DEF_I386_REG(TEMP),
//    _DEF_I386_REG(TEMPH),
//    _DEF_I386_REG(QUOTE),
//    _DEF_I386_REG(PCDR3),
//    _DEF_I386_REG(PCDR4),
//    _DEF_I386_REG(PCDR5),
//    _DEF_I386_REG(PCDR6),
//    _DEF_I386_REG(PCDR7),
//    _DEF_I386_REG(CR0),
//    _DEF_I386_REG(CR1),
//    _DEF_I386_REG(CR2),
//    _DEF_I386_REG(CR3),
//    _DEF_I386_REG(CR4),
//    _DEF_I386_REG(DR0),
//    _DEF_I386_REG(DR1),
//    _DEF_I386_REG(DR2),
//    _DEF_I386_REG(DR3),
//    _DEF_I386_REG(DR4),
//    _DEF_I386_REG(DR5),
//    _DEF_I386_REG(DR6),
//    _DEF_I386_REG(DR7),
//    _DEF_I386_REG(GDTR),
//    _DEF_I386_REG(GDTL),
//    _DEF_I386_REG(IDTR),
//    _DEF_I386_REG(IDTL),
//    _DEF_I386_REG(LDTR),
//    _DEF_I386_REG(TR)
//};
//#undef  _DEF_I386_REG
//
//const size_t Symbol::cntI386RegName = _countof(i386RegName);
//
//////////////////////////////////////////////////////////////////////////////////
//
//#define _DEF_AMD64_REG(x)    Symbol::ValueNameEntry(CV_AMD64_##x, #x)
//const Symbol::ValueNameEntry Symbol::amd64RegName[] = {
//    _DEF_AMD64_REG(AL),
//    _DEF_AMD64_REG(CL),
//    _DEF_AMD64_REG(DL),
//    _DEF_AMD64_REG(BL),
//    _DEF_AMD64_REG(AH),
//    _DEF_AMD64_REG(CH),
//    _DEF_AMD64_REG(DH),
//    _DEF_AMD64_REG(BH),
//    _DEF_AMD64_REG(AX),
//    _DEF_AMD64_REG(CX),
//    _DEF_AMD64_REG(DX),
//    _DEF_AMD64_REG(BX),
//    _DEF_AMD64_REG(SP),
//    _DEF_AMD64_REG(BP),
//    _DEF_AMD64_REG(SI),
//    _DEF_AMD64_REG(DI),
//    _DEF_AMD64_REG(EAX),
//    _DEF_AMD64_REG(ECX),
//    _DEF_AMD64_REG(EDX),
//    _DEF_AMD64_REG(EBX),
//    _DEF_AMD64_REG(ESP),
//    _DEF_AMD64_REG(EBP),
//    _DEF_AMD64_REG(ESI),
//    _DEF_AMD64_REG(EDI),
//    _DEF_AMD64_REG(ES),
//    _DEF_AMD64_REG(CS),
//    _DEF_AMD64_REG(SS),
//    _DEF_AMD64_REG(DS),
//    _DEF_AMD64_REG(FS),
//    _DEF_AMD64_REG(GS),
//    _DEF_AMD64_REG(FLAGS),
//    _DEF_AMD64_REG(RIP),
//    _DEF_AMD64_REG(EFLAGS),
//    _DEF_AMD64_REG(CR0),
//    _DEF_AMD64_REG(CR1),
//    _DEF_AMD64_REG(CR2),
//    _DEF_AMD64_REG(CR3),
//    _DEF_AMD64_REG(CR4),
//    _DEF_AMD64_REG(CR8),
//    _DEF_AMD64_REG(DR0),
//    _DEF_AMD64_REG(DR1),
//    _DEF_AMD64_REG(DR2),
//    _DEF_AMD64_REG(DR3),
//    _DEF_AMD64_REG(DR4),
//    _DEF_AMD64_REG(DR5),
//    _DEF_AMD64_REG(DR6),
//    _DEF_AMD64_REG(DR7),
//    _DEF_AMD64_REG(DR8),
//    _DEF_AMD64_REG(DR9),
//    _DEF_AMD64_REG(DR10),
//    _DEF_AMD64_REG(DR11),
//    _DEF_AMD64_REG(DR12),
//    _DEF_AMD64_REG(DR13),
//    _DEF_AMD64_REG(DR14),
//    _DEF_AMD64_REG(DR15),
//    _DEF_AMD64_REG(GDTR),
//    _DEF_AMD64_REG(GDTL),
//    _DEF_AMD64_REG(IDTR),
//    _DEF_AMD64_REG(IDTL),
//    _DEF_AMD64_REG(LDTR),
//    _DEF_AMD64_REG(TR),
//    _DEF_AMD64_REG(ST0),
//    _DEF_AMD64_REG(ST1),
//    _DEF_AMD64_REG(ST2),
//    _DEF_AMD64_REG(ST3),
//    _DEF_AMD64_REG(ST4),
//    _DEF_AMD64_REG(ST5),
//    _DEF_AMD64_REG(ST6),
//    _DEF_AMD64_REG(ST7),
//    _DEF_AMD64_REG(CTRL),
//    _DEF_AMD64_REG(STAT),
//    _DEF_AMD64_REG(TAG),
//    _DEF_AMD64_REG(FPIP),
//    _DEF_AMD64_REG(FPCS),
//    _DEF_AMD64_REG(FPDO),
//    _DEF_AMD64_REG(FPDS),
//    _DEF_AMD64_REG(ISEM),
//    _DEF_AMD64_REG(FPEIP),
//    _DEF_AMD64_REG(FPEDO),
//    _DEF_AMD64_REG(MM0),
//    _DEF_AMD64_REG(MM1),
//    _DEF_AMD64_REG(MM2),
//    _DEF_AMD64_REG(MM3),
//    _DEF_AMD64_REG(MM4),
//    _DEF_AMD64_REG(MM5),
//    _DEF_AMD64_REG(MM6),
//    _DEF_AMD64_REG(MM7),
//    _DEF_AMD64_REG(XMM0),
//    _DEF_AMD64_REG(XMM1),
//    _DEF_AMD64_REG(XMM2),
//    _DEF_AMD64_REG(XMM3),
//    _DEF_AMD64_REG(XMM4),
//    _DEF_AMD64_REG(XMM5),
//    _DEF_AMD64_REG(XMM6),
//    _DEF_AMD64_REG(XMM7),
//    _DEF_AMD64_REG(XMM0_0),
//    _DEF_AMD64_REG(XMM0_1),
//    _DEF_AMD64_REG(XMM0_2),
//    _DEF_AMD64_REG(XMM0_3),
//    _DEF_AMD64_REG(XMM1_0),
//    _DEF_AMD64_REG(XMM1_1),
//    _DEF_AMD64_REG(XMM1_2),
//    _DEF_AMD64_REG(XMM1_3),
//    _DEF_AMD64_REG(XMM2_0),
//    _DEF_AMD64_REG(XMM2_1),
//    _DEF_AMD64_REG(XMM2_2),
//    _DEF_AMD64_REG(XMM2_3),
//    _DEF_AMD64_REG(XMM3_0),
//    _DEF_AMD64_REG(XMM3_1),
//    _DEF_AMD64_REG(XMM3_2),
//    _DEF_AMD64_REG(XMM3_3),
//    _DEF_AMD64_REG(XMM4_0),
//    _DEF_AMD64_REG(XMM4_1),
//    _DEF_AMD64_REG(XMM4_2),
//    _DEF_AMD64_REG(XMM4_3),
//    _DEF_AMD64_REG(XMM5_0),
//    _DEF_AMD64_REG(XMM5_1),
//    _DEF_AMD64_REG(XMM5_2),
//    _DEF_AMD64_REG(XMM5_3),
//    _DEF_AMD64_REG(XMM6_0),
//    _DEF_AMD64_REG(XMM6_1),
//    _DEF_AMD64_REG(XMM6_2),
//    _DEF_AMD64_REG(XMM6_3),
//    _DEF_AMD64_REG(XMM7_0),
//    _DEF_AMD64_REG(XMM7_1),
//    _DEF_AMD64_REG(XMM7_2),
//    _DEF_AMD64_REG(XMM7_3),
//    _DEF_AMD64_REG(XMM0L),
//    _DEF_AMD64_REG(XMM1L),
//    _DEF_AMD64_REG(XMM2L),
//    _DEF_AMD64_REG(XMM3L),
//    _DEF_AMD64_REG(XMM4L),
//    _DEF_AMD64_REG(XMM5L),
//    _DEF_AMD64_REG(XMM6L),
//    _DEF_AMD64_REG(XMM7L),
//    _DEF_AMD64_REG(XMM0H),
//    _DEF_AMD64_REG(XMM1H),
//    _DEF_AMD64_REG(XMM2H),
//    _DEF_AMD64_REG(XMM3H),
//    _DEF_AMD64_REG(XMM4H),
//    _DEF_AMD64_REG(XMM5H),
//    _DEF_AMD64_REG(XMM6H),
//    _DEF_AMD64_REG(XMM7H),
//    _DEF_AMD64_REG(MXCSR),
//    _DEF_AMD64_REG(EMM0L),
//    _DEF_AMD64_REG(EMM1L),
//    _DEF_AMD64_REG(EMM2L),
//    _DEF_AMD64_REG(EMM3L),
//    _DEF_AMD64_REG(EMM4L),
//    _DEF_AMD64_REG(EMM5L),
//    _DEF_AMD64_REG(EMM6L),
//    _DEF_AMD64_REG(EMM7L),
//    _DEF_AMD64_REG(EMM0H),
//    _DEF_AMD64_REG(EMM1H),
//    _DEF_AMD64_REG(EMM2H),
//    _DEF_AMD64_REG(EMM3H),
//    _DEF_AMD64_REG(EMM4H),
//    _DEF_AMD64_REG(EMM5H),
//    _DEF_AMD64_REG(EMM6H),
//    _DEF_AMD64_REG(EMM7H),
//    _DEF_AMD64_REG(MM00),
//    _DEF_AMD64_REG(MM01),
//    _DEF_AMD64_REG(MM10),
//    _DEF_AMD64_REG(MM11),
//    _DEF_AMD64_REG(MM20),
//    _DEF_AMD64_REG(MM21),
//    _DEF_AMD64_REG(MM30),
//    _DEF_AMD64_REG(MM31),
//    _DEF_AMD64_REG(MM40),
//    _DEF_AMD64_REG(MM41),
//    _DEF_AMD64_REG(MM50),
//    _DEF_AMD64_REG(MM51),
//    _DEF_AMD64_REG(MM60),
//    _DEF_AMD64_REG(MM61),
//    _DEF_AMD64_REG(MM70),
//    _DEF_AMD64_REG(MM71),
//    _DEF_AMD64_REG(XMM8),
//    _DEF_AMD64_REG(XMM9),
//    _DEF_AMD64_REG(XMM10),
//    _DEF_AMD64_REG(XMM11),
//    _DEF_AMD64_REG(XMM12),
//    _DEF_AMD64_REG(XMM13),
//    _DEF_AMD64_REG(XMM14),
//    _DEF_AMD64_REG(XMM15),
//    _DEF_AMD64_REG(XMM8_0),
//    _DEF_AMD64_REG(XMM8_1),
//    _DEF_AMD64_REG(XMM8_2),
//    _DEF_AMD64_REG(XMM8_3),
//    _DEF_AMD64_REG(XMM9_0),
//    _DEF_AMD64_REG(XMM9_1),
//    _DEF_AMD64_REG(XMM9_2),
//    _DEF_AMD64_REG(XMM9_3),
//    _DEF_AMD64_REG(XMM10_0),
//    _DEF_AMD64_REG(XMM10_1),
//    _DEF_AMD64_REG(XMM10_2),
//    _DEF_AMD64_REG(XMM10_3),
//    _DEF_AMD64_REG(XMM11_0),
//    _DEF_AMD64_REG(XMM11_1),
//    _DEF_AMD64_REG(XMM11_2),
//    _DEF_AMD64_REG(XMM11_3),
//    _DEF_AMD64_REG(XMM12_0),
//    _DEF_AMD64_REG(XMM12_1),
//    _DEF_AMD64_REG(XMM12_2),
//    _DEF_AMD64_REG(XMM12_3),
//    _DEF_AMD64_REG(XMM13_0),
//    _DEF_AMD64_REG(XMM13_1),
//    _DEF_AMD64_REG(XMM13_2),
//    _DEF_AMD64_REG(XMM13_3),
//    _DEF_AMD64_REG(XMM14_0),
//    _DEF_AMD64_REG(XMM14_1),
//    _DEF_AMD64_REG(XMM14_2),
//    _DEF_AMD64_REG(XMM14_3),
//    _DEF_AMD64_REG(XMM15_0),
//    _DEF_AMD64_REG(XMM15_1),
//    _DEF_AMD64_REG(XMM15_2),
//    _DEF_AMD64_REG(XMM15_3),
//    _DEF_AMD64_REG(XMM8L),
//    _DEF_AMD64_REG(XMM9L),
//    _DEF_AMD64_REG(XMM10L),
//    _DEF_AMD64_REG(XMM11L),
//    _DEF_AMD64_REG(XMM12L),
//    _DEF_AMD64_REG(XMM13L),
//    _DEF_AMD64_REG(XMM14L),
//    _DEF_AMD64_REG(XMM15L),
//    _DEF_AMD64_REG(XMM8H),
//    _DEF_AMD64_REG(XMM9H),
//    _DEF_AMD64_REG(XMM10H),
//    _DEF_AMD64_REG(XMM11H),
//    _DEF_AMD64_REG(XMM12H),
//    _DEF_AMD64_REG(XMM13H),
//    _DEF_AMD64_REG(XMM14H),
//    _DEF_AMD64_REG(XMM15H),
//    _DEF_AMD64_REG(EMM8L),
//    _DEF_AMD64_REG(EMM9L),
//    _DEF_AMD64_REG(EMM10L),
//    _DEF_AMD64_REG(EMM11L),
//    _DEF_AMD64_REG(EMM12L),
//    _DEF_AMD64_REG(EMM13L),
//    _DEF_AMD64_REG(EMM14L),
//    _DEF_AMD64_REG(EMM15L),
//    _DEF_AMD64_REG(EMM8H),
//    _DEF_AMD64_REG(EMM9H),
//    _DEF_AMD64_REG(EMM10H),
//    _DEF_AMD64_REG(EMM11H),
//    _DEF_AMD64_REG(EMM12H),
//    _DEF_AMD64_REG(EMM13H),
//    _DEF_AMD64_REG(EMM14H),
//    _DEF_AMD64_REG(EMM15H),
//    _DEF_AMD64_REG(SIL),
//    _DEF_AMD64_REG(DIL),
//    _DEF_AMD64_REG(BPL),
//    _DEF_AMD64_REG(SPL),
//    _DEF_AMD64_REG(RAX),
//    _DEF_AMD64_REG(RBX),
//    _DEF_AMD64_REG(RCX),
//    _DEF_AMD64_REG(RDX),
//    _DEF_AMD64_REG(RSI),
//    _DEF_AMD64_REG(RDI),
//    _DEF_AMD64_REG(RBP),
//    _DEF_AMD64_REG(RSP),
//    _DEF_AMD64_REG(R8),
//    _DEF_AMD64_REG(R9),
//    _DEF_AMD64_REG(R10),
//    _DEF_AMD64_REG(R11),
//    _DEF_AMD64_REG(R12),
//    _DEF_AMD64_REG(R13),
//    _DEF_AMD64_REG(R14),
//    _DEF_AMD64_REG(R15),
//    _DEF_AMD64_REG(R8B),
//    _DEF_AMD64_REG(R9B),
//    _DEF_AMD64_REG(R10B),
//    _DEF_AMD64_REG(R11B),
//    _DEF_AMD64_REG(R12B),
//    _DEF_AMD64_REG(R13B),
//    _DEF_AMD64_REG(R14B),
//    _DEF_AMD64_REG(R15B),
//    _DEF_AMD64_REG(R8W),
//    _DEF_AMD64_REG(R9W),
//    _DEF_AMD64_REG(R10W),
//    _DEF_AMD64_REG(R11W),
//    _DEF_AMD64_REG(R12W),
//    _DEF_AMD64_REG(R13W),
//    _DEF_AMD64_REG(R14W),
//    _DEF_AMD64_REG(R15W),
//    _DEF_AMD64_REG(R8D),
//    _DEF_AMD64_REG(R9D),
//    _DEF_AMD64_REG(R10D),
//    _DEF_AMD64_REG(R11D),
//    _DEF_AMD64_REG(R12D),
//    _DEF_AMD64_REG(R13D),
//    _DEF_AMD64_REG(R14D),
//    _DEF_AMD64_REG(R15D)
//};
//#undef  _DEF_AMD64_REG
//
//const size_t Symbol::cntAmd64RegName= _countof(amd64RegName);
//
//////////////////////////////////////////////////////////////////////////////////
//
//}
