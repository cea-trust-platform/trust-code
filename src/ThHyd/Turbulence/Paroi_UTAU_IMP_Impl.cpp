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
// File:        Paroi_UTAU_IMP_Impl.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////
#include <Paroi_UTAU_IMP_Impl.h>
#include <Motcle.h>
#include <Objet_U.h>


Entree& Paroi_UTAU_IMP_Impl::lire_donnees(Entree& s)
{
  // Definition des mots-cles
  Motcles les_mots(3);
  les_mots[0] = "u_tau";
  les_mots[1] = "lambda_c";
  les_mots[2] = "diam_hydr";

  u_star_ok = 0;
  lambda_c_ok = 0;
  int diam_ok = 0;

  // Lecture et interpretation
  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  s >> motlu;
  if(motlu == accolade_ouverte)
    {
      // on passe l'accolade ouvrante
      s >> motlu;
    }
  while (motlu != accolade_fermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :   // u_star expression
          {
            u_star_ok = 1;
            s >> u_star;
            break;
          }
        case 1 :   // lambda_c expression
          {
            lambda_c_ok=1;
            Nom tmp;
            s >> tmp;
            Cerr << "Lecture et interpretation de la fonction " << tmp << " ... ";
            lambda_c.setNbVar(2+Objet_U::dimension);
            lambda_c.setString(tmp);
            lambda_c.addVar("x");
            if (Objet_U::dimension>1)
              lambda_c.addVar("y");
            if (Objet_U::dimension>2)
              lambda_c.addVar("z");
            lambda_c.addVar("Re");
            lambda_c.addVar("Dh");
            lambda_c.parseString();
            break;
          }
        case 2: // diam_hydr
          diam_ok = 1;
          s >> diam_hydr;
          break;
        default : // non compris
          Cerr << "Mot cle \"" << motlu << "\" non compris lors de la lecture d'un Paroi_UTAU_Impose" << endl;
          Process::exit();
        }
      s >> motlu;
    }

  if ((u_star_ok == 0) && (lambda_c_ok == 0))
    {
      Cerr << "Il faut definir le champ u_star a une composante ou lambda_c=f(Re,Dh,x,y,z)" << endl;
      Process::exit();
    }

  if (u_star_ok == 1)
    if (u_star->nb_comp()!=1)
      {
        Cerr << "Il faut definir le champ u_star a une composante" << endl;
        Process::exit();
      }

  if ((lambda_c_ok == 1) && (diam_ok == 0))
    {
      Cerr << "Il faut definir le diametre hydraulique si vous definissez lambda_c" << endl;
      Process::exit();

    }
  return s;
}

double Paroi_UTAU_IMP_Impl::calculer_utau(const DoubleVect& pos, double norm_v, double d_visco)
{
  if (u_star_ok == 1)
    {
      return u_star->valeur_a_compo(pos,0);
    }
  else
    {
      double dh = diam_hydr->valeur_a_compo(pos,0);
      double reynolds=norm_v*dh/d_visco;

      for (int i=0; i<Objet_U::dimension; i++)
        {
          lambda_c.setVar(i,pos[i]);
        }
      lambda_c.setVar("Re",reynolds);
      lambda_c.setVar("Dh",dh);
      double lambda = lambda_c.eval();
      return norm_v*sqrt(lambda/8.);
    }
}
