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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Tests_Util.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////




// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
#ifdef TESTS__
int test_Ref()
{
  Cerr << "test_Ref" << finl;
  DoubleVect x(5);
  REF(DoubleVect) xx;                                //Constructeur par defaut.
  assert(xx.non_nul() == 0);                        //non_nul
  REF(DoubleVect) yy(x);                        //Constructeur par const T&.
  REF(DoubleVect) zz(yy);                        //Constructeur par const T&.
  xx=x;                                        //Operateur = const T&.
  xx=yy;                                        //Operateur = const Deriv<T>&.
  assert(xx->size() == x.size());                //Operateur ->
  assert(yy.valeur().size() == zz->size());        //valeur()
  assert(xx == yy);                                //Operateur ==
  (static_cast<DoubleVect&>(xx))[2]=1;                        //cast
  assert (xx == x);                                //Operateur !=
  const Nom& name=xx.le_nom();                        //le_nom()
  return zz.non_nul();                                //non_nul
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int test_Deriv()
{
  Cerr << "test_Deriv" << finl;
  DoubleVect x(5);
  DERIV(DoubleVect) xx;                        //Constructeur par defaut.
  assert(xx.non_nul() == 0);
  DERIV(DoubleVect) yy(x);                        //Constructeur par const T&.
  DERIV(DoubleVect) zz(yy);                        //Constructeur par const T&.
  xx=x;                                        //Operateur = const T&.
  xx=yy;                                        //Operateur = const Deriv<T>&.
  assert(xx->size() == x.size());                //Operateur ->
  assert(yy.valeur().size() == zz->size());        //valeur()
  assert(xx == yy);                                //Operateur ==
  (static_cast<DoubleVect&>(xx))[2]=1;                        //cast
  assert (xx != x);                                //Operateur !=
  const Nom& name=xx.le_nom();                        //le_nom()
  return zz.non_nul();                                //non_nul
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int test_List()
{
  Cerr << "test_List" << finl;
  LIST(Nom) la_liste;
  la_liste.add("Chien");
  la_liste.add("Chat");
  la_liste.add("Chou");
  Cerr << la_liste;
  LIST(Nom) copie(la_liste);
  Cerr << copie;
  if(la_liste == copie)
    Cerr << "The two lists are identical" << finl;
  copie.add_if_not(Nom("Chien"));
  copie.add_if_not(Nom("Chiens"));
  copie["Chiens"]=Nom("Chats");
  la_liste[2]="Choux";
  Cerr << la_liste;
  Cerr << copie;
  if(la_liste != copie)
    Cerr << "The two lists are different" << finl;
  return 1;
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int test_Vect()
{
  Cerr << "test_Vect" << finl;
  DoubleVect x(5);
  VECT(DoubleVect) xx(2);
  xx[1]=x;
  assert( xx.search(x) == 1);
  Motcles les_mots(3);
  les_mots[0]=Motcle("Zero");
  les_mots[1]=Motcle("Un");
  les_mots[2]=Motcle("Deux");
  assert( les_mots.contient(Motcle("Un"))     );
  assert( les_mots.search(Motcle("Deux")) == 2);
  assert( les_mots.search(Motcle("Un")) == 1  );
  assert( les_mots.search(Motcle("Zero")) == 0);
  return 1;
}


// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int test_DoubleVect()
{
  Cerr << "test_DoubleVect" << finl;
  return 1;
}

// Description:
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int test_IntVect()
{
  Cerr << "test_IntVect" << finl;
  return 1;
}
#endif
