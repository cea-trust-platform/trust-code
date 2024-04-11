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


#include <IJK_Shear_Periodic_helpler.h>
#include <IJK_Splitting.h>


void IJK_Shear_Periodic_helpler::allocate(const int ni_local, const int nj_local, const int nk_local, int ghost_size, int ncompo, int type, double rov, double rol, int use_inv_rho_in_pressure_solver)
{
  order_interpolation_ = order_interpolation_poisson_solver_;
  x_.resize_array(order_interpolation_+1);
  denum_.resize_array(order_interpolation_+1);
  res_.resize_array(order_interpolation_+1);
  send_i_real_.resize_array(order_interpolation_+1);
  monofluide_variable_ = type;
  use_inv_rho_in_pressure_solver_= use_inv_rho_in_pressure_solver;
  // monofluide_variable_==1 is for pressure field in case of shear periodicity
  // monofluide_variable_==2 is for physical proerties (rho, mu, nu etc.) in case of shear periodicity
  if (monofluide_variable_==2)
    {
      indicatrice_ghost_zmin_.allocate(ni_local, nj_local, 4, ghost_size, 0, ncompo);
      indicatrice_ghost_zmax_.allocate(ni_local, nj_local, 4, ghost_size, 0, ncompo);
      I_sig_kappa_zmin_.allocate(ni_local, nj_local, 4, ghost_size, 0, ncompo);
      I_sig_kappa_zmax_.allocate(ni_local, nj_local, 4, ghost_size, 0, ncompo);
      Phi_ppty_v_ = rov;
      Phi_ppty_l_ = rol;
    }
  else if (monofluide_variable_==1)
    {
      I_sig_kappa_zmin_.allocate(ni_local, nj_local, 4, ghost_size, 0, ncompo);
      I_sig_kappa_zmax_.allocate(ni_local, nj_local, 4, ghost_size, 0, ncompo);
      Phi_ppty_v_ = rov;
      Phi_ppty_l_ = rol;
    }
}


void IJK_Shear_Periodic_helpler::prepare_interpolation_for_shear_periodicity(const int send_i, const double istmp, const int real_size_i)
{
  // prepare the shear-periodicity interpolation at a given i elevation.
  // shear periodicity is only available on the component DU/DZ
  int nb_points = order_interpolation_+1;

  if (nb_points==2)
    {
      x_[0] = (int) floor(istmp);
      x_[1] = (int) floor(istmp)+1;
    }
  else if(nb_points==3)
    {
      x_[0] = send_i-1;
      x_[1] = send_i;
      x_[2] = send_i+1;
    }
  else if(nb_points==5)
    {
      x_[0] = send_i-2;
      x_[1] = send_i-1;
      x_[2] = send_i;
      x_[3] = send_i+1;
      x_[4] = send_i+2;
    }
  else if(nb_points==7)
    {
      x_[0] = send_i-3;
      x_[1] = send_i-2;
      x_[2] = send_i-1;
      x_[3] = send_i;
      x_[4] = send_i+1;
      x_[5] = send_i+2;
      x_[6] = send_i+3;
    }

  for (int pt = 0; pt < nb_points ; pt++)
    {
      send_i_real_[pt]=(x_[pt] % real_size_i + real_size_i) % real_size_i;
      denum_[pt] = 1.;
      res_[pt] = 1.;
      for (int pt_autre = 0; pt_autre < nb_points ; pt_autre++)
        {
          if (pt_autre!=pt)
            {
              denum_[pt] *= (double)(x_[pt] - x_[pt_autre]);
              res_[pt] *= (istmp - x_[pt_autre]);
            }
        }
    }

  return;
}

