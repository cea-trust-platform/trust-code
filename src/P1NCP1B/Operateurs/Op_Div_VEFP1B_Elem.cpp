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
// File:        Op_Div_VEFP1B_Elem.cpp
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/53
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Div_VEFP1B_Elem.h>
#include <Domaine.h>
#include <Zone_Cl_VEF.h>
#include <Periodique.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Dirichlet_entree_fluide.h>
#include <Symetrie.h>
#include <Neumann.h>
#include <Neumann_val_ext.h>
#include <Zone_VEF_PreP1b.h>
#include <Debog.h>
#include <Probleme_base.h>
#include <Check_espace_virtuel.h>
#include <Schema_Temps_base.h>
#include <Porosites_champ.h>
#include <SChaine.h>
#include <VerifierCoin.h>
#include <SFichier.h>

Implemente_instanciable(Op_Div_VEFP1B_Elem,"Op_Div_VEFPreP1B_P1NC",Op_Div_VEF_Elem);

// printOn et readOn

Sortie& Op_Div_VEFP1B_Elem::printOn(Sortie& s ) const
{
  return s;
}

Entree& Op_Div_VEFP1B_Elem::readOn(Entree& is )
{
  return is;
}

static int chercher_arete(int elem, int somi, int somj,
                          const IntTab& elem_aretes,
                          const IntTab& aretes_som)
{
  if(somi>somj)
    {
      int k=somi;
      somi=somj;
      somj=k;
    }
  for(int i_arete=0; i_arete<6; i_arete++)
    {
      int arete=elem_aretes(elem, i_arete);
      int som1=aretes_som(arete,0);
      if(somi==som1)
        {
          int som2=aretes_som(arete,1);
          if(somj==som2)
            return arete;
        }
    }
  return -1;
}

static int verifier(const Op_Div_VEFP1B_Elem& op,
                    int& init,
                    const Zone_VEF_PreP1b& zone_VEF,
                    const DoubleTab& vit,
                    DoubleTab& div)
{
  init=1;
  DoubleTab v(vit);
  v=1;
  DoubleTab r(div);
  r=0;
  op.ajouter(v, r);
  Cerr << "div(1,..,1) = " << r << finl;
  Debog::verifier("div(1,..,1) =",r);
  const DoubleTab& xv = zone_VEF.xv();
  v=xv;
  {
    for (int i=0; i< v.dimension(0); i++)
      v(i,1)=0;
  }
  r=0;
  op.ajouter(v, r);
  Cerr << "div(x,0) = " << r << finl;
  Debog::verifier("div(x,0) =",r);
  return 1;
}

DoubleTab& Op_Div_VEFP1B_Elem::ajouter_elem(const DoubleTab& vit, DoubleTab& div) const
{
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b,la_zone_vef.valeur());
  assert(zone_VEF.get_alphaE());
  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  int nfe=zone.nb_faces_elem();
  int nb_elem=zone.nb_elem();
  int elem,indice,face,comp;
  double pscf,signe;

  for(elem=0; elem<nb_elem; elem++)
    {
      pscf=0;
      for(indice=0; indice<nfe; indice++)
        {
          face = elem_faces(elem,indice);
          signe=1;
          if(elem!=face_voisins(face,0))
            signe=-1;
          for(comp=0; comp<dimension; comp++)
            pscf+=signe*vit(face, comp)*
                  face_normales(face,comp);
        }
      div(elem)+=pscf;
    }
  assert_invalide_items_non_calcules(div);
  return div;
}

int find_cl_face(const Zone& zone, const int face)
{
  int i;
  const int nb_cl = zone.nb_front_Cl();
  for (i = 0; i < nb_cl; i++)
    {
      const Frontiere& fr = zone.frontiere(i);
      const int debut = fr.num_premiere_face();
      if (face >= debut && face < debut + fr.nb_faces())
        return i;
      const ArrOfInt& faces_virt = fr.get_faces_virt();
      const int nb_faces_virt = faces_virt.size_array();
      for (int j = 0; j < nb_faces_virt; j++)
        if (face == faces_virt[j])
          return i;
    }
  return -1;
}

double calculer_coef_som(int elem, int& nb_face_diri, ArrOfInt& indice_diri, const Zone_Cl_VEF& zcl, const Zone_VEF& zone_VEF)
{

  // prov ajouter verif par %2 ....
  int type_elem=0;
  int& dimension= Objet_U::dimension;
  int rang_elem=zone_VEF.rang_elem_non_std()(elem);
  if (rang_elem!=-1)
    type_elem=zcl.type_elem_Cl(rang_elem);
  if ((0)||(type_elem==0))
    {
      nb_face_diri=0;
      double coeff_som=1./(dimension*(dimension+1));
      return coeff_som;
    }
  else
    {
      if (dimension==2)
        {
          switch(type_elem)
            {
            case 1:
              nb_face_diri=1;
              indice_diri[0]=2;
              break;
            case 2:
              nb_face_diri=1;
              indice_diri[0]=1;
              break;
            case 4:
              nb_face_diri=1;
              indice_diri[0]=0;
              break;
            case 3:
              nb_face_diri=2;
              indice_diri[0]=1;
              indice_diri[1]=2;
              break;
            case 5:
              nb_face_diri=2;
              indice_diri[0]=0;
              indice_diri[1]=2;
              break;
            case 6:
              nb_face_diri=2;
              indice_diri[0]=0;
              indice_diri[1]=1;
              break;
            default:
              nb_face_diri=10000;
              Cerr<<__FILE__<<(int)__LINE__<<" impossible "<<finl;
              Process::exit();
              break;
            }
        }
      else if (dimension==3)
        {
          switch(type_elem)
            {
            case 1:
              nb_face_diri=1;
              indice_diri[0]=3;
              break;
            case 2:
              nb_face_diri=1;
              indice_diri[0]=2;
              break;
            case 4:
              nb_face_diri=1;
              indice_diri[0]=1;
              break;
            case 8:
              nb_face_diri=1;
              indice_diri[0]=0;
              break;
            case 3:
              nb_face_diri=2;
              indice_diri[0]=3;
              indice_diri[1]=2;
              break;
            case 5:
              nb_face_diri=2;
              indice_diri[0]=1;
              indice_diri[1]=3;
              break;
            case 6:
              nb_face_diri=2;
              indice_diri[0]=1;
              indice_diri[1]=2;
              break;
            case 9:
              nb_face_diri=2;
              indice_diri[0]=0;
              indice_diri[1]=3;
              break;
            case 10:
              nb_face_diri=2;
              indice_diri[0]=0;
              indice_diri[1]=2;
              break;
            case 12:
              nb_face_diri=2;
              indice_diri[0]=0;
              indice_diri[1]=1;
              break;
            case 7:
              nb_face_diri=3;
              indice_diri[0]=1;
              indice_diri[1]=2;
              indice_diri[2]=3;
              break;
            case 11:
              nb_face_diri=3;
              indice_diri[0]=0;
              indice_diri[1]=2;
              indice_diri[2]=3;
              break;
            case 13:
              nb_face_diri=3;
              indice_diri[0]=0;
              indice_diri[1]=1;
              indice_diri[2]=3;
              break;
            case 14:
              nb_face_diri=3;
              indice_diri[0]=0;
              indice_diri[1]=1;
              indice_diri[2]=2;
              break;
            default:
              nb_face_diri=10000;
              Cerr<<__FILE__<<(int)__LINE__<<" impossible "<<finl;
              Process::exit();
              break;
            }
        }
      else
        {
          nb_face_diri=10000;
          abort();
          Process::exit();
        }
    }
  double coeff_som=1./(dimension*(dimension+1-nb_face_diri));
  return coeff_som;
}

DoubleTab& Op_Div_VEFP1B_Elem::ajouter_som(const DoubleTab& vit, DoubleTab& div, DoubleTab& flux_b) const
{
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b,la_zone_vef.valeur());
  assert(zone_VEF.get_alphaS());
  const Zone& zone = zone_VEF.zone();
  const Domaine& dom=zone.domaine();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const IntTab& som_elem=zone.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  int nfe=zone.nb_faces_elem();
  int nb_elem_tot=zone.nb_elem_tot();
  int nps=zone_VEF.numero_premier_sommet();
  int elem,indice,face,comp,som;
  ArrOfDouble sigma(dimension);

  int nb_face_diri=0;
  double coeff_som=1./(dimension*(dimension+1));
  int modif_traitement_diri=zone_VEF.get_modif_div_face_dirichlet();
  double psc, signe;

  const Zone_Cl_VEF& zcl=ref_cast(Zone_Cl_VEF,la_zcl_vef.valeur());
  ArrOfInt indice_diri(dimension+1);
  for(elem=0; elem<nb_elem_tot; elem++)
    {
      if (modif_traitement_diri)
        coeff_som=calculer_coef_som(elem,nb_face_diri,indice_diri,zcl,zone_VEF);

      sigma = 0;
      for(indice=0; indice<nfe; indice++)
        {
          face = elem_faces(elem,indice);
          for(comp=0; comp<dimension; comp++)
            {
              sigma[comp]+=vit(face, comp);
            }
        }
      // on retire la contribution des faces dirichlets
      for (int fdiri=0; fdiri<nb_face_diri; fdiri++)
        {
          int indice2=indice_diri[fdiri];
          face = elem_faces(elem,indice2);
          for(comp=0; comp<dimension; comp++)
            {
              sigma[comp]-=vit(face, comp);
            }
        }
      for(indice=0; indice<nfe; indice++)
        {
          som = nps+dom.get_renum_som_perio(som_elem(elem,indice));
          face = elem_faces(elem,indice);
          psc=0;
          signe=1;
          if(elem!=face_voisins(face,0))
            signe=-1;
          for(comp=0; comp<dimension; comp++)
            {
              psc+=sigma[comp]*face_normales(face,comp);
            }
          div(som)+=signe*coeff_som*psc;
          nb_degres_liberte(som-nps)++;
        }
    }
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_sommets = zone_VEF.face_sommets();
  int nb_bords =les_cl.size();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      {
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        int nb_faces_bord = le_bord.nb_faces();
        int nb_faces_bord_tot = le_bord.nb_faces_tot();
        assert(le_bord.nb_faces()==zone_VEF.zone().frontiere(n_bord).nb_faces());
        if (!sub_type(Periodique,la_cl.valeur()))
          {
            int libre=1;
            if (sub_type(Dirichlet,la_cl.valeur())
                ||sub_type(Dirichlet_homogene,la_cl.valeur())
                ||sub_type(Dirichlet_entree_fluide,la_cl.valeur())
                ||sub_type(Symetrie,la_cl.valeur()))
              libre=0;
            // On boucle sur les faces de bord reelles et virtuelles
            for (int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
              {
                face = le_bord.num_face(ind_face);
                double flux = 0. ;
                for (comp=0; comp<dimension; comp++)
                  {
                    flux += vit(face,comp) * face_normales(face,comp) ;
                  }
                if (ind_face<nb_faces_bord) flux_b(face,0) = flux;
                flux*=1./dimension;
                for(indice=0; indice<(nfe-1); indice++)
                  {
                    som=dom.get_renum_som_perio(face_sommets(face,indice));
                    div(nps+som)+=flux;
                    if (libre) nb_degres_liberte(som)++;
                  }
              }
          }
        else
          {
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
            // On boucle sur les faces de bord reelles et virtuelles
            for (int ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
              {
                face = le_bord.num_face(ind_face);
                int face_associee = le_bord.num_face(la_cl_perio.face_associee(ind_face));
                double flux = 0. ;
                double flux_perio = 0. ;
                for (comp=0; comp<dimension; comp++)
                  {
                    flux += vit(face,comp) * face_normales(face,comp) ;
                    flux_perio += vit(face_associee,comp) * face_normales(face_associee,comp) ;
                  }
                if (ind_face<(nb_faces_bord/2))
                  {
                    flux_b(face,0) = -flux;
                    flux_b(face_associee,0) = flux_perio;
                  }
              }
          }
      }
    }
  return div;
}

DoubleTab& Op_Div_VEFP1B_Elem::ajouter_aretes(const DoubleTab& vit, DoubleTab& div) const
{
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b,la_zone_vef.valeur());
  assert(zone_VEF.get_alphaA());
  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const IntTab& som_elem=zone.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const ArrOfInt& renum_arete_perio=zone_VEF.get_renum_arete_perio();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  int npa=zone_VEF.numero_premiere_arete();
  int nb_elem_tot=zone.nb_elem_tot();
  int elem,comp;
  ArrOfDouble sigma(dimension);

  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  for(elem=0; elem<nb_elem_tot; elem++)
    {
      for(int isom=0; isom<3; isom++)
        {
          int somi = som_elem(elem,isom);
          int facei = elem_faces(elem,isom);
          double signei=1.;
          if (face_voisins(facei,0) != elem)
            signei=-1.;
          for(int jsom=isom+1; jsom<4; jsom++)
            {
              int somj = som_elem(elem,jsom);
              int facej = elem_faces(elem,jsom);
              double signej=1.;
              if (face_voisins(facej,0) != elem)
                signej=-1.;
              int arete = renum_arete_perio(chercher_arete(elem, somi, somj,
                                                           elem_aretes, aretes_som));
              if(ok_arete(arete))
                {
                  int niinij=0;
                  for(int ksom=0; ksom<4; ksom++)
                    {
                      if((ksom==isom)||(ksom==jsom))
                        {
                          if(niinij==ksom)
                            niinij++;
                          double psc=0;
                          for(comp=0; comp<3; comp++)
                            psc+=(signei*face_normales(facei,comp)+
                                  signej*face_normales(facej,comp))
                                 *vit(elem_faces(elem,ksom),comp);
                          div(npa+arete)-=1./15.*psc;
                        }
                      else
                        {
                          if(niinij==ksom)
                            niinij++;
                          while((niinij==jsom)||(niinij==isom))
                            niinij++;
                          assert(niinij<4);
                          assert(niinij!=isom);
                          assert(niinij!=jsom);
                          assert(niinij!=ksom);
                          int facek = elem_faces(elem,niinij);
                          double signek=1.;
                          if (face_voisins(facek,0) != elem)
                            signek=-1.;
                          double psc=0;
                          for(comp=0; comp<3; comp++)
                            psc+=signek*face_normales(facek,comp)
                                 *vit(elem_faces(elem,ksom),comp);
                          div(npa+arete)-=2./15.*psc;
                          niinij=ksom;
                        }
                    }
                }
            }
        }
    }
  return div;
}

// static void verifier_vit(const Zone_Cl_VEF& zone_Cl_VEF,
//                          const Zone_VEF& zone_VEF,
//                          const DoubleTab& vit)
// {
//   const DoubleTab& face_normales = zone_VEF.face_normales();
//   const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
//   int nb_bords =les_cl.size();
//   for (int n_bord=0; n_bord<nb_bords; n_bord++)
//     {
//       const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
//       {
//         if (sub_type(Periodique,la_cl.valeur()))
//           {
//             const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
//             const Periodique& la_cl_perio = ref_cast(Periodique,
//                                                      la_cl.valeur());
//             int num1 = le_bord.num_premiere_face();
//             int num2 = num1 + le_bord.nb_faces();
//             for (int face=num1; face<num2; face++)
//               {
//                 int face_associee=num1+la_cl_perio.face_associee(face-num1);
//                 for (int comp=0; comp<Objet_U::dimension; comp++)
//                   {
//                     if(fabs(vit(face,comp)-vit(face_associee,comp))>1.e-20)
//                       Cout << "ERREUR PERIO" << finl;
//                   }
//               }
//           }
//         else if (sub_type(Symetrie,la_cl.valeur()))
//         {
//           DoubleTab& inco = (DoubleTab&) vit;
//           const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
//           int num1 = le_bord.num_premiere_face();
//           int num2 = num1 + le_bord.nb_faces();
//           for (int face=num1; face<num2; face++)
//             {
//               double surf=0;
//               double flux=0;
//               double face_n;
//               for (int comp=0; comp<Objet_U::dimension; comp++)
//                 {
//                   face_n = face_normales(face,comp);
//                   flux += vit(face,comp)*face_n;
//                   surf += face_n*face_n;
//                 }
//               //flux_b(face,0) = flux;
//               flux /= surf;
//               if(fabs(flux)>1.e-12)
//                 Cout << "Erreur Symetrie!! " << flux << finl;
//               for (int comp=0; comp<Objet_U::dimension; comp++)
//                 inco(face,comp) -= flux * face_normales(face,comp);
//               }
//         }
//       }
//     }
// };

DoubleTab& Op_Div_VEFP1B_Elem::ajouter(const DoubleTab& vitesse_face_absolue, DoubleTab& div) const
{
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b,la_zone_vef.valeur());
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  // Quelques verifications:
  // L'espace virtuel de vitesse_face_absolue doit etre a jour (Le test est fait si check_enabled==1)
  assert_espace_virtuel_vect(vitesse_face_absolue);
  // On s'en fiche de l'espace virtuel de div a l'entree, mais on fait += dessus.
  assert_invalide_items_non_calcules(div, 0.);

#ifndef NDEBUG
  // On s'assure que la periodicite est respectee sur vitesse_face_absolue (Voir FA814)
  int nb_comp=vitesse_face_absolue.dimension(1);
  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nb_faces_bord=le_bord.nb_faces();
          for (int ind_face=0; ind_face<nb_faces_bord; ind_face++)
            {
              int ind_face_associee = la_cl_perio.face_associee(ind_face);
              int face = le_bord.num_face(ind_face);
              int face_associee = le_bord.num_face(ind_face_associee);
              // PL : test a 1.e-4 car certains cas ne respectent pas strictement (lente derive?)
              for (int comp=0; comp<nb_comp; comp++)
                if (!est_egal(vitesse_face_absolue(face, comp),vitesse_face_absolue(face_associee, comp),1.e-4))
                  {
                    Cerr << "vit1("<<face<<","<<comp<<")=" << vitesse_face_absolue(face, comp) << finl;
                    Cerr << "vit2("<<face_associee<<","<<comp<<")=" << vitesse_face_absolue(face_associee, comp) << finl;
                    Cerr << "Delta=" << vitesse_face_absolue(face, comp)-vitesse_face_absolue(face_associee, comp) << finl;
                    Cerr << "Periodic boundary condition is not correct in Op_Div_VEFP1B_Elem::ajouter" << finl;
                    Cerr << "Contact TRUST support." << finl;
                    exit();
                  }
            }// for face
        }// sub_type Perio
    }
#endif
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  DoubleTab phi_vitesse_face_;
  const DoubleTab& vit=modif_par_porosite_si_flag(vitesse_face_absolue,phi_vitesse_face_,1,porosite_face);

  // Tableau des degres de liberte
  int nps=zone_VEF.numero_premier_sommet();
  nb_degres_liberte.resize(zone_VEF.zone().nb_som_tot());
  nb_degres_liberte=-1;

  DoubleTab& flux_b = flux_bords_;
  flux_b.resize(zone_VEF.nb_faces_bord(),1);
  flux_b = 0.;

  static int init=1;
  if(!init) ::verifier(*this, init, zone_VEF, vit, div);
  if(zone_VEF.get_alphaE()) ajouter_elem(vit, div);
  if(zone_VEF.get_alphaS()) ajouter_som(vit, div, flux_b);
  if(zone_VEF.get_alphaA()) ajouter_aretes(vit, div);

  // correction de de div u si pression sommet imposee de maniere forte
  if ((zone_VEF.get_alphaS())&&((zone_VEF.get_cl_pression_sommet_faible()==0)))
    {
      const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
      int nb_bords =les_cl.size();
      for (int n_bord=0; n_bord<nb_bords; n_bord++)
        {
          const Cond_lim& la_cl = les_cl[n_bord];
          if (sub_type(Neumann,la_cl.valeur()) || sub_type(Neumann_val_ext,la_cl.valeur()))
            {
              const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
              const IntTab& faces_sommets = zone_VEF.face_sommets();
              int nb_faces=la_front_dis.nb_faces_tot();
              int nsf=0;
              if(nb_faces!=0)
                nsf=faces_sommets.dimension(1);

              int num2 = nb_faces;
              for (int ind_face=0; ind_face<num2; ind_face++)
                {
                  int face=la_front_dis.num_face(ind_face);
                  for(int som=0; som<nsf; som++)
                    {
                      int som1=faces_sommets(face, som);
                      div(nps+som1) = 0.;
                    }
                }
            }
        }
    }
  degres_liberte();
  //Optimisation, pas necessaire:
  //div.echange_espace_virtuel();
  return div;
}

#ifdef VersionP1
double tiers=1./3.;
const IntTab& aretes_som=zone_VEF.zone().aretes_som();
const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
ArrOfDouble gradK(3);
for(int isom=0; isom<3; isom++)
  {
    int somi = som_elem(elem,isom);
    int facei = elem_faces(elem,isom);
    double signei=1.;
    if (face_voisins(facei,0) != elem)
      signei=-1.;
    for(int jsom=isom+1; jsom<4; jsom++)
      {
        int somj = som_elem(elem,jsom);
        int facej = elem_faces(elem,jsom);
        double signej=1.;
        if (face_voisins(facej,0) != elem)
          signej=-1.;
        int arete = chercher_arete(elem, somi, somj,
                                   elem_aretes, aretes_som);
        for(comp=0; comp<dimension; comp++)
          gradK(comp) = (signei*face_normales(facei,comp)+
                         signej*face_normales(facej,comp));
        psc=0;
        for(comp=0; comp<dimension; comp++)
          psc+=tiers*(
                 -signei*face_normales(facei,comp)*vit(facei,comp)
                 -signej*face_normales(facej,comp)*vit(facej,comp)
                 +.5*gradK(comp)*sigma[comp]);
        div(npa+arete)-=psc;
      }
  }
#else
#endif

// Divise par le volume
void Op_Div_VEFP1B_Elem::volumique(DoubleTab& div) const
{
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b,la_zone_vef.valeur());
  int n=0;
  if (zone_VEF.get_alphaE())
    {
      Op_Div_VEF_Elem::volumique(div);
      n+=zone_VEF.nb_elem_tot();
    }
  if (zone_VEF.get_alphaS())
    {
      const DoubleVect& vol = zone_VEF.volume_aux_sommets();
      int size_tot = vol.size_totale();
      for (int i=0; i<size_tot; i++)
        div(n+i)/=vol(i);
      n+=zone_VEF.nb_som_tot();
    }
  if (zone_VEF.get_alphaA())
    {
      const DoubleVect& vol = zone_VEF.get_volumes_aretes();
      int size_tot = vol.size_totale();
      for (int i=0; i<size_tot; i++)
        div(n+i)/=vol(i);
    }
}

void Op_Div_VEFP1B_Elem::degres_liberte() const
{
  // On annulle la divergence aux sommets sans degre de liberte
  // (sommet uniquement commun a des faces avec des CL Diriclet)
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b,la_zone_vef.valeur());
  const Zone& zone = zone_VEF.zone();
  const Domaine& dom=zone.domaine();
  const IntTab& som_elem=zone.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const int nse=zone.nb_som_elem();
  int nb_faces_tot = zone_VEF.nb_faces_tot();

  Sortie& journal = Process::Journal();
  int afficher_message = 0;
  int nb_som=zone.nb_som();

  int ecrire_decoupage_som=0;
  int decoupage_som=0;
  // On n'ecrit qu'une seule fois le fichier decoupage_som
  // et uniquement en sequentiel
  if (Process::nproc()==1 && equation().schema_temps().nb_pas_dt()==0)
    decoupage_som=1;
  //SFichier* os=NULL;
  SChaine decoup_som;
  decoup_som << "1" << finl;
  decoup_som << Objet_U::dimension << " " << nb_som << finl;
  IntVect somm(dimension+2);

  for (int k=0; k<nb_som; k++)
    {
      int sommet = dom.get_renum_som_perio(k);
      if (nb_degres_liberte(sommet) != 0)
        continue;
      if (!afficher_message && VerifierCoin::expert_only==0)
        {
          afficher_message = 1;
          Cerr << finl << "Problem with the mesh used for the VEF P1Bulle discretization." << finl;
          journal << "List of nodes with no degrees of freedom :" << finl;
        }
      const double x = dom.coord(sommet, 0);
      const double y = dom.coord(sommet, 1);
      const double z = (Objet_U::dimension==3) ? dom.coord(sommet, 2) : 0.;

      journal << "Error node " << sommet << " ( " << x << " " << y << " " << z << " )\n";
      // On affiche la liste des indices d'elements reels et virtuels qui contiennent
      // ce sommet. On affiche la lettre "v" pour les elements virtuels.
      journal << "Elements ";
      const int nb_elem_tot = zone.nb_elem_tot();
      const int nb_elem     = zone.nb_elem();
      for (int elem = 0; elem < nb_elem_tot; elem++)
        for(int som = 0; som < nse; som++)
          if (som_elem(elem, som) == sommet)
            {
              journal << elem << ((elem>=nb_elem)?"v ":" ");

              // Ecriture dans le fichier decoupage_som
              int face_opp=elem_faces(elem,som);
              int elem_opp;
              somm=-1;
              somm(0)=sommet;

              int elem1 = face_voisins(face_opp,0);
              int elem2 = face_voisins(face_opp,1);

              if (elem1==elem)
                elem_opp=elem2;
              else
                elem_opp=elem1;

              int i=2;
              for(int som1=0; som1<nse; som1++)  // on parcourt les sommets de elem_opp
                {
                  int ok=1;
                  for(int som2=0; som2<nse; som2++)  // on parcourt les sommets de elem
                    if ( som_elem(elem, som2)==som_elem(elem_opp, som1) )
                      ok=0;
                  if (ok) somm(1)=som_elem(elem_opp, som1);
                  else
                    {
                      somm(i)=som_elem(elem_opp, som1);  // sommets de la face commune
                      i++;
                    }
                }
              if (decoupage_som)
                {
                  ecrire_decoupage_som=1;
                  for (int j=0; j<dimension+2; j++)
                    decoup_som << somm(j) << " " ;
                  decoup_som << elem << " " << elem_opp << finl;
                }
            }
      journal << "\n";
      // On affiche la liste des faces qui contiennent ce sommet.
      // Pour les faces de bord, on affiche la condlim,
      // pour les faces virtuelles, la lettre "v"
      journal << "\nFaces ";
      const int nb_faces = zone_VEF.nb_faces();
      const int nb_som_face = zone_VEF.face_sommets().dimension(1);
      for (int face = 0; face < nb_faces_tot; face++)
        {
          for (int som = 0; som < nb_som_face; som++)
            {
              if (zone_VEF.face_sommets(face, som) == sommet)
                {
                  journal << face;
                  if (face >= nb_faces) // Face virtuelle
                    journal << "v";
                  const int cl = find_cl_face(zone, face);
                  // Face de bord reelle:
                  if (cl >= 0)
                    {
                      const Nom& nom_bord = zone.frontiere(cl).le_nom();
                      journal << "(boundary=" << nom_bord << ")";
                    }
                  journal << " ";
                }
            }
        }
      journal << finl;
    }

  if (ecrire_decoupage_som)
    {
      decoup_som << "-1";
      SFichier fic(Objet_U::nom_du_cas()+".decoupage_som");
      fic << decoup_som.get_str() << finl;
      fic.close();

    }
  if (afficher_message && VerifierCoin::expert_only==0)
    {
      Nom nom_fichier(nom_du_cas());
      Cerr << "Look at the .log file of processor " << Process::me() << " to know which nodes" << finl;
      Cerr << "do not have enough degrees of freedom. You can also visualize" << finl;
      Cerr << "your mesh to identify which elements have a problem and" << finl;
      Cerr << "remesh your domain close to these nodes so that these have" << finl;
      Cerr << "at least a degree of freedom." << finl;
      Cerr << "The VEF P1Bulle discretization is not compatible with this type" << finl;
      Cerr << "of mesh and boundary conditions used (Dirichlet, Symmetry ,...)" << finl << finl;
      Cerr << "Other possibility, in your data file:"<<finl;
      if (dimension==2)
        Cerr << "If you have used \"Trianguler\", substituted by \"Trianguler_H\"." << finl << finl;
      if (dimension==3)
        Cerr << "If you have used \"Tetraedriser\", substituted by \"Tetraedriser_homogene\" or \"Tetraedriser_par_prisme\"." << finl << finl;
      Cerr << "Or insert the line:" << finl;
      Cerr << "VerifierCoin " << dom.le_nom() << " { [Read_file " << nom_fichier << ".decoupage_som] }" << finl;
      Cerr << "after the mesh is finished to be read and built." << finl;
      if (Process::nproc()>1)
        Cerr << "and BEFORE the keyword \"Decouper\" during the partition of the mesh." << finl;
      else
        Cerr << "and BEFORE the keyword \"Discretiser\"." << finl;
      Cerr << "A few cells will be divided into 3 (2D) or 4 (3D)." << finl;
      Cerr << finl;
      Cerr << "Last possibility, it is possible now for experimented users to not check if there is enough degrees of freedom" << finl;
      Cerr << "by adding the next line BEFORE the keyword \"Discretiser\":" << finl;
      Cerr << "VerifierCoin " << dom.le_nom() << " { expert_only } " << finl;
      Cerr << "In this case, check the results carefully." << finl;
      exit();
    }
}
