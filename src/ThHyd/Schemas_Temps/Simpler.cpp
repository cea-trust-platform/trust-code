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
// File:        Simpler.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/48
//
//////////////////////////////////////////////////////////////////////////////

#include <Simpler.h>
#include <Navier_Stokes_std.h>
#include <EChaine.h>
#include <Debog.h>
#include <Matrice_Bloc.h>
#include <Assembleur_base.h>
#include <Statistiques.h>
#include <Schema_Temps_base.h>
#include <DoubleTrav.h>
#include <Schema_Euler_Implicite.h>
#include <Probleme_base.h>

Implemente_instanciable(Simpler,"Simpler",Simple);

Sortie& Simpler::printOn(Sortie& os ) const
{
  return Simple::printOn(os);
}

Entree& Simpler::readOn(Entree& is )
{
  return Simple::readOn(is);
}


// DEscription:
// calcul D correction_en_vitesse= E current + resu )
// avec D diagonale de la matrice et E = D-matrice =-(matrice-D)
int inverser_par_diagonale(const Matrice_Morse& matrice,const DoubleTrav& resu,const DoubleTab& current,DoubleTrav& correction_en_vitesse)
{
  const IntVect& tab1 = matrice.tab1_;
  const IntVect& tab2 = matrice.tab2_;
  const DoubleVect& coeff = matrice.coeff_;

  int deux_entrees=0;
  int nb_comp=1;
  if (resu.nb_dim()==2)
    {
      deux_entrees=1;
      nb_comp=resu.dimension(1);
    }
  int nb_ligne_reel=correction_en_vitesse.dimension(0);
  // Determine U^
  if (deux_entrees==0)
    {
      for(int i=0; i<nb_ligne_reel; i++)
        {
          for(int j=0; j<nb_comp; j++)
            {
              double t=resu(i);
              for (int k=tab1(i*nb_comp+j); k<tab1(i*nb_comp+j+1)-1; k++)
                t -= coeff(k)*current(tab2(k)-1);

              correction_en_vitesse(i) = t/matrice(i*nb_comp+j,i*nb_comp+j);
            }
        }
      // verif
      // test
      if (0)
        for(int i=0; i<nb_ligne_reel; i++)
          {
            for (int j=0; j<nb_comp; j++)
              {
                int ii=i*nb_comp+j;
                double tmp=resu(i);
                for (int k = 0; k<matrice.nb_colonnes(); k++)
                  if ((k/nb_comp)!=i)
                    {
                      if (matrice(ii,k))
                        tmp -= matrice(ii,k)*current((k/nb_comp));
                    }

                double test=tmp/matrice(ii,ii);
                if (!est_egal(test,correction_en_vitesse(i),1e-5))
                  {
                    Cerr<<i<<" "<<j<<" "<<resu(i)<<finl;
                    Cerr<<test<<" ici "<<correction_en_vitesse(i)-test<<finl;
                    return -1;
                  }
              }
          }
    }

  else
    {
      for(int i=0; i<nb_ligne_reel; i++)
        {
          for(int j=0; j<nb_comp; j++)
            {
              double t=resu(i,j);
              for (int k=tab1(i*nb_comp+j); k<tab1(i*nb_comp+j+1)-1; k++)
                {
                  int i1 = (tab2(k)-1)/nb_comp;
                  int j1 = (tab2(k)-1)-i1*nb_comp;
                  t -= coeff(k)*current(i1,j1);
                }
              const double& diagonal_coefficient = matrice(i*nb_comp+j,i*nb_comp+j);
              assert(diagonal_coefficient!=0);
              correction_en_vitesse(i,j) = t/diagonal_coefficient;
            }
        }
      // test
      if (0)
        {
          Cerr<<"milieu"<<finl;
          DoubleTrav corr2(correction_en_vitesse);
          matrice.multvect(current,corr2);
          for(int i=0; i<nb_ligne_reel; i++)
            {
              for (int j=0; j<nb_comp; j++)
                {
                  int ii = i*nb_comp+j;
                  corr2(i,j) = (resu(i,j)-corr2(i,j)+matrice(ii,ii)*current(i,j))/matrice(ii,ii);
                  double test = corr2(i,j);
                  if (!est_egal(test,correction_en_vitesse(i,j)))
                    {
                      Cerr<<i<<" "<<j<<" "<<resu(i,j)<<finl;
                      Cerr<<test<<" ici "<<correction_en_vitesse(i,j)<<" "<<current(i,j)<<" "<<matrice(ii,ii) <<finl;
                      return -1;
                    }
                }
            }
          Cerr<<"fin"<<finl;
        }
    }
  correction_en_vitesse.echange_espace_virtuel();
  return 0;
}


//Entree : Uk-1 ; Pk-1
//Sortie Uk ; Pk
//k designe une iteration

void Simpler::iterer_NS(Equation_base& eqn,DoubleTab& current,DoubleTab& pression,double dt,Matrice_Morse& matrice,double seuil_resol,DoubleTrav& secmem,int nb_ite,int& converge)
{
  if (eqn.probleme().is_QC())
    {
      Cerr<<" Simpler cannot be used with a quasi-compressible fluid."<<finl;
      Cerr<<__FILE__<<__LINE__<<" non code" <<finl;
      exit();
    }

  Parametre_implicite& param = get_and_set_parametre_implicite(eqn);
  SolveurSys& le_solveur_ = param.solveur();

  Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std,eqn);
  DoubleTrav gradP(current);
  DoubleTrav correction_en_pression(pression);
  DoubleTrav correction_en_vitesse(current);
  DoubleTrav resu(current);

  //int deux_entrees = 0;
  //if (current.nb_dim()==2) deux_entrees = 1;
  Operateur_Grad& gradient = eqnNS.operateur_gradient();
  Operateur_Div& divergence = eqnNS.operateur_divergence();

  //  int nb_comp = 1;
  //int nb_dim = current.nb_dim();

  //if (nb_dim==2)
  //  nb_comp = current.dimension(1);

  //Construction de matrice et resu
  //matrice = A[Uk-1] = M/dt + CONV +DIFF
  //resu =  A[Uk-1]Uk-1 -(A[Uk-1]Uk-1-Ss) + Sv -BtPk-1
  gradient.calculer(pression,gradP);
  resu -= gradP;
  eqnNS.assembler_avec_inertie(matrice,current,resu);
  le_solveur_.valeur().reinit();

  //Resolution du systeme : D[Uk-1]UPk = E[Uk-1]Uk-1 + Sv + Ss -BtPk-1 + (M/dt)Uk-1
  //matrice = A[Uk-1] = D - E avec D partie diagonale de A
  //correction_en_vitesse = UPk ; current = Uk-1 ; resu = Sv + Ss -BtPk-1
  int status = inverser_par_diagonale(matrice,resu,current,correction_en_vitesse);
  if (status!=0) exit();
  Debog::verifier("Simpler::iterer_NS correction_en_vitesse",correction_en_vitesse);

  //Construction de matrice_en_pression_2 = BD-1Bt[Uk-1]
  Matrice& matrice_en_pression_2 = eqnNS.matrice_pression();
  assembler_matrice_pression_implicite(eqnNS,matrice,matrice_en_pression_2);
  SolveurSys& solveur_pression_ = eqnNS.solveur_pression();
  solveur_pression_.valeur().reinit();

  //Calcul de BUPk
  divergence.calculer(correction_en_vitesse,secmem);
  secmem *= -1;
  secmem.echange_espace_virtuel();
  if (nb_ite==1)
    eqnNS.assembleur_pression().valeur().modifier_secmem(secmem);

  //Resolution du systeme (BD-1Bt)P*_k = BUPk
  //correction_en_pression = P*_k ; secmem = BUPk
  solveur_pression_.resoudre_systeme(matrice_en_pression_2.valeur(),
                                     secmem,correction_en_pression);

  //Calcul de Pk = Pk-1 + P*_k
  operator_add(pression, correction_en_pression, VECT_ALL_ITEMS);

  eqnNS.assembleur_pression().valeur().modifier_solution(pression);

  //Calcul de Bt P*_k et ajustement de resu a -Bt Pk
  gradient.valeur().multvect(correction_en_pression,gradP);
  resu -= gradP;
  resu.echange_espace_virtuel();
  Debog::verifier("Simpler::iterer_NS resu",resu);

  //Resolution du systeme : A[Uk-1]U*_k = -BtPk + Sv + Ss + (M/dt)Uk-1
  //current = U*_k ; resu = A[Uk-1]Uk-1 - (A[Uk-1]Uk-1-Ss) + Sv -BtPk + (M/dt)Uk-1
  le_solveur_.resoudre_systeme(matrice,resu,current);

  //Calcul de BU*_k
  divergence.calculer(current,secmem);
  secmem *= -1;
  secmem.echange_espace_virtuel();
  Debog::verifier("Simpler::iterer_NS secmem",secmem);

  //Resolution du systeme (BD-1Bt)P'k = BU*_k
  //correction_en_pression = P'k ; secmem = BU*_k
  correction_en_pression = 0;
  solveur_pression_.resoudre_systeme(matrice_en_pression_2.valeur(),
                                     secmem,correction_en_pression);

  //Resolution du systeme D[Uk-1](Uk-U*_k) = -BtP'k
  //correction_en_vitesse = U'k = Uk-U*_k ; correction_en_pression = P'k
  calculer_correction_en_vitesse(correction_en_pression,gradP,correction_en_vitesse,matrice,gradient);

  if (0)
    {
      Cerr<<" rr "<<mp_max_abs_vect(secmem)<<finl;
      //Cerr<<secmem<<finl;
      secmem*=-1;
      divergence.ajouter(correction_en_vitesse, secmem);
      Cerr<<" rr2 "<<mp_max_abs_vect(secmem)<<finl;

    }

  //Calcul de Uk = U*_k + U'k
  current += correction_en_vitesse;
  current.echange_espace_virtuel();
  Debog::verifier("Simpler::iterer_NS current",current);

  if (0)
    {
      divergence.calculer(current, secmem);
      Cerr<<" apresdiv "<<mp_max_abs_vect(secmem)<<finl;;
    }
}

