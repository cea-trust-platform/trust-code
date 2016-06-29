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
// File:        Vect_test.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////
#include <cppunit.h>
#ifdef cppunit_
#include <EcrFicCollecte.h>

#include <EChaine.h>
#include <SChaine.h>
#include <TriouError.h>


#include <Bord.h>
#include <Noms.h>
#include <Vect.h>

Declare_vect(Bord);
Implemente_vect(Bord);


class VectTest: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( VectTest );

  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testLecture );
  CPPUNIT_TEST( testCopy );
  CPPUNIT_TEST(testAcces);
  CPPUNIT_TEST(testSearch);
  CPPUNIT_TEST(testAdd);
  CPPUNIT_TEST(testCurseur);

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp()
  {
    // init2();
  };
  VectTest():CPPUNIT_NS::TestFixture::TestFixture()
  {

    //init2();
  };
  int int1;
  double double1;
  int flag1;

  void tearDown()
  {
    //cerr<<"ICI" <<endl;;
  } ;
  void testConstructor()
  {
    VECT(Bord) titi;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("VECT(), size nulle",0,titi.size());
    VECT(Bord) titi3(3);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("VECT(3), size 3",3,titi3.size());
    VECT(Bord) titibis(titi);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("VECT(vect), size nulle",0,titi.size());
    VECT(Bord) titi3bis(titi3);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("VECT(vect3), size 3",3,titi3bis.size());
  };
  void testCopy()
  {
    VECT(Bord) a;
    {
      VECT(Bord) org(3);
      org[2].nommer("OK");
      a=org;
      CPPUNIT_ASSERT_EQUAL_MESSAGE("operator=",3,a.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("operator= copie interne",Nom("OK"),a[2].le_nom());
    }
    // on verifie apres destruction de org
    CPPUNIT_ASSERT_EQUAL_MESSAGE("operator= copie interne",Nom("OK"),a[2].le_nom());
    // on recommence sur un vect non  nul
    VECT(Bord) aa(6);
    aa[2].nommer("erreur2");
    {
      VECT(Bord) org(3);
      org[2].nommer("OK");
      aa=org;
      CPPUNIT_ASSERT_EQUAL_MESSAGE("operator=",3,aa.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("operator= copie interne",Nom("OK"),aa[2].le_nom());
    }
    // on verifie apres destruction de org
    CPPUNIT_ASSERT_EQUAL_MESSAGE("operator= copie interne",Nom("OK"),aa[2].le_nom());
  }
  void testAcces()
  {
    VECT(Bord) a(3);
    for (int i=0; i<a.size(); i++)
      CPPUNIT_ASSERT_EQUAL_MESSAGE("test acces case",0,a[i].nb_faces());
    const VECT(Bord)& b=a;
    for (int i=0; i<a.size(); i++)
      CPPUNIT_ASSERT_EQUAL_MESSAGE("test acces case",0,b[i].nb_faces());
  }
  void testSearch()
  {
    // maintenant search/contient/rang ne sont dispoque pour Noms et Motcles
    Noms aa(3);
    aa[0]=("ok0");
    aa[1]=("ok1");
    aa[2]=("ok2");

    Nom titi;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("search titi ",-1,aa.search(titi));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("search a[2] ", 2,aa.search(aa[2]));

    // recherche par nom
    CPPUNIT_ASSERT_EQUAL_MESSAGE("contient_ ok12 ",0,aa.contient_("ok12"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("rang ok12 ",-1,aa.rang("ok12"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("contient_ ok2 ",1,aa.contient_("ok2"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("rang ok2 ",2,aa.rang("ok2"));
  }
  void testAdd()
  {
    VECT(Bord) aa;
    Bord b;
    b.nommer("b");
    aa.add(b);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("size apres add ",1,aa.size());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("copie add ",Nom("b"),aa[0].le_nom());
    aa[0].nommer("ok0");
    CPPUNIT_ASSERT_EQUAL_MESSAGE("copie add ",Nom("b"),b.le_nom());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("copie add ",Nom("ok0"),aa[0].le_nom());
    aa.add(b);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("size apres add ",2,aa.size());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("copie add ",Nom("b"),aa[1].le_nom());


    VECT(Bord) bb(aa);
    aa.add(bb);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("size apres add ",4,aa.size());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("copie add ",Nom("b"),aa[3].le_nom());
    SChaine E;
    E<<aa;
  }
  void testLecture()
  {
    {
      VECT(Bord) aa(3);
      aa[0].nommer("ok0");

      aa[1].nommer("ok1");
      aa[2].nommer("ok2");
      SChaine out;
      out<<aa;
      CPPUNIT_ASSERT_EQUAL_MESSAGE("apres printon ",Nom("3 ok0\nvide_0D\n ok1\nvide_0D\n ok2\nvide_0D\n \n"),Nom(out.get_str()));
      {
        VECT(Bord) bb;

        EChaine in(out.get_str());
        EChaine in2(out.get_str());
        in >> bb;
        CPPUNIT_ASSERT_EQUAL_MESSAGE("apres readon ",Nom("ok2"),bb[2].le_nom());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("apres readon ",3,bb.size());
        bb.lit(in2);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("apres lit ",Nom("ok2"),bb[2].le_nom());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("apres lit ",3,bb.size());

      }
      {
        VECT(Bord) bb(16);
        bb[2].nommer("erreur");
        // readon interdit apres dimensionner
        EChaine in(out.get_str());
        bb.lit(in);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("apres lit ",Nom("ok2"),bb[2].le_nom());
        CPPUNIT_ASSERT_EQUAL_MESSAGE("apres lit ",3,bb.size());
      }
    }
  }
  void testCurseur()

  {
#ifdef curseurvect
    VECT(Bord) aa(3);
    aa[0].nommer("ok0");

    aa[1].nommer("ok1");
    aa[2].nommer("ok2");
    {
      int i=0;
      VECT_CURSEUR(Bord) curseur(aa);
      while(curseur)
        {
          Bord& b=curseur.valeur();
          Cerr<<b.le_nom()<<endl;
          CPPUNIT_ASSERT_EQUAL_MESSAGE("verif acces via curseur",aa[i].le_nom(),b.le_nom());
          ++curseur;
          i++;
        }
      CPPUNIT_ASSERT_EQUAL_MESSAGE("curseur bonne taille",3,i);
    }
    {
      const VECT(Bord)& ca= aa
                            CONST_VECT_CURSEUR(Bord) curseur(ca);
      int i=0;
      while(curseur)
        {
          const Bord& b=curseur.valeur();
          Cerr<<b.le_nom()<<endl;
          CPPUNIT_ASSERT_EQUAL_MESSAGE("verif acces via curseur",aa[i].le_nom(),b.le_nom());
          ++curseur;
          i++;
        }
      CPPUNIT_ASSERT_EQUAL_MESSAGE("curseur bonne taille",3,i);
    }
#endif
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION( VectTest );


#endif
