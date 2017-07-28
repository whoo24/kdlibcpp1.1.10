// kdlibtest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gmock/gmock.h"
#include "kdlib/dbgengine.h"
#include "eventhandlertest.h"
#include "processtest.h"
#include "moduletest.h"
#include "memorytest.h"
#include "typeinfotest.h"
#include "typedvartest.h"
#include "varianttest.h"
#include "breakhandler.h"
#include "cputest.h"
#include "stacktest.h"
#include "disasmtest.h"
#include "targettest.h"
#include "winapitest.h"
#include "crttest.h"
#include "clangtest.h"
    
#ifdef _M_AMD64 
#include "regtest_x64.h"
#endif

class Environment : public ::testing::Environment {
public:
  virtual ~Environment() {}
  virtual void SetUp() {
      kdlib::initialize();
  }

  virtual void TearDown()
  {
      kdlib::uninitialize();
  }
};

int _tmain(int argc, _TCHAR* argv[])
{
    ::testing::InitGoogleMock(&argc, argv);

    ::testing::AddGlobalTestEnvironment( new ::Environment );

    return RUN_ALL_TESTS();
}
