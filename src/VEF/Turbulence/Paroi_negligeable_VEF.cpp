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
// File:        Paroi_negligeable_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_negligeable_VEF.h>
#include <Les_Cl.h>
#include <DoubleTrav.h>

Implemente_instanciable_sans_constructeur(Paroi_negligeable_VEF,"negligeable_VEF",Paroi_hyd_base_VEF);

//     printOn()
/////

Sortie& Paroi_negligeable_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_negligeable_VEF::readOn(Entree& s)
{
  return s ;
}


/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_negligeable_VEF
//
/////////////////////////////////////////////////////////////////////

int Paroi_negligeable_VEF::init_lois_paroi()
{
  //Cerr << "Dimensionnement de Cisaillement_paroi_ !" << finl;

  init_lois_paroi_();

  // Dimensionnement du tableau elem_paroi
  int num_cl,fac,i;
  const Conds_lim& les_cl = la_zone_Cl_VEF->les_conditions_limites();
  //  const IntTab& elem_faces = la_zone_VEF->elem_faces();
  const IntTab& face_voisins = la_zone_VEF->face_voisins();

  DoubleTrav Verif_elem_double(la_zone_VEF->nb_elem());
  compteur_elem_paroi = 0;
  Verif_elem_double = 0;
  elem_paroi.resize(la_zone_VEF->nb_faces_bord());
  elem_paroi_double.resize(la_zone_VEF->nb_faces_bord());

  for (num_cl=0; num_cl<les_cl.size(); num_cl++)
    {
      const Cond_lim& la_cl = les_cl[num_cl];
      const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = la_front_dis.num_premiere_face();
      int nfin = ndeb + la_front_dis.nb_faces();
      int num1,verif;

      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          for (fac=ndeb; fac<nfin ; fac++)
            {
              num1 = face_voisins(fac,0);
              if (num1 == -1) num1 = face_voisins(fac,1);
              if (Verif_elem_double[num1] == 0)
                {
                  elem_paroi[compteur_elem_paroi]=num1;
                  elem_paroi_double[fac] = compteur_elem_paroi;
                  Verif_elem_double[num1]++;
                  compteur_elem_paroi++;
                }
              else
                {
                  verif = 0;
                  for (i=0; i<compteur_elem_paroi; i++)
                    {
                      if (elem_paroi[i] == num1)
                        {
                          elem_paroi_double[fac] = i;
                          verif++;
                        }
                    }
                  if (verif == 0)
                    {
                      Cerr << "Il y a un gros pbl dans la detremination de elem_paroi_double" << finl;
                    }
                }
            }
        }
    }
  elem_paroi.resize(compteur_elem_paroi);  // On a tous les elts qui touchent la paroi par une face
  // PBL : est ce qu on a plusieurs fois le meme???? -> Non car Verif_elem_double!!
  return 1;
}


int Paroi_negligeable_VEF::calculer_hyd(DoubleTab& tab_k_eps)
{
  // Cerr << "Dans Paroi_negligeable_VEF::calculer_hyd ne fait rien." << finl;
  return 1;
}

// C est celle la qui nous interesse!!!
int Paroi_negligeable_VEF::calculer_hyd(DoubleTab& tab_nu_t,DoubleTab& tab_k)
{
  // Cerr << "Dans Paroi_negligeable_VEF::calculer_hyd ne fait rien." << finl;
  return 1;
}


int Paroi_negligeable_VEF::calculer_scal(Champ_Fonc_base& )
{
  return 1;
}


// Description:
//    Give a boolean indicating that we don't need to use shear
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: boolean
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
//
bool Paroi_negligeable_VEF::use_shear() const
{
  Process::Journal()<<"Paroi_negligeable_VEF::use_shear() return false"<<finl;
  return false;
}
