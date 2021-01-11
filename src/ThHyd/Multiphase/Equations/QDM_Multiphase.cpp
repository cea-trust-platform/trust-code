/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        QDM_Multiphase.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Equations
// Version:     /main/39
//
//////////////////////////////////////////////////////////////////////////////

#include <QDM_Multiphase.h>
#include <Pb_Multiphase.h>
#include <Discret_Thyd.h>
#include <Fluide_base.h>
#include <Operateur_Diff_base.h>
#include <Schema_Temps_base.h>
#include <Schema_Temps.h>
#include <Param.h>
#include <EChaine.h>
#include <ConstDoubleTab_parts.h>

Implemente_instanciable(QDM_Multiphase,"QDM_Multiphase",Navier_Stokes_std);

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
Sortie& QDM_Multiphase::printOn(Sortie& is) const
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

Entree& QDM_Multiphase::readOn(Entree& is)
{
  evanescence.associer_eqn(*this);
  Navier_Stokes_std::readOn(is);
  assert(le_fluide.non_nul());
  if (!sub_type(Fluide_base,le_fluide.valeur()))
    {
      Cerr<<"ERROR : the QDM_Multiphase equation can be associated only to a fluid."<<finl;
      exit();
    }
  divergence.set_description((Nom)"Mass flow rate=Integral(rho*u*ndS) [kg.s-1]");
  terme_convectif.valeur().set_incompressible(0);

  if (!evanescence.non_nul())
    {
      EChaine eva("{ homogene { alpha_res 1e-6 } }");
      eva >> evanescence;
    }

  /* champs de vitesse par phase pour le postpro */
  const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, probleme());
  noms_vit_phases_.dimensionner(pb.nb_phases()), vit_phases_.resize(pb.nb_phases());
  for (int i = 0; i < pb.nb_phases(); i++)
    champs_compris_.ajoute_nom_compris(noms_vit_phases_[i] = Nom("vitesse_") + pb.nom_phase(i));

  return is;
}

void QDM_Multiphase::set_param(Param& param)
{
  Navier_Stokes_std::set_param(param);
  param.ajouter_non_std("evanescence|vanishing",(this));
}

int QDM_Multiphase::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="evanescence") is >> evanescence;
  else return Navier_Stokes_std::lire_motcle_non_standard(mot, is);
  return 1;
}

/* l'evanescence passe en dernier */
void QDM_Multiphase::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  Navier_Stokes_std::dimensionner_blocs(matrices, semi_impl);
  evanescence.valeur().dimensionner_blocs(matrices, semi_impl);
}

void QDM_Multiphase::assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  Navier_Stokes_std::assembler_blocs_avec_inertie(matrices, secmem, semi_impl);
  evanescence.valeur().ajouter_blocs(matrices, secmem, semi_impl);
}

void QDM_Multiphase::mettre_a_jour(double temps)
{
  la_pression.mettre_a_jour(temps);
  Equation_base::mettre_a_jour(temps);  //on saute celui de Navier_Stokes_std

  int i, j, n, N = ref_cast(Pb_Multiphase, probleme()).nb_phases(), d, D = dimension;
  for (n = 0; n < N; n++) if (vit_phases_[n].non_nul())
      {
        vit_phases_[n].mettre_a_jour(temps);
        DoubleTab_parts psrc(inconnue().valeurs()), pdst(vit_phases_[n].valeurs());
        for (i = 0; i < psrc.size(); i++)
          {
            DoubleTab& src = psrc[i], &dst = pdst[i];
            if (src.line_size() == N) /* une colonne par composante */
              for (j = 0; j < src.dimension_tot(0); j++) dst(j) = src(j, n);
            else if (src.line_size() == N * D) /* stockage N * d + n */
              for (j = 0; j < src.dimension_tot(0); j++) for (d = 0; d < D; d++)
                  dst(j, d) = src(D * j + d, n);
            else abort(); //on ne connait pas
          }
      }
}

bool QDM_Multiphase::initTimeStep(double dt)
{
  Schema_Temps_base& sch=schema_temps();
  for (int i=1; i<=sch.nb_valeurs_futures(); i++)
    {
      // Mise a jour du temps dans la pression
      pression()->changer_temps_futur(sch.temps_futur(i),i);
      pression()->futur(i)=pression()->valeurs();
    }
  return Equation_base::initTimeStep(dt);
}

void QDM_Multiphase::discretiser_vitesse()
{
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  dis.vitesse(schema_temps(), zone_dis(), la_vitesse, ref_cast(Pb_Multiphase, probleme()).nb_phases());
}

const Champ_Don& QDM_Multiphase::diffusivite_pour_transport()
{
  return le_fluide->viscosite_dynamique();
}

const Champ_base& QDM_Multiphase::diffusivite_pour_pas_de_temps()
{
  return le_fluide->viscosite_cinematique();
}

const Champ_base& QDM_Multiphase::vitesse_pour_transport()
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
void QDM_Multiphase::completer()
{
  Cerr<<" Navier_Stokes_std::completer_deb"<<finl;
  Navier_Stokes_std::completer();
  Cerr<<" Navier_Stokes_std::completer_fin"<<finl;
  Cerr<<"unknow field type"<<inconnue()->que_suis_je()<<finl;
  Cerr<<"unknow field name  "<<inconnue()->le_nom()<<finl;
  Cerr<<"equation type "<<inconnue()->equation().que_suis_je()<<finl;

  /* liste des choses qui doivent etre compatibles avec le multiphase */
  std::vector<const MorEqn*> morceaux = { &solveur_masse.valeur(), &les_sources, &terme_convectif.valeur(), &terme_diffusif.valeur(), &gradient.valeur() };
  for (auto mor : morceaux) mor->check_multiphase_compatibility();
}

void QDM_Multiphase::creer_champ(const Motcle& motlu)
{
  Navier_Stokes_std::creer_champ(motlu);
  int i = noms_vit_phases_.rang(motlu);
  if (i < 0 || vit_phases_[i].non_nul()) return;
  discretisation().discretiser_champ("vitesse",zone_dis(), noms_vit_phases_[i], "m/s",dimension, 1, 0, vit_phases_[i]);
  champs_compris_.ajoute_champ(vit_phases_[i]);
}

Entree& QDM_Multiphase::lire_cond_init(Entree& is)
{
  Cerr << "Reading of initial conditions\n";
  Nom nom;
  is >> nom;
  if (nom != "{") Cerr << que_suis_je() << ": expected { instead of " << nom << finl, Process::exit();
  int vit_lu = 0, press_lu = 0;
  for (is >> nom; nom != "}"; is >> nom)
    if (nom == "vitesse" || nom == "velocity")
      {
        Champ_Don src;
        is >> src;
        verifie_ch_init_nb_comp(la_vitesse, src.nb_comp());
        la_vitesse->affecter(src), vit_lu = 1;
      }
    else if (nom == "pression" || nom == "pressure")
      {
        Champ_Don src;
        is >> src, verifie_ch_init_nb_comp(la_pression, src.nb_comp());
        la_pression->affecter(src), press_lu = 1;
      }
    else Cerr << que_suis_je() << ": expected vitesse|velocity|pression|pressure instead of " << nom << finl, Process::exit();

  if (!vit_lu)   Cerr << que_suis_je() << ": velocity initial condition not found." << finl, Process::exit();
  if (!press_lu) Cerr << que_suis_je() << ": pressure initial condition not found." << finl, Process::exit();

  return is;
}

int QDM_Multiphase::preparer_calcul()
{
  return Equation_base::preparer_calcul(); //pour eviter Navier_Stokes_std::preparer_calcul() !
}
