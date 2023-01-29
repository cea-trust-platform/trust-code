/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Terme_Source_Canal_RANS_LES_VDF_Face_included
#define Terme_Source_Canal_RANS_LES_VDF_Face_included

#include <TRUSTTabs_forward.h>
#include <Source_base.h>
#include <Champ_Don.h>
#include <TRUST_Ref.h>
#include <Nom.h>

class Navier_Stokes_std;
class Probleme_base;
class Domaine_Cl_VDF;
class Domaine_VDF;

/*! @brief class Terme_Source_Canal_RANS_LES_VDF_Face Cette classe concerne un terme source calcule en partie grace
 *
 *   a un calcul RANS preliminaire et applique au calcul LES en cours
 *
 *
 */
class Terme_Source_Canal_RANS_LES_VDF_Face : public Source_base
{
  Declare_instanciable_sans_destructeur(Terme_Source_Canal_RANS_LES_VDF_Face);

public :
  ~Terme_Source_Canal_RANS_LES_VDF_Face() override;
  void associer_pb(const Probleme_base& ) override;
  DoubleTab& calculer(DoubleTab& ) const override;

  void init_calcul_moyenne_spat();
  void init();
  DoubleTab norme_vit(void) const;
  void mettre_a_jour(double) override;

  void moy_spat(DoubleVect&, DoubleVect&, DoubleVect&, DoubleVect&, DoubleVect&, DoubleVect&, DoubleVect&);
  void calculer_integrale_temporelle(DoubleVect&, const DoubleVect&);
  void ecriture_moy_spat(DoubleVect&, DoubleVect&, DoubleVect&, DoubleVect&, DoubleVect&, DoubleVect&);
  void ecriture_moy_temp(DoubleVect&, DoubleVect&, DoubleVect&, DoubleVect&, DoubleVect&, DoubleVect&, const double);


  inline void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override {}
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override;
  inline int has_interface_blocs() const override
  {
    return 1;
  };

protected :
  void associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis& ) override;
  REF2(Domaine_VDF) le_dom_VDF;
  REF2(Domaine_Cl_VDF) le_dom_Cl_VDF;

private :
  int moyenne= 0; //type de moyenne
  int cpt= 0, compteur_reprise= 0;
  double alpha_tau = -100., Ly= -100.;
  double f_start= 0., t_av=-1;

  int Nxy = -10, Nyy= -10, Nzy= -10;

  DoubleTab tau;
  DoubleTab U_RANS;

  DoubleVect umoy_spat, umoy_x,
             umoy_y, umoy_z, umoy ;

  /*    DoubleVect champ_spat_x; */
  /*    DoubleVect champ_spat_y; */
  /*    DoubleVect champ_spat_z; */

  /*    DoubleVect champ_spat_x_2; */
  /*    DoubleVect champ_spat_y_2; */
  /*    DoubleVect champ_spat_z_2; */

  /*    DoubleVect champ_spat; */

  DoubleVect utemp_gliss, utemp, utemp_sum;

  DoubleVect Yu, Yv, Yw;                      // Coordonnees des points ou est definie la vitesse
  IntVect compt_x,compt_y,compt_z ;            // Nombre de points a meme Y pour chaque grandeur
  IntVect corresp_u, corresp_v, corresp_w; // Correspondance numerotation globale -> numerotation locale dans Yxx

  Nom nom_pb_rans;

  //DoubleVect U, U_RANS;
  //DoubleVect force;


};
#endif
