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
// File:        Simpler_Base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/31
//
//////////////////////////////////////////////////////////////////////////////


#include <Simpler_Base.h>
#include <SChaine.h>
#include <EChaine.h>
#include <Navier_Stokes_std.h>
#include <Zone_VF.h>
#include <Assembleur_base.h>
#include <Probleme_base.h>

Implemente_base_sans_constructeur(Simpler_Base,"Simpler_Base",Solveur_non_lineaire);

Sortie& Simpler_Base::printOn(Sortie& os ) const
{
  return os;
}

Entree& Simpler_Base::readOn(Entree& is )
{
  int is_seuil_resol_lu = 0;

  double seuil_convergence_solveur_prov=-1;
  is_seuil_convg_variable = 0;
  controle_residu_ = 0;
  Motcle accferme = "}";
  Motcle accouverte = "{";
  int le_solveur_est_lu = 0;
  Motcle motlu;
  is >> motlu;
  assert(motlu==accouverte);
  no_qdm_ = 0;
  is >> motlu;
  Nom le_solveur_lu_;
  while(motlu!=accferme )
    {
      if (motlu=="max_iter_implicite")
        {
          Cerr << motlu << finl;
          Cerr << motlu <<" is no more longer understood by Simpler but by the time scheme"<<finl;
          exit();
        }
      else if (motlu=="seuil_convergence_implicite")
        {
          Cerr << motlu << finl;
          is >> param_defaut_.seuil_convergence_implicite();
        }
      else if (motlu=="seuil_convergence_variable")
        {
          Cerr << "Option seuil_convergence_variable is not yet understood." << finl;
          exit();
          Cerr << motlu << finl;
          facteur_convg_ = 100;
          is_seuil_convg_variable = 1;
        }
      else if (motlu=="seuil_generation_solveur")
        {
          Cerr << motlu << finl;
          is_seuil_resol_lu = 1;
          is >> param_defaut_.seuil_generation_solveur();
        }
      else if (motlu=="seuil_verification_solveur")
        {
          Cerr << motlu << finl;
          is >> param_defaut_.seuil_verification_solveur();
        }
      else if (motlu=="seuil_test_preliminaire_solveur")
        {
          Cerr << motlu << finl;
          is >> param_defaut_.seuil_test_preliminaire_solveur();
        }
      else if (motlu=="seuil_convergence_solveur")
        {
          Cerr << motlu << finl;
          is_seuil_resol_lu = 1;
          is >> seuil_convergence_solveur_prov;
        }
      else if (motlu=="nb_it_max")
        {
          is >> param_defaut_.nb_it_max();
        }
      else if (motlu=="controle_residu")
        {
          controle_residu_ = 1;
        }
      else if (motlu=="no_qdm")
        {
          Cerr << motlu << finl;
          no_qdm_ = 1;
        }
      else if (motlu=="solveur")
        {
          le_solveur_est_lu = 1;
          SChaine toto;
          // redefinition de motlu pour garder les minuscules/majuscules
          Nom motlubis;
          int nb_acc=0;
          int ok=0;
          while (nb_acc!=0 || !ok)
            {
              is >>motlubis;
              toto<<" "<<motlubis;
              if (motlubis=="}") nb_acc--;
              else if (motlubis=="{")
                {
                  ok=1;
                  nb_acc++;
                };
            }
          le_solveur_lu_ = Nom(toto.get_str());
        }
      else
        {
          lire(motlu,is);
        }
      is >> motlu;
    }

  if (seuil_convergence_solveur_prov>0)
    {
      param_defaut_.set_seuil_solveur_avec_seuil_convergence_solveur(seuil_convergence_solveur_prov);
    }
  if ((is_seuil_resol_lu==0) && (le_solveur_est_lu==0))
    {
      Cerr<<"Neither the solving object nor the threshold seuil_convergence_solveur has been indicated."<<finl;
      Cerr<<"At least one of them (or both) must be specified."<<finl;
      exit();
    }

  if (param_defaut_.seuil_convergence_implicite()<0)
    param_defaut_.seuil_convergence_implicite() = DMAXFLOAT;

  if (param_defaut_.seuil_verification_solveur()<0)
    param_defaut_.seuil_verification_solveur() = DMAXFLOAT;
  // on laisse seuil_test_preliminaire <0


  if (le_solveur_est_lu==0)
    {
      SChaine toto;
      toto<<"Gmres { diag seuil "<<param_defaut_.seuil_generation_solveur()<<" nb_it_max "<< param_defaut_.nb_it_max() << " controle_residu " <<controle_residu_<<" } "<<finl;
      le_solveur_lu_=Nom(toto.get_str());
    }

  {
    EChaine titi(le_solveur_lu_);
    titi>> param_defaut_.solveur();
  }
  param_defaut_.solveur().nommer("solveur_implicite");
  return is;
}

// Description:
// retourne le parametre_implicte de l'equation si il existe si il n'existe pas le cree...
// si les params sont vides on copie ceux du simpler
Parametre_implicite& Simpler_Base::get_and_set_parametre_implicite(Equation_base& eqn)
{
  Parametre_equation& param = eqn.parametre_equation();
  if (!param.non_nul())
    {
      param.typer("Parametre_implicite");
    }
  if (!sub_type(Parametre_implicite,param.valeur()))
    {
      Cerr<<eqn.que_suis_je()<<" has parameters of type "<<param.que_suis_je()<<" not coherent with "<<que_suis_je()<<finl;
      exit();
    }

  Parametre_implicite& param_impl = ref_cast(Parametre_implicite,param.valeur());
  // on regarde si il y a des valeurs par defaut a recopier
  if (param_impl.seuil_convergence_implicite()<0)
    param_impl.seuil_convergence_implicite() = param_defaut_.seuil_convergence_implicite();
  if (param_impl.seuil_verification_solveur()<0)
    param_impl.seuil_verification_solveur() = param_defaut_.seuil_verification_solveur();
  if (param_impl.seuil_test_preliminaire_lu()==0)
    param_impl.seuil_test_preliminaire_solveur() = param_defaut_.seuil_test_preliminaire_solveur();
  if (!param_impl.solveur().non_nul())
    param_impl.solveur() = param_defaut_.solveur();

  // Some checks:
  if (eqn.probleme().get_coupled())
    {
      if (param_impl.seuil_convergence_implicite()>0.1*DMAXFLOAT)
        {
          Cerr << finl << "Error!" << finl;
          Cerr << "seuil_convergence_implicite option should be defined in your time scheme." << finl;
          Cerr << "It is a mandatory option for a calculation with coupled problems." << finl;
          Cerr << "See User's manual for recommended values." << finl;
          exit();
        }
    }
  /*
  else if (param_impl.seuil_convergence_implicite()<0.1*DMAXFLOAT)
  {
        Cerr << finl << "Error!" << finl;
  Cerr << "seuil_convergence_implicite option should be NOT be defined in your time scheme" << finl;
  Cerr << "during calculation of a single problem." << finl;
  Cerr << "Remove the option." << finl;
  exit();
  } */
  return param_impl;
}

void Simpler_Base::assembler_matrice_pression_implicite(Equation_base& eqn_NS,const Matrice_Morse& matrice,Matrice& matrice_en_pression_2)
{
  Navier_Stokes_std& eqnNS = ref_cast(Navier_Stokes_std,eqn_NS);
  const IntVect& tab1 = matrice.get_tab1();
  const DoubleVect& coeff = matrice.get_coeff();
  const DoubleTab& present = eqn_NS.inconnue().valeurs();
  int nb_comp = 1;
  int deux_entrees = 0;

  if (present.nb_dim()==2)
    {
      deux_entrees = 1;
      nb_comp = present.dimension(1);
    }

  const Zone_VF& la_zone = ref_cast(Zone_VF,eqnNS.zone_dis().valeur());
  if (deux_entrees==0)
    {
      DoubleVect vol2 = la_zone.volumes_entrelaces();
      int ns = vol2.size();
      for (int i=0; i<ns; i++)
        {
          int idiag = tab1[i*nb_comp]-1;
          double ref = coeff[idiag];
          for (int c=1; c<nb_comp; c++)
            if (!est_egal(ref,coeff[tab1[i*nb_comp+c]-1]))
              {
                Cerr<<"Pb dans Piso sur la diagonale case"<<i<<" comp "<< c<<" ref "<<ref<<" valeurs "<<coeff[tab1[i*nb_comp+c]-1]<<finl;
                exit();
              }
          vol2[i] = coeff[idiag];
        }
      vol2.echange_espace_virtuel();
      eqnNS.assembleur_pression().valeur().assembler_mat(matrice_en_pression_2,vol2,1,1);
    }
  else
    {
      DoubleTab vol2(present);
      int ns = vol2.dimension(0);
      for (int i=0; i<ns; i++)
        for (int c=0; c<nb_comp; c++)
          vol2(i,c) = matrice(i*nb_comp+c,i*nb_comp+c);

      vol2.echange_espace_virtuel();
      eqnNS.assembleur_pression().valeur().assembler_mat(matrice_en_pression_2,vol2,1,1);
    }

  SolveurSys& solveur_pression_ = eqnNS.solveur_pression();
  solveur_pression_.valeur().reinit();
}
