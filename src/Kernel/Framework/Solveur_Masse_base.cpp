/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Solveur_Masse_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Solveur_Masse_base.h>
#include <Equation_base.h>
#include <Matrice_Morse.h>
#include <DoubleTrav.h>
#include <Debog.h>
#include <ConstDoubleTab_parts.h>

Implemente_base_sans_constructeur(Solveur_Masse_base,"Solveur_Masse_base",Objet_U);

Solveur_Masse_base::Solveur_Masse_base() : has_coefficient_temporel_(0),penalisation_(0) {}

// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Imprime le solveur de masse sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Solveur_Masse_base::printOn(Sortie& os) const
{
  return os;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Lecture d'un solveur de masse sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot d'entree est modifie
// Postcondition:
Entree& Solveur_Masse_base::readOn(Entree& is)
{
  return is;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Mise a jour en temps du solveur de masse.
// Precondition:
// Parametre: double
//    Signification: le pas de temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Solveur_Masse_base::mettre_a_jour(double )
{
}


// Description:
//    NE FAIT RIEN
//    Eventuellement a surcharger dans les classes derivees
//    si la matrice de masse necessite un assemblage.
//    Assemble le solveur de masse (en general la matrice de masse)
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
// Postcondition: la matrice de masse est assemblee
void Solveur_Masse_base::assembler()
{
  ;
}
// Description: permet de choisir le nom du coefficient temporelle
// que l'on veut utiliser pour appliquer
// verifie que le champ exsite bien
void Solveur_Masse_base::set_name_of_coefficient_temporel(const Nom& name)
{
  name_of_coefficient_temporel_=name;
  has_coefficient_temporel_=1;
  if (name=="no_coeff")
    {
      has_coefficient_temporel_=0;
      return;
    }
  REF(Champ_base) ref_coeff;
  try
    {
      ref_coeff = equation().get_champ(name_of_coefficient_temporel_);
    }
  catch (Champs_compris_erreur)
    {
      Cerr<< name_of_coefficient_temporel_<< " not understood by "<< equation().que_suis_je()<<finl;
      Process::exit();
    }

}

// Description: renvoie appliquer_impl(x/coeffient_temporelle)
// si on a un coefficient temporelle
// sinon renvoie appliquer_impl(x)
// Return M-1.x
DoubleTab& Solveur_Masse_base::appliquer(DoubleTab& x) const
{
  if (has_coefficient_temporel_)
    {
      REF(Champ_base) ref_coeff;
      ref_coeff = equation().get_champ(name_of_coefficient_temporel_);

      DoubleTab values;
      if (sub_type(Champ_Inc_base,ref_coeff.valeur()))
        {
          const Champ_Inc_base& coeff = ref_cast(Champ_Inc_base,ref_coeff.valeur());
          values.ref(coeff.valeurs());

        }
      else if (sub_type(Champ_Fonc_base,ref_coeff.valeur()))
        {
          const Champ_Fonc_base& coeff = ref_cast(Champ_Fonc_base,ref_coeff.valeur());
          values.ref(coeff.valeurs());

        }
      else if (sub_type(Champ_Don_base,ref_coeff.valeur()))
        {
          DoubleTab nodes;
          equation().inconnue().valeur().remplir_coord_noeuds(nodes);
          ref_coeff.valeur().valeur_aux(nodes,values);
        }

      Debog::verifier("Solveur_Masse_base::appliquer coeffs",values);
      //tab_divide_any_shape(x, values, VECT_REAL_ITEMS);
      DoubleTab_parts values_parts(values);
      tab_divide_any_shape(x, values_parts[0], VECT_REAL_ITEMS);
    }

  return appliquer_impl(x); // M-1.x
}

// Add M/dt into matrix
Matrice_Base& Solveur_Masse_base::ajouter_masse(double dt, Matrice_Base& matrice, int penalisation) const
{
  Matrice_Morse& matmo=ref_cast(Matrice_Morse, matrice);
  DoubleTrav diag(equation().inconnue().valeurs());
  if (has_interface_blocs())
    {
      ajouter_blocs({{ equation().inconnue().le_nom().getString(), &matmo }}, diag, dt, {}, 0); //on tente ajouter_blocs()
      return matrice;
    }
  const int sz = equation().inconnue().valeurs().dimension_tot(0) * diag.line_size();
  diag=1.;
  appliquer(diag); // M-1
  int prems=0;
  if(penalisation)
    {
      if (penalisation_==0)
        {
          double penal=0;
          for(int i=0; i<sz; i++)
            penal=std::max(penal, matmo(i,i));
          penal=mp_max(penal);
          penalisation_=(mp_max_vect(diag)/dt + penal)*1.e3;
          prems=1;
        }
    }
  else
    penalisation_=0;

  for(int i=0; i<sz; i++)
    {
      if (diag.addr()[i]==0)
        {
          if(prems)
            {
              matmo(i,i)+=penalisation_;
              prems=0;
            }
          matmo(i,i)+=penalisation_/dt;
        }
      else
        matmo(i,i)+=1./(diag.addr()[i]*dt); // M/dt
    }
  return matrice;
}

// Add M*y/dt to x
DoubleTab& Solveur_Masse_base::ajouter_masse(double dt, DoubleTab& x, const DoubleTab& y, int penalisation) const
{
  if (has_interface_blocs())
    {
      ajouter_blocs({}, x, dt, {}, 0); //on tente ajouter_blocs()
      return x;
    }
  int sz=y.size();
  DoubleTab diag;
  diag.copy(equation().inconnue().valeurs(), Array_base::NOCOPY_NOINIT);
  diag=1.;
  appliquer(diag); // M-1
  if (penalisation)
    {
      if (penalisation_==0)
        penalisation_ = mp_max_vect(diag)*1.e3;
    }
  else
    penalisation_=1;

  for(int i=0; i<sz; i++)
    {
      if (diag.addr()[i]==0)
        x.addr()[i]=penalisation_*y.addr()[i];
      else
        x.addr()[i]+=1./(diag.addr()[i]*dt)*y.addr()[i];
    }
  //x.echange_espace_virtuel();Debog::verifier("Solveur_Masse::ajouter_masse",x);
  return x;
}

Matrice_Base& Solveur_Masse_base::ajouter_masse_dt_local(DoubleVect& dt_locaux, Matrice_Base& matrice, int penalisation) const
{
  Matrice_Morse& matmo=ref_cast(Matrice_Morse, matrice);
  int sz=matmo.nb_lignes();;
  DoubleTrav diag(equation().inconnue().valeurs());
  diag=1.;
  appliquer(diag); // M-1
  int prems=0;
  if(penalisation)
    {
      if (penalisation_==0)
        {
          double penal=0;
          for(int i=0; i<sz; i++)
            penal=std::max(penal, matmo(i,i));
          penal=mp_max(penal);
          penalisation_=(mp_max_vect(diag)/dt_locaux.local_max_vect() + penal)*1.e3;
          prems=1;
        }
    }
  else
    penalisation_=0;

  for(int i=0; i<sz; i++)
    {
      if (diag.addr()[i]==0)
        {
          if(prems)
            {
              matmo(i,i)+=penalisation_;
              prems=0;
            }
          matmo(i,i)+=penalisation_/dt_locaux[i];
        }
      else
        matmo(i,i)+=1./(diag.addr()[i]*dt_locaux[i]);
    }
  return matrice;
}

DoubleTab& Solveur_Masse_base::ajouter_masse_dt_local(DoubleVect& dt_locaux, DoubleTab& x, const DoubleTab& y, int penalisation) const
{
  int sz=y.size();
  DoubleTrav diag;
  diag.copy(equation().inconnue().valeurs(), Array_base::NOCOPY_NOINIT);
  diag=1.;
  appliquer(diag);
  if (penalisation)
    {
      if (penalisation_==0)
        penalisation_ = mp_max_vect(diag)*1.e3;
    }
  else
    penalisation_=1;
  for(int i=0; i<sz; i++)
    {
      if (diag.addr()[i]==0)
        x.addr()[i]=penalisation_*y.addr()[i];
      else
        x.addr()[i]+=1./(diag.addr()[i]*dt_locaux[i])*y.addr()[i];

    }
  //x.echange_espace_virtuel();

  //test

  return x;
}

void Solveur_Masse_base::get_masse_dt_local(DoubleVect& m_dt_locaux, DoubleVect& dt_locaux, int penalisation)
{
  int sz=dt_locaux.size();
  DoubleTab diag;
  diag.copy(equation().inconnue().valeurs(), Array_base::NOCOPY_NOINIT);
  diag=1.;
  appliquer(diag);
  if (penalisation)
    {
      if (penalisation_==0)
        penalisation_ = mp_max_vect(diag)*1.e3;
    }
  else
    penalisation_=1;
  for(int i=0; i<sz; i++)
    {
      if (diag.addr()[i]==0)
        m_dt_locaux[i]=0.;
      else
        m_dt_locaux[i]=(1./diag.addr()[i])*dt_locaux[i];

    }
  m_dt_locaux.echange_espace_virtuel();

  //Debog::verifier("Solveur_Masse::ajouter_masse",x);
}


void Solveur_Masse_base::get_masse_divide_by_local_dt(DoubleVect& m_dt_locaux, DoubleVect& dt_locaux, int penalisation)
{
  int sz=dt_locaux.size();
  DoubleTab diag;
  diag.copy(equation().inconnue().valeurs(), Array_base::NOCOPY_NOINIT);
  diag=1.;
  appliquer(diag);
  if (penalisation)
    {
      if (penalisation_==0)
        penalisation_ = mp_max_vect(diag)*1.e3;
    }
  else
    penalisation_=1;
  for(int i=0; i<sz; i++)
    {
      if (diag.addr()[i]==0)
        m_dt_locaux[i]=0.;
      else
        m_dt_locaux[i]=(1./diag.addr()[i])/dt_locaux[i];

    }
  m_dt_locaux.echange_espace_virtuel();

  //Debog::verifier("Solveur_Masse::ajouter_masse",x);

}

DoubleTab& Solveur_Masse_base::corriger_solution(DoubleTab& x, const DoubleTab& y, int incr) const
{
  int sz = y.dimension_tot(0) * y.line_size();
  DoubleTrav diag(equation().inconnue().valeurs());
  diag=1.;
  appliquer(diag); // M-1
  for(int i=0; i<sz; i++)
    {
      if (diag.addr()[i]<1.e-12)
        {
          x.addr()[i] = y.addr()[i];
        }
    }
  return x;
}

// Ajout d'une methode dimensionner()
// qui dimensionne la matrice a la diagonale quand
// tous les operateurs sont negligeables
// Le code est celui utilise dans la version Noyau de Equation_base::dimensionner_matrice()
void Solveur_Masse_base::dimensionner(Matrice_Morse& matrix) const
{
  if (has_interface_blocs())
    {
      dimensionner_blocs({{ equation().inconnue().le_nom().getString(), &matrix }}, {}); //on tente dimensionner_blocs
      return;
    }
  const DoubleTab& champ_inconnue = equation().inconnue().valeurs();
  int size = champ_inconnue.dimension_tot(0) * champ_inconnue.line_size();

  IntTab indice(size, 2);
  for(int i=0; i<size; ++i)
    {
      indice(i,0) = indice(i,1) = i;
    }
  matrix.dimensionner(indice);
}

void Solveur_Masse_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  Process::exit(que_suis_je() + " : dimensionner_blocs not coded!");
}

void Solveur_Masse_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const
{
  Process::exit(que_suis_je() + " : ajouter_blocs not coded!");
}

// Ajout d'une methode completer
// Ne fait rien par defaut
void Solveur_Masse_base::completer(void) { }

// Ajout d'une methode preparer_calcul
// Ne fait rien par defaut
void Solveur_Masse_base::preparer_calcul(void) { }

