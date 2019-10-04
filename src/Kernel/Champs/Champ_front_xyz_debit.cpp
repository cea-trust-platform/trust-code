/****************************************************************************
* Copyright (c) 2019, CEA
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
// XD champ_front_xyz_debit front_field_base champ_front_xyz_debit 0 This field is used to define a flow rate field with a velocity profil which will be normalized to match the flow rate choosed.

Sortie& Champ_front_xyz_debit::printOn(Sortie& os) const
{
  return Champ_front_normal::printOn(os);
}


Entree& Champ_front_xyz_debit::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("velocity_profil", &velocity_profil_, Param::REQUIRED); // XD_ADD_P chaine velocity field to define the profil of velocity.
  param.ajouter("flow_rate", &flow_rate_, Param::REQUIRED); //  XD_ADD_P chaine uniform field in space to define the flow rate. It could be, for example, champ_front_uniforme, ch_front_input_uniform or champ_front_fonc_t
  param.lire_avec_accolades_depuis(is);
  fixer_nb_comp(dimension);
  return is;
}

int Champ_front_xyz_debit::initialiser(double tps, const Champ_Inc_base& inco)
{
  Cerr << "Champ_front_xyz_debit::initialiser flow_rate_alone_ " << flow_rate_alone_ << finl;
  if (!Champ_front_normal::initialiser(tps,inco))
    return 0;
  const Front_VF& le_bord= ref_cast(Front_VF,frontiere_dis());
  flow_rate_.valeur().initialiser(tps,inco);
  if ( !sub_type(Champ_front_uniforme,flow_rate_.valeur()) && !sub_type(Ch_front_input_uniforme,flow_rate_.valeur()) && !sub_type(Champ_front_t,flow_rate_.valeur()) && !sub_type(Champ_front_Tabule,flow_rate_.valeur()))
    {
      Cerr << "\nError in Champ_front_xyz_debit::initialiser() \nflow_rate must be of type champ_front_uniforme, ch_front_input_uniforme, champ_front_tabule or champ_front_fonc_t!" << finl;
      exit();
    }
  if (flow_rate_.valeur().nb_comp()!=1)
    {
      Cerr << "\nError in Champ_front_xyz_debit::initialiser() \nflow_rate must only have 1 component!" << finl;
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
  initialiser_coefficient(inco);

  // For tabular sizing only
  normal_vectors_=les_valeurs[0].valeurs();
  integrale_ = 0.;
  DoubleTab velocity_user;
  if (!flow_rate_alone_)
    velocity_user=velocity_profil_.valeur().valeurs();
  else
    {
      velocity_user.resize(1,dimension);
      velocity_user=1;
    }
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
      double flow_rate=flow_rate_.valeur().valeurs()(0,0);
      calculer_champ_vitesse(le_bord,velocity_field, flow_rate, velocity_user);
      const Zone_VF& zone_VF = ref_cast(Zone_VF,zone_dis());
      double un_ij=0;
      double Q_nvo=0;
      double u_scal_n=0;
      int face =0;
      double dS=0;
      int premiere_face=le_bord.num_premiere_face();
      for(int i=0; i<le_bord.nb_faces(); i++)
        {
          face = i + premiere_face;
          dS=zone_VF.face_surfaces( face );
          u_scal_n=0;
          for(int j=0; j<dimension; j++)
            {
              if ( velocity_field.size()>dimension )
                un_ij=normal_vectors_(i,j)*velocity_field(i,j);
              else
                un_ij=normal_vectors_(i,j)*velocity_field(0,j); // champ uniforme
              u_scal_n+=un_ij;
            }
          Q_nvo+=dS*u_scal_n;
        }
      Q_nvo=mp_sum(Q_nvo);
      Cerr << "Q_nvo " << Q_nvo << finl;
      if (dimension==2)
        Cerr << "Champ_front_xyz_debit::initialiser velocity_field(0,0) " << velocity_field(0,0) << " velocity_field(0,1) " << velocity_field(0,1)  << finl;
      else
        Cerr << "Champ_front_xyz_debit::initialiser velocity_field(0,0) " << velocity_field(0,0) << " velocity_field(0,1) " << velocity_field(0,1) << " velocity_field(0,2) " << velocity_field(0,2) << finl;
    }
  return 1;
}

void Champ_front_xyz_debit::calculer_normales_et_integrale(const Front_VF& le_bord, DoubleTab& velocity_user)
{
  Cerr << "Champ_front_xyz_debit::calculer_normales_et_integrale" << finl;
  const Zone_VF& zone_VF = ref_cast(Zone_VF,zone_dis());
  const IntTab& face_voisins = zone_VF.face_voisins();
  double dS=0.;
  int face =0;
  int elem =0;
  int signe = 0;
  double u_scal_n=0;
  int premiere_face=le_bord.num_premiere_face();
  double un_ij=0;
  for(int i=0; i<le_bord.nb_faces(); i++)
    {
      face = i + premiere_face;
      dS=zone_VF.face_surfaces( face );
      u_scal_n=0;
      elem = face_voisins(face,0);
      signe = -1;
      if (elem == -1)
        {
          elem = face_voisins(face,1);
          signe = 1;
        }
      // fill the j^th component of the normal vector associated to the surface number i
      for(int j=0; j<dimension; j++)
        {
          if (flow_rate_alone_)
            normal_vectors_(i,j)=signe*zone_VF.face_normales(face,j)/dS;
          else
            normal_vectors_(i,j)=zone_VF.face_normales(face,j)/dS; // normale sortante
          if ( velocity_user.size()>dimension ) // champ non uniforme
            un_ij=normal_vectors_(i,j)*velocity_user(i,j);
          else
            un_ij=normal_vectors_(i,j)*velocity_user(0,j);
          u_scal_n+=un_ij;
        }
      integrale_+=dS*u_scal_n*zone_VF.porosite_face(face);
    }
  integrale_=mp_sum(integrale_);
  Cerr << "integrale_ " << integrale_ << finl;
}

void Champ_front_xyz_debit::initialiser_coefficient(const Champ_Inc_base& inco)
{
  Cerr << "Champ_front_xyz_debit::initialiser_coefficient" << finl;
  coeff_ = 1.;
}

void Champ_front_xyz_debit::calculer_champ_vitesse(const Front_VF& le_bord, DoubleTab& velocity_field, double flow_rate, DoubleTab& velocity_user)
{
  Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse" << finl;
  Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse velocity_field.size_array() = " << velocity_field.size_array() << finl;

  if (velocity_field.size_array())
    {
      // Allows weighting by rho in Champ_front_debit_massique
      Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse integrale_ = " << integrale_ << finl;
      velocity_field=flow_rate*coeff_/integrale_ ;
      Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse flow_rate_alone_ = " << flow_rate_alone_ << finl;

      if (!flow_rate_alone_)
        {
          Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse velocity_user.size() = " << velocity_user.size() << " velocity_user(0,0) = " << velocity_user(0,0) << " velocity_user(0,1) = " << velocity_user(0,1) << finl;
          Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse velocity_field.size() = " << velocity_field.size() << finl;
          for(int i=0; i<le_bord.nb_faces(); i++)
            {
              for(int j=0; j<dimension; j++)
                {
                  if ( velocity_user.size()>dimension ) // champ non uniforme
                    velocity_field(i,j)=velocity_field(i,j)*velocity_user(i,j);
                  else
                    velocity_field(i,j)=velocity_field(i,j)*velocity_user(0,j);
                }
            }
          Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse 3" << finl;
        }
      else
        {
          Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse 4" << finl;
          for(int i=0; i<le_bord.nb_faces(); i++)
            for(int j=0; j<dimension; j++)
              velocity_field=velocity_field(i,j)*normal_vectors_(i,j);
        }
    }
  if (dimension==2)
    Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse velocity_field(0,0) " << velocity_field(0,0) << " velocity_field(0,1) " << velocity_field(0,1)  << finl;
  else
    Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse velocity_field(0,0) " << velocity_field(0,0) << " velocity_field(0,1) " << velocity_field(0,1) << " velocity_field(0,2) " << velocity_field(0,2) << finl;
  Cerr << "Champ_front_xyz_debit::calculer_champ_vitesse fin" << finl;
  // no else here because in // if the boundary isn't on the proc, it crashes!
}

void Champ_front_xyz_debit::associer_fr_dis_base(const Frontiere_dis_base& fr)
{
  flow_rate_.valeur().associer_fr_dis_base(fr);
  Cerr << "Champ_front_xyz_debit::associer_fr_dis_base flow_rate_alone_ " << flow_rate_alone_ << finl;
  if (!flow_rate_alone_)
    velocity_profil_.valeur().associer_fr_dis_base(fr);
  Champ_front_normal::associer_fr_dis_base(fr);
}

void Champ_front_xyz_debit::set_temps_defaut(double temps)
{
  Cerr << "Champ_front_xyz_debit::set_temps_defaut" << finl;
  flow_rate_.valeur().set_temps_defaut(temps);
  if (!flow_rate_alone_)
    velocity_profil_.valeur().set_temps_defaut(temps);
  Champ_front_normal::set_temps_defaut(temps);
}

void Champ_front_xyz_debit::fixer_nb_valeurs_temporelles(int nb_cases)
{
  Cerr << "Champ_front_xyz_debit::fixer_nb_valeurs_temporelles" << finl;
  flow_rate_.valeur().fixer_nb_valeurs_temporelles(nb_cases);
  if (!flow_rate_alone_)
    velocity_profil_.valeur().fixer_nb_valeurs_temporelles(nb_cases);
  Champ_front_normal::fixer_nb_valeurs_temporelles(nb_cases);
}

void Champ_front_xyz_debit::changer_temps_futur(double temps,int i)
{
  Cerr << "Champ_front_xyz_debit::changer_temps_futur" << finl;
  Champ_front_normal::changer_temps_futur(temps,i);
  flow_rate_.valeur().changer_temps_futur(temps,i);
  if (!flow_rate_alone_)
    velocity_profil_.valeur().changer_temps_futur(temps,i);
}

// Description:
//    Turn the wheel of the CL
int Champ_front_xyz_debit::avancer(double temps)
{
  Cerr << "Champ_front_xyz_debit::avancer" << finl;
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
  Cerr << "Champ_front_xyz_debit::reculer" << finl;
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
  Cerr << "Champ_front_xyz_debit::mettre_a_jour  temps = " << temps << finl;
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
  DoubleTab& velocity_field=valeurs_au_temps(temps); // values of the field at time t, type Roue_ptr
  double flow_rate=flow_rate_.valeur().valeurs_au_temps(temps)(0,0);
  if (velocity_field.size_array())
    {
      const Front_VF& le_bord= ref_cast(Front_VF,frontiere_dis());
      calculer_champ_vitesse(le_bord, velocity_field, flow_rate, velocity_user);
    }
  if (dimension==2)
    Cerr << "Champ_front_xyz_debit::mettre_a_jour velocity_field(0,0) " << velocity_field(0,0) << " velocity_field(0,1) " << velocity_field(0,1)  << finl;
  else
    Cerr << "Champ_front_xyz_debit::mettre_a_jour velocity_field(0,0) " << velocity_field(0,0) << " velocity_field(0,1) " << velocity_field(0,1) << " velocity_field(0,2) " << velocity_field(0,2) << finl;

}

