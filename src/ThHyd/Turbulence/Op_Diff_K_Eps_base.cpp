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
// File:        Op_Diff_K_Eps_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_K_Eps_base.h>
#include <Modele_turbulence_hyd_K_Eps.h>
#include <Discretisation_base.h>
#include <stat_counters.h>
#include <Champ_Uniforme.h>
#include <Champ_Don.h>

Implemente_base(Op_Diff_K_Eps_base,"Op_Diff_K_Eps_base",Operateur_base);
Implemente_instanciable(Op_Diff_K_Eps_negligeable,"Op_Diff_K_Eps_negligeable",Op_Diff_K_Eps_base);
Implemente_deriv(Op_Diff_K_Eps_base);
Implemente_instanciable(Op_Diff_K_Eps,"Op_Diff_K_Eps",DERIV(Op_Diff_K_Eps_base));


// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie& s
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Op_Diff_K_Eps_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie& s
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Op_Diff_K_Eps_negligeable::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie& s
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Op_Diff_K_Eps::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Op_Diff_K_Eps_base::readOn(Entree& s )
{
  return s ;
}

// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Op_Diff_K_Eps_negligeable::readOn(Entree& s )
{
  return s ;
}

// Description:
//    Simple appel a Operateur::lire(Entree&)
//    Lit l'operateur a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Op_Diff_K_Eps::readOn(Entree& s )
{
  Operateur::lire(s);
  return s;
}


// // Description:
// //    Associe la diffusivite a l'operateur.
// // Precondition:
// // Parametre: Champ_Don& ch
// //    Signification: le champ representant la diffusivite
// //    Valeurs par defaut:
// //    Contraintes: reference constante
// //    Acces: entree
// // Retour:
// //    Signification:
// //    Contraintes:
// // Exception:
// // Effets de bord:
// // Postcondition:
// void Op_Diff_K_Eps_negligeable::associer_diffusivite(const Champ_Don& ch)
// {
//   la_diffusivite=ch;
// }


// // Description:
// //    Renvoie le champ representant la diffusivite.
// // Precondition:
// // Parametre:
// //    Signification:
// //    Valeurs par defaut:
// //    Contraintes:
// //    Acces:
// // Retour: Champ_Don_base&
// //    Signification: le champ representant la diffusivite
// //    Contraintes:
// // Exception:
// // Effets de bord:
// // Postcondition: la methode ne modifie pas l'objet
// const Champ_Don_base& Op_Diff_K_Eps_negligeable::diffusivite() const
// {
//   return la_diffusivite->valeur();
// }



// Description:
//    Type l'operateur
//    s'associe a son equation
//    Associe la diffusivite turbulente a l'operateur base.
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
// Postcondition: l'operateur est type
void Op_Diff_K_Eps::typer()
{
  if (typ=="negligeable")
    {
      DERIV(Op_Diff_K_Eps_base)::typer("Op_Diff_K_Eps_negligeable");
      valeur().associer_diffusivite_turbulente();
    }
  else
    {
      Nom nom_type="Op_Diff_K_Eps_";
      Nom discr = equation().discretisation().que_suis_je();

      // les operateurs de diffusion sont communs aux discretisations VEF et VEFP1B
      if(discr=="VEFPreP1B") discr="VEF";
      //Cerr <<" >>>>>>>>>>>>>>>>>>>>>>>>>>>" <<  equation().que_suis_je() << finl;
      Transport_K_Eps_base& leq = ref_cast(Transport_K_Eps_base,equation());
      Nom qc = leq.modele_turbulence().equation().que_suis_je();
      Cerr << ">>>>>>> Nom eq = " << qc << finl;
      if (qc=="Navier_Stokes_QC" || qc == "Navier_Stokes_Turbulent_QC")
        {
          nom_type+="QC_";
        }

      if (!sub_type(Champ_Uniforme,diffusivite()))
        {
          if (discr!="VEF")
            {
              nom_type+="var_";
            }
        }

      nom_type +=discr;
      Cerr << " >>>>>> Operateur = " << nom_type << finl;
      if (discr!="VDF_Hyper")
        {
          nom_type += "_";
          Nom type_inco=equation().inconnue()->que_suis_je();
          nom_type+=(type_inco.suffix("Champ_"));
          if (axi)
            nom_type += "_Axi";
        }
      DERIV(Op_Diff_K_Eps_base)::typer(nom_type);
      valeur().associer_eqn(equation());
      valeur().associer_diffusivite_turbulente();
      valeur().associer_diffusivite(diffusivite());
      Cerr << valeur().que_suis_je() << finl;
    }
}

// Description:
//    Appel a l'objet sous-jacent.
//    Ajoute la contribution de l'operateur au tableau
//    passe en parametre
// Precondition:
// Parametre: DoubleTab& inconnue
//    Signification: tableau contenant les donnees sur lesquelles on applique
//                   l'operateur.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& resu
//    Signification: tableau auquel on ajoute la contribution de l'operateur
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le tableau contenant le resultat
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Op_Diff_K_Eps::ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  statistiques().begin_count(diffusion_counter_);
  DoubleTab& tmp = valeur().ajouter(inconnue, resu);
  statistiques().end_count(diffusion_counter_);
  return tmp;
}


// Description:
//    Appel a l'objet sous-jacent.
//    Initialise le tableau passe en parametre avec la contribution
//    de l'operateur.
// Precondition:
// Parametre: DoubleTab& inconnue
//    Signification: tableau contenant les donnees sur lesquelles on applique
//                   l'operateur.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& resu
//    Signification: tableau dans lequel stocke la contribution de l'operateur
//    Valeurs par defaut:
//    Contraintes: l'ancien contenu est ecrase
//    Acces: sortie
// Retour: DoubleTab&
//    Signification: le tableau contenant le resultat
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Op_Diff_K_Eps::calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  statistiques().begin_count(diffusion_counter_);
  DoubleTab& tmp = valeur().calculer(inconnue, resu);
  statistiques().end_count(diffusion_counter_);
  return tmp;
}


