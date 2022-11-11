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

#include <IJK_Lata_Swap_JK.h>
#include <Param.h>
#include <Interprete_bloc.h>
#include <IJK_Lata_writer.h>
#include <FixedVector.h>
#include <IJK_Field.h>
#include <IJK_Splitting.h>
#include <IJK_Navier_Stokes_tools.h>

Implemente_instanciable(IJK_Lata_Swap_JK,"IJK_Lata_Swap_JK",Interprete) ;

Sortie& IJK_Lata_Swap_JK::printOn(Sortie& os) const
{
  Objet_U::printOn(os);
  return os;
}

Entree& IJK_Lata_Swap_JK::readOn(Entree& is)
{
  Objet_U::readOn(is);
  return is;
}

Entree& IJK_Lata_Swap_JK::interpreter(Entree& is)
{
  Cerr << "titi" << finl;
  Nom fichier_reprise_vitesse_;
  int timestep_reprise_vitesse_;
  Nom fichier_sortie_vitesse_;
  Nom ijk_splitting_name;
  Nom ijk_splitting_name_dest;

  Param param(que_suis_je());
  param.ajouter("ijk_splitting_source", &ijk_splitting_name, Param::REQUIRED);
  param.ajouter("ijk_splitting_dest", &ijk_splitting_name_dest, Param::REQUIRED);
  param.ajouter("fichier_reprise_vitesse", &fichier_reprise_vitesse_);
  param.ajouter("timestep_reprise_vitesse", &timestep_reprise_vitesse_);
  param.ajouter("fichier_sortie_vitesse", &fichier_sortie_vitesse_);
  param.lire_avec_accolades(is);

  IJK_Splitting splitting_;
  IJK_Splitting splitting_dest_;
  FixedVector<IJK_Field_double, 3> velocity_;
  FixedVector<IJK_Field_double, 3> velocity_dest_;
  IJK_Field_double rho, dest_rho;
  // Recuperation des donnees de maillage
  splitting_ = ref_cast(IJK_Splitting, Interprete_bloc::objet_global(ijk_splitting_name));

  allocate_velocity(velocity_, splitting_, 1);
  Cerr << "lecture de la vitesse" << finl;
  lire_dans_lata(fichier_reprise_vitesse_, timestep_reprise_vitesse_, "DOM", "VELOCITY",
                 velocity_[0], velocity_[1], velocity_[2]); // fonction qui lit un champ a partir d'un lata .
  Cerr << "swap jk" << finl;

  splitting_dest_ = ref_cast(IJK_Splitting, Interprete_bloc::objet_global(ijk_splitting_name_dest));
  allocate_velocity(velocity_dest_, splitting_dest_, 1);

  for (int direction = 0; direction < 3; direction++)
    {
      int direction2 = -1;
      switch(direction)
        {
        case 0:
          direction2 = 0;
          break;
        case 1:
          direction2 = 2;
          break;
        case 2:
          direction2 = 1;
          break;
        default:
          Process::exit();
        }
      const IJK_Field_double& src = velocity_[direction];
      IJK_Field_double& dest = velocity_dest_[direction2];
      const int ni = src.ni();
      const int nj = src.nj();
      const int nk = src.nk();
      for (int k = 0; k < nk; k++)
        for (int j = 0; j < nj; j++)
          for (int i = 0; i < ni; i++)
            dest(i,k,j) = src(i,j,k);
    }
  Cerr << "Write " << fichier_sortie_vitesse_ << finl;
  Nom lata_name(fichier_sortie_vitesse_);
  dumplata_header(lata_name, velocity_dest_[0] /* on passe un champ pour ecrire la geometrie */);
  dumplata_newtime(lata_name,0. /* time */);
  dumplata_vector(lata_name,"VELOCITY", velocity_dest_[0], velocity_dest_[1], velocity_dest_[2], 0);

  rho.allocate(splitting_, IJK_Splitting::ELEM, 0);
  dest_rho.allocate(splitting_dest_, IJK_Splitting::ELEM, 0);
  Cerr << "lecture de rho" << finl;
  lire_dans_lata(fichier_reprise_vitesse_, timestep_reprise_vitesse_, "DOM", "RHO",
                 rho); // fonction qui lit un champ a partir d'un lata .
  Cerr << "swap jk" << finl;

  {
    const IJK_Field_double& src = rho;
    IJK_Field_double& dest = dest_rho;
    const int ni = src.ni();
    const int nj = src.nj();
    const int nk = src.nk();
    for (int k = 0; k < nk; k++)
      for (int j = 0; j < nj; j++)
        for (int i = 0; i < ni; i++)
          dest(i,k,j) = src(i,j,k);
  }
  Cerr << "Write rho " << fichier_sortie_vitesse_ << finl;
  dumplata_scalar(lata_name,"RHO", dest_rho, 0);



  return is;
}
