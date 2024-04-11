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

#ifndef IJK_Shear_Periodic_helpler_included
#define IJK_Shear_Periodic_helpler_included

#include <IJK_Field_local_template.h>
#include <TRUSTTab.h>

class IJK_Shear_Periodic_helpler
{
public:
  static double shear_x_time_ ;
  static double Lx_for_shear_perio ;
  static int defilement_ ;
  static int order_interpolation_poisson_solver_;
  static double rho_liq_ref_for_poisson_;
  static double rho_vap_ref_for_poisson_;

  int monofluide_variable_ = -123;
  int order_interpolation_ = -123;
  int compo_ ;
  int real_size_i_ ;
  ArrOfInt x_;
  ArrOfInt send_i_real_;
  ArrOfDouble denum_;
  ArrOfDouble res_;
  int use_inv_rho_in_pressure_solver_ ;
  double DU_ ;
  double Phi_ppty_v_ = -123., Phi_ppty_l_ = -123.;

  IJK_Field_local_template<double,ArrOfDouble> indicatrice_ghost_zmin_ ;
  IJK_Field_local_template<double,ArrOfDouble> indicatrice_ghost_zmax_ ;
  IJK_Field_local_template<double,ArrOfDouble> I_sig_kappa_zmin_;
  IJK_Field_local_template<double,ArrOfDouble> I_sig_kappa_zmax_;
  //IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_> I_sigma_kappa_ghost_zmin_ ;
  //IJK_Field_local_template<_TYPE_,_TYPE_ARRAY_> I_sigma_kappa_ghost_zmax_ ;
  void allocate(const int ni_local, const int nj_local, const int nk_local, int ghost_size, int ncompo, int type, double rov, double rol, int use_inv_rho_in_pressure_solver);
  void prepare_interpolation_for_shear_periodicity(const int send_i, const double istmp, const int real_size_i);

  const IJK_Field_local_template<double,ArrOfDouble>& get_indicatrice_ghost_zmin_() const
  {
    return indicatrice_ghost_zmin_;
  }
  const IJK_Field_local_template<double,ArrOfDouble>& get_indicatrice_ghost_zmax_() const
  {
    return indicatrice_ghost_zmax_;
  }
  const IJK_Field_local_template<double,ArrOfDouble>& get_I_kappa_ghost_zmin_() const
  {
    return I_sig_kappa_zmin_;
  }
  const IJK_Field_local_template<double,ArrOfDouble>& get_I_kappa_ghost_zmax_() const
  {
    return I_sig_kappa_zmax_;
  }

  void set_indicatrice_ghost_zmin_(const IJK_Field_local_template<double,TRUSTArray<double>>& indic_z_min, int decallage = 0)
  {
    for (int k = 0; k < indicatrice_ghost_zmin_.nk() ; k++)
      for (int j = 0; j < indicatrice_ghost_zmin_.nj() ; j++)
        for (int i = 0; i < indicatrice_ghost_zmin_.ni(); i++)
          indicatrice_ghost_zmin_(i,j,k)=indic_z_min(i,j,k+decallage);
  }
  void set_indicatrice_ghost_zmax_(const IJK_Field_local_template<double,TRUSTArray<double>>& indic_z_max, int decallage = 0)
  {
    for (int k = 0; k < indicatrice_ghost_zmax_.nk() ; k++)
      for (int j = 0; j < indicatrice_ghost_zmax_.nj() ; j++)
        for (int i = 0; i < indicatrice_ghost_zmax_.ni(); i++)
          indicatrice_ghost_zmax_(i,j,k)=indic_z_max(i,j,k+decallage);

  }
  void set_I_sig_kappa_zmin_(const IJK_Field_local_template<double,TRUSTArray<double>>& indic_z_min, const IJK_Field_local_template<double,TRUSTArray<double>>& kappa_z_min, const double sigma, const int decallage = 0)
  {
    if (monofluide_variable_==1)
      {
        for (int k = 0; k < I_sig_kappa_zmin_.nk() ; k++)
          for (int j = 0; j < I_sig_kappa_zmin_.nj() ; j++)
            for (int i = 0; i < I_sig_kappa_zmin_.ni(); i++)
              I_sig_kappa_zmin_(i,j,k)=indic_z_min(i,j,k+decallage)*kappa_z_min(i,j,k+decallage)*sigma;
      }
  }
  void set_I_sig_kappa_zmax_(const IJK_Field_local_template<double,TRUSTArray<double>>& indic_z_max, const IJK_Field_local_template<double,TRUSTArray<double>>& kappa_z_max, const double sigma, const int decallage = 0)
  {
    if (monofluide_variable_==1)
      {
        for (int k = 0; k < I_sig_kappa_zmax_.nk() ; k++)
          for (int j = 0; j < I_sig_kappa_zmax_.nj() ; j++)
            for (int i = 0; i < I_sig_kappa_zmax_.ni(); i++)
              I_sig_kappa_zmax_(i,j,k)=indic_z_max(i,j,k+decallage)*kappa_z_max(i,j,k+decallage)*sigma;
      }

  }
  void set_dU_(double DU)
  {
    DU_ = DU;
  }
};

#endif /* IJK_Shear_Periodic_helpler_included */
