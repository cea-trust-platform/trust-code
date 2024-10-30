/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        Testeur_cppunit.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Testeur_cppunit.h>
#include <cppunit.h>
#ifdef cppunit_
#include <cppunit/CompilerOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#endif

Implemente_instanciable( Testeur_cppunit, "Testeur_cppunit", Interprete ) ;

Sortie& Testeur_cppunit::printOn( Sortie& os ) const
{
  Interprete::printOn( os );
  return os;
}

Entree& Testeur_cppunit::readOn( Entree& is )
{
  Interprete::readOn( is );
  return is;
}
Entree& Testeur_cppunit::interpreter(Entree& is)
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
  Cerr<<"version compiled without cppunit"<<finl;
  exit();
#endif
  //

  Process::exception_sur_exit=save_mode;

  return is;
}
