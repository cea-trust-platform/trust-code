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
// File:        EDO_Pression_th_VEF.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible/VEF
// Version:     /main/36
//
//////////////////////////////////////////////////////////////////////////////

#include <EDO_Pression_th_VEF.h>
#include <Domaine.h>
#include <Zone_Cl_VEF.h>
#include <Les_Cl.h>
#include <Zone_VEF_PreP1b.h>
#include <Sortie_libre_pression_imposee_QC.h>
#include <Debog.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Loi_Etat_Melange_GP.h>
#include <Porosites_champ.h>
#include <Zone_VEF.h>
#include <Champ_P1NC.h>
#include <Check_espace_virtuel.h>
Implemente_base(EDO_Pression_th_VEF,"EDO_Pression_th_VEF",EDO_Pression_th_base);


Sortie& EDO_Pression_th_VEF::printOn(Sortie& os) const
{
  return EDO_Pression_th_base::printOn(os);
}
Entree& EDO_Pression_th_VEF::readOn(Entree& is)
{
  return EDO_Pression_th_base::readOn(is);
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
void  EDO_Pression_th_VEF::associer_zones(const Zone_dis& zone, const Zone_Cl_dis& zone_cl)
{
  la_zone = ref_cast(Zone_VEF,zone.valeur());
  la_zone_Cl = zone_cl;
}

// Description:
//    Complete l'EDO : calcule rho sur les faces
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void EDO_Pression_th_VEF::completer()
{
  if (!ref_cast(Zone_VEF_PreP1b,la_zone.valeur()).get_alphaE())
    {
      Cerr << "Le modele quasi compressible ne fonctionne pas encore avec cette discretisation." << finl;
      Cerr << "En VEF, la discretisation doit avoir le support P0. Donc utiliser P1Bulle ou P0P1." << finl;
      Process::exit();
    }
  const DoubleTab& tab_ICh = le_fluide_->inco_chaleur().valeurs();
  double Pth=le_fluide_->pression_th();
  M0=masse_totale(Pth,tab_ICh);
  le_fluide_->checkTraitementPth(la_zone_Cl);
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
double EDO_Pression_th_VEF::moyenne_vol(const DoubleTab& tab) const
{
  assert(tab.line_size() == 1);
  double x = Champ_P1NC::calculer_integrale_volumique(la_zone.valeur(), tab, FAUX_EN_PERIO);
  // La facon simple de faire serait celle-ci, mais c'est faux a cause de FAUX_EN_PERIO
  //  qui compte deux fois les volumes entrelaces des faces periodiques:
  // double y = la_zone.valeur().zone().volume_total();
  DoubleVect un;
  la_zone.valeur().creer_tableau_faces(un, Array_base::NOCOPY_NOINIT);
  un = 1.;
  double y = Champ_P1NC::calculer_integrale_volumique(la_zone.valeur(), un, FAUX_EN_PERIO);
  return x / y;
}

// Description:
// Div(u) is computed on faces from Div(u) on elements
void EDO_Pression_th_VEF::divu_discvit(const DoubleTab& DivVelocityElements, DoubleTab& DivVelocityFaces)
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
//    Calcule le second membre de l'equation de continuite :
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
void EDO_Pression_th_VEF::secmembre_divU_Z(DoubleTab& tab_W) const
{
  return secmembre_divU_Z_VEFP1B(tab_W);
}

void EDO_Pression_th_VEF::calculer_grad(const DoubleTab& inco, DoubleTab& grad)
{
  assert(0);
  const IntTab& face_voisins = la_zone->face_voisins();
  const DoubleTab& face_normales = la_zone->face_normales();
  const DoubleVect& volumes_entrelaces = la_zone->volumes_entrelaces();
  const DoubleVect& volumes_entrelaces_Cl = ref_cast(Zone_Cl_VEF,la_zone_Cl->valeur()).volumes_entrelaces_Cl();

  double diff;
  int face,comp1;

  // Gradient d'un champ scalaire localise aux centres des elements
  // On initialise grad a zero
  grad = 0;
  if (1==1)
    {
      int elem1,elem2;
      // On traite les faces des joints:
      for (face=0; face<la_zone->nb_faces_joint(); face++)
        {
          elem1 = face_voisins(face,0);
          if (elem1==-1)
            elem1 = face_voisins(face,1);
          diff = -inco[elem1];
          for (comp1=0; comp1<dimension; comp1++)
            grad(face,comp1) = diff*face_normales(face,comp1);
        }

      // On traite les conditions limites
      // Seule la condition aux limites de type Periodicite modifie le gradient
      for (int n_bord=0; n_bord<la_zone->nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = la_zone_Cl->les_conditions_limites(n_bord);
          if (sub_type(Periodique,la_cl.valeur()))
            {
              //      const Periodique& la_cl_perio = (Periodique&) la_cl.valeur();
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              for (face=num1; face<num2; face++)
                {
                  elem1 = face_voisins(face,0);
                  elem2 = face_voisins(face,1);
                  diff = inco[elem2] - inco[elem1];
                  for (int comp2=0; comp2<dimension; comp2++)
                    grad(face,comp2) = diff*face_normales(face,comp2);
                }
            }
        }

      // On traite les faces internes
      //    Cerr << "Faces internes." << finl;
      for (face=la_zone->premiere_face_int(); face<la_zone->nb_faces(); face++)
        {
          elem1 = face_voisins(face,0);
          elem2 = face_voisins(face,1);
          diff = inco[elem2] - inco[elem1];
          for (int comp=0; comp<dimension; comp++)
            grad(face,comp) = diff*face_normales(face,comp);
        }
      // On divise par le volume!!!
      int premiere_fac_std=la_zone->premiere_face_std();
      for (face=0; face<premiere_fac_std; face++)
        {
          if (volumes_entrelaces_Cl(face)!=0)
            for (int comp=0; comp<dimension; comp++)
              grad(face,comp) /=volumes_entrelaces_Cl(face);
          else
            for (int comp=0; comp<dimension; comp++)
              grad(face,comp) =0.;
        }
      for (face=premiere_fac_std; face<la_zone->nb_faces(); face++)
        {
          for (int comp=0; comp<dimension; comp++)
            grad(face,comp) /=volumes_entrelaces(face);
        }
    }
  else
    {
      if (Objet_U::dimension==2)
        {
          int elem, nb_som = la_zone->zone().nb_som();
          int som,nsom, nse = la_zone->zone().nb_som_elem();
          const IntTab& som_elem = la_zone->zone().les_elems();
          int s0,s1,elem0,elem1;
          const IntTab& face_sommets = la_zone->face_sommets();
          const DoubleTab& coord_sommets = la_zone->zone().domaine().coord_sommets();
          const DoubleTab& xp = la_zone->xp();
          //    const DoubleTab& xv = la_zone->xv();
          DoubleTab incosom(nb_som);
          DoubleTab volsom(nb_som);
          incosom = 0;
          volsom = 0;
          double v,inc;
          //calcul de P aux sommets
          for (elem=0 ; elem<la_zone->nb_elem() ; elem++)
            {
              v = la_zone->volumes(elem);
              inc = inco[elem];
              for (som=0 ; som<nse ; som++)
                {
                  nsom = som_elem(elem,som);
                  incosom(nsom) += v*inc;
                  volsom(nsom) += v;
                }
            }
          for (nsom=0 ; nsom<nb_som ; nsom++)
            {
              incosom(nsom) /= volsom(nsom);
            }
          double xs0,ys0,xs1,ys1,xe0,ye0,xe1,ye1, ds,de;
          //faces de bord
          for (int n_bord=0; n_bord<la_zone->nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl = la_zone_Cl->les_conditions_limites(n_bord);

              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              for (face=num1; face<num2; face++)
                {
                  s0 = face_sommets(face,0);
                  s1 = face_sommets(face,1);
                  xs0 = coord_sommets(s0,0);
                  ys0 = coord_sommets(s0,1);
                  xs1 = coord_sommets(s1,0);
                  ys1 = coord_sommets(s1,1);
                  v = volumes_entrelaces(face);
                  ds = (incosom(s1) - incosom(s0))/((xs1-xs0)*(xs1-xs0) + (ys1-ys0)*(ys1-ys0));
                  grad(face,0) = v* (ds*(xs1-xs0));
                  grad(face,1) = v* (ds*(ys1-ys0));
                }
            }

          //faces internes
          for (face=la_zone->premiere_face_int() ; face<la_zone->nb_faces(); face++)
            {
              s0 = face_sommets(face,0);
              s1 = face_sommets(face,1);
              elem0 = face_voisins(face,0);
              elem1 = face_voisins(face,1);
              xs0 = coord_sommets(s0,0);
              ys0 = coord_sommets(s0,1);
              xs1 = coord_sommets(s1,0);
              ys1 = coord_sommets(s1,1);
              xe0 = xp(elem0,0);
              ye0 = xp(elem0,1);
              xe1 = xp(elem1,0);
              ye1 = xp(elem1,1);
              v = volumes_entrelaces(face);
              ds = (incosom(s1) - incosom(s0))/((xs1-xs0)*(xs1-xs0) + (ys1-ys0)*(ys1-ys0));
              de = (inco(elem1) - inco(elem0))/((xe1-xe0)*(xe1-xe0) + (ye1-ye0)*(ye1-ye0));
              grad(face,0) = v* (ds*(xs1-xs0) + de*(xe1-xe0));
              grad(face,1) = v* (ds*(ys1-ys0) + de*(ye1-ye0));
            }
        }
      else
        {
          Cerr<<"ATTENTION : gradient de l'inco mal calcule en 3D"<<finl;
        }
    }
}

// Description:
//    Calcule le second membre de l'equation de continuite pour une discretisation VEF_P1B:
//    W = -dZ/dT avec Z=rho
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
void EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B(DoubleTab& tab_W) const
{
  double dt = le_fluide().vitesse()->equation().schema_temps().pas_de_temps();
  int face;
  int elem;
  int som,som_glob;
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
  Debog::verifier("EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B tab_dZ=",tab_dZ);
  Debog::verifier("EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B tab_rhonP1=",tab_rhonP1);
  Debog::verifier("EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B tab_rhonp1P1=",tab_rhonp1P1);
  Debog::verifier("EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B dt=",dt);

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
  Debog::verifier("EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B tab_dZ=",tab_dZ);
  int p_has_som=zp1b.get_alphaS();
  nb_case=nb_som_tot*p_has_som;
  for (som=0 ; som<nb_case ; som++)
    {
      tab_dZ(decal+som) = ((tab_rhonp1_som(som))-(tab_rhon_som(som)))/dt;
    }
  decal+=nb_case;
  int p_has_arrete=zp1b.get_alphaA();
  int nb_ar_tot=la_zone->zone().nb_aretes_tot();
  nb_case=nb_ar_tot*p_has_arrete        ;
  for (int ar=0 ; ar<nb_case ; ar++)
    {
      tab_dZ(decal+ar) =0;
    }
  assert(decal+nb_case==tab_W.size_totale());
  tab_dZ.echange_espace_virtuel();
  Debog::verifier("EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B tab_dZ=",tab_dZ);

  double coefdivelem=1;
  double coefdivsom=1;

  decal=0;

  nb_case=nb_elem_tot*p_has_elem;
  for (elem=0 ; elem<nb_case ; elem++)
    {
      tab_W(elem) = -coefdivelem*tab_dZ(elem) * volumes(elem);
    }
  Debog::verifier("EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1BtabWelem",tab_W);
  decal+=nb_case;
  nb_case=nb_som_tot*p_has_som;
  for (som=0 ; som<nb_case ; som++)
    {
      //Corrections pour nouvelle formulation du quasi-compressible
      tab_W(decal+som) = -coefdivsom*tab_dZ(decal+som)*volumes_controle(som)*1;
    }
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
  Debog::verifier("EDO_Pression_th_VEF::secmembre_divU_Z_VEFP1B tab_W=",tab_W);
}

double EDO_Pression_th_VEF::masse_totale(double P,const DoubleTab& T)
{
  int nb_faces = la_zone->nb_faces();

  // assert(tab.dimension(0)==nb_faces);
  const Loi_Etat_base& loi_ = ref_cast(Loi_Etat_base,le_fluide_->loi_etat().valeur());

  DoubleVect tmp;
  tmp.copy(T, ArrOfDouble::NOCOPY_NOINIT); // just copy the structure
  if (!sub_type(Loi_Etat_Melange_GP,loi_))
    {
      for (int i = 0; i < nb_faces; i++)
        tmp[i] = loi_.calculer_masse_volumique(P, T[i]);
    }
  else
    {
      const Loi_Etat_Melange_GP& loi_mel_GP = ref_cast(Loi_Etat_Melange_GP,loi_);
      const DoubleTab& Masse_mol_mel = loi_mel_GP.masse_molaire();
      for (int i = 0; i < nb_faces; i++)
        {
          double r = 8.314 / Masse_mol_mel[i];
          tmp[i] = loi_mel_GP.calculer_masse_volumique_case(P, T[i], r,i);
        }
    }
  const double M = Champ_P1NC::calculer_integrale_volumique(la_zone.valeur(), tmp, FAUX_EN_PERIO);
  return M;
}

void EDO_Pression_th_VEF::mettre_a_jour_CL(double P)
{
  for (int n_bord=0; n_bord<la_zone->nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl->les_conditions_limites(n_bord);
      if (sub_type(Sortie_libre_pression_imposee_QC, la_cl.valeur()))
        {
          Sortie_libre_pression_imposee_QC& cl = ref_cast_non_const(Sortie_libre_pression_imposee_QC,la_cl.valeur());
          cl.set_Pth(P);
        }
    }
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
const DoubleTab& EDO_Pression_th_VEF::rho_discvit() const
{
  return le_fluide_->masse_volumique().valeurs();
}
// renvoie rho_n
const DoubleTab& EDO_Pression_th_VEF::rho_face_n() const
{
  return le_fluide_->rho_n();
}
// renvoie rho_n
const DoubleTab& EDO_Pression_th_VEF::rho_face_np1() const
{
  return le_fluide_->rho_np1();
}
