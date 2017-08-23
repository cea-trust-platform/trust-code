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
// File:        Param_lecture_test.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////
#include <cppunit.h>
#ifdef cppunit_
#include <EcrFicCollecte.h>
#include <Param.h>
#include <EChaine.h>
#include <TriouError.h>
#include <Motcle.h>
#include <ArrOfDouble.h>
#include <Deriv_Comm_Group.h>
#include <Comm_Group.h>

class Nom_test: public Nom
{
public:
  inline int lire_motcle_non_standard_public(const Motcle& mot, Entree& is)
  {
    return lire_motcle_non_standard(mot,is);
  };
private:
  int lire_motcle_non_standard(const Motcle& mot , Entree& is);
};

int Nom_test::lire_motcle_non_standard(const Motcle& motcle , Entree& is)
{
  if (motcle==Motcle("objet_non_std"))
    {
      Motcle motcle2;
      is >> motcle2;
      if (motcle2==Motcle("mot_test_ok"))
        {

          is>>(*this);
          return 0;
        }
    }
  return -1;
}

class ParamTest: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( ParamTest );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testLecture );
  CPPUNIT_TEST( testLecture_objets );
  CPPUNIT_TEST(testConditions);
  CPPUNIT_TEST(testDictionnaire);
  CPPUNIT_TEST(testdoublelecture);
  CPPUNIT_TEST(testajouterderiv);
  CPPUNIT_TEST(test_ajouter_param);

  CPPUNIT_TEST(test_lecture_sans_accolade);
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp()
  {
    // init2();
  };
  ParamTest():CPPUNIT_NS::TestFixture::TestFixture()
  {
    Cerr<<"builder "<<finl;
    //init2();
  };
  int int1;
  double double1;
  int flag1;
  void init2(Param& param)
  {
    int1=-1;
    double1=-1;
    flag1=-1;
    param.ajouter("int",&int1,Param::REQUIRED);
    param.ajouter("double",&double1);
    param.ajouter_flag("flag",&flag1);

  };
  void tearDown()
  {
    //Cerr<<"ICI" <<finl;;
  } ;
  void testConstructor()
  {
    Param param("test_param");
    init2(param);
    CPPUNIT_ASSERT_EQUAL(0,param.get_list_mots_lus().size());
    //CPPUNIT_ASSERT_EQUAL(,param.verifier_avant_ajout("INT"));

    // CPPUNIT_ASSERT_EQUAL(1,param.verifier_avant_ajout("INT2"));
  };
  void test_lecture_sans_accolade()
  {
    Param param("test_param");
    param.ajouter("int",&int1,Param::REQUIRED);
    {
      EChaine is(" 5 ");
      param.lire_sans_accolade(is);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des int",5,int1);
    }
    {
      EChaine is(" { 5  } ");

      CPPUNIT_ASSERT_THROW_MESSAGE("ne doit pas lire 5 dans lire_sans_accolade",param.lire_sans_accolade(is), TriouError);
    }

    param.ajouter("double",&double1);
    {
      EChaine is(" 5 ");
      CPPUNIT_ASSERT_THROW_MESSAGE("Optionne pas supporte dans lire_sans_accolade",param.lire_sans_accolade(is), TriouError);
    }

#if 0
    // on ne veut pas les flags pour l'instant ... trop dangeureux
    param.supprimer("double");
    int test_fl;
    param.ajouter_flag("OPT",&test_fl);
    {
      EChaine is(" 5 ");
      param.lire_sans_accolade(is);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des flag",0,test_fl);
    }
    {
      EChaine is(" OPT 5 ");
      param.lire_sans_accolade(is);

      CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des flag",1,test_fl);
    }
    {
      EChaine is(" OPT2 5   ");

      CPPUNIT_ASSERT_THROW_MESSAGE("ne doit pas lire 5 dans lire_sans_accolade",param.lire_sans_accolade(is), TriouError);
    }


#endif
  };

  void testLecture()
  {
    Param param("test_param");
    init2(param);
    {
      // erreur int non lu mais double lu
      EChaine is("{ double  5 }");
      CPPUNIT_ASSERT_THROW_MESSAGE("verification de la lecture des objets non optionnels",param.lire_avec_accolades_depuis(is), TriouError);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des doubles",5.,double1);
    }
    {
      EChaine is("{ int 5 }");
      param.lire_avec_accolades_depuis(is);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("pb nb mots lus",param.get_list_mots_lus().size(),2);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("pb avec la lecture des int ",5,int1);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("pb lecture flag ",0,flag1);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des ints pour les doubles",5.,double1);
    }
    {
      // erreur mot non compris
      EChaine is("{  int2 5 }");
      CPPUNIT_ASSERT_THROW_MESSAGE("mot incompris",param.lire_avec_accolades_depuis(is), TriouError);
    }
    {
      // erreur manque une accolade ouvrante
      EChaine is(" int 5 }");
      CPPUNIT_ASSERT_THROW_MESSAGE("accolade ouvrante",param.lire_avec_accolades_depuis(is), TriouError);
    }
    {
      // erreur manque une paranthese non compris
      EChaine is(" { int 5 ");
      CPPUNIT_ASSERT_THROW_MESSAGE("accolade fermante",param.lire_avec_accolades_depuis(is), TriouError);
    }
    {
      // test flag
      EChaine is("{  flag }");
      param.lire_avec_accolades_depuis(is);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("pb lecture flag ",1,flag1);
    }
    {
      ArrOfDouble arrdouble(2);
      param.ajouter_arr_size_predefinie("arrofdouble",&arrdouble);
      EChaine is ("{ arrofdouble 2 3 }");
      param.lire_avec_accolades_depuis(is);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des ArrofDoubles",3.,arrdouble[1]);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des ArrofDoubles",2.,arrdouble[0]);
    }
    {
      ArrOfInt arrdouble(2);
      param.ajouter_arr_size_predefinie("arrofint",&arrdouble);
      EChaine is ("{ arrofint 2 3 }");
      param.lire_avec_accolades_depuis(is);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des ArrofInt",3,arrdouble[1]);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des ArrofInt",2,arrdouble[0]);
    }
  };

  void testdoublelecture()
  {

    // on verifie que si on remet le meme mot les specs sont ignores
    {
      Param param("test_param");
      double val;
      int i;

      param.ajouter("double|float",&i);
      CPPUNIT_ASSERT_THROW_MESSAGE("normalement cela ne focntionne pas ",param.ajouter("double",&val),TriouError);
      // maintenant on retire avant
      param.supprimer("float");
      param.ajouter("double",&val);


      EChaine is("{ double  5.1 }");
      param.lire_avec_accolades_depuis(is);
      CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des doubles",5.1,val);
    }
    {
      // le contraire
      Param param("test_param");
      double val;
      int i;
      param.ajouter("int",&val);
      CPPUNIT_ASSERT_THROW_MESSAGE("normalement cela ne focntionne pas ",param.ajouter("int",&val),TriouError);
      // maintenant on retire avant
      param.supprimer("int");
      param.ajouter("int|entier",&i);
      param.ajouter_condition("is_read_int","int must be read, via int or entier");


      {
        EChaine is("{ int  5 }");
        param.lire_avec_accolades_depuis(is);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(" pb avec la lecture des doubles",5,i);
      }
      {
        EChaine is("{ int  5.1 }");
        CPPUNIT_ASSERT_THROW_MESSAGE("normalement cela ne focntionne pas ",param.lire_avec_accolades_depuis(is),TriouError);

      }
      param.supprimer("int");
      CPPUNIT_ASSERT_THROW_MESSAGE("normalement cela ne focntionne pas plus de int ",param.supprimer("int"),TriouError);
    }
  };
  void testDictionnaire()
  {
    int val,val2;
    Param param("test_dictionnaire");
    param.ajouter("value",&val);
    param.dictionnaire("zero",0);
    param.dictionnaire("un",1);
    param.ajouter("value2",&val2);

    {
      // test lecture dico
      EChaine is("{ value zero value2 3 }");
      param.lire_avec_accolades_depuis(is);

      CPPUNIT_ASSERT_MESSAGE("pb lecture dictionnaire ",val==0);
      CPPUNIT_ASSERT_MESSAGE("pb lecture val2 ",val2==3);
    }
    {
      // test lecture dico
      EChaine is("{ value un value2 3 }");
      param.lire_avec_accolades_depuis(is);

      CPPUNIT_ASSERT_MESSAGE("pb lecture dictionnaire ",val==1);
      CPPUNIT_ASSERT_MESSAGE("pb lecture val2 ",val2==3);
    }
    {
      // test lecture dico erreur
      EChaine is("{ value 3 value2 3 }");
      CPPUNIT_ASSERT_THROW_MESSAGE("test lecture dico erreur value not in dict ",param.lire_avec_accolades_depuis(is),TriouError);
    }

    Param& ssp= param.dictionnaire_param("lecture",2);
    double dd;
    ssp.ajouter("valeur",&dd);
    param.dictionnaire("mot",3);

    {
      // test lecture dico
      EChaine is("{  value2 mot }");
      param.lire_avec_accolades_depuis(is);
      CPPUNIT_ASSERT_MESSAGE("test lecture dico standrard ",val2==3);
    }

    {
      // test lecture dico param
      EChaine is("{  value2 lecture { valeur 7. } }");
      param.lire_avec_accolades_depuis(is);
      CPPUNIT_ASSERT_MESSAGE("test lecture dico avec param ",val2==2);
      CPPUNIT_ASSERT_MESSAGE("test lecture dico avec param ",dd==7.);
    }

    {
      // test lecture dico param
      EChaine is("{  value2 lecture { valeur2 7. } }");

      CPPUNIT_ASSERT_THROW_MESSAGE("test lecture dico erreur mot cles pas dans saous param ",param.lire_avec_accolades_depuis(is),TriouError);

    }

  };
  void testLecture_objets()
  {
    Nom objet_test;
    Nom_test nom_test;
    Param param("test_objets");
    param.ajouter("objet",&objet_test);
    param.ajouter_non_std("objet_non_std",&nom_test);
    {
      // test lecture objet
      EChaine is("{ objet Objet_coucou }");
      param.lire_avec_accolades_depuis(is);

      CPPUNIT_ASSERT_MESSAGE("pb lecture objet ",objet_test==Nom("Objet_coucou"));
      CPPUNIT_ASSERT_MESSAGE("pb lecture objet ",objet_test!=Nom("objet_coucou"));
    }

    {
      // test lecture_non_standard
      EChaine is("{ objet_non_std mot_test_ok Objet_coucou }");
      param.lire_avec_accolades_depuis(is);
      CPPUNIT_ASSERT_MESSAGE("pb lecture objet ",objet_test==Nom("Objet_coucou"));
      CPPUNIT_ASSERT_MESSAGE("pb lecture objet ",objet_test!=Nom("objet_coucou"));
    }
    {
      // test lire_mot_cle_non_std
      EChaine is("Objet_coucou ");
      int status=nom_test.lire_motcle_non_standard_public(Motcle("mot_non_compris"),is);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("lire_motcle_non_standard doit renvoyer -1 si il ne comprend pas ",-1,status);
    }
    {
      // test lecture_non_standard erreur
      EChaine is("{ objet_non_std mot_test_pas_ok Objet_coucou }");
      CPPUNIT_ASSERT_THROW_MESSAGE("test lecture_non_standard erreur",param.lire_avec_accolades_depuis(is),TriouError);

    }
  };
  void testConditions()
  {
    {
      Param param("test_condition");
      init2(param);
      Nom objet_test;

      param.ajouter("objet",&objet_test);
      param.ajouter_condition("is_read_double","double doit etre lu!!!!!");

      {
        // test lecture_non_standard erreur
        EChaine is("{ int 5 double -1 }");

        param.read(is); // on test la fonction read qui lit sans check
        int status=param.check();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("test_is_read_double",1,status);
        param.ajouter_condition("is_read_objet","objet doit etre lu!!!!!");
        status=param.check();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("is_read_test_objet",0,status);
      }
      {
        EChaine is("{ objet coucou }");

        param.lire_avec_accolades_depuis(is);
        int status=param.check();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("test_is_read_objet",1,status);
      }

      {

        param.ajouter_condition("(value_of_double_EQ_(-1))+(value_of_double_EQ_3)"," double  lu vaut -1");

        int status=param.check();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("double==-1|| double =3 ",1,status);

        param.ajouter_condition("is_read_double*(value_of_double>0)"," double doit etre lu et positif","test1");
        status=param.check();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("is_read_double*(value_of_double>0)",0,status);
        param.supprimer_condition("test1");
        status=param.check();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("plus de condition ",1,status);
        param.ajouter_condition("is_read_double*(value_of_double>0)"," double doit etre lu et positif","test1");

        status=param.check();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("is_read_double*(value_of_double>0)",0,status);
        //EChaine is("{ double 3. }");
        //param.lire_avec_accolades_depuis(is);
        double1=3.;
        status=param.check();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("value_of_double>0, vrai maintenant ",1,status);

        param.ajouter_condition("is_read_double*(value_of_flag_eq_0)"," double doit etre lu et flag faux");
        status=param.check();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("test val non lu",1,status);




        // test NOT
        param.ajouter_condition("NOT(value_of_flag)"," test not");
        status=param.check();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("test not",1,status);

        param.ajouter_condition("is_read_double*(flag_eq_0)"," double doit etre lu et flag non mais mal ecit");
        CPPUNIT_ASSERT_THROW_MESSAGE("test condition mal entree",param.check(),TriouError);
      }
    }
    {
      Param param("test_condition");
      init2(param);
      Nom objet_test;
      EChaine is("{ int 5 double -1 }");
      param.ajouter("objet",&objet_test);
      // verification des erreurs getvalue...

      // param.ajouter_condition("objet_val"," test not");
      param.read(is);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("test_get_value_int",5.,param.get_value("value_of_int"));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("test_get_value_double",-1.,param.get_value("value_of_double"));
      double test=0;
      param.ajouter("test",&test);
      Cerr<<"value of test "<<param.get_value("value_of_test")<<finl;
      //int status=param.check();
      CPPUNIT_ASSERT_THROW_MESSAGE("variable non defini",param.get_value("value_of_objet"),TriouError);
    }

  };
  void testajouterderiv()
  {
    Param param("deriv");
    DERIV(Comm_Group) deriv;
    param.ajouter_deriv("deriv","Comm_Group_",&deriv);
    {
      EChaine is("{ deriv MPI }");
      /*
        on bloque le test pour l'instant car Comm_Group_MPI::readOn fait exit
        param.lire_avec_accolades_depuis(is);
        CPPUNIT_ASSERT_MESSAGE("typage correct du deriv ?",deriv.valeur().que_suis_je()=="Comm_Group_MPI");
      */
    }
    {
      EChaine is("{ deriv bidon }");
      CPPUNIT_ASSERT_THROW_MESSAGE("classe inconnue",param.lire_avec_accolades_depuis(is),TriouError);

    }
  }
  void test_ajouter_param()
  {
    int i=-1;
    Param pere("pere");
    Param& p = pere.ajouter_param("fils");
    p.ajouter("test",&i);
    {
      EChaine is(" { test 3 }  ");
      p.lire_avec_accolades_depuis(is);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("test_read_value",3,i);
    }
    i=-1;
    {
      EChaine is(" { fils { test 3 } } ");
      pere.lire_avec_accolades_depuis(is);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("test_read_value",3,i);
    }
    {
      EChaine is(" { fils { test2 3 } } ");
      CPPUNIT_ASSERT_THROW_MESSAGE("test2 incompris par pere/fils",pere.lire_avec_accolades_depuis(is),TriouError);
    }
  }
};

CPPUNIT_TEST_SUITE_REGISTRATION( ParamTest );


#endif
