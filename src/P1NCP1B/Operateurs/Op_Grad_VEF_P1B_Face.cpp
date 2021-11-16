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
// File:        Op_Grad_VEF_P1B_Face.cpp
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/49
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Grad_VEF_P1B_Face.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Neumann_sortie_libre.h>
#include <Navier_Stokes_std.h>
#include <Domaine.h>
#include <Schema_Temps.h>
#include <Debog.h>
#include <Check_espace_virtuel.h>
#include <DoubleTrav.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <EcrFicPartage.h>
#include <communications.h>

Implemente_instanciable(Op_Grad_VEF_P1B_Face,"Op_Grad_VEFPreP1B_P1NC",Op_Grad_VEF_Face);

//// printOn
//

Sortie& Op_Grad_VEF_P1B_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Grad_VEF_P1B_Face::readOn(Entree& s)
{
  return s ;
}

const Zone_VEF_PreP1b& Op_Grad_VEF_P1B_Face::zone_Vef() const
{
  return ref_cast(Zone_VEF_PreP1b, la_zone_vef.valeur());
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
static void verifier(const Op_Grad_VEF_P1B_Face& op,
                     int& init,
                     const Zone_VEF_PreP1b& zone_VEF,
                     const DoubleTab& pre,
                     DoubleTab& grad)

{
  // Methode verifier ne marche que pour P0+P1 en 2D et P0+P1+Pa en 3D
  if (Objet_U::dimension==2 && zone_VEF.get_alphaE()+zone_VEF.get_alphaS()!=2)
    {
      Cerr << "Methode verifier de Op_Grad_VEF_P1B_Face non valable pour cette discretisation." << finl;
      Process::exit();
    }
  if (Objet_U::dimension==3 && zone_VEF.get_alphaE()+zone_VEF.get_alphaS()+zone_VEF.get_alphaA()!=3)
    {
      Cerr << "Methode verifier de Op_Grad_VEF_P1B_Face non valable pour cette discretisation." << finl;
      Process::exit();
    }
  const Zone& zone = zone_VEF.zone();
  const Domaine& dom=zone.domaine();
  int nb_elem_tot=zone.nb_elem_tot();
  int nb_som_tot=dom.nb_som_tot();
  int nb_elem=zone.nb_elem();
  init=1;
  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  const DoubleTab& xp = zone_VEF.xp();
  const DoubleTab& coord_sommets=dom.coord_sommets();

  // Verification de la pression arete
  DoubleTab tab(pre);
  exemple_champ_non_homogene(zone_VEF, tab);

  DoubleTab r(grad);
  r=0;
  op.ajouter(tab, r);

  // Pression mise a 1 sur tous les noeuds
  DoubleTab p(pre);
  p=1;
  r=0;
  op.ajouter(p, r);
  if (nb_elem<30)
    {
      Cerr << Process::me() << " grad(1) som = " << finl;
      r.ecrit(Cerr);
    }
  // Pression mise a 1 sur les elements seulement
  p=0;
  int i=0;
  for(; i<nb_elem_tot; i++)
    p(i)=0;
  r=0;
  op.ajouter(p, r);
  Cerr << "["<<Process::me() << "] p(1) elem = " << finl;
  p.ecrit(Cerr);
  Cerr << "["<<Process::me() << "] grad(1) elem = " << finl;
  r.ecrit(Cerr);

  // Pression mise a 1 sur les sommets seulement
  p=0;
  for(; i<nb_elem_tot+nb_som_tot; i++)
    p(i)=1;
  r=0;
  op.ajouter(p, r);
  Cerr << Process::me() << " grad(1) som = " << finl;
  r.ecrit(Cerr);

  // Pression mise a 1 sur les aretes seulement
  p=0;
  int sz=p.size_totale();
  for(; i<sz; i++)
    p(i)=1;
  r=0;
  op.ajouter(p, r);
  Cerr << Process::me() << " grad(1) aretes = " << finl;
  r.ecrit(Cerr);
  // Pression variable sur les elements seulement
  p=0;
  for(i=0; i<nb_elem_tot; i++)
    p(i)=xp(i,0)-1;
  r=0;
  op.ajouter(p, r);
  int nbf=volumes_entrelaces.size();
  for(i=0; i<nbf; i++)
    for(int comp=0; comp<Objet_U::dimension; comp++)
      r(i,comp)/=volumes_entrelaces(i);
  Cerr << Process::me() << " grad(x-1) elem = " << finl;
  r.ecrit(Cerr);

  // Pression variable sur les sommets seulement
  p=0;
  for(i=nb_elem_tot; i<nb_elem_tot+nb_som_tot; i++)
    {
      p(i)=coord_sommets(i-nb_elem_tot,0)-1;
    }
  r=0;
  op.ajouter(p, r);
  for(i=0; i<nbf; i++)
    for(int comp=0; comp<Objet_U::dimension; comp++)
      r(i,comp)/=volumes_entrelaces(i);
  Cerr << Process::me() << " grad(x-1) som = " << finl;
  r.ecrit(Cerr);

  /*
    const IntTab& som_elem=zone.les_elems();
    int nfe=zone.nb_faces_elem();
    p=0;
    int ii=nb_elem_tot+nb_som_tot/2;
    p(ii)=1;
    r=0;
    op.ajouter(p, r);
    Debog::verifier("toto",r);
    for(i=0; i<nbf; i++)
    for(int comp=0; comp<Objet_U::dimension; comp++)
    r(i,comp)/=volumes_entrelaces(i);
    Cerr << "grad(1K) = " << r << finl;
    for(i=0; i<nb_elem_tot; i++)
    for(int indice=0; indice<nfe; indice++)
    {
    int som = nb_elem_tot+dom.get_renum_som_perio(som_elem(i,indice));
    if(som==ii) p(i)+=1./12.;
    }
    r=0;
    op.ajouter(p, r);
    for(i=0; i<nbf; i++)
    for(int comp=0; comp<Objet_U::dimension; comp++)
    r(i,comp)/=volumes_entrelaces(i);
    Cerr << "grad(eta) = " << r << finl;
    }
  */
}


DoubleTab& Op_Grad_VEF_P1B_Face::
modifier_grad_pour_Cl(DoubleTab& grad ) const
{
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b,
                                             la_zone_vef.valeur());
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const Zone_Cl_VEF& zone_Cl_VEF=la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  int nb_bords =les_cl.size();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      {
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        int num1 = le_bord.num_premiere_face();
        int num2 = num1 + le_bord.nb_faces();
        if (sub_type(Periodique,la_cl.valeur()))
          {
            //periodicite
            const Periodique& la_cl_perio = ref_cast(Periodique,
                                                     la_cl.valeur());
            ArrOfInt fait(le_bord.nb_faces());
            for (int face=num1; face<num2; face++)
              {
                if(!fait(face-num1))
                  {
                    int face_associee=num1+la_cl_perio.face_associee(face-num1);
                    fait(face-num1)=(fait(face_associee-num1)=1);
                    for (int comp=0; comp<dimension; comp++)
                      grad(face_associee,comp) =
                        (grad(face,comp)+= grad(face_associee,comp));
                  }
              }
          }
        else
          {
            if(sub_type(Symetrie,la_cl.valeur()))
              {
                for (int face=num1; face<num2; face++)
                  {
                    double psc=0;
                    double norm=0;
                    int comp;
                    for (comp=0; comp<dimension; comp++)
                      {
                        psc += grad(face,comp)*face_normales(face,comp);
                        norm += face_normales(face,comp)*face_normales(face,comp);
                      }
                    // psc/=norm; // Fixed bug: Arithmetic exception
                    if (fabs(norm)>=DMINFLOAT) psc/=norm;
                    for (comp=0; comp<dimension; comp++)
                      grad(face,comp)-=psc*face_normales(face,comp);
                  }
              }
            else if(sub_type(Dirichlet,la_cl.valeur())||sub_type(Dirichlet_homogene,la_cl.valeur()))
              {
                for (int face=num1; face<num2; face++)
                  {
                    for (int comp=0; comp<dimension; comp++)
                      grad(face,comp)=0;
                  }
              }
          }
      }
    }
  return grad;
}
DoubleTab& Op_Grad_VEF_P1B_Face::
ajouter_elem(const DoubleTab& pre,
             DoubleTab& grad) const
{
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b,
                                             la_zone_vef.valeur());
  assert(zone_VEF.get_alphaE());
  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleVect& porosite_face=zone_VEF.porosite_face();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  int nfe=zone.nb_faces_elem();
  int nb_elem_tot=zone.nb_elem_tot();
  // Si pas de support P1, on impose Neumann sur P0
  if (zone_VEF.get_alphaS()==0)
    {
      const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
      const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
      int nb_bords =les_cl.size();
      for (int n_bord=0; n_bord<nb_bords; n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
            {
              const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre,la_cl.valeur());
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              for (int face=num1; face<num2; face++)
                {
                  double P_imp = la_sortie_libre.flux_impose(face-num1);
                  double diff = P_imp;
                  for (int comp=0; comp<dimension; comp++)
                    grad(face,comp) += diff*face_normales(face,comp)*porosite_face(face);
                }
            }
        }
    }
  int elem,indice,face,comp;
  ArrOfDouble sigma(dimension);
  for(elem=0; elem<nb_elem_tot; elem++)
    {
      for(indice=0; indice<nfe; indice++)
        {
          double pe=pre(elem);
          face = elem_faces(elem,indice);
          double signe=1;
          if(elem!=face_voisins(face,0)) signe=-1;
          for(comp=0; comp<dimension; comp++)
            grad(face, comp) -= pe*signe*face_normales(face,comp)*porosite_face(face);
        }
    }
  return grad;
}
DoubleTab& Op_Grad_VEF_P1B_Face::
ajouter_som(const DoubleTab& pre,
            DoubleTab& grad) const
{
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b,
                                             la_zone_vef.valeur());
  assert(zone_VEF.get_alphaS());
  const Zone& zone = zone_VEF.zone();
  const Domaine& dom=zone.domaine();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleVect& porosite_face=zone_VEF.porosite_face();
  const IntTab& som_elem=zone.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  int nfe=zone.nb_faces_elem();
  int nps=zone_VEF.numero_premier_sommet();
  int nb_elem_tot=zone.nb_elem_tot();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  ArrOfDouble sigma(dimension);
  for(int elem=0; elem<nb_elem_tot; elem++)
    {
      double coeff_som=calculer_coef_som(elem, zone_Cl_VEF, zone_VEF);
      for(int indice=0; indice<nfe; indice++)
        {
          int som = nps+dom.get_renum_som_perio(som_elem(elem,indice));
          double ps=pre(som);
          int face = elem_faces(elem,indice);
          double signe=1;
          if(elem!=face_voisins(face,0))
            signe=-1;
          for(int comp=0; comp<dimension; comp++)
            sigma[comp]=signe*face_normales(face,comp);

          for(int indice2=0; indice2<nfe; indice2++)
            {
              int face2 = elem_faces(elem,indice2);
              for(int comp=0; comp<dimension; comp++)
                grad(face2,comp)-=coeff_som*ps*sigma[comp]*porosite_face(face2);
            }
        }
    }

  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_sommets = zone_VEF.face_sommets();
  int nb_bords = les_cl.size();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if(sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          const Neumann_sortie_libre& sortie_libre = ref_cast(Neumann_sortie_libre,la_cl.valeur());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (int face=num1; face<num2; face++)
            {
              const double P_imp = sortie_libre.flux_impose(face-num1);
              for(int indice=0; indice<(nfe-1); indice++)
                {
                  int som=nps+dom.get_renum_som_perio(face_sommets(face,indice));
                  for (int comp=0; comp<dimension; comp++)
                    grad(face,comp)-=
                      1./dimension*face_normales(face,comp)*(pre(som)-P_imp)
                      *porosite_face(face);
                }
            } //fin du if sur "Neumann_sortie_libre"
        }
    }
  return grad;
}
DoubleTab& Op_Grad_VEF_P1B_Face::
ajouter_aretes(const DoubleTab& pre,
               DoubleTab& grad) const
{
  const Zone_VEF_PreP1b& zone_VEF =
    ref_cast(Zone_VEF_PreP1b, la_zone_vef.valeur());
  assert(zone_VEF.get_alphaA());
  const Zone& zone = zone_VEF.zone();
  //const Domaine& dom=zone.domaine();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleVect& porosite_face=zone_VEF.porosite_face();
  const IntTab& som_elem=zone.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const IntTab& elem_aretes=zone_VEF.zone().elem_aretes();
  const ArrOfInt& renum_arete_perio=zone_VEF.get_renum_arete_perio();
  const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
  int nfe=zone.nb_faces_elem();
  int nb_elem_tot=zone.nb_elem_tot();
  //  int nps=zone_VEF.numero_premier_sommet();
  int npa=zone_VEF.numero_premiere_arete();
  int indice;
  ArrOfDouble sigma(dimension);
  for(int elem=0; elem<nb_elem_tot; elem++)
    {
      for(indice=0; indice<nfe; indice++)
        {
          //som = nps+dom.get_renum_som_perio(som_elem(elem,indice));
          int face = elem_faces(elem,indice);
          double signe=1;
          if(elem!=face_voisins(face,0))
            signe=-1;
          for(int comp=0; comp<dimension; comp++)
            sigma[comp]=signe*face_normales(face,comp);
        }
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
              arete=renum_arete_perio(arete);
              if(ok_arete(arete))
                {
                  double pa=pre(npa+arete);
                  int niinij=0;
                  for(int ksom=0; ksom<4; ksom++)
                    {
                      int face2=elem_faces(elem,ksom);
                      if((ksom==isom)||(ksom==jsom))
                        {
                          if(niinij==ksom)
                            niinij++;
                          for(int comp=0; comp<3; comp++)
                            grad(face2,comp)+=porosite_face(face2)*1./15.*
                                              (signei*face_normales(facei,comp)+
                                               signej*face_normales(facej,comp))*pa;
                        }
                      else
                        {
                          if(niinij==ksom)
                            niinij++;
                          while((niinij==jsom)||(niinij==isom))
                            niinij++;
                          assert(niinij<4);
                          int facek = elem_faces(elem,niinij);
                          double signek=1.;
                          if (face_voisins(facek,0) != elem)
                            signek=-1.;
                          for(int comp=0; comp<3; comp++)
                            grad(face2,comp)+=porosite_face(face2)*2./15.*
                                              signek*face_normales(facek,comp)*pa;
                          niinij=ksom;
                        }
                    }
                }
            }
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
        int num1 = le_bord.num_premiere_face();
        int num2 = num1 + le_bord.nb_faces();
        if(sub_type(Neumann_sortie_libre,la_cl.valeur()))
          {
            for (int face=num1; face<num2; face++)
              {
                int elem=face_voisins(face,0);
                for(int i=0; i<2; i++)
                  {
                    int somi=face_sommets(face,i);
                    for(int j=i+1; j<3; j++)
                      {
                        int somj=face_sommets(face,j);
                        int arete=chercher_arete(elem, somi, somj,
                                                 elem_aretes, aretes_som);
                        arete=renum_arete_perio(arete);
                        for (int comp=0; comp<dimension; comp++)
                          grad(face,comp)-= porosite_face(face)*0*
                                            face_normales(face,comp)*pre(npa+arete);
                      }
                  }
              }
          }
      }
    }
  return grad;
}

double Op_Grad_VEF_P1B_Face::calculer_coef_som(int elem,  const Zone_Cl_VEF& zcl,
                                               const Zone_VEF_PreP1b& zone_VEF)
{
  double coef_std=1./(Objet_U::dimension*(Objet_U::dimension+1));
  if (zone_VEF.get_modif_div_face_dirichlet()==0)
    return coef_std;
  int type_elem=0;
  int rang_elem=zone_VEF.rang_elem_non_std()(elem);
  if (rang_elem!=-1)
    type_elem=zcl.type_elem_Cl(rang_elem);
  double coef;
  if (type_elem==0)
    return coef_std;
  if (Objet_U::dimension==2)
    {
      switch(type_elem)
        {
        case 0:
          coef=3;
          break;
        case 1:
        case 2:
        case 4:
          coef=2;
          break;
        case 3:
        case 5:
        case 6:
          coef=1;
          break;
        default:
          coef=-1;
          Process::exit();
          break;
        }
    }
  else if (Objet_U::dimension==3)
    {
      switch(type_elem)
        {
        case 0:
          coef=4;
          break;
        case 1:
        case 2:
        case 4:
        case 8:
          coef=3;
          break;
        case 3:
        case 5:
        case 6:
        case 9:
        case 10:
        case 12:
          coef=2;
          break;
        case 7:
        case 11:
        case 13:
        case 14:
          coef=1;
          break;
        default:
          coef=-1;
          Process::exit();
          break;
        }
    }
  else
    {
      coef=-1;
      Process::exit();
    }
  return 1./(Objet_U::dimension*coef);
}

DoubleTab& Op_Grad_VEF_P1B_Face::ajouter(const DoubleTab& pre,
                                         DoubleTab& grad) const
{
  // pre doit avoir son espace virtuel a jour
  assert_espace_virtuel_vect(pre);
  // on va faire += sur l'espace virtuel, mais sans utiliser les valeurs
  assert_invalide_items_non_calcules(grad);
  //Debog::verifier("Op_Grad_VEF_P1B_Face::ajouter pre", pre);
  const Zone_VEF_PreP1b& zone_VEF = ref_cast(Zone_VEF_PreP1b,
                                             la_zone_vef.valeur());
  static int init=1;
  if(!init)
    verifier(*this, init, zone_VEF, pre, grad);
  if (zone_VEF.get_alphaE()) ajouter_elem(pre, grad);
  if (zone_VEF.get_alphaS()) ajouter_som(pre, grad);
  if (zone_VEF.get_alphaA()) ajouter_aretes(pre, grad);
  modifier_grad_pour_Cl(grad);
  //Optimisation car pas necessaire:
  //grad.echange_espace_virtuel();
  //Debog::verifier("Op_Grad_VEF_P1B_Face::ajouter grad en sortie:", grad);
  return grad;
}


int Op_Grad_VEF_P1B_Face::impr(Sortie& os) const
{
  const int impr_mom=la_zone_vef->zone().Moments_a_imprimer();
  const int impr_sum=(la_zone_vef->zone().Bords_a_imprimer_sum().est_vide() ? 0:1);
  const int impr_bord=(la_zone_vef->zone().Bords_a_imprimer().est_vide() ? 0:1);
  const Schema_Temps_base& sch = equation().probleme().schema_temps();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF_PreP1b& zone_VEF = zone_Vef();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const IntTab& som_elem=la_zone_vef->zone().les_elems();
  const Equation_base& eqn = equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,eqn);
  const Champ_P1_isoP1Bulle& la_pression_P1B = ref_cast(Champ_P1_isoP1Bulle,eqn_hydr.pression_pa().valeur());
  // Si on filtre:
  la_pression_P1B.filtrage(zone_VEF,la_pression_P1B);
  const DoubleVect& pression_P1B = la_pression_P1B.champ_filtre();
  Debog::verifier("la pression filtree dans Op_Grad_VEF_P1B_Face::impr=",pression_P1B);
  double pres_som, pres_arete, pres_tot ;
  double n0, n1, n2 ;
  const int nb_faces =  zone_VEF.nb_faces_tot();
  int nse=zone_VEF.zone().nb_som_elem();
  int nps=zone_VEF.numero_premier_sommet();
  int npa=zone_VEF.numero_premiere_arete();
  int elem ;
  int face;
  int som_opp, som, som_glob;
  DoubleTab xgr(nb_faces,dimension);
  xgr=0.;
  //DoubleVect moment(dimension);
  if (impr_mom)
    {
      const DoubleTab& xgrav = zone_VEF.xv();
      const ArrOfDouble& c_grav=la_zone_vef->zone().cg_moments();
      for (int num_face=0; num_face <nb_faces; num_face++)
        for (int i=0; i<dimension; i++)
          xgr(num_face,i)=xgrav(num_face,i)-c_grav(i);
    }
  flux_bords_.resize(zone_VEF.nb_faces_bord(),dimension);
  flux_bords_ = 0.;

  DoubleTrav tab_flux_bords(3,zone_VEF.nb_front_Cl(),dimension);
  tab_flux_bords=0.;
  /*
    fluxx_s       ->   flux_bords2(0,num_cl,0)
    fluxy_s       ->   flux_bords2(0,num_cl,1)
    fluxz_s       ->   flux_bords2(0,num_cl,2)
    fluxx_sum_s   ->   flux_bords2(1,num_cl,0)
    fluxy_sum_s   ->   flux_bords2(1,num_cl,1)
    fluxz_sum_s   ->   flux_bords2(1,num_cl,2)
    moment(0)     ->   flux_bords2(2,num_cl,0)
    moment(1)     ->   flux_bords2(2,num_cl,1)
    moment(2)     ->   flux_bords2(2,num_cl,2)
  */
  double coeff_P1 = 1./dimension;
  double coeff_Pa = 1./dimension;
  int nb_bord = zone_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int impr_boundary = (zone_VEF.zone().Bords_a_imprimer_sum().contient(le_bord.le_nom()) ? 1 : 0);
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      for (face=ndeb; face<nfin; face++)
        {
          int voisin = 0;
          elem = face_voisins(face,voisin);
          if (zone_VEF.get_num_fac_loc(face, voisin)==-1)
            {
              voisin = 1;
              elem = face_voisins(face,voisin);
            }
          int signe=zone_VEF.oriente_normale(face,elem);
          som_opp = la_zone_vef->get_num_fac_loc(face, voisin);
          som_opp = som_elem(elem, som_opp);
          pres_tot = 0. ;

          // Contribution de la pression P0
          if (zone_VEF.get_alphaE())
            pres_tot += pression_P1B(elem);

          // Contribution de la pression P1
          if (zone_VEF.get_alphaS())
            {
              pres_som = 0.;
              for(som=0; som<nse; som++)
                {
                  som_glob = som_elem(elem, som);
                  if (som_glob != som_opp)
                    pres_som += pression_P1B(nps+som_glob);
                }
              pres_tot += coeff_P1 * pres_som;
            }

          // Contribution de la pression Pa
          // GF descativation car bilan incorret
          if (zone_VEF.get_alphaA()*0!=0)
            {
              pres_arete = 0.;
              const IntTab& elem_aretes=la_zone_vef->zone().elem_aretes();
              const IntTab& aretes_som=la_zone_vef->zone().aretes_som();
              for(int arete=0; arete<6; arete++)
                {
                  int arete_glob = elem_aretes(elem, arete);
                  if (aretes_som(arete_glob,0)!=som_opp && aretes_som(arete_glob,1)!=som_opp)
                    pres_arete += pression_P1B(npa+arete_glob);
                }
              pres_tot += coeff_Pa * pres_arete;
            }

          // Calcul de la resultante et du couple de pression
          if (dimension == 2)
            {
              n0 = signe*face_normales(face,0) ;
              n1 = signe*face_normales(face,1) ;

              flux_bords_(face,0) = pres_tot*n0  ;
              flux_bords_(face,1) = pres_tot*n1  ;

              tab_flux_bords(0,n_bord,0) += flux_bords_(face,0) ;
              tab_flux_bords(0,n_bord,1) += flux_bords_(face,1) ;

              // Calcul du moment exerce par le fluide sur le bord (OM/\F)
              if (impr_mom)
                tab_flux_bords(2,n_bord,0)+=flux_bords_(face,1)*xgr(face,0)-flux_bords_(face,0)*xgr(face,1);
              if (impr_boundary)
                {
                  tab_flux_bords(1,n_bord,0) += flux_bords_(face,0) ;
                  tab_flux_bords(1,n_bord,1) += flux_bords_(face,1) ;
                }
            }
          else if (dimension == 3)
            {
              n0   = signe*face_normales(face,0) ;
              n1   = signe*face_normales(face,1) ;
              n2   = signe*face_normales(face,2) ;

              flux_bords_(face,0) = pres_tot*n0 ;
              flux_bords_(face,1) = pres_tot*n1 ;
              flux_bords_(face,2) = pres_tot*n2 ;

              tab_flux_bords(0,n_bord,0) += flux_bords_(face,0) ;
              tab_flux_bords(0,n_bord,1) += flux_bords_(face,1) ;
              tab_flux_bords(0,n_bord,2) += flux_bords_(face,2) ;

              if (impr_mom)
                {
                  // Calcul du moment exerce par le fluide sur le bord (OM/\F)
                  tab_flux_bords(2,n_bord,0)+=flux_bords_(face,2)*xgr(face,1)-flux_bords_(face,1)*xgr(face,2);
                  tab_flux_bords(2,n_bord,1)+=flux_bords_(face,0)*xgr(face,2)-flux_bords_(face,2)*xgr(face,0);
                  tab_flux_bords(2,n_bord,2)+=flux_bords_(face,1)*xgr(face,0)-flux_bords_(face,0)*xgr(face,1);
                }
              if (impr_boundary)
                {
                  tab_flux_bords(1,n_bord,0) += flux_bords_(face,0) ;
                  tab_flux_bords(1,n_bord,1) += flux_bords_(face,1) ;
                  tab_flux_bords(1,n_bord,2) += flux_bords_(face,2) ;
                }
            }
        }
    } // fin for n_bord

  // On somme les contributions de chaque processeur
  mp_sum_for_each_item(tab_flux_bords);

  if (je_suis_maitre())
    {
      // Open files if needed
      SFichier Flux_grad;
      ouvrir_fichier(Flux_grad,"",1);
      SFichier Flux_grad_moment;
      ouvrir_fichier(Flux_grad_moment,"moment",impr_mom);
      SFichier Flux_grad_sum;
      ouvrir_fichier(Flux_grad_sum,"sum",impr_sum);

      // Write time
      Flux_grad.add_col(sch.temps_courant());
      if (impr_sum) Flux_grad_sum.add_col(sch.temps_courant());
      if (impr_mom) Flux_grad_moment.add_col(sch.temps_courant());

      // Write flux on boundaries
      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          if (dimension == 2)
            {
              Flux_grad.add_col(tab_flux_bords(0,n_bord,0));
              Flux_grad.add_col(tab_flux_bords(0,n_bord,1));
              if (impr_sum)
                {
                  Flux_grad_sum.add_col(tab_flux_bords(1,n_bord,0));
                  Flux_grad_sum.add_col(tab_flux_bords(1,n_bord,1));
                }
              if (impr_mom) Flux_grad_moment.add_col(tab_flux_bords(2,n_bord,0));
            }
          else if (dimension == 3)
            {
              Flux_grad.add_col(tab_flux_bords(0,n_bord,0));
              Flux_grad.add_col(tab_flux_bords(0,n_bord,1));
              Flux_grad.add_col(tab_flux_bords(0,n_bord,2));
              if (impr_sum)
                {
                  Flux_grad_sum.add_col(tab_flux_bords(1,n_bord,0));
                  Flux_grad_sum.add_col(tab_flux_bords(1,n_bord,1));
                  Flux_grad_sum.add_col(tab_flux_bords(1,n_bord,2));
                }
              if (impr_mom)
                {
                  Flux_grad_moment.add_col(tab_flux_bords(2,n_bord,0));
                  Flux_grad_moment.add_col(tab_flux_bords(2,n_bord,1));
                  Flux_grad_moment.add_col(tab_flux_bords(2,n_bord,2));
                }
            }
        }
      Flux_grad << finl;
      if (impr_sum) Flux_grad_sum << finl;
      if (impr_mom) Flux_grad_moment << finl;
    }

  const LIST(Nom)& Liste_Bords_a_imprimer = zone_VEF.zone().Bords_a_imprimer();
  if (!Liste_Bords_a_imprimer.est_vide())
    {
      EcrFicPartage Flux_grad_face;
      ouvrir_fichier_partage(Flux_grad_face,"",impr_bord);
      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          if (zone_VEF.zone().Bords_a_imprimer().contient(le_bord.le_nom()))
            {
              if (je_suis_maitre())
                {
                  Flux_grad_face << "# Force par face sur " << le_bord.le_nom() << " au temps ";
                  sch.imprimer_temps_courant(Flux_grad_face);
                  Flux_grad_face << " : " << finl;
                }
              for (face=ndeb; face<nfin; face++)
                {
                  Flux_grad_face << "# Face a x= " << zone_VEF.xv(face,0) << " y= " << zone_VEF.xv(face,1);
                  if (dimension==3) Flux_grad_face << " z= " << zone_VEF.xv(face,2);
                  Flux_grad_face << " : Fx= " << flux_bords_(face,0) << " Fy= " << flux_bords_(face,1);
                  if (dimension==3) Flux_grad_face << " Fz= " << flux_bords_(face,2);
                  Flux_grad_face << finl;
                }
              Flux_grad_face.syncfile();
            }
        }
    }
  return 1;
}

