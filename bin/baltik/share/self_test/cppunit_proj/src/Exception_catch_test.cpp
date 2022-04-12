#include <cppunit.h>
#ifdef cppunit_
#include <TRUSTArray.h>
#include <EChaine.h>
#include <TriouError.h>
#include <Motcle.h>


class Exception_catch_test: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( Exception_catch_test );
  CPPUNIT_TEST( testMemoryerror );
  CPPUNIT_TEST_SUITE_END();

public:
  void testMemoryerror() { 
      ArrOfDouble toto;
      EChaine erreur(" 20000000000000000000");
      CPPUNIT_ASSERT_THROW_MESSAGE("verification de la levee d'erreur si depassement memoire",(erreur>>toto), TriouError);
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( Exception_catch_test );


#endif
