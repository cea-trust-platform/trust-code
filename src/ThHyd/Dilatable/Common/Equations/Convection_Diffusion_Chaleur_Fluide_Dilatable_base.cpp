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
// File:        Convection_Diffusion_Chaleur_Fluide_Dilatable_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/Equations
// Version:     /main/52
//
//////////////////////////////////////////////////////////////////////////////

#include <Convection_Diffusion_Chaleur_Fluide_Dilatable_base.h>
#include <EcritureLectureSpecial.h>
#include <Fluide_Dilatable_base.h>
#include <Neumann_sortie_libre.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Discret_Thyd.h>
#include <Domaine.h>
#include <Avanc.h>

Implemente_base(Convection_Diffusion_Chaleur_Fluide_Dilatable_base,"Convection_Diffusion_Chaleur_Fluide_Dilatable_base",Convection_Diffusion_Fluide_Dilatable_base);

Sortie& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::printOn(Sortie& is) const
{
  return Convection_Diffusion_Fluide_Dilatable_base::printOn(is);
}

Entree& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::readOn(Entree& is)
{
  Convection_Diffusion_Fluide_Dilatable_base::readOn(is);
  terme_convectif.set_fichier("Convection_chaleur");
  terme_convectif.set_description((Nom)"Convective heat transfer rate=Integral(-rho*cp*T*u*ndS) [W] if SI units used");
  terme_diffusif.set_fichier("Diffusion_chaleur");
  terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) [W] if SI units used");
  //On modifie le nom ici pour que le champ puisse etre reconnu si une sonde d enthalpie est demandee
  if (le_fluide->type_fluide()=="Gaz_Reel") l_inco_ch->nommer("enthalpie");
  return is;
}

const Champ_base& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::diffusivite_pour_pas_de_temps() const
{
  return milieu().diffusivite();
}

const Champ_base& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::vitesse_pour_transport() const
{
  const Probleme_base& pb = probleme();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,pb.equation(0));
  const Champ_Inc& vitessetransportante = eqn_hydr.rho_la_vitesse(); // rho * u
  return vitessetransportante;
}

void Convection_Diffusion_Chaleur_Fluide_Dilatable_base::discretiser()
{
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  Cerr << "Energy equation discretization " << finl;
  dis.temperature(schema_temps(), zone_dis(), l_inco_ch);
  champs_compris_.ajoute_champ(l_inco_ch);
  Equation_base::discretiser();
  Cerr << "Convection_Diffusion_Chaleur_Fluide_Dilatable_base::discretiser() ok" << finl;
}

DoubleTab& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::derivee_en_temps_inco(DoubleTab& derivee)
{
  derivee=0;
  return derivee_en_temps_inco_sans_solveur_masse_impl(*this,derivee,true /* explicit */);
}

void Convection_Diffusion_Chaleur_Fluide_Dilatable_base::assembler( Matrice_Morse& matrice,const DoubleTab& inco, DoubleTab& resu)
{
  Convection_Diffusion_Fluide_Dilatable_Proto::assembler_impl(*this,matrice,inco,resu);
}

int Convection_Diffusion_Chaleur_Fluide_Dilatable_base::sauvegarder(Sortie& os) const
{
  int bytes=0;
  bytes += Equation_base::sauvegarder(os);
  // en mode ecriture special seul le maitre ecrit
  int a_faire,special;
  EcritureLectureSpecial::is_ecriture_special(special,a_faire);

  if (a_faire)
    {
      Nom ident_Pth("pression_thermo");
      ident_Pth += probleme().domaine().le_nom();
      double temps = inconnue().temps();
      ident_Pth += Nom(temps,"%e");
      os << ident_Pth<<finl;
      os << "constante"<<finl;
      os << le_fluide->pression_th();
      os << flush ;
      Cerr << "Saving thermodynamic pressure at time : " <<  Nom(temps,"%e") << finl;
    }
  return bytes;
}

// Description:
//     Effectue une reprise a partir d'un flot d'entree.
//     Appelle Equation_base::reprendre()
//     et reprend l'inconnue de la chaleur et la pression thermodynamique
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception: la reprise a echoue, identificateur de la pression non trouve
// Effets de bord:
// Postcondition:
int Convection_Diffusion_Chaleur_Fluide_Dilatable_base::reprendre(Entree& is)
{
  if (le_fluide->type_fluide() != "Gaz_Parfait") l_inco_ch->nommer("enthalpie");
  Equation_base::reprendre(is);
  double temps = schema_temps().temps_courant();
  Nom ident_Pth("pression_thermo");
  ident_Pth += probleme().domaine().le_nom();
  ident_Pth += Nom(temps,probleme().reprise_format_temps());
  avancer_fichier(is, ident_Pth);
  double pth;
  is>>pth;
  le_fluide->set_pression_th(pth);
  return 1;
}

int Convection_Diffusion_Chaleur_Fluide_Dilatable_base::preparer_calcul()
{
  Convection_Diffusion_Fluide_Dilatable_base::preparer_calcul();
  return 1;
}

// Description:
// remplissage de la zone cl modifiee avec 1 partout au bord...
int Convection_Diffusion_Chaleur_Fluide_Dilatable_base::remplir_cl_modifiee()
{
  zcl_modif_=(zone_Cl_dis());
  Conds_lim& condlims=zcl_modif_.valeur().les_conditions_limites();
  int nb=condlims.size();
  // pour chaque condlim on recupere le champ_front et on met 1
  // meme si la cond lim est un flux (dans ce cas la convection restera nullle.)
  for (int i=0; i<nb; i++)
    {
      DoubleTab& T=condlims[i].valeur().champ_front().valeurs();
      T=1.;
      if (sub_type(Neumann_sortie_libre,condlims[i].valeur()))
        ref_cast(Neumann_sortie_libre,condlims[i].valeur()).tab_ext()=1;
    }
  zcl_modif_.les_conditions_limites().set_modifier_val_imp(0);
  return 1;
}

// Description:
//    Renvoie le nom du domaine d'application de l'equation.
//    Ici "Thermique".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Motcle&
//    Signification: le nom du domaine d'application de l'equation
//    Contraintes: toujours egal a "Thermique"
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Motcle& Convection_Diffusion_Chaleur_Fluide_Dilatable_base::domaine_application() const
{
  static Motcle domaine = "Thermique_H";
  if (le_fluide->type_fluide()=="Gaz_Parfait") domaine = "Thermique";
  return domaine;
}
