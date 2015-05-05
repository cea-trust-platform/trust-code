//////////////////////////////////////////////////////////////////////////////
//
// File:        Testeur.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Testeur.h>

Implemente_instanciable(Testeur,"Testeur",Interprete);


// printOn et readOn

Sortie& Testeur::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Testeur::readOn(Entree& is )
{
  //
  // VERIFIER ICI QU'ON A BIEN TOUT LU;
  //
  return is;
}
#include <cppunit.h>

#ifdef cppunit_
#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#endif
Entree& Testeur::interpreter(Entree& is)
{
  int save_mode=Process::exception_sur_exit;
  Process::exception_sur_exit=1;
  // on test cppunit
#ifdef cppunit_
  CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();

  // Adds the test to the list of test to run
  CPPUNIT_NS::TextUi::TestRunner runner;
  runner.addTest( suite );

  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  //  CPPUNIT_NS::TextTestProgressListener progress;
  controller.addListener( &progress );

  // Change the default outputter to a compiler error format outputter
  //  runner.setOutputter( new CPPUNIT_NS::CompilerOutputter( &runner.result(),    CPPUNIT_NS::stdCOut() ) );
  runner.setOutputter( new CPPUNIT_NS::CompilerOutputter( &result,    CPPUNIT_NS::stdCOut() ) );
  // Run the test.
  //bool wasSucessful = runner.run();
  runner.run(controller);
  bool wasSucessful = result.wasSuccessful() ;

  CPPUNIT_NS::stdCOut()<<" \n \n \n \n";

  CPPUNIT_NS::CompilerOutputter outputter( &result,    CPPUNIT_NS::stdCOut() ) ;
  outputter.write();


  if (!wasSucessful)
    //if (1) Cerr<<" dommage "<<finl; else
    exit();
#else
  Cerr<<"cppunit pas dispo"<<finl;
  exit();
#endif
  //

  Process::exception_sur_exit=save_mode;

  return is;
}
