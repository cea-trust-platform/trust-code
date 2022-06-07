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

#include <Champ_front_contact_fictif_VEF.h>
#include <Probleme_base.h>
#include <Champ_Uniforme.h>
#include <Fluide_Incompressible.h>
#include <Interprete.h>
#include <Domaine.h>
#include <Champ_Fonc_P0_VEF.h>

Implemente_instanciable(Champ_front_contact_fictif_VEF,"Champ_front_contact_fictif_VEF",Champ_front_contact_VEF);


Sortie& Champ_front_contact_fictif_VEF::printOn(Sortie& os) const
{
  return os;
}


Entree& Champ_front_contact_fictif_VEF::readOn(Entree& is)
{
  Champ_front_contact_VEF::readOn(is);
  is >>  conduct_fictif >> ep_fictif ;
  Cerr << "Champ_front_contact_fictif_VEF::readOn : " << nom_pb1 << nom_bord1 << finl;
  Cerr << "      connecte a                : " << nom_pb2 << nom_bord2 << finl;
  //connect_est_remplit=0;
  return is;
}

void Champ_front_contact_fictif_VEF::mettre_a_jour(double temps )
{


  const Frontiere& la_front=la_frontiere_dis->frontiere();
  int nb_faces=la_front.nb_faces();
  DoubleTab& tab=valeurs();

  // On recupere les coefficients gradient_num_transf et gradient_fro_transf de l'autre probleme
  DoubleVect gradient_num_transf_autre_pb(nb_faces);
  DoubleVect gradient_fro_transf_autre_pb(nb_faces);
  if (!ch_fr_autre_pb.non_nul())
    {
      Cerr << "Attention: Vous utilisez une condition de contact Champ_front_contact_fictif_VEF sur le bord " << nom_bord1 << " sur le probleme "<< nom_pb1 <<" " << finl;
      Cerr << "Vous devez avoir un Champ_front_contact_fictif_VEF equivalent sur le bord " << nom_bord2 << " du probleme "<<nom_pb2<<" " << finl;
      Process::exit();
    }
  trace_face_raccord(fr_vf_autre_pb.valeur(),ch_fr_autre_pb->get_gradient_num_transf(),gradient_num_transf_autre_pb);
  trace_face_raccord(fr_vf_autre_pb.valeur(),ch_fr_autre_pb->get_gradient_fro_transf(),gradient_fro_transf_autre_pb);


  // PQ : 10/09/07 : pour plus de lisibilite, on renomme les tableaux servant
  //                     au calcul de la temperature parietale, de sorte d'avoir :
  //
  //                h1.Tf1 + (h2.hs/(h2+hs)). Tf2
  //        Tw_1 = ------------------------------
  //                (h1+hs) - (hs*hs/(h2+hs))
  //
  //  avec :

  double hs = conduct_fictif / ep_fictif;         // coefficient d'echange du solide fictif
  DoubleVect& h1Tf1=(gradient_num_local);                   // temperature du fluide 1
  DoubleVect& h1=(gradient_fro_local);                   // coefficient d'echange du fluide 1
  DoubleVect& h2Tf2=(gradient_num_transf_autre_pb);  // temperature du fluide 2
  DoubleVect& h2=(gradient_fro_transf_autre_pb);   // coefficient d'echange du fluide 2


  // Calcul de la temperature imposee
  int fac_front;
  for (fac_front=0; fac_front<nb_faces; fac_front++)
    {
      tab(fac_front,0)= -( h1Tf1(fac_front) + (hs*h2Tf2(fac_front)/(hs+h2(fac_front))) )
                        / ( hs+h1(fac_front) - (hs*hs/(hs+h2(fac_front))) );

      //        Cerr << " H1 " << h1(fac_front) << finl;
      //        Cerr << " T1 " << Tf1(fac_front) << finl;
      //         Cerr << " H2 " << h2(fac_front) << finl;
      //        Cerr << " T2 " << Tf2(fac_front) << finl;
      //         Cerr << " Hs " << hs << finl;
      //         Cerr << " TW " << tab(fac_front,0) << finl;
      //        Cerr << " " << finl;

    }
  tab.echange_espace_virtuel();
  // Verification de la coherence des temperatures de bord calculees
  //verifier_temperature_bord();
}

