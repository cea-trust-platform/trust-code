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

#include <Op_Conv_Amont_old_VEF_Face.h>
#include <Hexaedre_VEF.h>
#include <Periodique.h>
#include <Neumann_sortie_libre.h>

Implemente_instanciable(Op_Conv_Amont_old_VEF_Face,"Op_Conv_Amont_old_VEF_P1NC",Op_Conv_VEF_base);


//// printOn
//

Sortie& Op_Conv_Amont_old_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Conv_Amont_old_VEF_Face::readOn(Entree& s )
{
  return s ;
}


//// convbis correspond au calcul de -1*terme_convection
//
static void convbis(const double psc,const int num1,const int num2,
                    const DoubleTab& transporte,const int ncomp,
                    DoubleTab& resu, DoubleVect& fluent)
{
  int comp,amont;
  double flux;

  if (psc >= 0)
    {
      amont = num1;
      fluent[num2] += psc;
    }
  else
    {
      amont = num2;
      fluent[num1] -= psc;
    }

  for (comp=0; comp<ncomp; comp++)
    {
      flux = transporte(amont,comp)*psc;
      resu(num1,comp) -= flux;
      resu(num2,comp) += flux;
    }
}
static void convbisimplicite(const double psc,const int num1,const int num2,
                             const DoubleTab& transporte,const int ncomp,
                             DoubleVect& coeff, Matrice_Morse& matrice)
{
  int comp,k;
  IntVect& tab1 = matrice.get_set_tab1();
  IntVect& tab2 = matrice.get_set_tab2();

  for (comp=0; comp<ncomp; comp++)
    {
      if (psc >=0)
        {
          for (k=tab1[num1*ncomp+comp]-1; k<tab1[num1*ncomp+comp+1]-1; k++)
            {
              if (tab2[k]-1== num1*ncomp+comp)
                coeff(k) += psc;
            }
          for (k=tab1[num2*ncomp+comp]-1; k<tab1[num2*ncomp+comp+1]-1; k++)
            {
              if (tab2[k]-1== num1*ncomp+comp)
                coeff(k) -= psc;
            }
        }
      else
        {
          for (k=tab1[num1*ncomp+comp]-1; k<tab1[num1*ncomp+comp+1]-1; k++)
            {
              if (tab2[k]-1== num2*ncomp+comp)
                coeff(k) += psc;
            }
          for (k=tab1[num2*ncomp+comp]-1; k<tab1[num2*ncomp+comp+1]-1; k++)
            {
              if (tab2[k]-1== num2*ncomp+comp)
                coeff(k) -= psc;
            }
        }
    }
}


DoubleTab& Op_Conv_Amont_old_VEF_Face::ajouter(const DoubleTab& transporte,
                                               DoubleTab& resu) const
{
  //  Cerr<<"Op_Conv_Amont_old_VEF_Face::ajouter"<<finl;
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const Zone& zone = zone_VEF.zone();
  const Elem_VEF& type_elem=zone_VEF.type_elem();
  const int nfa7 = type_elem.nb_facette();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();
  DoubleVect& fluent_ = fluent;
  int nfac = zone.nb_faces_elem();
  int nsom = zone.nb_som_elem();
  int nb_som_facette = zone.type_elem().nb_som_face();
  const Elem_geom_base& elem_geom = zone.type_elem().valeur();
  if ( sub_type(Hexaedre_VEF,elem_geom))
    {
      nb_som_facette--;
    }

  const Elem_VEF_base& type_elemvef= zone_VEF.type_elem().valeur();

  // MODIF SB su 10/09/03
  // Pour les 3 elements suivants, il y a autant de sommets que de face
  // constituant l'element geometrique
  // PB avec les hexa, 8 sommets et 6 faces, donc l'utilisation du tableau
  // face[i] ne fonctionne plus
  // la methode retenue pour eviter de calculer la vitesse aux sommets sans
  // les fonctions de forme n'est donc pas utilisable,
  // pour l'hexa on n'a pas acces a la face.
  // il existe le tableau Face=>sommets mais pas l'inverse.
  // trop couteux et pour le moment on n'etend pas les porosites aux hexa
  // ||(nom_elem=="Quadri_VEF")
  int istetra=0;
  Nom nom_elem=type_elemvef.que_suis_je();
  if ((nom_elem=="Tetra_VEF")||(nom_elem=="Tri_VEF"))
    istetra=1;

  // Pour le traitement de la convection on distingue les polyedres
  // standard qui ne "voient" pas les conditions aux limites et les
  // polyedres non standard qui ont au moins une face sur le bord.
  // Un polyedre standard a n facettes sur lesquelles on applique le
  // schema de convection.
  // Pour un polyedre non standard qui porte des conditions aux limites
  // de Dirichlet, une partie des facettes sont portees par les faces.
  // En bref pour un polyedre le traitement de la convection depend
  // du type (triangle, tetraedre ...) et du nombre de faces de Dirichlet.

  double psc;
  int poly,face_adj,fa7,i,j,n_bord,num_face, rang ,itypcl, num10,num20,num_som;
  const int ncomp_ch_transporte = transporte.line_size();

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

  DoubleTab tab(nb_faces_perio,ncomp_ch_transporte);
  // Boucle pour remplir tab
  nb_faces_perio=0;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          //          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
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
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();
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
      // int ncomp;
      if (istetra==1)
        {
          for (j=0; j<nsom; j++)
            for (int ncomp=0; ncomp<Objet_U::dimension; ncomp++)
              vsom(j,ncomp) =vs[ncomp] - Objet_U::dimension*la_vitesse(face[j],ncomp)*porosite_face(face[j]);
        }
      else
        {
          // pour que cela soit valide avec les hexa
          // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
          // int ncomp;
          for (j=0; j<nsom; j++)
            {
              num_som = zone.sommet_elem(poly,j);
              for (int ncomp=0; ncomp<dimension; ncomp++)
                vsom(j,ncomp) = la_vitesse.valeur_a_sommet_compo(num_som,poly,ncomp);
            }
        }

      type_elem.calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);

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
          for (i=0; i<nb_som_facette-1; i++)
            {
              psc =0;
              for (j=0; j<dimension; j++)
                psc+= vsom(KEL(i+2,fa7),j)*cc[j];

              // Boucle sur les facettes du polyedre
              psc /= nb_som_facette;
              num10 = face[KEL(0,fa7)];
              num20 = face[KEL(1,fa7)];
              //psc *= (porosite_face(num1)+porosite_face(num2))/2. ;

              convbis(psc,num10,num20,transporte,ncomp_ch_transporte,resu,fluent_);
            }
          // On traite le sommet confondu avec le centre de gravite du polyedre
          psc=0;
          for (j=0; j<dimension; j++)
            psc += vc[j]*cc[j];
          psc /= nb_som_facette;
          num10 = face[KEL(0,fa7)];
          num20 = face[KEL(1,fa7)];
          //psc *= (porosite_face(num1)+porosite_face(num2))/2. ;

          convbis(psc,num10,num20,transporte,ncomp_ch_transporte,resu,fluent_);
        }

    } // fin de la boucle
  //   if(Process::nproc()==1)
  //     Process::Journal()<<"OpVEFFaAmont ap interne resu[8]="<<resu(8,0)<<","<<resu(8,1)<<finl;
  //   if((Process::nproc()==2)&&(Process::me()==0))
  //     Process::Journal()<<"OpVEFFaAmont ap interne resu[4]="<<resu(4,0)<<","<<resu(4,1)<<finl;

  int voisine;
  nb_faces_perio = 0;
  double diff1,diff2;

  // Dimensionnement du tableau des flux convectifs au bord du domaine
  // de calcul
  DoubleTab& flux_b = flux_bords_;
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
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += la_vitesse(num_face,i)*face_normales(num_face,i)*porosite_face(num_face);
              if (psc>0)
                {
                  for (i=0; i<ncomp_ch_transporte; i++)
                    {
                      resu(num_face,i) -= psc*transporte(num_face,i);
                      flux_b(num_face,i) -= psc*transporte(num_face,i);
                    }
                }
              else
                {
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
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face(), num2 = num1 + le_bord.nb_faces();
          IntVect fait(le_bord.nb_faces());
          fait = 0;
          for (num_face=num1; num_face<num2; num_face++)
            {
              if (fait[num_face-num1] == 0)
                {
                  voisine = la_cl_perio.face_associee(num_face-num1) + num1;
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


void Op_Conv_Amont_old_VEF_Face::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const Zone& zone = zone_VEF.zone();
  const Elem_VEF& type_elem = zone_VEF.type_elem();
  const int nfa7 = type_elem.nb_facette();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();
  int nfac = zone.nb_faces_elem(), nsom = zone.nb_som_elem(), nb_som_facette = zone.type_elem().nb_som_face();

  // Pour le traitement de la convection on distingue les polyedres
  // standard qui ne "voient" pas les conditions aux limites et les
  // polyedres non standard qui ont au moins une face sur le bord.
  // Un polyedre standard a n facettes sur lesquelles on applique le
  // schema de convection.
  // Pour un polyedre non standard qui porte des conditions aux limites
  // de Dirichlet, une partie des facettes sont portees par les faces.
  // En bref pour un polyedre le traitement de la convection depend
  // du type (triangle, tetraedre ...) et du nombre de faces de Dirichlet.

  double psc;
  DoubleTab pscl=0;
  int poly,face_adj,fa7,i,j,k,n_bord, num_face, rang ,itypcl, num10,num20,num_som;
  const int ncomp_ch_transporte = transporte.line_size();

  IntVect face(nfac);
  DoubleVect vs(dimension);
  DoubleVect vc(dimension);
  DoubleTab vsom(nsom,dimension);
  DoubleVect cc(dimension);
  IntVect& tab1 = matrice.get_set_tab1();
  IntVect& tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();

  // Traitement particulier pour les faces de periodicite
  int voisine, nb_faces_perio = 0;
  double diff1,diff2;

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

  DoubleTab tab(nb_faces_perio,ncomp_ch_transporte);

  // Boucle pour remplir tab
  nb_faces_perio=0;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              for (int comp=0; comp<ncomp_ch_transporte; comp++)
                tab(nb_faces_perio,comp) = coeff(num_face*ncomp_ch_transporte+comp);
              nb_faces_perio++;
            }
        }
    }

  // Les polyedres non standard sont ranges en 2 groupes dans la Zone_VEF:
  //  - polyedres bords et joints
  //  - polyedres bords et non joints
  // On traite les polyedres en suivant l'ordre dans lequel ils figurent
  // dans la zone

  // boucle sur les polys
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();
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

      // calcul de la vitesse aux sommets des polyedres
      for (j=0; j<dimension; j++)
        {
          vs[j] = la_vitesse(face[0],j);
          for (i=1; i<nfac; i++)
            vs[j]+= la_vitesse(face[i],j);
        }

      // calcul de la vitesse aux sommets des polyedres
      // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl

      int ncomp;
      for (j=0; j<nsom; j++)
        {
          num_som = zone.sommet_elem(poly,j);
          for(int kk=0; kk<Objet_U::dimension; kk++)
            for (ncomp=0; ncomp<dimension; ncomp++)
              vsom(j,ncomp) = la_vitesse.valeur_a_sommet_compo(num_som,poly,ncomp);
        }

      // calcul de vc
      type_elem.calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);

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

          for (i=0; i<nb_som_facette-1; i++)
            {
              psc =0;

              /////////////////////////////////////////////////////////////////////
              // Remplissage du tableau de coefficients correspondant a convbis
              /////////////////////////////////////////////////////////////////////

              for (j=0; j<dimension; j++)
                psc += vsom(KEL(i+2,fa7),j)*cc[j];
              psc/= nb_som_facette;
              num10 = face[KEL(0,fa7)];
              num20 = face[KEL(1,fa7)];
              convbisimplicite(psc,num10,num20,transporte,ncomp_ch_transporte,coeff,matrice);

            } // fin de boucle sur les sommets des facettes.

          // on traite le sommet confondu avec le centre de gravite du polyedre
          psc=0;
          for (j=0; j<dimension; j++)
            psc += vc[j]*cc[j];
          psc /= nb_som_facette;
          num10 = face[KEL(0,fa7)];
          num20 = face[KEL(1,fa7)];
          convbisimplicite(psc,num10,num20,transporte,ncomp_ch_transporte,coeff,matrice);

        } // fin de boucle sur les facettes.

    } // fin de boucle sur les polyedres.

  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          //        const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += la_vitesse(num_face,i)*face_normales(num_face,i);
              if (psc>0)
                {
                  for (j=0; j<ncomp_ch_transporte; j++)
                    {
                      for (k=tab1[num_face*ncomp_ch_transporte+j]-1; k<tab1[num_face*ncomp_ch_transporte+j+1]-1; k++)
                        {
                          if (tab2[k]-1==num_face*ncomp_ch_transporte+j)
                            coeff(k) += psc;
                        }
                    }
                }
              else /*psc < 0 */
                {
                  for (j=0; j<ncomp_ch_transporte; j++)
                    {
                      for (k=tab1[num_face*ncomp_ch_transporte+j]-1; k<tab1[num_face*ncomp_ch_transporte+j+1]-1; k++)
                        {
                          if (tab2[k]-1==num_face*ncomp_ch_transporte+j)
                            coeff(k) += 0;
                        }
                    }
                }
            }
        }
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
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
                  for (int comp=0; comp<ncomp_ch_transporte; comp++)
                    {
                      diff1 = -1*tab(nb_faces_perio,comp);
                      diff2 = -1*tab(nb_faces_perio+voisine-num_face,comp);

                      for (k=tab1[num_face*ncomp_ch_transporte+comp]-1; k<tab1[num_face*ncomp_ch_transporte+1+comp]-1; k++)
                        if (tab2[k]-1==num_face*ncomp_ch_transporte+comp)
                          coeff(k) += diff2;

                      for (k=tab1[voisine*ncomp_ch_transporte+comp]-1; k<tab1[voisine*ncomp_ch_transporte+1+comp]-1; k++)
                        if (tab2[k]-1==voisine*ncomp_ch_transporte+comp)
                          coeff(k) += diff1;
                    }

                  fait[num_face-num1]= 1;
                  fait[voisine-num1] = 1;
                }
              nb_faces_perio++;
            }
        }
    }
}

void Op_Conv_Amont_old_VEF_Face::contribue_au_second_membre(DoubleTab& resu ) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const Zone& zone = zone_VEF.zone();
  int nfac = zone.nb_faces_elem();
  double psc;
  int i,n_bord, num_face;
  const int ncomp = resu.line_size();

  IntVect face(nfac);

  // Traitement particulier pour les faces de periodicite
  int voisine, nb_faces_perio = 0;
  double diff1,diff2;

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

  DoubleTab tab(nb_faces_perio,ncomp);
  // Boucle pour remplir tab
  nb_faces_perio=0;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              for (int comp=0; comp<ncomp; comp++)
                tab(nb_faces_perio,comp) = resu(num_face,comp);

              nb_faces_perio++;
            }
        }
    }
  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += la_vitesse(num_face,i)*face_normales(num_face,i);
              if (psc>0)
                for (i=0; i<ncomp; i++)
                  resu(num_face,i) += 0;
              else
                for (i=0; i<ncomp; i++)
                  resu(num_face,i) -= psc*la_sortie_libre.val_ext(num_face-num1,i);
            }
        }
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
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
                  for (int comp=0; comp<ncomp; comp++)
                    {
                      diff1 = resu(num_face,comp)-tab(nb_faces_perio,comp);
                      diff2 = resu(voisine,comp)-tab(nb_faces_perio+voisine-num_face,comp);
                      resu(voisine,comp)  += diff1;
                      resu(num_face,comp) += diff2;
                    }

                  fait[num_face-num1]= 1;
                  fait[voisine-num1] = 1;
                }
              nb_faces_perio++;
            }
        }
    }
}
