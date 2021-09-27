/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        EOS_Tools_VEF.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Common/VEF
// Version:     /main/36
//
//////////////////////////////////////////////////////////////////////////////

#include <EOS_Tools_VEF.h>
#include <Domaine.h>
#include <Zone_VEF_PreP1b.h>
#include <Debog.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Fluide_Dilatable_base.h>
#include <Loi_Etat_Melange_GP_base.h>
#include <Porosites_champ.h>
#include <Champ_P1NC.h>
#include <Check_espace_virtuel.h>

Implemente_instanciable(EOS_Tools_VEF,"EOS_Tools_VEF",EOS_Tools_base);

// Description:
//    Imprime sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& EOS_Tools_VEF::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

// Description:
//    Lecture sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree pour la lecture des parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& EOS_Tools_VEF::readOn(Entree& is)
{
  return is;
}

// Description:
//    Associe les zones a l'EDO
// Precondition:
// Parametre: Zone_dis& zone
//    Signification: zone
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: Zone_Cl_dis& zone_cl
//    Signification: zone cl
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: double
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void  EOS_Tools_VEF::associer_zones(const Zone_dis& zone, const Zone_Cl_dis& zone_cl)
{
  la_zone = ref_cast(Zone_VEF,zone.valeur());
  la_zone_Cl = zone_cl;
}

// Description:
//    Renvoie rho avec la meme discretisation que la vitesse :
//    une valeur par face en VEF
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: rho discretise par face
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
const DoubleTab& EOS_Tools_VEF::rho_discvit() const
{
  return le_fluide_->masse_volumique().valeurs();
}

const DoubleTab& EOS_Tools_VEF::rho_face_n() const
{
  return le_fluide_->rho_n();
}

const DoubleTab& EOS_Tools_VEF::rho_face_np1() const
{
  return le_fluide_->rho_np1();
}

// Description:
//    Calcule la moyenne volumique de la grandeur P1NC donnee
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: rho discretise par face
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
double EOS_Tools_VEF::moyenne_vol(const DoubleTab& tab) const
{
  assert(tab.line_size() == 1);
  double x = Champ_P1NC::calculer_integrale_volumique(la_zone.valeur(), tab, FAUX_EN_PERIO);
  // La facon simple de faire serait celle-ci, mais c'est faux a cause de FAUX_EN_PERIO
  //  qui compte deux fois les volumes entrelaces des faces periodiques:
  DoubleVect un;
  la_zone.valeur().creer_tableau_faces(un, Array_base::NOCOPY_NOINIT);
  un = 1.;
  double y = Champ_P1NC::calculer_integrale_volumique(la_zone.valeur(), un, FAUX_EN_PERIO);
  return x / y;
}

// Description:
// Div(u) is computed on faces from Div(u) on elements
void EOS_Tools_VEF::divu_discvit(const DoubleTab& DivVelocityElements, DoubleTab& DivVelocityFaces)
{
  assert_espace_virtuel_vect(DivVelocityElements);
  //Corrections pour moyenner div(u) sur les faces
  const DoubleVect& volumes = la_zone->volumes();
  const DoubleVect& volumes_entrelaces = la_zone->volumes_entrelaces();
  const DoubleVect& volumes_entrelaces_Cl = ref_cast(Zone_Cl_VEF,la_zone_Cl->valeur()).volumes_entrelaces_Cl();

  IntTab& face_voisins = la_zone->face_voisins();
  int premiere_fac_std=la_zone->premiere_face_std();

  //remplissage de div(u) sur les faces
  for (int face=0; face<premiere_fac_std; face++)
    {
      int nb_comp=0;
      DivVelocityFaces(face)=0;
      for (int i=0 ; i<2 ; i++)
        {
          int elem= face_voisins(face,i);
          if (elem!=-1)
            {
              nb_comp++;
              DivVelocityFaces(face) += DivVelocityElements(elem)*(volumes_entrelaces_Cl(face)/volumes(elem));
            }
        }
      DivVelocityFaces(face) /= nb_comp;
    }

  int size = la_zone->nb_faces();
  for (int face=premiere_fac_std; face<size; face++)
    {
      int nb_comp=0;
      DivVelocityFaces(face)=0;
      for (int i=0 ; i<2 ; i++)
        {
          int elem= face_voisins(face,i);
          if (elem!=-1)
            {
              nb_comp++;
              DivVelocityFaces(face) += DivVelocityElements(elem)*(volumes_entrelaces(face)/volumes(elem));
            }
        }
      DivVelocityFaces(face) /= nb_comp;
    }
}

// Description:
//    Calcule le second membre de l'equation de continuite pour une discretisation VEF_P1B:
//    div(U) = W = dZ/dT + U.grad(Z)    avec Z=ln(rho)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: rho discretise par face
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void EOS_Tools_VEF::secmembre_divU_Z(DoubleTab& tab_W) const
{
  double dt = le_fluide().vitesse()->equation().schema_temps().pas_de_temps();
  int face, elem, som,som_glob;
  const IntTab& face_sommets = la_zone->face_sommets();
  int nb_elem_tot=la_zone->nb_elem_tot();
  int nb_som_tot=la_zone->zone().nb_som_tot();
  int nb_faces_tot=la_zone->nb_faces_tot();
  const Equation_base& eq=le_fluide().vitesse()->equation();
  DoubleTab tab_dZ;

  // Dimensionnement de tab_dZ
  const Navier_Stokes_std& eqns = ref_cast(Navier_Stokes_std,eq);
  const DoubleTab& pression = eqns.pression().valeurs();
  tab_dZ = pression;

  DoubleVect tab_rhon_som(nb_som_tot);
  DoubleVect volume_int_som(nb_som_tot);
  DoubleVect tab_rhonp1_som(nb_som_tot);

  const DoubleTab& tab_rhon = le_fluide().loi_etat()->rho_n();
  const DoubleTab& tab_rhonp1 = le_fluide().loi_etat()->rho_np1();
  DoubleTab tab_rhonP1_,tab_rhonp1P1_;
  const DoubleVect& porosite_face = la_zone->porosite_face();
  const DoubleTab& tab_rhonP1=modif_par_porosite_si_flag(tab_rhon,tab_rhonP1_,1,porosite_face);
  const DoubleTab& tab_rhonp1P1=modif_par_porosite_si_flag(tab_rhonp1,tab_rhonp1P1_,1,porosite_face);
  const IntTab& elem_faces = la_zone->elem_faces();
  const DoubleVect& volumes = la_zone->volumes();
  const DoubleVect& volumes_entrelaces = la_zone->volumes_entrelaces();
  const Zone_VEF_PreP1b& zp1b=ref_cast(Zone_VEF_PreP1b,la_zone.valeur());
  const DoubleVect& volumes_controle=zp1b.volume_aux_sommets();

  double rn,rnp1;
  int nfe = la_zone->zone().nb_faces_elem();
  int nsf = la_zone->nb_som_face();
  const Domaine& dom=la_zone->zone().domaine();

  // calcul de la somme des volumes entrelacees autour d'un sommet
  volume_int_som=0.;
  for (face=0 ; face<nb_faces_tot ; face++)
    for (som=0 ; som<nsf ; som++)
      {
        som_glob=dom.get_renum_som_perio(face_sommets(face,som));
        volume_int_som(som_glob)+=volumes_entrelaces(face);
      }
  //discretisation de rho sur les sommets
  tab_rhon_som=0;
  tab_rhonp1_som=0;

  double pond;
  pond=1./nsf; // version_originale
  for (face=0 ; face<nb_faces_tot ; face++)
    {
      for (som=0 ; som<nsf ; som++)
        {
          som_glob=dom.get_renum_som_perio(face_sommets(face,som));
          pond=volumes_entrelaces(face)/volume_int_som(som_glob);
          tab_rhon_som(som_glob) += tab_rhonP1(face)*pond;
          tab_rhonp1_som(som_glob) += tab_rhonp1P1(face)*pond;
        }
    }

  //Corrections pour test de la moyenne de la derivee de la masse volumique
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_dZ=",tab_dZ);
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_rhonP1=",tab_rhonP1);
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_rhonp1P1=",tab_rhonp1P1);
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z dt=",dt);

  int decal=0;
  int p_has_elem=zp1b.get_alphaE();
  int nb_case=nb_elem_tot*p_has_elem;
  for (elem=0 ; elem<nb_case ; elem++)
    {
      rn=0;
      rnp1=0;
      for (face=0 ; face<nfe ; face++)
        {
          rn += tab_rhonP1(elem_faces(elem,face));
          rnp1 += tab_rhonp1P1(elem_faces(elem,face));
        }
      tab_dZ(elem) = (rnp1-rn)/(nfe*dt);
    }

  decal+=nb_case;
  tab_dZ.echange_espace_virtuel();

  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_dZ=",tab_dZ);
  int p_has_som=zp1b.get_alphaS();
  nb_case=nb_som_tot*p_has_som;

  for (som=0 ; som<nb_case ; som++)
    tab_dZ(decal+som) = ((tab_rhonp1_som(som))-(tab_rhon_som(som)))/dt;

  decal+=nb_case;
  int p_has_arrete=zp1b.get_alphaA();
  int nb_ar_tot=la_zone->zone().nb_aretes_tot();
  nb_case=nb_ar_tot*p_has_arrete;

  for (int ar=0 ; ar<nb_case ; ar++)
    tab_dZ(decal+ar) =0;

  assert(decal+nb_case==tab_W.size_totale());
  tab_dZ.echange_espace_virtuel();
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_dZ=",tab_dZ);

  double coefdivelem=1,coefdivsom=1;
  decal=0;
  nb_case=nb_elem_tot*p_has_elem;

  for (elem=0 ; elem<nb_case ; elem++)
    tab_W(elem) = -coefdivelem*tab_dZ(elem) * volumes(elem);

  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tabW elem",tab_W);
  decal+=nb_case;
  nb_case=nb_som_tot*p_has_som;

  for (som=0 ; som<nb_case ; som++)
    tab_W(decal+som) = -coefdivsom*tab_dZ(decal+som)*volumes_controle(som)*1;

  decal+=nb_case;
  nb_case=nb_ar_tot*p_has_arrete;
  for (int ar=0 ; ar<nb_case ; ar++)
    {
      //pour l'instant on n'a pas calcule tab_dZ aux aretes
      assert(tab_dZ(decal+ar)==0);
      tab_W(decal+ar) =0;
    }

  assert(decal+nb_case==tab_W.size_totale());
  tab_W.echange_espace_virtuel();
  Debog::verifier("EOS_Tools_VEF::secmembre_divU_Z tab_W=",tab_W);
}
