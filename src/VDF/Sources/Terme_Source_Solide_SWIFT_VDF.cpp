/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Terme_Source_Solide_SWIFT_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_Solide_SWIFT_VDF.h>
#include <math.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Conduction.h>
#include <Interprete.h>
#include <Probleme_base.h>
#include <Milieu_base.h>

Implemente_instanciable(Terme_Source_Solide_SWIFT_VDF,"Solide_SWIFT_VDF_P0_VDF",Source_base);

//// printOn
//

Sortie& Terme_Source_Solide_SWIFT_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

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
          Cerr << "pb_swift is: " << pb_swift.le_nom() << finl;
        }
      else if(motlu == "pb_corse")
        {
          compteur++;
          Nom corse;
          is >> corse;
          pb_corse=ref_cast(Probleme_base, Interprete::objet(corse));
          Cerr << "pb_coarse is: " << pb_corse.le_nom() << finl;
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







void Terme_Source_Solide_SWIFT_VDF::associer_zones(const Zone_dis& zone_dis,
                                                   const Zone_Cl_dis& zone_Cl_dis)
{
  /*    la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
        la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur()) */
  ;
}

void Terme_Source_Solide_SWIFT_VDF::associer_pb(const Probleme_base& pb)
{
  ;
}









void Terme_Source_Solide_SWIFT_VDF::init_calcul_moyenne(const Conduction& my_eqn, DoubleVect& Y, IntVect& corresp, IntVect& compt)
{
  int num_elem,j,indic,trouve;
  double y;

  const Zone_dis_base& zdisbase = my_eqn.inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF      = ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& xp           = zone_VDF.xp();

  int nb_elems   = zone_VDF.zone().nb_elem();
  int nb_faces_x = zone_VDF.nb_faces_X();
  int nb_faces_z = zone_VDF.nb_faces_Z();

  int Nx        = nb_elems/(nb_faces_x-nb_elems);
  int Nz        = nb_elems/(nb_faces_z-nb_elems);
  int Ny        = nb_elems/Nx/Nz;

  Y.resize(Ny);
  compt.resize(Ny);
  corresp.resize(nb_elems);

  Y = -100.;
  compt = 0;
  corresp = -1;

  j=0;
  indic = 0;
  for (num_elem=0; num_elem<nb_elems; num_elem++)
    {
      y = xp(num_elem,1);
      trouve = 0;
      for (j=0; j<indic+1; j++)
        {
          if(dabs(y-Y[j])<=1e-8)
            {
              corresp[num_elem] = j;
              compt[j] ++;
              j=indic+1;
              trouve = 1;
            }
        }
      if (trouve==0)
        {
          corresp[num_elem] = indic;
          Y[indic]=y;
          compt[indic] ++;
          indic++;
        }
    }
}










void Terme_Source_Solide_SWIFT_VDF::correspondance_SWIFT_coarse( )
{
  int t_S = Y_swift.size();
  int t_C = Y_corse.size();
  corresp_SC.resize(t_S);

  for(int j=0; j<t_S; j++)
    {
      int burk=1;
      for(int i=0; i<t_C; i++)
        {
          if(dabs(Y_swift[j]-Y_corse[i])<=1e-8)
            {
              burk=0;
              corresp_SC[j]=i;
            }
        }
      if(burk)
        {
          Cerr << "## ACHTUNG !!! On n'a pas trouve l'equivalence entre" << finl;
          Cerr << "## les deux domaines au niveau des Y !" << finl;
        }
    }
}










void Terme_Source_Solide_SWIFT_VDF::calcul_moyenne(const Conduction& my_eqn, DoubleVect& T_moy, const IntVect& corresp, const IntVect& compt) const
{
  const Zone_dis_base& zdisbase = my_eqn.inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF      = ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& Temp         = my_eqn.inconnue().valeurs();

  int nb_elems   = zone_VDF.zone().nb_elem();
  int nb_faces_x = zone_VDF.nb_faces_X();
  int nb_faces_z = zone_VDF.nb_faces_Z();

  int Nx        = nb_elems/(nb_faces_x-nb_elems);
  int Nz        = nb_elems/(nb_faces_z-nb_elems);
  int Ny        = nb_elems/Nx/Nz;


  int j=0;
  T_moy.resize(Ny);
  T_moy = 0.;

  for (j=0; j<nb_elems; j++)  T_moy[corresp[j]] += Temp[j];
  for (j=0; j<Ny; j++)  T_moy[j] /= compt[j];
}










DoubleTab& Terme_Source_Solide_SWIFT_VDF::ajouter(DoubleTab& resu) const
{

  const Zone_dis_base& zdisbase = eq_swift->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF      = ref_cast(Zone_VDF, zdisbase);
  int nb_elems                   = zone_VDF.nb_elem();
  const DoubleVect& volume         = zone_VDF.volumes();

  DoubleVect Tmoy_swift;        // Profils de temperature moyenne.
  DoubleVect Tmoy_corse;

  /*  const Champ_Don& rho_don = pb_swift->milieu().masse_volumique();
      const Champ_Don& Cp_don  = pb_swift->milieu().capacite_calorifique();
      double rho=1.,Cp=1.;

      if (sub_type(Champ_Uniforme,rho_don.valeur()))  rho = rho_don(0,0);
      else
      {
      Cerr << "### Rho is not uniform in the solid !" << finl;
      Cerr << "### Problem in Terme_Source_Solide_SWIFT_VDF..." << finl;
      exit();
      }

      if (sub_type(Champ_Uniforme,Cp_don.valeur()))  Cp = Cp_don(0,0);
      else
      {
      Cerr << "### Cp is not uniform in the solid !" << finl;
      Cerr << "### Problem in Terme_Source_Solide_SWIFT_VDF..." << finl;
      exit();
      }
      Cerr << "Rho = " << rho << finl;
      Cerr << "Cp = " << Cp << finl; */


  // On calcule les profils de temperature moyenne dans les deux solides.
  calcul_moyenne(eq_swift.valeur(),Tmoy_swift,corresp_swift,compt_swift);
  calcul_moyenne(eq_corse.valeur(),Tmoy_corse,corresp_corse,compt_corse);

  for(int num_elem = 0 ; num_elem<nb_elems ; num_elem++)
    {
      resu(num_elem) += volume(num_elem)*(Tmoy_corse(corresp_SC[corresp_swift[num_elem]])-Tmoy_swift(corresp_swift[num_elem]))/tau;
    }

  return resu;
}








DoubleTab& Terme_Source_Solide_SWIFT_VDF::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

