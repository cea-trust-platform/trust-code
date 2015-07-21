/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Parser_test.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <cppunit.h>
#ifdef cppunit_
#include <EcrFicCollecte.h>
#include <EChaine.h>
#include <Motcle.h>
#include <Parser_U.h>
#include <UserUnaryFunction.h>
#include <TriouError.h>


double f_test(double x)
{
  return x*x-x+1;
}
double f_test(double x, double y)
{
  return (x*x-x+1)*(y*y*y-y*x+1.);
}

class ParserTest: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( ParserTest );
  CPPUNIT_TEST( testSimpleOperation );
  CPPUNIT_TEST( testComparaison );
  CPPUNIT_TEST( testLogic );
  CPPUNIT_TEST( testStdFunction );
  CPPUNIT_TEST( testFunction );
  CPPUNIT_TEST( testVar );
  CPPUNIT_TEST( testSyntaxe );
  CPPUNIT_TEST_SUITE_END();

public:
  ParserTest():CPPUNIT_NS::TestFixture::TestFixture()
  {
    Cerr<<"builder "<<finl;
  };
  void tearDown()
  {
    //cerr<<"ICI" <<endl;;
  } ;
  void test_expr_sol( Nom* expr_sol,int  nb_expr)
  {
    Parser_U p;
    p.setNbVar(0);
    for (int i=0; i<nb_expr; i++)
      {
        Nom expr = expr_sol[2*i];
        double resu = atof(expr_sol[2*i+1].getChar());
        p.setString(expr);
        p.parseString();
        Nom pb("pb ");
        pb+=expr;
        if (i==nb_expr-1)
          CPPUNIT_ASSERT_MESSAGE("test fin de liste ",resu!=p.eval());
        else
          CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(pb.getChar(),resu,p.eval(),1e-8);
      }


  } ;
  void testSimpleOperation()
  {
    int nb_expr=19;
    Nom expr_sol[] = { "1+1", "2"
                       , "2+3*5" , "17"
                       , "2+3/5" , "2.6"
                       , "3*5+2" , "17"
                       , "-3*5+2" , "-13"
                       , "-3*(5+2)" , "-21"
                       , "3*(-5+2)" , "-9"
                       , "3*(-2+2)" , "0"
                       , "-6+3*5+(2+3)*3-7*(-2+1)" , "31"
                       , "1e2" , "100"
                       , "1e-2" , "0.01"
                       , "1e+2" , "100"
                       , "1E2" , "100"
                       , "1E-2*5" , "0.05"
                       , "1E+2*5" , "500"
                       , "4.^0.5","2"
                       , "+3","3"
                       ,"-3*2","-6"
                       , "1", "0"
                     };
    test_expr_sol(expr_sol,nb_expr);
  };

  void testComparaison()
  {
    int nb_expr=53;
    Nom expr_sol[] = { "1<2", "1"
                       , "2<1", "0"
                       , "1<1", "0"
                       , "1>2", "0"
                       , "2>1", "1"
                       , "2>2", "0"
                       , "2[1", "0"
                       , "1[2", "1"
                       , "2[2", "1"
                       , "2]1", "1"
                       , "1]2", "0"
                       , "2]2", "1"
                       , "(1<2)+1", "2"
                       , "(1<2)+1", "2"
                       , "(1<2)+1", "2"
                       , "(1<2)+1", "2"
                       , "1<(2+1)", "1"
                       , "1[1", "1"
                       , "1>2", "0"
                       , "1]2", "0"
                       , "-1<2", "1"
                       , "1<(-2)", "0"
                       , "-2<1", "1"
                       ,"1_LT_2", "1"
                       , "2_LT_1", "0"
                       , "1_LT_1", "0"
                       , "1_GT_2", "0"
                       , "2_GT_1", "1"
                       , "2_GT_2", "0"
                       , "2_LE_1", "0"
                       , "1_LE_2", "1"
                       , "2_LE_2", "1"
                       , "2_GE_1", "1"
                       , "1_GE_2", "0"
                       , "2_GE_2", "1"
                       , "(1_LT_2)+1", "2"
                       , "(1_LT_2)+1", "2"
                       , "(1_LT_2)+1", "2"
                       , "(1_LT_2)+1", "2"
                       , "1_LT_(2+1)", "1"
                       , "1_LE_1", "1"
                       , "1_GT_2", "0"
                       , "1_GE_2", "0"
                       , "-1_LT_2", "1"
                       , "1_LT_(-2)", "0"
                       , "-2_LT_1", "1"
                       ,"(1)_MAX_(-1.)","1."
                       ,"99._max_101.","101."
                       ,"1._min_(-1.)","-1."
                       ,"99._min_101.","99."
                       ,"12_mod_2","0"
                       , "11%3","2"
                       , "1", "0"
                     };
    test_expr_sol(expr_sol,nb_expr);
  };

  void testStdFunction()
  {


    int nb_expr=9;
    Nom expr_sol[] = { "COS(0)", "1"
                       , "SIN(0.)" , "0"
                       , "COS(0.1)*COS(0.1)+SIN(0.1)*SIN(0.1)" , "1"
                       , "EXP(LN(2))" , "2"
                       , "INT(1.123)" , "1"
                       , "INT(-1.123)" , "-1"
                       , "Abs(-1.3)","1.3"
                       , "Abs(7.9)","7.9"
                       , "1", "0"
                     };
    test_expr_sol(expr_sol,nb_expr);
  };



  void testLogic()
  {


    int nb_expr=15;
    Nom expr_sol[] = { "1_and_1", "1"
                       ,     "1_AnD_0", "0"
                       ,     "0_AND_0", "0"
                       ,     "1_OR_1", "1"
                       ,     "1_OR_0", "1"
                       ,     "0_OR_0", "0"
                       ,     "(1+2)_EQ_3", "1"
                       ,     "(1+2)_NEQ_10", "1"
                       ,     "(1+2)_NEQ_3", "0"
                       ,     "(1+2)_EQ_10", "0"
                       ,     "(1_EQ_1)_AND_((2-3)_EQ_(-1))", "1"
                       ,     "(1_EQ_0)_AND_((2-3)_EQ_(-1))", "0"
                       ,     "NOT(12)", "0"
                       ,     "NOT(0)", "1"
                       , "1", "0"
                     };
    test_expr_sol(expr_sol,nb_expr);
  };
  void testFunction()
  {
    Parser_U p;
    p.setNbVar(0);

    UserUnaryFunction f;
    Nom nom="f";
    Nom var="x";
    Nom expr_f="x*x-x+1";
    f.setFunction(nom, var, expr_f);
    p.addFunc(f);


    for (int i=0; i<10; i++)
      {
        Nom expr="f(";
        expr+=Nom(i);
        expr+=Nom(")");
        p.setString(expr);
        p.parseString();
        Nom pb("Pb test ");
        pb+=expr;

        CPPUNIT_ASSERT_EQUAL_MESSAGE(pb.getChar(),f_test(i),p.eval());
      }
  };

  void testVar()
  {
    Parser_U p;
    p.setNbVar(2);
    p.addVar("x");
    p.addVar("y");

    Nom expr="(x*x-x+1)*(y*Y*y-y*X+1.)";
    p.setString(expr);
    p.parseString();


    for (int i=0; i<10; i++)
      {
        for (int j=0; j<10; j++)
          {
            p.setVar("x",i);
            p.setVar("y",j);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Pb ",f_test(i,j),p.eval());
          }
      }
    /*
      on devrait avoir une erreur dans ce bloc */
    Parser_U p2;
    Nom pb("2x");
    p2.setNbVar(1);
    p2.addVar("x");
    p2.setString(pb);
    p2.setVar("x",0);
    CPPUNIT_ASSERT_THROW_MESSAGE("verification que 2x ne marche pas", p2.parseString(),TriouError);

  };
  void testSyntaxe()
  {
    Parser_U p;
    p.setNbVar(2);
    p.addVar("x");
    p.addVar("y");

    Nom expr="(12,34*2)";
    p.setString(expr);
    CPPUNIT_ASSERT_THROW_MESSAGE("verification que 12,34*2 ne marche pas", p.parseString(),TriouError);
    expr="(12,34e+4)";
    p.setString(expr);
    CPPUNIT_ASSERT_THROW_MESSAGE("verification que 12,34e4 ne marche pas", p.parseString(),TriouError);
    expr="(1234e+1,4)";
    p.setString(expr);
    CPPUNIT_ASSERT_THROW_MESSAGE("verification que 1234e1,4 ne marche pas", p.parseString(),TriouError);

  };

};
CPPUNIT_TEST_SUITE_REGISTRATION( ParserTest );


#endif
