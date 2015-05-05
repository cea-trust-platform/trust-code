#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/ui/text/TestRunner.h>
#include <string.h>

class Error
{
	public:
	  Error(const char* _s) {;};
	  Error() {;};
	    char* msg() { return s;}
	    char *s;

};
int
main( int argc, char* argv[] )
{
	//  on ne veur pas executer la ligne suivante mais tester
	//  si on abien la macro
	CPPUNIT_ASSERT_THROW_MESSAGE("toto",throw Error("normal"), Error);

	
  // Add the top suite to the test runner
   CPPUNIT_NS::TextUi::TestRunner runner;

  runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
   bool wasSucessful = runner.run();
  

  // Print test in a compiler compatible format.
//  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
 // outputter.write(); 
    // Return error code 1 if the one of test failed.
       return wasSucessful ? 0 : 1;

}

