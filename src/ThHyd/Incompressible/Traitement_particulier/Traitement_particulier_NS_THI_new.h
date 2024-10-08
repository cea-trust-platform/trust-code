/****************************************************************************
* Copyright (c) 2024, CEA
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


#ifndef Traitement_particulier_NS_THI_new_included
#define Traitement_particulier_NS_THI_new_included

#include <Traitement_particulier_NS_base.h>
#include <TRUSTTabs_forward.h>

#include <Domaine_VF.h>


/*! @brief classe Traitement_particulier_THI_new Cette classe permet de faire les traitements particuliers
 *
 *      pour les calculs de THI, en particulier initialisation
 *      particuliere et calculs des spectres!!
 *
 *
 * @sa Navier_Stokes_Turbulent, Traitement_particulier_base, Traitement_particulier_VEF
 */
class Traitement_particulier_NS_THI_new : public Traitement_particulier_NS_base
{
  Declare_base(Traitement_particulier_NS_THI_new);

public :

  Entree& lire(Entree& ) override;
  void preparer_calcul_particulier() override ;
  void post_traitement_particulier() override ;
  void en_cours_de_resolution(int , DoubleTab&, DoubleTab& ,double) override ;
  inline void sauver_stat()  const override ;
  inline void reprendre_stat() override  ;
  inline int a_pour_Champ_Fonc(const Motcle& mot, OBS_PTR(Champ_base)& ch_ref) const  ;
  inline int comprend_champ(const Motcle& mot) const  ;

protected :

  virtual void renorm_Ec() =0;
  virtual void init_calc_spectre()=0;
  virtual void calcul_spectre()=0;
  int& calcul_nb_som_dir(const Domaine&);
  void msg_erreur_maillage(const char*);

  int init = 0, oui_transf = 0, oui_calc_spectre = 0;
  double Ec_init = -100.;
  int nb_som_dir = 0;
  int fac_init = 0;
};

#endif

inline void Traitement_particulier_NS_THI_new::sauver_stat()  const
{
  ;
}

inline void Traitement_particulier_NS_THI_new::reprendre_stat()
{
  ;
}

inline int Traitement_particulier_NS_THI_new::a_pour_Champ_Fonc(const Motcle& mot, OBS_PTR(Champ_base)& ch_ref) const
{
  return 0 ;
}

inline int Traitement_particulier_NS_THI_new::comprend_champ(const Motcle& mot) const
{
  return 0 ;
}
