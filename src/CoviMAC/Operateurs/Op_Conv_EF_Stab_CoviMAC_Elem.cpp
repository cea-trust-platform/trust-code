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
// File:        Op_Conv_EF_Stab_CoviMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_EF_Stab_CoviMAC_Elem.h>
#include <Probleme_base.h>
#include <Convection_Diffusion_std.h>
#include <Milieu_base.h>
#include <Schema_Temps_base.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_P0_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <Masse_CoviMAC_Elem.h>
#include <Pb_Multiphase.h>
#include <Discretisation_base.h>

#include <Param.h>
#include <cmath>
#include <vector>

Implemente_instanciable( Op_Conv_EF_Stab_CoviMAC_Elem, "Op_Conv_EF_Stab_CoviMAC_Elem_CoviMAC", Op_Conv_CoviMAC_base ) ;
Implemente_instanciable( Op_Conv_Amont_CoviMAC_Elem, "Op_Conv_Amont_CoviMAC_Elem_CoviMAC", Op_Conv_EF_Stab_CoviMAC_Elem ) ;
Implemente_instanciable( Op_Conv_Centre_CoviMAC_Elem, "Op_Conv_Centre_CoviMAC_Elem_CoviMAC", Op_Conv_EF_Stab_CoviMAC_Elem ) ;

Sortie& Op_Conv_EF_Stab_CoviMAC_Elem::printOn( Sortie& os ) const
{
  Op_Conv_CoviMAC_base::printOn( os );
  return os;
}

Sortie& Op_Conv_Amont_CoviMAC_Elem::printOn( Sortie& os ) const
{
  Op_Conv_CoviMAC_base::printOn( os );
  return os;
}

Sortie& Op_Conv_Centre_CoviMAC_Elem::printOn( Sortie& os ) const
{
  Op_Conv_CoviMAC_base::printOn( os );
  return os;
}

Entree& Op_Conv_EF_Stab_CoviMAC_Elem::readOn( Entree& is )
{
  Op_Conv_CoviMAC_base::readOn( is );
  if (que_suis_je() == "Op_Conv_EF_Stab_CoviMAC_Elem_CoviMAC") //on n'est pas dans Op_Conv_Amont/Centre
    {
      Param param(que_suis_je());
      param.ajouter("alpha", &alpha);            // XD_ADD_P double parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)
      param.lire_avec_accolades_depuis(is);
    }

  if (sub_type(Masse_Multiphase, equation())) //convection dans Masse_Multiphase -> champs de debit
    {
      const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, equation().probleme());
      noms_cc_phases_.dimensionner(pb.nb_phases()), cc_phases_.resize(pb.nb_phases());
      noms_vd_phases_.dimensionner(pb.nb_phases()), vd_phases_.resize(pb.nb_phases());
      for (int i = 0; i < pb.nb_phases(); i++)
        {
          champs_compris_.ajoute_nom_compris(noms_cc_phases_[i] = Nom("debit_") + pb.nom_phase(i));
          champs_compris_.ajoute_nom_compris(noms_vd_phases_[i] = Nom("vitesse_debitante_") + pb.nom_phase(i));
        }
    }

  return is;
}

Entree& Op_Conv_Amont_CoviMAC_Elem::readOn( Entree& is )
{
  alpha = 1;
  return Op_Conv_EF_Stab_CoviMAC_Elem::readOn( is );
}

Entree& Op_Conv_Centre_CoviMAC_Elem::readOn( Entree& is )
{

  alpha = 0;
  return Op_Conv_EF_Stab_CoviMAC_Elem::readOn( is );
}

void Op_Conv_EF_Stab_CoviMAC_Elem::preparer_calcul()
{
  Op_Conv_CoviMAC_base::preparer_calcul();

  /* au cas ou... */
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const Champ_P0_CoviMAC& ch = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur());
  ch.init_cl(), zone.init_equiv(), equation().init_champ_conserve();

  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 2)
    Cerr << "Op_Conv_EF_Stab_CoviMAC_Elem : largeur de joint insuffisante (minimum 2)!" << finl, Process::exit();
}

void Op_Conv_EF_Stab_CoviMAC_Elem::dimensionner_blocs(matrices_t mats, const tabs_t& semi_impl) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  int i, j, e, eb, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), n, N = equation().inconnue().valeurs().line_size(), d_scal = 0, d_vit = mats.count("vitesse");
  //devra-t-on calculer des derivees scalaires?
  const Champ_Inc_base& cc = equation().champ_conserve();
  if (!semi_impl.count(cc.le_nom().getString())) for (auto &&i_m : mats) if (cc.derivees().count(i_m.first)) d_scal = 1;

  IntTab s_scal(0, 2), s_vit(0, 2); //stencil des inconnues scalaires, de la vitesse
  s_scal.set_smart_resize(1), s_vit.set_smart_resize(1);

  for (f = 0; f < zone.nb_faces_tot(); f++) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem())
        {
          if (d_vit) for (n = 0; n < N; n++) s_vit.append_line(N * e + n, N * f + n); //derivee en la vitesse
          if (d_scal) for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++) for (n = 0; n < N; n++) s_scal.append_line(N * e + n, N * eb + n); //derivee en les incos scalaires amont/aval
        }

  Matrice_Morse m_vit, m_scal;
  if (d_vit)
    {
      tableau_trier_retirer_doublons(s_vit);
      Matrix_tools::allocate_morse_matrix(N * ne_tot, N * nf_tot, s_vit, m_vit);
      mats["vitesse"]->nb_colonnes() ? *mats["vitesse"] += m_vit : *mats["vitesse"] = m_vit;
    }

  if (d_scal)
    {
      tableau_trier_retirer_doublons(s_scal);
      Matrix_tools::allocate_morse_matrix(N * ne_tot, N * ne_tot, s_scal, m_scal);
      if (!semi_impl.count(cc.le_nom().getString())) for (auto &deriv : cc.derivees()) //une matrice par derivee de champ_conserve
          if (mats.count(deriv.first)) mats[deriv.first]->nb_colonnes() ? *mats[deriv.first] += m_scal : *mats[deriv.first] = m_scal;
    }
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Conv_EF_Stab_CoviMAC_Elem::ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &fcl = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur()).fcl;
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face();
  const Champ_Inc_base& cc = equation().champ_conserve();
  const std::string& nom_cc = cc.le_nom().getString();
  const DoubleTab& vit = vitesse_->valeurs(), &vcc = semi_impl.count(nom_cc) ? semi_impl.at(nom_cc) : cc.valeurs(), bcc = cc.valeur_aux_bords();

  Matrice_Morse *m_vit = mats["vitesse"];
  std::vector<std::pair<const DoubleTab *, Matrice_Morse *>> d_cc; //liste des derivees de cc a renseigner : couples (derivee de cc, matrice)
  if (!semi_impl.count(nom_cc)) for (auto &i_m : mats) if (cc.derivees().count(i_m.first))
        d_cc.push_back(std::make_pair(&cc.derivees().at(i_m.first), i_m.second));

  int i, j, e, eb, f, n, N = vcc.line_size();
  DoubleTrav dv_flux(N), dc_flux(2, N); //derivees du flux convectif a la face par rapport a la vitesse / au champ convecte amont / aval

  /* convection aux faces interne (fcl(f, 0) == 0), de Neumann_val_ext ou de Dirichlet */
  for (f = 0; f < zone.nb_faces(); f++) if (!fcl(f, 0) || (fcl(f, 0) > 4 && fcl(f, 0) < 7))
      {
        for (dv_flux = 0, dc_flux = 0, i = 0; i < 2; i++) for (e = f_e(f, i), n = 0; n < N; n++)
            {
              double fac = pf(f) * fs(f) * (1. + (vit.addr()[N * f + n] * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2;
              dv_flux(n) += fac * (e >= 0 ? vcc.addr()[N * e + n] : bcc.addr()[N * f + n]); //f est reelle -> indice trivial dans bcc
              dc_flux(i, n) = e >= 0 ? fac * vit.addr()[N * f + n] : 0;
            }

        //second membre
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (n = 0; n < N; n++)
              secmem.addr()[N * e + n] -= (i ? -1 : 1) * dv_flux(n) * vit.addr()[N * f + n];
        //derivees : vitesse
        if (m_vit) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (n = 0; n < N; n++)
                (*m_vit)(N * e + n, N * f + n) += (i ? -1 : 1) * dv_flux(n);
        //derivees : champ convecte
        for (auto &&d_m : d_cc) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
                for (n = 0; n < N; n++) (*d_m.second)(N * e + n, N * eb + n) += (i ? -1 : 1) * dc_flux(j, n) * d_m.first->addr()[N * eb + n];
      }
}

void Op_Conv_EF_Stab_CoviMAC_Elem::creer_champ(const Motcle& motlu)
{
  Op_Conv_CoviMAC_base::creer_champ(motlu);
  int i = noms_cc_phases_.rang(motlu), j = noms_vd_phases_.rang(motlu);
  if (i >= 0 && !cc_phases_[i].non_nul())
    {
      equation().discretisation().discretiser_champ("vitesse", equation().zone_dis(), noms_cc_phases_[i], "kg/m2/s",dimension, 1, 0, cc_phases_[i]);
      champs_compris_.ajoute_champ(cc_phases_[i]);
    }
  if (j >= 0 && !vd_phases_[j].non_nul())
    {
      equation().discretisation().discretiser_champ("vitesse", equation().zone_dis(), noms_vd_phases_[j], "m/s",dimension, 1, 0, vd_phases_[j]);
      champs_compris_.ajoute_champ(vd_phases_[j]);
    }
}

void Op_Conv_EF_Stab_CoviMAC_Elem::mettre_a_jour(double temps)
{
  Op_Conv_CoviMAC_base::mettre_a_jour(temps);
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  const Champ_Inc_base& cc = equation().champ_conserve();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem();
  const DoubleTab& vit = vitesse_->valeurs(), &vcc = equation().champ_conserve().valeurs(), bcc = cc.valeur_aux_bords(), &alp = equation().inconnue().valeurs();
  DoubleTab balp;
  if (vd_phases_.size()) balp = equation().inconnue().valeur().valeur_aux_bords();


  int i, e, f, n, N = vcc.line_size(), nf_tot = zone.nb_faces_tot();
  if (cc_phases_.size()) for (n = 0; n < N; n++) if (cc_phases_[n].non_nul()) /* mise a jour des champs de debit */
        {
          Champ_Face_CoviMAC& c_ph = ref_cast(Champ_Face_CoviMAC, cc_phases_[n].valeur());
          DoubleTab& v_ph = c_ph.valeurs();
          /* on remplit la partie aux faces, puis on demande au champ d'interpoler aux elements */
          for (f = 0; f < zone.nb_faces(); f++)
            {
              for (v_ph.addr()[f] = 0, i = 0; i < 2; i++)
                v_ph.addr()[f] += (1. + (vit.addr()[N * f + n] * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2 * ((e = f_e(f, i)) >= 0 ? vcc.addr()[N * e + n] : bcc.addr()[N * f + n]);
              v_ph.addr()[f] *= vit.addr()[N * f + n];
            }
          c_ph.update_ve(v_ph); //interpolation face -> element d'ordre 1 (sans matrices)
          for (f = 0; f < zone.nb_faces(); f++) v_ph.addr()[f] *= pf(f);
          for (e = 0; e < zone.nb_elem(); e++) v_ph.addr()[nf_tot + e] *= pe(e); //pour repasser en debitant
          c_ph.changer_temps(temps);
        }

  if (vd_phases_.size()) for (n = 0; n < N; n++) if (vd_phases_[n].non_nul()) /* mise a jour des champs de vitesse debitante */
        {
          Champ_Face_CoviMAC& c_ph = ref_cast(Champ_Face_CoviMAC, vd_phases_[n].valeur());
          DoubleTab& v_ph = c_ph.valeurs();
          /* on remplit la partie aux faces, puis on demande au champ d'interpoler aux elements */
          for (f = 0; f < zone.nb_faces(); f++)
            {
              for (v_ph.addr()[f] = 0, i = 0; i < 2; i++)
                v_ph.addr()[f] += (1. + (vit.addr()[N * f + n] * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2 * ((e = f_e(f, i)) >= 0 ? alp.addr()[N * e + n] : balp.addr()[N * f + n]);
              v_ph.addr()[f] *= vit.addr()[N * f + n];
            }
          c_ph.update_ve(v_ph); //interpolation face -> element d'ordre 1 (sans matrices)
          for (f = 0; f < zone.nb_faces(); f++) v_ph.addr()[f] *= pf(f);
          for (e = 0; e < zone.nb_elem(); e++) v_ph.addr()[nf_tot + e] *= pe(e); //pour repasser en debitant
          c_ph.changer_temps(temps);
        }

}

