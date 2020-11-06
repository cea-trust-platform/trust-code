/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Perte_Charge_Singuliere_CoviMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Sources
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Perte_Charge_Singuliere_CoviMAC_Face.h>
#include <Zone_CoviMAC.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Motcle.h>
#include <Domaine.h>
#include <Matrice_Morse.h>
#include <Param.h>

Implemente_instanciable(Perte_Charge_Singuliere_CoviMAC_Face,"Perte_Charge_Singuliere_Face_CoviMAC",Perte_Charge_CoviMAC_Face);

//// printOn
//

Sortie& Perte_Charge_Singuliere_CoviMAC_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Perte_Charge_Singuliere_CoviMAC_Face::readOn(Entree& s)
{
  Perte_Charge_Singuliere::lire_donnees(s);
  remplir_num_faces(s);
  if (regul_) //fichier de sortie si regulation
    {
      bilan().resize(3); //K deb cible
      set_fichier(Nom("K_") + identifiant_);
      set_description(Nom("Regulation du Ksing de la surface ") + identifiant_ + "\nt K deb cible");
    }
  return s;
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe Perte_Charge_Singuliere_CoviMAC_Face
//
////////////////////////////////////////////////////////////////////

void Perte_Charge_Singuliere_CoviMAC_Face::remplir_num_faces(Entree& s)
{
  const Domaine& le_domaine = equation().probleme().domaine();
  const Zone_CoviMAC& zone_CoviMAC = ref_cast(Zone_CoviMAC,equation().zone_dis().valeur());
  int taille_bloc = zone_CoviMAC.nb_elem();
  num_faces.resize(taille_bloc);
  lire_surfaces(s,le_domaine,zone_CoviMAC,num_faces, sgn);
  // int nfac_tot = mp_sum(num_faces.size());
  int nfac_max = (int)mp_max(num_faces.size()); // not to count several (number of processes) times the same face

  if(je_suis_maitre() && nfac_max == 0)
    {
      Cerr << "Error when defining the surface plane for the singular porosity :" << finl;
      Cerr << "No mesh faces has been found for the surface plane." << finl;
      Cerr << "Check the coordinate of the surface plane which should match mesh coordinates." << finl;
      exit();
    }
}

DoubleTab& Perte_Charge_Singuliere_CoviMAC_Face::ajouter(DoubleTab& resu) const
{
  const Zone_CoviMAC& zone_CoviMAC = la_zone_CoviMAC.valeur();
  //const DoubleVect& volumes_entrelaces = zone_CoviMAC.volumes_entrelaces();
  const DoubleVect& p_f = zone_CoviMAC.porosite_face();
  const DoubleTab& vit = la_vitesse->valeurs();

  for (int i = 0, f; i < num_faces.size(); i++) if ((f = num_faces(i)) < zone_CoviMAC.nb_faces())
      {
        double Ud = vit(f) * p_f(f),
               surf = direction_perte_charge() < 0 ? zone_CoviMAC.face_surfaces(f) : dabs(zone_CoviMAC.face_normales(f,direction_perte_charge())); // Taking account of inclined plane
        resu(f) -= 0.5 * surf * p_f(f) * K() * Ud * dabs(Ud);
      }
  return resu;
}

DoubleTab& Perte_Charge_Singuliere_CoviMAC_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

void Perte_Charge_Singuliere_CoviMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Zone_CoviMAC& zone_CoviMAC = la_zone_CoviMAC.valeur();
  const DoubleVect& p_f = zone_CoviMAC.porosite_face();
  const DoubleTab& vit = la_vitesse->valeurs();

  for (int i = 0, f; i < num_faces.size(); i++) if ((f = num_faces(i)) < zone_CoviMAC.nb_faces())
      {
        double Ud = vit(f) * p_f(f),
               surf = direction_perte_charge() < 0 ? zone_CoviMAC.face_surfaces(f) : dabs(zone_CoviMAC.face_normales(f,direction_perte_charge())); // Taking account of inclined plane
        matrice.coef(f, f) += surf * p_f(f) * K() * p_f(f) * dabs(Ud);
      }
}

void Perte_Charge_Singuliere_CoviMAC_Face::mettre_a_jour(double temps)
{
  Perte_Charge_CoviMAC_Face::mettre_a_jour(temps);
  update_K(equation(), calculate_Q(equation(), num_faces, sgn), bilan());
}
