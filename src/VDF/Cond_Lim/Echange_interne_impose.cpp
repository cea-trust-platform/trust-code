/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Echange_interne_impose.cpp
// Directory:   $TRUST_ROOT/src/VDF/Cond_Lim
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_interne_impose.h>
#include <Equation_base.h>
#include <Champ_front_calc_interne.h>
#include <Probleme_base.h>
#include <Frontiere_dis_base.h>
#include <Zone_Cl_dis_base.h>
#include <Zone_VDF.h>
#include <Front_VF.h>
#include <Milieu_base.h>
#include <Champ_front_uniforme.h>
#include <EChaine.h>

Implemente_instanciable(Echange_interne_impose,"Paroi_echange_interne_impose",Echange_externe_impose);
// XD Echange_interne_impose condlim_base Paroi_echange_interne_impose -1 Internal heat exchange boundary condition with exchange coefficient.
// XD attr h_imp chaine h_imp 0 Exchange coefficient value expressed in W.m-2.K-1.
// XD attr ch front_field_base ch 0 Boundary field type.

// Description:
//    Ecrit le type de l'objet sur un flot de sortie
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Echange_interne_impose::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    Simple appel a Echange_impose_base::readOn(Entree&)
//    Lit les specifications des conditions aux limites
//    a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Echange_interne_impose::readOn(Entree& s )
{
  Motcle motlu;
  Motcle motcle("h_gap");

  s >> motlu;
  if (motlu ==motcle)
    s >> h_gap_;
  else
    {
      Cerr << "Error when reading boundary condition with type Echange_interne_impose " << finl;
      Cerr << "We expected " << motcle << " instead of " <<  motlu << finl;
      exit();
    }

  init();

  return s;
}

void Echange_interne_impose::init()
{
  le_champ_front.typer("Champ_front_calc_interne"); // preparation of T_ext
  EChaine e("Champ_front_fonc_xyz 1 1e-10");
  e >> h_imp_;        // won't be used anyway, just make sure we have a big enough array to store future values of h
}


// Override to skip verification - we know we will always be dealing with temperature, and we define ourselves the T_ext with
// a Champ_front_calc_interne.
void Echange_interne_impose::verifie_ch_init_nb_comp() const
{
}

void  Echange_interne_impose::set_temps_defaut(double temps)
{
  if (h_gap_.non_nul())
    h_gap_.valeur().set_temps_defaut(temps);
  Echange_externe_impose::set_temps_defaut(temps);
}
void  Echange_interne_impose::fixer_nb_valeurs_temporelles(int nb_cases)
{
  if (h_gap_.non_nul())
    h_gap_.valeur().fixer_nb_valeurs_temporelles(nb_cases);
  Echange_externe_impose::fixer_nb_valeurs_temporelles(nb_cases);
}
//
void  Echange_interne_impose::changer_temps_futur(double temps,int i)
{
  if (h_gap_.non_nul())
    h_gap_.valeur().changer_temps_futur(temps,i);
  Echange_externe_impose::changer_temps_futur(temps,i);
}
int  Echange_interne_impose::avancer(double temps)
{
  if (h_gap_.non_nul())
    h_gap_.valeur().avancer(temps);
  return Echange_externe_impose::avancer(temps);
}

int  Echange_interne_impose::reculer(double temps)
{
  if (h_gap_.non_nul())
    h_gap_.valeur().reculer(temps);
  return Echange_externe_impose::reculer(temps);
}
void  Echange_interne_impose::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  if (h_gap_.non_nul())
    h_gap_.valeur().associer_fr_dis_base(fr);
  Echange_externe_impose::associer_fr_dis_base(fr);
}

// Finish building the champ_front_calc_interne
void Echange_interne_impose::completer()
{
  Nom nom_pb(zone_Cl_dis().equation().probleme().le_nom());
  Nom nom_bord(frontiere_dis().le_nom());

  lambda_ref_ = zone_Cl_dis().equation().probleme().milieu().conductivite();

  Champ_front_calc_interne& t_ext = ref_cast(Champ_front_calc_interne, T_ext().valeur());
  t_ext.creer(nom_pb, nom_bord, "temperature");

  const Front_VF& fvf = ref_cast(Front_VF, frontiere_dis());
  inv_lambda_.resize(fvf.nb_faces());

  Echange_externe_impose::completer();
}

double Echange_interne_impose::calcul_h_imp(const double h_gap, const double invLambda) const
{
  return 1./(1./h_gap + invLambda);      // harmonic mean
}

// Compute e/lambda - this will be used:
//   - in calcul_h_imp to adjust for the half cell in VDF
//   - in Echange_interne_vargap for the computation of the h_gap correlation.
void Echange_interne_impose::update_inv_lambda()
{
  bool isVDF = sub_type(Zone_VDF, frontiere_dis().zone_dis());
  const Front_VF& fvf = ref_cast(Front_VF, frontiere_dis());

  inv_lambda_ = 0.0;

  if (isVDF)
    {
      DoubleTab& lambdas = lambda_ref_->valeurs();

      const Zone_VDF& zon = ref_cast(Zone_VDF, frontiere_dis().zone_dis());
      for (int numf=0; numf < fvf.nb_faces(); numf++)
        {
          int facei = fvf.num_face(numf);
          int vois0 = zon.face_voisins(facei, 0);
          int elem = (vois0 != -1) ? vois0 : zon.face_voisins(facei, 1);
          inv_lambda_(numf) = zon.dist_norm_bord(facei)/lambdas(elem, 0);
        }
    }
}

void Echange_interne_impose::mettre_a_jour(double tps)
{
  Echange_externe_impose::mettre_a_jour(tps);
  h_gap_.mettre_a_jour(tps);

  update_inv_lambda();

  bool isVDF = sub_type(Zone_VDF, frontiere_dis().zone_dis());

  DoubleTab& h_impose = h_imp_.valeurs();
  DoubleTab& h_gap = h_gap_.valeurs();

  // If VDF, fix h_imp to take into account half cell on the other side of the wall:
  if (isVDF)
    {
      const Champ_front_calc_interne& t_ext = ref_cast(Champ_front_calc_interne, T_ext().valeur());
      const IntTab& face_map = t_ext.face_map();
      const Front_VF& fvf = ref_cast(Front_VF, frontiere_dis());

      for (int numf=0; numf < fvf.nb_faces(); numf++)
        {
          int opp_face = face_map(numf);  // opposite face
          if (sub_type(Champ_front_uniforme, h_gap_.valeur()))
            h_impose(numf,0) = calcul_h_imp(h_gap(0,0), inv_lambda_(opp_face));  // inv lambda from the **opposite** side
          else
            h_impose(numf,0) = calcul_h_imp(h_gap(numf,0), inv_lambda_(opp_face));
        }
    }
  else
    {
      if (sub_type(Champ_front_uniforme, h_gap_.valeur()))
        h_impose = h_gap(0,0);
      else
        h_impose = h_gap;
    }
}

int Echange_interne_impose::initialiser(double temps)
{
  int ret = Echange_externe_impose::initialiser(temps);
  update_inv_lambda();
  return ret;
}
