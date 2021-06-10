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
// File:        Champ_front_xyz_debit.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_front_xyz_debit.h>
#include <Param.h>
#include <Champ_front_uniforme.h>
#include <Ch_front_input_uniforme.h>
#include <Champ_front_t.h>
#include <Champ_front_Tabule.h>


Implemente_instanciable(Champ_front_xyz_debit,"Champ_front_xyz_debit",Champ_front_normal);
// XD champ_front_xyz_debit front_field_base champ_front_xyz_debit 1 This field is used to define a flow rate field with a velocity profil which will be normalized to match the flow rate chosen.

Sortie& Champ_front_xyz_debit::printOn(Sortie& os) const
{
  return Champ_front_normal::printOn(os);
}


Entree& Champ_front_xyz_debit::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("velocity_profil", &velocity_profil_, Param::OPTIONAL); // XD_ADD_P front_field_base velocity_profil 0 velocity field to define the profil of velocity.
  param.ajouter("flow_rate", &flow_rate_, Param::REQUIRED); //  XD_ADD_P front_field_base flow_rate 1 uniform field in space to define the flow rate. It could be, for example, champ_front_uniforme, ch_front_input_uniform or champ_front_fonc_t
  param.lire_avec_accolades_depuis(is);
  fixer_nb_comp(dimension * flow_rate_.nb_comp());
  return is;
}

int Champ_front_xyz_debit::initialiser(double tps, const Champ_Inc_base& inco)
{
//  Cerr << "Champ_front_xyz_debit::initialiser" << finl;
  if (!Champ_front_normal::initialiser(tps,inco))
    return 0;
  const Front_VF& le_bord= ref_cast(Front_VF,frontiere_dis());
  flow_rate_.valeur().initialiser(tps,inco);
  if ( !sub_type(Champ_front_uniforme,flow_rate_.valeur()) && !sub_type(Ch_front_input_uniforme,flow_rate_.valeur()) && !sub_type(Champ_front_t,flow_rate_.valeur()) && !sub_type(Champ_front_Tabule,flow_rate_.valeur()))
    {
      Cerr << "\nError in Champ_front_xyz_debit::initialiser() \nflow_rate must be of type champ_front_uniforme, ch_front_input_uniforme, champ_front_tabule or champ_front_fonc_t!" << finl;
      exit();
    }
  if (!flow_rate_alone_)
    {
      velocity_profil_.valeur().initialiser(tps,inco);
      if (velocity_profil_.valeur().nb_comp()!=dimension)
        {
          Cerr << "\nError in Champ_front_xyz_debit::initialiser() \nvelocity_profil must have " << dimension << " components!" << finl;
          exit();
        }
    }
  initialiser_coefficient(inco, tps);

  // For tabular sizing only
  normal_vectors_=les_valeurs[0].valeurs();
  integrale_.resize(flow_rate_.valeur().valeurs().line_size());
  DoubleTab velocity_user;
  if (!flow_rate_alone_)
    velocity_user=velocity_profil_.valeur().valeurs();
  calculer_normales_et_integrale(le_bord,velocity_user);

  // the flow rate Q_user and the velocity profil U_user are imposed on the boundary
  // so the velocity field u is computed as following :
  // u = ( Q_user U_user n)/ Integrale( U_user . n dS)
  // where n is the normal vector associated to the boundary

  // fill at every time
  int nb_cases=les_valeurs->nb_cases(); // cases number of the wheel
  for (int t=0; t<nb_cases; t++)
    {
      DoubleTab& velocity_field=les_valeurs[t].valeurs(); // values of the field at time t, type Roue_ptr
      calculer_champ_vitesse(le_bord, velocity_field, velocity_user, tps);
    }
  return 1;
}

void Champ_front_xyz_debit::calculer_normales_et_integrale(const Front_VF& le_bord, DoubleTab& velocity_user)
{
  const int N = flow_rate_.nb_comp();
  const Zone_VF& zone_VF = ref_cast(Zone_VF,zone_dis());
  integrale_ = 0.;

  for(int i = 0; i < le_bord.nb_faces_tot(); i++)
    {
      int f = le_bord.num_face(i);
      const double dS = zone_VF.face_surfaces(f);
      for (int n = 0; n < N; ++n)
        {
          double u_scal_n = 0;
          for(int j = 0; j < dimension; j++)
            {
              normal_vectors_(i, j) = -zone_VF.normalized_boundaries_outward_vector(f, 1.)(j); // inward normal
              u_scal_n += normal_vectors_(i, j) * (velocity_user.size() ? velocity_user(velocity_user.size() > dimension ? i : 0, j) : normal_vectors_(i, j));
            }
          if (i < le_bord.nb_faces()) integrale_(n) += dS * u_scal_n / coeff_(i, n) * zone_VF.porosite_face(f); //real faces only
        }
    }
  for (int n = 0; n < N; ++n)
    integrale_(n) = mp_sum(integrale_(n));
}

void Champ_front_xyz_debit::initialiser_coefficient(const Champ_Inc_base& inco, double tps)
{
  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  coeff_.resize(le_bord.nb_faces_tot(), flow_rate_.nb_comp());
  coeff_ = 1.;
}

void Champ_front_xyz_debit::calculer_champ_vitesse(const Front_VF& le_bord, DoubleTab& velocity_field, DoubleTab& velocity_user, double temps)
{
  const int N = flow_rate_.nb_comp();

  for(int i = 0; i < le_bord.nb_faces_tot(); i++) for (int n = 0; n < N; ++n) for(int j = 0; j < dimension; j++)
        {
          double v_mult = flow_rate_.valeur().valeurs_au_temps(temps)(0, n) / integrale_(n) ; //the profile/normals must be multiplied by this to get the correct inward flow
          double n_mult = flow_rate_alone_ ? normal_vectors_(i,j) : (velocity_user(velocity_user.size() > dimension ? i : 0, j));
          velocity_field(i, N * j + n) = v_mult * n_mult;
        }
}

void Champ_front_xyz_debit::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  flow_rate_.valeur().associer_fr_dis_base(fr);
//  Cerr << "Champ_front_xyz_debit::associer_fr_dis_base flow_rate_alone_ " << flow_rate_alone_ << finl;
  if (!flow_rate_alone_)
    velocity_profil_.valeur().associer_fr_dis_base(fr);
  Champ_front_normal::associer_fr_dis_base(fr);
}

void Champ_front_xyz_debit::set_temps_defaut(double temps)
{
//  Cerr << "Champ_front_xyz_debit::set_temps_defaut" << finl;
  flow_rate_.valeur().set_temps_defaut(temps);
  if (!flow_rate_alone_)
    velocity_profil_.valeur().set_temps_defaut(temps);
  Champ_front_normal::set_temps_defaut(temps);
}

void Champ_front_xyz_debit::fixer_nb_valeurs_temporelles(int nb_cases)
{
//  Cerr << "Champ_front_xyz_debit::fixer_nb_valeurs_temporelles" << finl;
  flow_rate_.valeur().fixer_nb_valeurs_temporelles(nb_cases);
  if (!flow_rate_alone_)
    velocity_profil_.valeur().fixer_nb_valeurs_temporelles(nb_cases);
  Champ_front_normal::fixer_nb_valeurs_temporelles(nb_cases);
}

void Champ_front_xyz_debit::changer_temps_futur(double temps,int i)
{
//  Cerr << "Champ_front_xyz_debit::changer_temps_futur" << finl;
  Champ_front_normal::changer_temps_futur(temps,i);
  flow_rate_.valeur().changer_temps_futur(temps,i);
  if (!flow_rate_alone_)
    velocity_profil_.valeur().changer_temps_futur(temps,i);
}

// Description:
//    Turn the wheel of the CL
int Champ_front_xyz_debit::avancer(double temps)
{
//  Cerr << "Champ_front_xyz_debit::avancer" << finl;
  Champ_front_normal::avancer(temps);
  if (flow_rate_.valeur().avancer(temps))
    if (!flow_rate_alone_)
      return velocity_profil_.valeur().avancer(temps);
    else
      return 1;
  else
    return 0;
}

// Description:
//    Turn the wheel of the CL
int Champ_front_xyz_debit::reculer(double temps)
{
//  Cerr << "Champ_front_xyz_debit::reculer" << finl;
  Champ_front_normal::reculer(temps);
  if (flow_rate_.valeur().reculer(temps))
    if (!flow_rate_alone_)
      return velocity_profil_.valeur().reculer(temps);
    else
      return 1;
  else
    return 0;
}

void Champ_front_xyz_debit::mettre_a_jour(double temps)
{
//  Cerr << "Champ_front_xyz_debit::mettre_a_jour  temps = " << temps << finl;
  if (update_coeff_) update_coeff(temps);
  DoubleTab velocity_user;

  flow_rate_.valeur().mettre_a_jour(temps);
  if (!flow_rate_alone_)
    {
      velocity_profil_.valeur().mettre_a_jour(temps);
      velocity_user=velocity_profil_.valeur().valeurs_au_temps(temps);
    }
  else
    {
      velocity_user.resize(1,dimension);
      velocity_user=1;
    }
  if (update_coeff_) calculer_normales_et_integrale(ref_cast(Front_VF,frontiere_dis()),velocity_user);
  DoubleTab& velocity_field=valeurs_au_temps(temps); // values of the field at time t, type Roue_ptr
  const Front_VF& le_bord= ref_cast(Front_VF,frontiere_dis());
  if (velocity_field.size_array())
    calculer_champ_vitesse(le_bord, velocity_field, velocity_user, temps);
}

