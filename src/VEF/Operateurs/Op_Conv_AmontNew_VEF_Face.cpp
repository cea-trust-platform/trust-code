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
// File:        Op_Conv_AmontNew_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_AmontNew_VEF_Face.h>
#include <Hexaedre_VEF.h>

Implemente_base(Op_Conv_AmontNew_VEF_Face,"Op_Conv_AmontNew_VEF_P1NC",Op_Conv_VEF_base);


//// printOn
//

Sortie& Op_Conv_AmontNew_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Conv_AmontNew_VEF_Face::readOn(Entree& s )
{
  return s ;
}

//// convbis correspond au calcul de -1*terme_convection
//
static inline void convbis(const double& psc,const int num1,const int num2,
                           double T1, double T2 , int comp, int ncomp,
                           DoubleTab& resu, DoubleVect& fluent)
{
  int amont;
  double flux;

  if (psc >= 0)
    {
      amont = num1;
      if(comp==0)
        fluent[num2] += psc;
    }
  else
    {
      amont = num2;
      if(comp==0)
        fluent[num1] -= psc;
    }

  if(amont==num1)
    flux = T1*psc;
  else
    flux = T2*psc;
  if(ncomp>1)
    {
      resu(num1,comp) -= flux;
      resu(num2,comp) += flux;
    }
  else
    {
      resu(num1) -= flux;
      resu(num2) += flux;
    }
}

DoubleTab& Op_Conv_AmontNew_VEF_Face::ajouter(const DoubleTab& transporte,
                                              DoubleTab& resu) const
{
  DoubleVect& fluent_ = ref_cast(DoubleVect, fluent);

  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const Zone& zone = zone_VEF.zone();
  const Elem_VEF& type_elem=zone_VEF.type_elem();
  const int nfa7 = type_elem.nb_facette();
  int nb_faces_tot=zone_VEF.nb_faces_tot();
  int nb_elem_tot = zone_VEF.nb_elem_tot();
  const DoubleVect& volumes=zone_VEF.volumes();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();
  int ncomp_ch_transporte;

  if (transporte.nb_dim() == 1)
    ncomp_ch_transporte=1;
  else
    ncomp_ch_transporte= transporte.dimension(1);

  DoubleTab TbarK(nb_elem_tot, ncomp_ch_transporte);
  DoubleTab Ttilde1(nb_faces_tot, ncomp_ch_transporte);
  DoubleTab Ttilde2(nb_faces_tot, ncomp_ch_transporte);
  int f, j=0;
  //
  // calcul des Ttilde :
  //
  for(f=0; f<nb_faces_tot; f++)
    {
      int elem=face_voisins(f,0);
      if(ncomp_ch_transporte>1)
        for(j=0; j<ncomp_ch_transporte; j++)
          TbarK(elem, j)+=transporte(f, j);
      else
        TbarK(elem, 0)+=transporte(f);
      elem=face_voisins(f,1);
      if(elem>-1)
        {
          if(ncomp_ch_transporte>1)
            for(j=0; j<ncomp_ch_transporte; j++)
              TbarK(elem, j)+=transporte(f, j);
          else
            TbarK(elem, 0)+=transporte(f);
        }
    }
  double x=1./(dimension+1);
  for(f=0; f<nb_faces_tot; f++)
    {
      int elem=face_voisins(f,0);
      if(ncomp_ch_transporte>1)
        for(j=0; j<ncomp_ch_transporte; j++)
          Ttilde1(f, j)=x*(TbarK(elem, j)-transporte(f, j));
      else
        Ttilde1(f, j)=x*(TbarK(elem, j)-transporte(f));
      elem=face_voisins(f,1);
      if(elem>-1)
        if(ncomp_ch_transporte>1)
          for(j=0; j<ncomp_ch_transporte; j++)
            Ttilde2(f, j)+=x*(TbarK(elem, j)-transporte(f, j));
        else
          Ttilde2(f, j)+=x*(TbarK(elem, j)-transporte(f));
      else if(ncomp_ch_transporte>1)
        Ttilde2(f, j)=transporte(f, j);
      else
        Ttilde2(f, j)=transporte(f);
    }
  // On recree les chocs ...
  for(f=0; f<nb_faces_tot; f++)
    {
      int elem2=face_voisins(f,1);
      if(elem2>-1)
        {
          int elem1=face_voisins(f,0);
          double alpha=volumes(elem1)/(volumes(elem1)+volumes(elem2));
          for(j=0; j<ncomp_ch_transporte; j++)
            {
              double epsilon;
              double Tfj;
              {
                if(ncomp_ch_transporte>1)
                  Tfj=transporte(f, j);
                else
                  Tfj=transporte(f);
                if( ( (Tfj<Ttilde1(f, j)) &&
                      (Tfj<Ttilde2(f, j)) ) ||
                    ( (Tfj>Ttilde1(f, j)) &&
                      (Tfj>Ttilde2(f, j)) ) )
                  {
                    Ttilde1(f, j)=Ttilde2(f, j)=Tfj;
                  }
                else
                  {
                    if(Ttilde1(f, j)<Ttilde2(f, j))
                      {
                        epsilon=(Tfj-alpha*Ttilde1(f, j)+
                                 (1-alpha)*Ttilde2(f, j))/alpha;
                        Ttilde2(f, j)-=epsilon;
                        Ttilde1(f, j)=(Tfj-(1-alpha)*Ttilde2(f, j))
                                      /alpha;
                      }
                    else
                      {
                        epsilon=(Tfj-(1-alpha)*Ttilde2(f, j)+
                                 alpha*Ttilde1(f, j))/(1-alpha);
                        Ttilde1(f, j)-=epsilon;
                        Ttilde2(f, j)=(Tfj-alpha*Ttilde1(f, j))
                                      /(1-alpha);
                      }
                    Cerr << "!!" << finl;
                  }
              }
            }
        }
      else if(ncomp_ch_transporte>1)
        for(j=0; j<ncomp_ch_transporte; j++)
          Ttilde1(f, j)=Ttilde2(f, j)=transporte(f, j);
      else
        Ttilde1(f, 0)=Ttilde2(f, 0)=transporte(f);

    }
  // pas d'inversion de pente :

  for(int elem=0; elem<nb_elem_tot; elem++)
    {
      for(int f1=0; f1<dimension; f1++)
        {
          int face1=elem_faces(elem,f1);
          int i1=0;
          if(face_voisins(face1,0)!=elem)
            i1=1;
          for(int f2=f1; f2<dimension+1; f2++)
            {
              int face2=elem_faces(elem,f2);
              int i2=0;
              if(face_voisins(face2,0)!=elem)
                i2=1;
              for(j=0; j<ncomp_ch_transporte; j++)
                {
                  double Tfj1;
                  if(ncomp_ch_transporte>1)
                    Tfj1=transporte(face1, j);
                  else
                    Tfj1=transporte(face1);
                  double Tfj2;
                  if(ncomp_ch_transporte>1)
                    Tfj2=transporte(face2, j);
                  else
                    Tfj2=transporte(face2);
                  double T1=Ttilde1(face1, j);
                  if (i1==1)
                    T1=Ttilde2(face1, j);
                  double T2=Ttilde1(face2, j);
                  if (i2==1)
                    T2=Ttilde2(face2, j);
                  //double Tbar=0.5*(Tfj1+Tfj2);
                  if((T1-T2)*(Tfj1-Tfj2)<1.e-12)
                    {
                      if (i1==0)
                        Ttilde1(face1, j)=Tfj1;
                      else
                        Ttilde2(face1, j)=Tfj1;
                      if (i2==0)
                        Ttilde1(face2, j)=Tfj2;
                      else
                        Ttilde2(face2, j)=Tfj2;
                    }
                }
            }
        }
    }
  // calcul des flux ...
  int nfac = zone.nb_faces_elem();
  int nsom = zone.nb_som_elem();
  int nb_som_facette = zone.type_elem().nb_som_face();
  const Elem_geom_base& elem_geom = zone.type_elem().valeur();
  if ( sub_type(Hexaedre_VEF,elem_geom))
    {
      Process::exit();
    }
  const Elem_VEF_base& type_elemvef= zone_VEF.type_elem().valeur();
  int istetra=0;
  Nom nom_elem=type_elemvef.que_suis_je();
  if ((nom_elem=="Tetra_VEF")||(nom_elem=="Tri_VEF"))
    istetra=1;
  double psc;
  int poly,face_adj,fa7,i,n_bord;
  int num_face, rang ,itypcl;
  int num10,num20,num_som;

  if (transporte.nb_dim() == 1)
    ncomp_ch_transporte=1;
  else
    ncomp_ch_transporte= transporte.dimension(1);

  IntVect face(nfac);
  DoubleVect vs(dimension);
  DoubleVect vc(dimension);
  DoubleTab vsom(nsom,dimension);
  DoubleVect cc(dimension);


  // On remet a zero le tableau qui sert pour
  // le calcul du pas de temps de stabilite
  fluent_ = 0;

  // Traitement particulier pour les faces de periodicite

  int nb_faces_perio = 0;
  // Boucle pour compter le nombre de faces de periodicite
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          nb_faces_perio += le_bord.nb_faces();
        }
    }

  DoubleTab tab;
  if (ncomp_ch_transporte == 1)
    tab.resize(nb_faces_perio);
  else
    tab.resize(nb_faces_perio,ncomp_ch_transporte);

  // Boucle pour remplir tab
  nb_faces_perio=0;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          //          const Periodique& la_cl_perio = (Periodique&) la_cl.valeur();
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              if (ncomp_ch_transporte == 1)
                tab(nb_faces_perio) = resu(num_face);
              else
                for (int comp=0; comp<ncomp_ch_transporte; comp++)
                  tab(nb_faces_perio,comp) = resu(num_face,comp);
              nb_faces_perio++;
            }
        }
    }

  IntVect compteur(nsom);
  compteur = 0;
  vsom=0.;

  // Les polyedres non standard sont ranges en 2 groupes dans la Zone_VEF:
  //  - polyedres bords et joints
  //  - polyedres bords et non joints
  // On traite les polyedres en suivant l'ordre dans lequel ils figurent
  // dans la zone

  // boucle sur les polys

  for (poly=0; poly<nb_elem_tot; poly++)
    {

      rang = rang_elem_non_std(poly);
      if (rang==-1)
        itypcl=0;
      else
        itypcl=zone_Cl_VEF.type_elem_Cl(rang);

      // calcul des numeros des faces du polyedre
      for (face_adj=0; face_adj<nfac; face_adj++)
        face[face_adj]= elem_faces(poly,face_adj);
      // on conserve cette partie
      for (j=0; j<dimension; j++)
        {
          vs[j] = la_vitesse(face[0],j)*porosite_face(face[0]);
          for (i=1; i<nfac; i++)
            vs[j]+= la_vitesse(face[i],j)*porosite_face(face[i]);
        }

      // calcul de la vitesse aux sommets des polyedres

      //int ncomp;
      if (istetra==1)
        {
          for (j=0; j<nsom; j++)
            {
              for (int ncomp=0; ncomp<Objet_U::dimension; ncomp++)
                vsom(j,ncomp) =vs[ncomp] - Objet_U::dimension*la_vitesse(face[j],ncomp)*porosite_face(face[j]);
            }
        }
      else
        {
          // pour que cela soit valide avec les hexa
          // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
          int ncomp;
          for (j=0; j<nsom; j++)
            {
              num_som = zone.sommet_elem(poly,j);
              for (ncomp=0; ncomp<dimension; ncomp++)
                {
                  vsom(j,ncomp) = la_vitesse.valeur_a_sommet_compo(num_som,poly,ncomp);
                }
            }
        }
      // calcul de vc
      zone_VEF.type_elem().calcul_vc(face,vc,vs,vsom,vitesse(), itypcl,porosite_face);

      //for (j=0; j<dimension; j++) vc(j) =  vs(j)/nfac ;
      type_elem.calcul_vc(face,vc,vs,vsom,vitesse(),
                          itypcl,porosite_face);

      // Boucle sur les facettes du polyedre

      for (fa7=0; fa7<nfa7; fa7++)
        {
          if (rang==-1)
            for (i=0; i<dimension; i++)
              cc[i] = facette_normales(poly,fa7,i);
          else
            for (i=0; i<dimension; i++)
              cc[i] = normales_facettes_Cl(rang,fa7,i);
          // On applique le schema de convection a chaque sommet de la facette

          // On traite le ou les sommets qui sont aussi des sommets du polyedre

          const IntTab& KEL=type_elem->KEL();
          for (i=0; i<nb_som_facette-1; i++)
            {
              psc =0;
              for (j=0; j<dimension; j++)
                {
                  psc+= vsom(KEL(i+2,fa7),j)*cc[j];
                }
              type_elem.calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);
              // Boucle sur les facettes du polyedre
              psc /= nb_som_facette;
              num10 = face[KEL(0,fa7)];
              num20 = face[KEL(1,fa7)];
              double T1, T2;
              for(int comp=0; comp<ncomp_ch_transporte; comp++)
                {
                  if(face_voisins(num10,0)==poly)
                    T1 = Ttilde1(num10, comp);
                  else
                    T1 = Ttilde2(num10, comp);
                  if(face_voisins(num20,0)==poly)
                    T2 = Ttilde1(num20, comp);
                  else
                    T2 = Ttilde2(num20, comp);
                  if( (transporte(num20)-transporte(num10)) * (T1-transporte(num10)) < 0)
                    convbis(psc,num10,num20,T1, T2, comp,
                            ncomp_ch_transporte,
                            resu,fluent_);
                }
            }
          // On traite le sommet confondu avec le centre de gravite du polyedre
          psc=0;
          for (j=0; j<dimension; j++)
            psc += vc[j]*cc[j];
          psc /= nb_som_facette;
          num10 = face[KEL(0,fa7)];
          num20 = face[KEL(1,fa7)];

          double T1, T2;
          for(int comp=0; comp<ncomp_ch_transporte; comp++)
            {
              if(face_voisins(num10,0)==poly)
                T1 = Ttilde1(num10, comp);
              else
                T1 = Ttilde2(num10, comp);
              if(face_voisins(num20,0)==poly)
                T2 = Ttilde1(num20, comp);
              else
                T2 = Ttilde2(num20, comp);
              convbis(psc,num10,num20,T1, T2, comp,
                      ncomp_ch_transporte,
                      resu,fluent_);
            }
        }

    } // fin de la boucle

  int voisine;
  nb_faces_perio = 0;
  double diff1,diff2;

  // Dimensionnement du tableau des flux convectifs au bord du domaine
  // de calcul
  DoubleTab& flux_b = ref_cast(DoubleTab, flux_bords_);
  flux_b.resize(zone_VEF.nb_faces_bord(),ncomp_ch_transporte);
  flux_b = 0.;

  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre =
            ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += la_vitesse(num_face,i)*face_normales(num_face,i)*porosite_face(num_face);
              if (psc>0)
                if (ncomp_ch_transporte == 1)
                  {
                    resu(num_face) -= psc*transporte(num_face);
                    flux_b(num_face,0) -= psc*transporte(num_face);
                  }
                else
                  for (i=0; i<ncomp_ch_transporte; i++)
                    {
                      resu(num_face,i) -= psc*transporte(num_face,i);
                      flux_b(num_face,i) -= psc*transporte(num_face,i);
                    }
              else
                {
                  if (ncomp_ch_transporte == 1)
                    {
                      resu(num_face) -= psc*la_sortie_libre.val_ext(num_face-num1);
                      flux_b(num_face,0) -= psc*la_sortie_libre.val_ext(num_face-num1);
                    }
                  else
                    for (i=0; i<ncomp_ch_transporte; i++)
                      {
                        resu(num_face,i) -= psc*la_sortie_libre.val_ext(num_face-num1,i);
                        flux_b(num_face,i) -= psc*la_sortie_libre.val_ext(num_face-num1,i);
                      }
                  fluent_[num_face] -= psc;
                }
            }
        }
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio =  ref_cast(Periodique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          IntVect fait(le_bord.nb_faces());
          fait = 0;
          for (num_face=num1; num_face<num2; num_face++)
            {
              if (fait[num_face-num1] == 0)
                {
                  voisine = la_cl_perio.face_associee(num_face-num1) + num1;

                  if (ncomp_ch_transporte == 1)
                    {
                      diff1 = resu(num_face)-tab(nb_faces_perio);
                      diff2 = resu(voisine)-tab(nb_faces_perio+voisine-num_face);
                      resu(voisine)  += diff1;
                      resu(num_face) += diff2;
                      flux_b(voisine,0) += diff1;
                      flux_b(num_face,0) += diff2;
                    }
                  else
                    for (int comp=0; comp<ncomp_ch_transporte; comp++)
                      {
                        diff1 = resu(num_face,comp)-tab(nb_faces_perio,comp);
                        diff2 = resu(voisine,comp)-tab(nb_faces_perio+voisine-num_face,comp);
                        resu(voisine,comp)  += diff1;
                        resu(num_face,comp) += diff2;
                        flux_b(voisine,comp) += diff1;
                        flux_b(num_face,comp) += diff2;
                      }

                  fait[num_face-num1]= 1;
                  fait[voisine-num1] = 1;
                }
              nb_faces_perio++;
            }
        }
    }
  modifier_flux(*this);
  return resu;
}
