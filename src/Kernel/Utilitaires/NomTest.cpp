/****************************************************************************
* Copyright (c) 2022, CEA
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
#include <cppunit.h>
#ifdef cppunit_
#include <EcrFicCollecte.h>
#include <Nom.h>
#include <EChaine.h>
#include <Motcle.h>

class NomTest: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( NomTest );
  CPPUNIT_TEST( testConstructor );

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() override
  {
  };
  NomTest():CPPUNIT_NS::TestFixture::TestFixture()
  {
  };
  void tearDown() override
  {
    //Cerr<<"ICI" <<finl;;
  } ;
  void testConstructor()
  {
    {
      Nom nom_me("test.es");
      Nom ref("test_0999.es");
      nom_me=nom_me.nom_me(999);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("\"test.es\".me(999)",ref,nom_me);
    }
    {
      Nom nom_me("test");
      Nom ref("test_0999");
      nom_me=nom_me.nom_me(999);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("\"test.es\".me(999)",ref,nom_me);
    }


  };
};

CPPUNIT_TEST_SUITE_REGISTRATION( NomTest );


#endif
