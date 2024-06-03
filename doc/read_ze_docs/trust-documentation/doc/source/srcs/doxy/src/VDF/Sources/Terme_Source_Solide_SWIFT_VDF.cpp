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

#include <Terme_Source_Solide_SWIFT_VDF.h>
#include <math.h>
#include <Champ_Uniforme.h>
#include <Domaine_VDF.h>
#include <Conduction.h>
#include <Interprete.h>
#include <Probleme_base.h>
#include <Milieu_base.h>

Implemente_instanciable(Terme_Source_Solide_SWIFT_VDF,"Solide_SWIFT_VDF_P0_VDF",Source_base);

Sortie& Terme_Source_Solide_SWIFT_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Terme_Source_Solide_SWIFT_VDF::readOn(Entree& is )
{
  int i,compteur = 0;
  Motcle motlu;
  Cerr << "         ######### Warning ! #########" << finl;
  Cerr << " For the moment, this forcing term is only available" << finl;
  Cerr << "     for recovering identical solid meshes." << finl;
  Cerr << " If otherwise, it will produce a beautiful core-dump..." << finl;

  for(i=0 ; i<3 ; i++)
    {
      is >> motlu;

      if(motlu == "TAU")
        {
          compteur++;
          is >> tau;
          if(tau==0)
            {
              Cerr << "TAU can not be equal to 0." << finl;
              exit();
            }
          Cerr << "Characteristic time TAU is : " << tau << finl;
        }
      else if(motlu == "pb_swift")
        {
          compteur++;
          Nom swift;
          is >> swift;
          pb_swift=ref_cast(Probleme_base, Interprete::objet(swift));
          Cerr << "pb_swift is: " << pb_swift->le_nom() << finl;
        }
      else if(motlu == "pb_corse")
        {
          compteur++;
          Nom corse;
          is >> corse;
          pb_corse=ref_cast(Probleme_base, Interprete::objet(corse));
          Cerr << "pb_coarse is: " << pb_corse->le_nom() << finl;
        }
      else
        {
          Cerr << "An argument of Terme_Source_Solide_SWIFT_VDF is not correct !" << finl;
          exit();
        }
    }
  if(compteur != 3)
    {
      Cerr << "The forcing term for the mean solid temperature field with the SWIFT method needs 3 arguments" << finl;
      Cerr << "Syntax : " << finl;
      Cerr << "Solide_SWIFT SWIFT pb_sol_SWIFT COARSE pb_sol_coarse TAU tau " << finl;
      exit();
    }


  // On recupere les equations de conduction des deux domaines (SWIFT et grossier)
  int flag=0;
  for(i=0; i<pb_swift->nombre_d_equations(); i++)
    {
      if(sub_type(Conduction,pb_swift->equation(i)))
        {
          eq_swift = ref_cast(Conduction,pb_swift->equation(i));
          flag=1;
        }
    }
  if (flag==0)
    {
      Cerr << "Error : The Conduction equation was not found...!" << finl;
      Cerr << "for the SWIFT solid forcing term (in pb_swift)." << finl;
      exit();
    }

  flag=0;
  for(i=0; i<pb_corse->nombre_d_equations(); i++)
    {
      if(sub_type(Conduction,pb_corse->equation(i)))
        {
          eq_corse = ref_cast(Conduction,pb_corse->equation(i));
          flag=1;
        }
    }
  if (flag==0)
    {
      Cerr << "Error : The Conduction equation was not found...!" << finl;
      Cerr << "for the SWIFT solid forcing term (in pb_coarse)." << finl;
      exit();
    }

  init_calcul_moyenne(eq_swift.valeur(),Y_swift,corresp_swift,compt_swift);
  init_calcul_moyenne(eq_corse.valeur(),Y_corse,corresp_corse,compt_corse);

  // Calcul du tableau de correspondance entre un element de Tmoy_swift et l'element de Tmoy_corse correspondant
  // pour calculer le terme de forcage.
  correspondance_SWIFT_coarse();

  return is;

}

void Terme_Source_Solide_SWIFT_VDF::associer_domaines(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis)
{
}

void Terme_Source_Solide_SWIFT_VDF::associer_pb(const Probleme_base& pb)
{
}

void Terme_Source_Solide_SWIFT_VDF::init_calcul_moyenne(const Conduction& my_eqn, DoubleVect& Y, IntVect& corresp, IntVect& compt)
{
  int num_elem, j, indic, trouve;
  double y;

  const Domaine_dis_base& zdisbase = my_eqn.inconnue().domaine_dis_base();
  const Domaine_VDF& domaine_VDF      = ref_cast(Domaine_VDF, zdisbase);
  const DoubleTab& xp           = domaine_VDF.xp();

  int nb_elems   = domaine_VDF.domaine().nb_elem();
  int nb_faces_x = domaine_VDF.nb_faces_X();
  int nb_faces_z = domaine_VDF.nb_faces_Z();

  int Nx = nb_elems / (nb_faces_x - nb_elems);
  int Nz = nb_elems / (nb_faces_z - nb_elems);
  int Ny = nb_elems / Nx / Nz;

  Y.resize(Ny);
  compt.resize(Ny);
  corresp.resize(nb_elems);

  Y = -100.;
  compt = 0;
  corresp = -1;

  j = 0;
  indic = 0;
  for (num_elem = 0; num_elem < nb_elems; num_elem++)
    {
      y = xp(num_elem, 1);
      trouve = 0;
      for (j = 0; j < indic + 1; j++)
        {
          if (std::fabs(y - Y[j]) <= 1e-8)
            {
              corresp[num_elem] = j;
              compt[j]++;
              j = indic + 1;
              trouve = 1;
              break;
            }
        }
      if (trouve == 0)
        {
          corresp[num_elem] = indic;
          Y[indic] = y;
          compt[indic]++;
          indic++;
        }
    }
}

void Terme_Source_Solide_SWIFT_VDF::correspondance_SWIFT_coarse()
{
  int t_S = Y_swift.size();
  int t_C = Y_corse.size();
  corresp_SC.resize(t_S);

  for (int j = 0; j < t_S; j++)
    {
      int burk = 1;
      for (int i = 0; i < t_C; i++)
        {
          if (std::fabs(Y_swift[j] - Y_corse[i]) <= 1e-8)
            {
              burk = 0;
              corresp_SC[j] = i;
            }
        }
      if (burk)
        {
          Cerr << "## ACHTUNG !!! On n'a pas trouve l'equivalence entre" << finl;
          Cerr << "## les deux domaines au niveau des Y !" << finl;
        }
    }
}

void Terme_Source_Solide_SWIFT_VDF::calcul_moyenne(const Conduction& my_eqn, DoubleVect& T_moy, const IntVect& corresp, const IntVect& compt) const
{
  const Domaine_dis_base& zdisbase = my_eqn.inconnue().domaine_dis_base();
  const Domaine_VDF& domaine_VDF      = ref_cast(Domaine_VDF, zdisbase);
  const DoubleTab& Temp         = my_eqn.inconnue().valeurs();

  int nb_elems   = domaine_VDF.domaine().nb_elem();
  int nb_faces_x = domaine_VDF.nb_faces_X();
  int nb_faces_z = domaine_VDF.nb_faces_Z();

  int Nx = nb_elems / (nb_faces_x - nb_elems);
  int Nz = nb_elems / (nb_faces_z - nb_elems);
  int Ny = nb_elems / Nx / Nz;

  int j = 0;
  T_moy.resize(Ny);
  T_moy = 0.;

  for (j = 0; j < nb_elems; j++)
    T_moy[corresp[j]] += Temp[j];
  for (j = 0; j < Ny; j++)
    T_moy[j] /= compt[j];
}

void Terme_Source_Solide_SWIFT_VDF::ajouter_blocs(matrices_t matrices, DoubleTab& resu, const tabs_t& semi_impl) const
{

  const Domaine_dis_base& zdisbase = eq_swift->inconnue().domaine_dis_base();
  const Domaine_VDF& domaine_VDF      = ref_cast(Domaine_VDF, zdisbase);
  int nb_elems                   = domaine_VDF.nb_elem();
  const DoubleVect& volume         = domaine_VDF.volumes();

  DoubleVect Tmoy_swift;        // Profils de temperature moyenne.
  DoubleVect Tmoy_corse;

  // On calcule les profils de temperature moyenne dans les deux solides.
  calcul_moyenne(eq_swift.valeur(), Tmoy_swift, corresp_swift, compt_swift);
  calcul_moyenne(eq_corse.valeur(), Tmoy_corse, corresp_corse, compt_corse);

  for (int num_elem = 0; num_elem < nb_elems; num_elem++)
    {
      resu(num_elem) += volume(num_elem) * (Tmoy_corse(corresp_SC[corresp_swift[num_elem]]) - Tmoy_swift(corresp_swift[num_elem])) / tau;
    }

}

DoubleTab& Terme_Source_Solide_SWIFT_VDF::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}
