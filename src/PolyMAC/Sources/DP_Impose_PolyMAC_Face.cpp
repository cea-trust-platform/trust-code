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
// File:        DP_Impose_PolyMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <DP_Impose_PolyMAC_Face.h>
#include <Zone_PolyMAC.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Champ_Don_base.h>
#include <cfloat>

Implemente_instanciable(DP_Impose_PolyMAC_Face,"DP_Impose_Face_PolyMAC",Perte_Charge_PolyMAC_Face);

//// printOn
//

Sortie& DP_Impose_PolyMAC_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& DP_Impose_PolyMAC_Face::readOn(Entree& s)
{
  DP_Impose::lire_donnees(s);
  remplir_num_faces(s);
  if (!mp_max(sgn.size()))
    Cerr << "DP_Impose_PolyMAC_Face : champ d'orientation non renseigne!" << finl, Process::exit();
  //fichier de sortie
  set_fichier(Nom("DP_") + identifiant_);
  set_description(Nom("DP impose sur la surface ") + identifiant_ + "\nt DP dDP/dQ Q Q0");
  return s;
}


/////////////////////////////////////////////////////////////////////
//
//                    Implementation des fonctions
//
//               de la classe DP_Impose_PolyMAC_Face
//
////////////////////////////////////////////////////////////////////

void DP_Impose_PolyMAC_Face::remplir_num_faces(Entree& s)
{
  const Domaine& le_domaine = equation().probleme().domaine();
  const Zone_PolyMAC& zone_PolyMAC = ref_cast(Zone_PolyMAC,equation().zone_dis().valeur());
  int taille_bloc = zone_PolyMAC.nb_elem();
  num_faces.resize(taille_bloc);
  lire_surfaces(s,le_domaine,zone_PolyMAC,num_faces, sgn);
  // int nfac_tot = mp_sum(num_faces.size());
  int nfac_max = (int)mp_max(num_faces.size()); // not to count several (number of processes) times the same face

  if(je_suis_maitre() && nfac_max == 0)
    {
      Cerr << "Error when defining the surface plane for the singular porosity :" << finl;
      Cerr << "No mesh faces has been found for the surface plane." << finl;
      Cerr << "Check the coordinate of the surface plane which should match mesh coordinates." << finl;
      exit();
    }

  DoubleTrav S;
  zone_PolyMAC.creer_tableau_faces(S);
  for (int i = 0; i < num_faces.size(); i++) S(num_faces(i)) = zone_PolyMAC.face_surfaces(num_faces(i));
  surf = mp_somme_vect(S);
}

DoubleTab& DP_Impose_PolyMAC_Face::ajouter(DoubleTab& resu) const
{
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  const DoubleVect& pf = zone_PolyMAC.porosite_face(), &fs = zone_PolyMAC.face_surfaces();
  const DoubleTab& vit = equation().inconnue().valeurs();

  //valeurs du champ de DP
  DoubleTrav xvf(num_faces.size(), dimension), DP(num_faces.size(), 3);
  for (int i = 0; i < num_faces.size(); i++) for (int j = 0; j < dimension; j++) xvf(i, j) = zone_PolyMAC.xv()(num_faces(i), j);
  DP_.valeur().valeur_aux(xvf, DP);

  double rho = equation().milieu().masse_volumique().valeur()(0, 0), fac_rho = equation().probleme().is_QC() ? 1.0 : 1.0 / rho;

  for (int i = 0, f; i < num_faces.size(); i++) if ((f = num_faces(i)) < zone_PolyMAC.nb_faces())
      resu(f) += fs(f) * pf(f) * sgn(i) * (DP(i, 0) + DP(i, 1) * (surf * sgn(i) * vit(f) - DP(i, 2))) * fac_rho;

  bilan().resize(4); //DP dDP/dQ Q Q0
  bilan()(0) = Process::mp_max(num_faces.size() ? DP(0, 0)       : -DBL_MAX);
  bilan()(1) = Process::mp_max(num_faces.size() ? DP(0, 1) / rho : -DBL_MAX);
  bilan()(3) = Process::mp_max(num_faces.size() ? DP(0, 2) * rho : -DBL_MAX);
  if (Process::me()) bilan() = 0; //pour eviter un sommage en sortie
  return resu;
}

void DP_Impose_PolyMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& mat) const
{
  const Zone_PolyMAC& zone_PolyMAC = la_zone_PolyMAC.valeur();
  const DoubleVect& pf = zone_PolyMAC.porosite_face(), &fs = zone_PolyMAC.face_surfaces();

  //valeurs du champ de DP
  DoubleTrav xvf(num_faces.size(), dimension), DP(num_faces.size(), 3);
  for (int i = 0; i < num_faces.size(); i++) for (int j = 0; j < dimension; j++) xvf(i, j) = zone_PolyMAC.xv()(num_faces(i), j);
  DP_.valeur().valeur_aux(xvf, DP);

  double rho = equation().milieu().masse_volumique().valeur()(0, 0), fac_rho = equation().probleme().is_QC() ? 1.0 : 1.0 / rho;
  for (int i = 0, f; i < num_faces.size(); i++) if ((f = num_faces(i)) < zone_PolyMAC.nb_faces())
      mat(f, f) -= fs(f) * pf(f) * DP(i, 1) * surf * fac_rho;
}

DoubleTab& DP_Impose_PolyMAC_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}

void DP_Impose_PolyMAC_Face::mettre_a_jour(double temps)
{
  DP_Impose::mettre_a_jour(temps);
  bilan().resize(4); //DP dDP/dQ Q Q0
  bilan()(2) = calculate_Q(equation(), num_faces, sgn) * (Process::me() ? 0 : 1); //pour eviter le sommage en sortie
}
