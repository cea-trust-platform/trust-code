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
// File:        Navier_Stokes_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/39
//
//////////////////////////////////////////////////////////////////////////////

#include <Navier_Stokes_QC.h>
#include <Discret_Thyd.h>
#include <Fluide_Quasi_Compressible.h>
#include <Operateur_Diff_base.h>
#include <Schema_Temps_base.h>
#include <Schema_Temps.h>
#include <Param.h>

Implemente_instanciable(Navier_Stokes_QC,"Navier_Stokes_QC",Navier_Stokes_std);

// Description:
//    Simple appel a: Equation_base::printOn(Sortie&)
//    Ecrit l'equation sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet la methode ne modifie pas l'objet
Sortie& Navier_Stokes_QC::printOn(Sortie& is) const
{
  return Equation_base::printOn(is);
}


// Description:
//    Appel Equation_base::readOn(Entree& is)
//    En sortie verifie que l'on a bien lu:
//        - le terme diffusif,
//        - le terme convectif,
//        - le solveur en pression
// Precondition: l'equation doit avoir un milieu fluide associe
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: terme diffusif non specifie dans jeu de donnees, specifier
//            un type negligeable pour l'operateur si il est a negliger
// Exception: terme convectif non specifie dans jeu de donnees, specifier
//            un type negligeable pour l'operateur si il est a negliger
// Exception: solveur pression non defini dans jeu de donnees
// Effets de bord:
// Postcondition:
Entree& Navier_Stokes_QC::readOn(Entree& is)
{
  Navier_Stokes_std::readOn(is);
  assert(le_fluide.non_nul());
  if (!sub_type(Fluide_Quasi_Compressible,le_fluide.valeur()))
    {
      Cerr<<"ERROR : the Navier_Stokes_QC equation can be associated only to a quasi compressible fluid."<<finl;
      exit();
    }
  divergence.set_description((Nom)"Mass flow rate=Integral(rho*u*ndS) [kg.s-1]");
  return is;
}

const Champ_Don& Navier_Stokes_QC::diffusivite_pour_transport()
{
  return le_fluide->viscosite_dynamique();
}

const Champ_base& Navier_Stokes_QC::diffusivite_pour_pas_de_temps()
{
  return le_fluide->viscosite_cinematique();
}

const Champ_base& Navier_Stokes_QC::vitesse_pour_transport()
{
  return la_vitesse;
}

// Description:
//    Complete l'equation base,
//    associe la pression a l'equation,
//    complete la divergence, le gradient et le solveur pression.
//    Ajout de 2 termes sources: l'un representant la force centrifuge
//    dans le cas axi-symetrique,l'autre intervenant dans la resolution
//    en 2D axisymetrique
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
void Navier_Stokes_QC::completer()
{
  if (le_fluide->a_gravite())
    {
      Cerr<<"Gravity="<<le_fluide->gravite()<<finl;
      //l'equation de NS en quasi compressible peut contenir un terme source de gravite
      if (le_fluide->beta_t().non_nul())    //pour debogage    A ENLEVER APRES
        {
          Cerr << "Beta_t !=0  -> Boussinesq is currently used :"<< finl;
        }
      else
        {
          Cerr<<"Creation of the buoyancy source term for the Navier_Stokes_QC equation :"<<finl;
          Source t;
          Source& so=les_sources.add(t);
          Nom type_so = "Source_Gravite_Quasi_Compressible_";
          Nom disc = discretisation().que_suis_je();
          if (disc=="VEFPreP1B")
            disc = "VEF";
          type_so += disc;
          so.typer_direct(type_so);
          so->associer_eqn(*this);
          Cerr<<so->que_suis_je()<<finl;
        }
    }

  Cerr<<" Navier_Stokes_std::completer_deb"<<finl;
  Navier_Stokes_std::completer();
  Cerr<<" Navier_Stokes_std::completer_fin"<<finl;
  Cerr<<"unknow field type"<<inconnue()->que_suis_je()<<finl;
  Cerr<<"unknow field name  "<<inconnue()->le_nom()<<finl;
  Cerr<<"equation type "<<inconnue()->equation().que_suis_je()<<finl;
}

int Navier_Stokes_QC::impr(Sortie& os) const
{
  Navier_Stokes_QC_impl::impr_impl((*this), os);
  return Navier_Stokes_std::impr(os);
}

void Navier_Stokes_QC::discretiser()
{
  Navier_Stokes_std::discretiser();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());

  dis.vitesse(schema_temps(), zone_dis(), rho_la_vitesse_);
  rho_la_vitesse_.nommer("rho_u");
  rho_la_vitesse_.valeur().nommer("rho_u");
}

const Champ_base& Navier_Stokes_QC::get_champ(const Motcle& nom) const
{

  if (nom=="rho_u")
    return rho_la_vitesse().valeur();
  try
    {
      return Navier_Stokes_std::get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }

  try

    {
      return milieu().get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }
  throw Champs_compris_erreur();

}


// Description:
//    Calcule la derivee en temps de l'inconnue vitesse,
//    i.e. l'acceleration dU/dt et la renvoie.
//    Appelle Equation_base::derivee_en_temps_inco(DoubleTab& )
//    Calcule egalement la pression.
// Precondition:
// Parametre: DoubleTab& vpoint
//    Signification: le tableau des valeurs de l'acceleration dU/dt
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs de l'acceleration (derivee de la vitesse)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:


DoubleTab& Navier_Stokes_QC::derivee_en_temps_inco(DoubleTab& vpoint)
{
  return Navier_Stokes_QC_impl::derivee_en_temps_inco((*this),vpoint,le_fluide.valeur(),matrice_pression_,assembleur_pression_,schema_temps().diffusion_implicite());
}

void Navier_Stokes_QC::assembler( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem)
{
  return assembler_impl((*this),mat_morse,present,secmem);
}
void Navier_Stokes_QC::assembler_avec_inertie( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem)
{
  return assembler_avec_inertie_impl((*this),mat_morse,present,secmem);
}

// Description:
//     cf Equation_base::preparer_calcul()
//     Assemblage du solveur pression et
//     initialisation de la pression.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Navier_Stokes_QC::preparer_calcul()
{
  int ASQC=0;
  if (ASQC==1)
    {
      Equation_base::preparer_calcul();
      //solveur_pression->assembler_QC(le_fluide->masse_volumique().valeurs());

      //Corrections pour eviter l assemblage de la matrice de pression
      //On fait appel a assembler au lieu de assembler_QC

      assembleur_pression_.assembler_QC(le_fluide->masse_volumique().valeurs(), matrice_pression_);

      //assembleur_pression_.assembler(matrice_pression_);

      if (le_traitement_particulier.non_nul())
        le_traitement_particulier.preparer_calcul_particulier();

      la_pression.valeurs()=0.;
      la_pression.changer_temps(schema_temps().temps_courant());
      Cerr << "Projection of initial and boundaries conditions" << finl;
      projeter();
      return 1;
    }
  else
    {
      return Navier_Stokes_std::preparer_calcul();
    }
}

bool Navier_Stokes_QC::initTimeStep(double dt)
{
  DoubleTab& tab_vitesse=inconnue().valeurs();
  Fluide_Quasi_Compressible& fluide_QC=ref_cast(Fluide_Quasi_Compressible,le_fluide.valeur());

  const DoubleTab& tab_rho = fluide_QC.rho_discvit();

  DoubleTab& rhovitesse = rho_la_vitesse_.valeurs();
  rho_vitesse_impl(tab_rho,tab_vitesse,rhovitesse);

  return  Navier_Stokes_std::initTimeStep(dt);
}


