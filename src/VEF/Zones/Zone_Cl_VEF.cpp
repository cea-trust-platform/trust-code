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
// File:        Zone_Cl_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Zones
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////

#include <Zone_Cl_VEF.h>
#include <Zone_VEF.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_entree_fluide.h>
#include <Scalaire_impose_paroi.h>
#include <Neumann_sortie_libre.h>
#include <Champ_P0_VEF.h>
#include <Champ_P1NC.h>
#include <Domaine.h>
#include <Tri_VEF.h>
#include <Tetra_VEF.h>
#include <Quadri_VEF.h>
#include <Hexa_VEF.h>
#include <Hexaedre_VEF.h>
#include <Debog.h>
#include <MD_Vector_tools.h>
#include <Check_espace_virtuel.h>

Implemente_instanciable(Zone_Cl_VEF,"Zone_Cl_VEF",Zone_Cl_dis_base);

int Deux_Puissance(int n)
{
  switch(n)
    {
    case 0 :
      return 1;
    case 1 :
      return 2;
    case 2 :
      return 4;
    case 3 :
      return 8;
    default:
      return 8*Deux_Puissance((int)(n-3));
    }
}

//// printOn
//

Sortie& Zone_Cl_VEF::printOn(Sortie& os ) const
{
  os << volumes_entrelaces_Cl_;
  os << normales_facettes_Cl_;
  os << type_elem_Cl_;
  return os;
}

//// readOn
//

Entree& Zone_Cl_VEF::readOn(Entree& is )
{
  return Zone_Cl_dis_base::readOn(is) ;
}


/////////////////////////////////////////////////////////////////////
//
// Implementation des fonctions de la classe Zone_Cl_VEF
//
/////////////////////////////////////////////////////////////////////

// Description:
// etape de discretisation : dimensionnement des tableaux
void Zone_Cl_VEF::associer(const Zone_VEF& la_zone_VEF)
{
  int nb_faces_non_std = la_zone_VEF.nb_faces_non_std();
  const Elem_VEF& type_elem= la_zone_VEF.type_elem();
  int nb_fa7_elem = type_elem.nb_facette();

  {
    IntVect renum;
    la_zone_VEF.creer_tableau_faces(renum, ArrOfInt::NOCOPY_NOINIT);
    renum = -1;
    // Marquer les faces non standard reelles (premieres faces dans la zone)
    int i;
    for (i = 0; i < nb_faces_non_std; i++)
      renum[i] = 0;
    // Echange espace virtuel => marquer les faces virtuelles non standard
    renum.echange_espace_virtuel();
    MD_Vector md_vect;
    MD_Vector_tools::creer_md_vect_renum_auto(renum, md_vect);
    MD_Vector_tools::creer_tableau_distribue(md_vect, volumes_entrelaces_Cl_, Array_base::NOCOPY_NOINIT);
  }
  {
    // Construction du descripteur pour le tableau des elements "Cl":
    MD_Vector md_vect;
    MD_Vector_tools::creer_md_vect_renum(la_zone_VEF.rang_elem_non_std(), md_vect);

    // Creation de tableaux aux elements "Cl":
    normales_facettes_Cl_.resize(0, nb_fa7_elem, dimension);
    MD_Vector_tools::creer_tableau_distribue(md_vect, normales_facettes_Cl_, Array_base::NOCOPY_NOINIT);

    vecteur_face_facette_Cl_.resize(0, nb_fa7_elem, dimension, 2);
    MD_Vector_tools::creer_tableau_distribue(md_vect, vecteur_face_facette_Cl_, Array_base::NOCOPY_NOINIT);

    MD_Vector_tools::creer_tableau_distribue(md_vect, type_elem_Cl_, Array_base::NOCOPY_NOINIT);
  }
}

// Description:
// remplissage des tableaux
void Zone_Cl_VEF::completer(const Zone_dis& une_zone_dis)
{
  modif_perio_fait_ =0;
  if (sub_type(Zone_VEF,une_zone_dis.valeur()))
    {
      const Zone_VEF& la_zone_VEF = ref_cast(Zone_VEF, une_zone_dis.valeur());
      remplir_type_elem_Cl(la_zone_VEF);
      remplir_volumes_entrelaces_Cl(la_zone_VEF);
      remplir_normales_facettes_Cl(la_zone_VEF);
    }
  else
    {
      cerr << "Zone_Cl_VEF::completer() prend comme argument une Zone_VEF\n";
      exit();
    }
}

// Description:
// appele par completer() : remplissage de type_elem_Cl_ et
// volumes_entrelaces_Cl_
void Zone_Cl_VEF::remplir_volumes_entrelaces_Cl(const Zone_VEF& la_zone_VEF)
{
  const DoubleVect& volumes = la_zone_VEF.volumes();
  const DoubleVect& volumes_entrelaces = la_zone_VEF.volumes_entrelaces();
  const IntVect& rang_elem = la_zone_VEF.rang_elem_non_std();

  // Initialisation du tableau volumes_entrelaces_Cl
  // a priori les faces non standard ne sont pas touchees par les C.L

  for (int i=0; i<la_zone_VEF.nb_faces_non_std(); i++)
    volumes_entrelaces_Cl_[i] = volumes_entrelaces(i);

  // Calcul des valeurs des volumes entrelaces modifiees par les C.L:

  int nb_poly_tot = la_zone_VEF.zone().nb_elem_tot();
  ArrOfInt poly_fait(nb_poly_tot);
  poly_fait = 0;
  for(int i=0; i<les_conditions_limites_.size(); i++)
    {
      const Cond_lim_base& la_cl=les_conditions_limites_[i].valeur();
      if ( (sub_type(Dirichlet,la_cl))
           ||
           (sub_type(Dirichlet_homogene,la_cl))
         )
        {
          const Front_VF& le_bord= la_zone_VEF.front_VF(i);
          const Frontiere& front=le_bord.frontiere();
          int premiere=le_bord.num_premiere_face();
          int derniere=premiere+le_bord.nb_faces();
          for(int j=premiere; j<derniere; j++)
            {
              volumes_entrelaces_Cl_[j] =0;
              int elem=la_zone_VEF.face_voisins(j,0);
              int n_poly=rang_elem(elem);
              if(!poly_fait[n_poly])
                {
                  poly_fait[n_poly]=1;
                  const Elem_VEF& type_elem= la_zone_VEF.type_elem();
                  type_elem.modif_volumes_entrelaces(j,elem,la_zone_VEF,
                                                     volumes_entrelaces_Cl(),
                                                     type_elem_Cl(n_poly)) ;
                }
            }
          // faces virtuelles de bord :
          const ArrOfInt& faces_virt = front.get_faces_virt();
          int nb_faces_virt = faces_virt.size_array();
          for (int ind_face=0; ind_face<nb_faces_virt; ind_face++)
            {
              int num_face = faces_virt[ind_face];
              int elem=la_zone_VEF.face_voisins(num_face,0);
              int n_poly=rang_elem(elem);
              if(n_poly!=-1)
                {
                  if(!poly_fait[n_poly])
                    {
                      poly_fait[n_poly]=1;
                      const Elem_VEF& type_elem= la_zone_VEF.type_elem();
                      type_elem.modif_volumes_entrelaces_faces_joints(num_face,elem,la_zone_VEF,
                                                                      volumes_entrelaces_Cl(),
                                                                      type_elem_Cl(n_poly)) ;
                    }
                }
            }
        }
    }
  {
    double sum_std=0, sum_cl=0, sum_tetra=0;
    for(int i=0; i<volumes_entrelaces.size_array(); i++)
      sum_std +=volumes_entrelaces(i);
    for(int i=0; i<volumes_entrelaces_Cl_.size_array(); i++)
      sum_cl +=volumes_entrelaces_Cl_[i];
    for(int i=0; i<la_zone_VEF.nb_elem(); i++)
      sum_tetra +=volumes(i);
  }
}

// Description:
// appele par completer() : remplissage de normales_facettes_Cl_
// et vecteur_face_caette_Cl_
// CHANGER LE NOM
void Zone_Cl_VEF::remplir_normales_facettes_Cl(const Zone_VEF& la_zone_VEF)
{
  const Zone& z = la_zone_VEF.zone();
  const Domaine& dom = z.domaine();
  const DoubleTab& les_coords =dom.coord_sommets();
  const IntTab& les_Polys = z.les_elems() ;
  const Elem_VEF& elemvef = la_zone_VEF.type_elem();
  const IntVect& rang_elem=la_zone_VEF.rang_elem_non_std();
  const IntTab& elem_faces = la_zone_VEF.elem_faces();
  const DoubleTab& xv=la_zone_VEF.xv();
  const IntTab& KEL=elemvef.valeur().KEL();


  int num_elem, fa7;
  int isom,ncomp;
  int idirichlet=-1,n1=-1,n2=-1,n3=-1;
  int nb_elem = z.nb_elem();
  int nsom = z.nb_som_elem();
  int nfa7 = elemvef.nb_facette();
  //int nb_som_facette = zone().type_elem().nb_som_face();
  //if ( sub_type(Hexaedre_VEF,zone().type_elem().valeur())) { nb_som_facette--; }
  int nb_som_facette=dimension;

  IntVect num_som(nsom);
  DoubleTab x(nsom,dimension);
  DoubleVect xg(dimension);
  DoubleVect xj0(dimension);
  DoubleVect u(dimension);
  DoubleVect v(dimension);

  // Calcul des valeurs des normales aux facettes modifiees par les C.L:

  for (int elem=0; elem<nb_elem; elem++)
    {

      if (rang_elem(elem)!= -1)
        {
          num_elem = rang_elem(elem);

          // numero des sommets du polyedre:
          for (isom = 0; isom<nsom; isom++)
            num_som[isom] = les_Polys(elem,isom);


          // Calcul des coordonnees des sommets du polyedre:
          for (isom = 0; isom<nsom; isom++)
            for (ncomp =0; ncomp<dimension; ncomp++)
              x(isom,ncomp) = les_coords(num_som[isom],ncomp);


          // Calcul des coordonnees du point G en fonction du type du polyedre
          // idirichlet= nb de faces de dirichlet de l'elem
          // pour triangle si idirichlet=2, n1=sommet confondu avec G
          // pour quadrangle se reporter a Quadri_VEF.cpp

          elemvef.calcul_xg(xg,x,type_elem_Cl_[num_elem],
                            idirichlet,n1,n2,n3);
          // Calcul des valeurs du tableau normales_facettes_Cl:


          for (fa7=0; fa7<nfa7; fa7++)
            {
              elemvef.creer_normales_facettes_Cl(normales_facettes_Cl(),fa7, num_elem,x,xg,z);
              // Correction des valeurs du tableau normales_facettes_Cl:
              elemvef.modif_normales_facettes_Cl(normales_facettes_Cl(),fa7,num_elem,idirichlet,n1,n2,n3);
              int num1= elem_faces(elem,KEL(0,fa7));
              int num2= elem_faces(elem,KEL(1,fa7));
              for (int dir=0; dir<dimension; dir++)
                {
                  double coord_centre_fa7= xg(dir);


                  for (int num_som_fa7=0; num_som_fa7<nb_som_facette-1; num_som_fa7++)
                    {
                      int isom_loc = KEL(num_som_fa7+2,fa7);
                      int isom_glob = les_Polys(elem,isom_loc);
                      coord_centre_fa7 += les_coords(isom_glob,dir);
                    }
                  coord_centre_fa7 /= nb_som_facette;

                  vecteur_face_facette_Cl_(num_elem,fa7,dir,0)=coord_centre_fa7-xv(num1,dir);
                  vecteur_face_facette_Cl_(num_elem,fa7,dir,1)=coord_centre_fa7-xv(num2,dir);
                }

            }
        }
    }
  normales_facettes_Cl_.echange_espace_virtuel();
  vecteur_face_facette_Cl_.echange_espace_virtuel();
}

int trois_puissance(int n)
{
  switch(n)
    {
    case 0:
      return 1;
    case 1:
      return 3;
    case 2:
      return 9;
    case 3:
      return 27;
    default :
      return 27*trois_puissance(n-3);
    }
}

// Description:
// appele par remplir_volumes_entrelaces_Cl() : remplissage de type_elem_Cl_
void Zone_Cl_VEF::remplir_type_elem_Cl(const Zone_VEF& la_zone_VEF)
{
  const Zone& z = la_zone_VEF.zone();
  int nfac= z.type_elem().nb_faces();   // dans Elem_geom
  const IntTab& elem_faces = la_zone_VEF.elem_faces();
  const IntTab& face_voisins = la_zone_VEF.face_voisins();
  const IntVect& rang_elem = la_zone_VEF.rang_elem_non_std();
  type_elem_Cl_ = 0;
  int elem, num_elem;

  for(int i=0; i<les_conditions_limites_.size(); i++)
    {
      Cond_lim_base& la_cl=les_conditions_limites_[i].valeur();
      if ( (sub_type(Dirichlet,la_cl))
           ||
           (sub_type(Dirichlet_homogene,la_cl))
         )
        {
          const Front_VF& le_bord= la_zone_VEF.front_VF(i);
          int premiere = le_bord.num_premiere_face();
          int derniere = premiere+le_bord.nb_faces();
          for(int j=premiere; j<derniere; j++)
            {
              elem=face_voisins(j,0);
              int numero1,numero2 =-1;
              for(numero1=0; numero1<nfac; numero1++)
                if(elem_faces(elem,numero1) == j)
                  numero2 = numero1;
              if(numero2 == -1)
                cerr << "Zone_Cl_VEF::creer_type_elem_Cl() PAS POSSIBLE!!\n";
              num_elem = rang_elem(elem);
              assert(num_elem!=-1);
              if (sub_type(Tri_VEF,la_zone_VEF.type_elem().valeur()) || sub_type(Tetra_VEF,la_zone_VEF.type_elem().valeur()))
                type_elem_Cl_[num_elem]+= Deux_Puissance((int)(nfac-1-numero2));
              else if (sub_type(Quadri_VEF,la_zone_VEF.type_elem().valeur()) || sub_type(Hexa_VEF,la_zone_VEF.type_elem().valeur()) )
                type_elem_Cl_[num_elem]+= trois_puissance((int)(nfac-1-numero2));
              else
                {
                  Cerr << "erreur dans Zone_Cl_VEF::remplir_type_elem_Cl mauvais typage d'element " << finl;
                  exit();
                }

            }
        }
    }

  type_elem_Cl_.echange_espace_virtuel();
}


// Description:
// Impose les conditions aux limites a la valeur temporelle "temps" du
// Champ_Inc
void Zone_Cl_VEF::imposer_cond_lim(Champ_Inc& ch, double temps)
{
  DoubleTab& ch_tab = ch->valeurs(temps);
  if (sub_type(Champ_P0_VEF,ch.valeur()))
    {
      ;
    }
  else if (sub_type(Champ_P1NC,ch.valeur()) || sub_type(Champ_Q1NC,ch.valeur()))
    {
      int nb_comp;

      if (sub_type(Champ_P1NC,ch.valeur()))
        {
          const Champ_P1NC& chnc = ref_cast(Champ_P1NC,ch.valeur());
          nb_comp = chnc.nb_comp();
          int ndeb,nfin,num_face,ncomp;
          for(int i=0; i<nb_cond_lim(); i++)
            {
              const Cond_lim_base& la_cl = les_conditions_limites(i).valeur();
              if (sub_type(Periodique,la_cl))
                {

                  // On fait en sorte que le champ ait la meme valeur
                  // sur deux faces de periodicite qui sont en face l'une de l'autre
                  const Periodique& la_cl_perio = ref_cast(Periodique,la_cl);
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  int voisine;
                  double moy;
                  for (num_face=ndeb; num_face<nfin; num_face++)
                    {
                      voisine = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
                      if (nb_comp == 1)
                        {
                          if ( ch_tab[num_face] != ch_tab[voisine] )
                            {
                              moy = 0.5*(ch_tab[num_face] + ch_tab[voisine]);
                              ch_tab[num_face] = moy;
                              ch_tab[voisine] = moy;
                            }
                        }
                      else
                        {
                          for (ncomp=0; ncomp<nb_comp; ncomp++)
                            if ( ch_tab(num_face,ncomp) != ch_tab(voisine,ncomp) )
                              {
                                moy = 0.5*(ch_tab(num_face,ncomp) + ch_tab(voisine,ncomp));
                                ch_tab(num_face,ncomp) = moy;
                                ch_tab(voisine,ncomp) = moy;
                              }
                        }
                    }
                }
              else if ( (sub_type(Symetrie,la_cl) ) &&
                        (ch->nature_du_champ()==vectoriel) )
                {
                  const Zone_VEF& zvef = chnc.zone_vef();
                  const DoubleTab& face_normales = zvef.face_normales();
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp == dimension)
                    {
                      for (num_face=ndeb; num_face<nfin; num_face++)
                        {
                          double face_n, surf=0, flux=0;
                          for (ncomp=0; ncomp<nb_comp; ncomp++)
                            {
                              face_n = face_normales(num_face,ncomp);
                              flux += ch_tab(num_face,ncomp)*face_n;
                              surf += face_n*face_n;
                            }
                          // flux /= surf; // Fixed bug: Arithmetic exception
                          if (dabs(surf)>=DMINFLOAT) flux /= surf;
                          for (ncomp=0; ncomp<nb_comp; ncomp++)
                            ch_tab(num_face,ncomp) -= flux*face_normales(num_face,ncomp);
                        }
                    }
                }
              else if ( sub_type(Dirichlet_entree_fluide,la_cl) )
                {
                  const Dirichlet_entree_fluide& la_cl_diri = ref_cast(Dirichlet_entree_fluide,la_cl);
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp == 1)
                    for (num_face=ndeb; num_face<nfin; num_face++)
                      ch_tab[num_face] = la_cl_diri.val_imp_au_temps(temps,num_face-ndeb);
                  else
                    {
                      for (ncomp=0; ncomp<nb_comp; ncomp++)
                        for (num_face=ndeb; num_face<nfin; num_face++)
                          ch_tab(num_face,ncomp) =la_cl_diri.val_imp_au_temps(temps,num_face-ndeb,ncomp);
                    }
                }
              else if ( sub_type(Scalaire_impose_paroi,la_cl) )
                {
                  const Scalaire_impose_paroi& la_cl_diri = ref_cast(Scalaire_impose_paroi,la_cl);
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp == 1)
                    for (num_face=ndeb; num_face<nfin; num_face++)
                      {
                        ch_tab[num_face] = la_cl_diri.val_imp_au_temps(temps,num_face-ndeb);
                      }
                  else
                    {
                      for (ncomp=0; ncomp<nb_comp; ncomp++)
                        for (num_face=ndeb; num_face<nfin; num_face++)
                          ch_tab(num_face,ncomp) =la_cl_diri.val_imp_au_temps(temps,num_face-ndeb,ncomp);
                    }
                }
              else if ( (sub_type(Dirichlet_paroi_fixe,la_cl) ) &&
                        (ch->nature_du_champ()==multi_scalaire) )
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp == 1)
                    for (num_face=ndeb; num_face<nfin; num_face++)
                      ch_tab[num_face] = 0;
                  else
                    {
                      for (ncomp=0; ncomp<nb_comp; ncomp++)
                        for (num_face=ndeb; num_face<nfin; num_face++)
                          {
                            ch_tab(num_face,0) = 0;
                            ch_tab(num_face,1) = 0;
                          }
                    }
                }
              else if ( sub_type(Dirichlet_paroi_fixe,la_cl) )
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp == 1)
                    for (num_face=ndeb; num_face<nfin; num_face++)
                      ch_tab[num_face] = 0;
                  else
                    {
                      for (ncomp=0; ncomp<nb_comp; ncomp++)
                        for (num_face=ndeb; num_face<nfin; num_face++)
                          ch_tab(num_face,ncomp) = 0;
                    }
                }
              else if ( sub_type(Dirichlet_paroi_defilante,la_cl) )
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  const Dirichlet_paroi_defilante& la_cl_diri=ref_cast(Dirichlet_paroi_defilante,la_cl);
                  const DoubleTab& face_normales = chnc.zone_vef().face_normales();
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp != dimension)
                    {
                      Cerr << "Cas non prevu dans Zone_Cl_VEF::imposer_cond_lim." << finl;
                      exit();
                    }
                  else
                    {
                      for (num_face=ndeb; num_face<nfin; num_face++)
                        {
                          double surf=0, flux=0;
                          for (ncomp=0; ncomp<nb_comp; ncomp++)
                            {
                              double face_n = face_normales(num_face,ncomp);
                              flux += la_cl_diri.val_imp_au_temps(temps,num_face-ndeb, ncomp)*face_n;
                              surf += face_n*face_n;
                            }
                          // flux /= surf; // Fixed bug: Arithmetic exception
                          if (dabs(surf)>=DMINFLOAT) flux /= surf;
                          for (ncomp=0; ncomp<nb_comp; ncomp++)
                            ch_tab(num_face,ncomp) = la_cl_diri.val_imp_au_temps(temps,num_face-ndeb, ncomp) - flux * face_normales(num_face,ncomp);
                        }
                    }
                }
            }
        }
      else if (sub_type(Champ_Q1NC,ch.valeur()))
        {
          const Champ_Q1NC& chnc = ref_cast(Champ_Q1NC,ch.valeur());
          nb_comp = chnc.nb_comp();
          int ndeb,nfin,num_face,ncomp;
          for(int i=0; i<nb_cond_lim(); i++)
            {
              const Cond_lim_base& la_cl = les_conditions_limites(i).valeur();
              if (sub_type(Periodique,la_cl))
                {
                  // On fait en sorte que le champ ait la meme valeur
                  // sur deux faces de periodicite qui sont en face l'une de l'autre
                  const Periodique& la_cl_perio = ref_cast(Periodique,la_cl);
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  int voisine;
                  double moy;
                  for (num_face=ndeb; num_face<nfin; num_face++)
                    {
                      voisine = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
                      if (nb_comp == 1)
                        {
                          if ( ch_tab[num_face] != ch_tab[voisine] )
                            {
                              moy = 0.5*(ch_tab[num_face] + ch_tab[voisine]);
                              ch_tab[num_face] = moy;
                              ch_tab[voisine] = moy;
                            }
                        }
                      else
                        {
                          for (ncomp=0; ncomp<nb_comp; ncomp++)
                            if ( ch_tab(num_face,ncomp) != ch_tab(voisine,ncomp) )
                              {
                                moy = 0.5*(ch_tab(num_face,ncomp) + ch_tab(voisine,ncomp));
                                ch_tab(num_face,ncomp) = moy;
                                ch_tab(voisine,ncomp) = moy;
                              }
                        }
                    }
                }
              else if ( (sub_type(Symetrie,la_cl) ) &&
                        (ch->nature_du_champ()==vectoriel) )
                {
                  const Zone_VEF& zvef = chnc.zone_vef();
                  const DoubleTab& face_normales = zvef.face_normales();
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp == dimension)
                    {
                      for (num_face=ndeb; num_face<nfin; num_face++)
                        {
                          double face_n, surf=0, flux=0;
                          for (ncomp=0; ncomp<nb_comp; ncomp++)
                            {
                              face_n = face_normales(num_face,ncomp);
                              flux += ch_tab(num_face,ncomp)*face_n;
                              surf += face_n*face_n;
                            }
                          // flux /= surf; // Fixed bug: Arithmetic exception
                          if (dabs(surf)>=DMINFLOAT) flux /= surf;
                          for (ncomp=0; ncomp<nb_comp; ncomp++)
                            ch_tab(num_face,ncomp) -= flux*face_normales(num_face,ncomp);
                        }
                    }
                }
              else if ( sub_type(Dirichlet_entree_fluide,la_cl) )
                {
                  const Dirichlet_entree_fluide& la_cl_diri = ref_cast(Dirichlet_entree_fluide,la_cl);
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp == 1)
                    for (num_face=ndeb; num_face<nfin; num_face++)
                      ch_tab[num_face] = la_cl_diri.val_imp_au_temps(temps,num_face-ndeb);
                  else
                    {
                      for (ncomp=0; ncomp<nb_comp; ncomp++)
                        for (num_face=ndeb; num_face<nfin; num_face++)
                          ch_tab(num_face,ncomp) =la_cl_diri.val_imp_au_temps(temps,num_face-ndeb,ncomp);
                    }
                }
              else if ( sub_type(Scalaire_impose_paroi,la_cl) )
                {
                  const Scalaire_impose_paroi& la_cl_diri = ref_cast(Scalaire_impose_paroi,la_cl);
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp == 1)
                    for (num_face=ndeb; num_face<nfin; num_face++)
                      {
                        ch_tab[num_face] = la_cl_diri.val_imp_au_temps(temps,num_face-ndeb);
                      }
                  else
                    {
                      for (ncomp=0; ncomp<nb_comp; ncomp++)
                        for (num_face=ndeb; num_face<nfin; num_face++)
                          ch_tab(num_face,ncomp) =la_cl_diri.val_imp_au_temps(temps,num_face-ndeb,ncomp);
                    }
                }
              else if ( sub_type(Dirichlet_paroi_fixe,la_cl) )
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp == 1)
                    for (num_face=ndeb; num_face<nfin; num_face++)
                      ch_tab[num_face] = 0;
                  else
                    {
                      for (ncomp=0; ncomp<nb_comp; ncomp++)
                        for (num_face=ndeb; num_face<nfin; num_face++)
                          ch_tab(num_face,ncomp) = 0;
                    }
                }
              else if ( sub_type(Dirichlet_paroi_defilante,la_cl) )
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  const Dirichlet_paroi_defilante& la_cl_diri=ref_cast(Dirichlet_paroi_defilante,la_cl);
                  ndeb = le_bord.num_premiere_face();
                  nfin = ndeb + le_bord.nb_faces();
                  if (nb_comp == 1)
                    for (num_face=ndeb; num_face<nfin; num_face++)
                      ch_tab[num_face] = la_cl_diri.val_imp_au_temps(temps,num_face-ndeb);
                  else
                    {
                      for (ncomp=0; ncomp<nb_comp; ncomp++)
                        for (num_face=ndeb; num_face<nfin; num_face++)
                          ch_tab(num_face,ncomp) =la_cl_diri.val_imp_au_temps(temps,num_face-ndeb, ncomp);
                    }
                }
            }
        }
    }
  else
    {
      Cerr << "Le type de Champ_Inc " << ch->que_suis_je() << " n'est pas prevu en VEF\n" << finl;

      exit();
    }
  ch_tab.echange_espace_virtuel();
  //Debog::verifier("Zone_Cl_VEF::imposer_cond_lim ch_tab",ch_tab);
}

int Zone_Cl_VEF::nb_faces_sortie_libre() const
{
  int compteur=0;
  for(int cl=0; cl<les_conditions_limites_.size(); cl++)
    {
      if(sub_type(Neumann_sortie_libre, les_conditions_limites_[cl].valeur()))
        {
          const Front_VF& le_bord=ref_cast(Front_VF,les_conditions_limites_[cl]->frontiere_dis());
          compteur+=le_bord.nb_faces();
        }
    }
  return compteur;
}

int Zone_Cl_VEF::nb_bord_periodicite() const
{
  int compteur=0;
  for(int cl=0; cl<les_conditions_limites_.size(); cl++)
    {
      if (sub_type(Periodique,les_conditions_limites_[cl].valeur()))
        compteur++;
    }
  return compteur;
}

int Zone_Cl_VEF::initialiser(double temps)
{
  Zone_Cl_dis_base::initialiser(temps);

  if (nb_bord_periodicite()>0)
    {
      if (modif_perio_fait_ == 0)
        {
          Cerr <<"modification de la Zone_Cl_VEF pour periodicite" << finl;
          const Zone_VEF& la_zone_VEF = ref_cast(Zone_VEF,zone_dis().valeur());
          const DoubleVect& volumes_entrelaces = la_zone_VEF.volumes_entrelaces();
          remplir_volumes_entrelaces_Cl(la_zone_VEF);
          for(int i=0; i<les_conditions_limites_.size(); i++)
            {
              const Cond_lim_base& la_cl=les_conditions_limites_[i].valeur();

              if (sub_type(Periodique,la_cl) )
                {

                  const Periodique& la_cl_perio = ref_cast(Periodique,la_cl);
                  const Front_VF& le_bord= la_zone_VEF.front_VF(i);
                  int premiere=le_bord.num_premiere_face();
                  int derniere=premiere+le_bord.nb_faces();
                  IntVect fait(le_bord.nb_faces());
                  fait = 0;
                  int ind_fac_loc;

                  for(int j=premiere; j<derniere; j++)
                    {
                      ind_fac_loc = j-premiere;

                      if (fait[ind_fac_loc]== 0)
                        {
                          int fac_associee = la_cl_perio.face_associee(j-premiere)+premiere;
                          if (volumes_entrelaces_Cl_[j] != volumes_entrelaces[j])
                            volumes_entrelaces_Cl_[fac_associee] = volumes_entrelaces_Cl_[j];
                          else
                            volumes_entrelaces_Cl_[j] = volumes_entrelaces_Cl_[fac_associee];

                          fait[ind_fac_loc] = 1;
                          fait[fac_associee-premiere] = 1;
                        }
                    }
                }
            }

          modif_perio_fait_ = 1;
        }
    }
  return 1;
}

Zone_VEF& Zone_Cl_VEF::zone_VEF()
{
  return ref_cast(Zone_VEF, zone_dis().valeur());
}

const Zone_VEF& Zone_Cl_VEF::zone_VEF() const
{
  return ref_cast(Zone_VEF, zone_dis().valeur());
}
