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
// File:        Perte_Charge_Singuliere_PolyMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Perte_Charge_Singuliere_PolyMAC_Face.h>
#include <Zone_PolyMAC.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Motcle.h>
#include <Domaine.h>
#include <Matrice_Morse.h>

Implemente_instanciable(Perte_Charge_Singuliere_PolyMAC_Face,"Perte_Charge_Singuliere_Face_PolyMAC",Perte_Charge_PolyMAC_Face);

//// printOn
//

Sortie& Perte_Charge_Singuliere_PolyMAC_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Perte_Charge_Singuliere_PolyMAC_Face::readOn(Entree& s)
{
  Perte_Charge_Singuliere::lire_donnees(s);
  remplir_num_faces(s);
  return s;
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe Perte_Charge_Singuliere_PolyMAC_Face
//
////////////////////////////////////////////////////////////////////

void Perte_Charge_Singuliere_PolyMAC_Face::remplir_num_faces(Entree& s)
{
  const Domaine& le_domaine = equation().probleme().domaine();
  const Zone_PolyMAC& zone_PolyMAC = ref_cast(Zone_PolyMAC,equation().zone_dis().valeur());
  int taille_bloc = zone_PolyMAC.nb_elem();
  num_faces.resize(taille_bloc);
  lire_surfaces(s,le_domaine,zone_PolyMAC,num_faces);
  // int nfac_tot = mp_sum(num_faces.size());
  int nfac_tot = (int)mp_max(num_faces.size()); // not to count several (number of processes) times the same face

  if(je_suis_maitre())
    {
      Cerr << "Faces number found in Perte_Charge_Singuliere_PolyMAC_Face : " << nfac_tot << finl;

      if (nfac_tot==0)
        {
          Cerr << "Error when defining the surface plane for the singular porosity :" << finl;
          Cerr << "No mesh faces has been found for the surface plane." << finl;
          Cerr << "Check the coordinate of the surface plane which should match mesh coordinates." << finl;
          exit();
        }
    }
}

DoubleTab& Perte_Charge_Singuliere_PolyMAC_Face::ajouter(DoubleTab& resu) const
{
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  //const DoubleVect& volumes_entrelaces = zone_PolyMAC.volumes_entrelaces();
  const DoubleVect& porosite_surf = zone_PolyMAC.porosite_face();
  const DoubleTab& vit = la_vitesse->valeurs();

  int nb_faces = num_faces.size();
  int numfa;
  double Ud; // vitesse debitante
  double area;
  double coefK; // pressure loss

  for (int i=0; i<nb_faces; i++) if ((numfa = num_faces[i]) < zone_PolyMAC.nb_faces())
      {
        Ud = vit(numfa)*porosite_surf[numfa];
        area=dabs(zone_PolyMAC.face_normales(numfa,direction_perte_charge())); // Taking account of inclined plane
        coefK=K();
        resu(numfa) -= 0.5*coefK*Ud*dabs(Ud)*area*porosite_surf[numfa];
      }
  return resu;
}

DoubleTab& Perte_Charge_Singuliere_PolyMAC_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

void Perte_Charge_Singuliere_PolyMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  const DoubleVect& porosite_surf = zone_PolyMAC.porosite_face();
  const DoubleTab& vit = la_vitesse->valeurs();

  int nb_faces = num_faces.size();
  int numfa;
  double Ud; // vitesse debitante
  double area;
  double coefK; // pressure loss

  for (int i=0; i<nb_faces; i++) if ((numfa = num_faces[i]) < zone_PolyMAC.nb_faces())
      {
        Ud = porosite_surf[numfa];
        double Ud2= vit(numfa)*porosite_surf[numfa];
        area=dabs(zone_PolyMAC.face_normales(numfa,direction_perte_charge())); // Taking account of inclined plane
        coefK=K();
        matrice.coef(numfa, numfa)+= 0.5*coefK*Ud*dabs(Ud2)*area*porosite_surf[numfa];
      }
}
