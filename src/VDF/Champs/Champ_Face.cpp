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

#include <Champ_Face.h>
#include <Champ_Uniforme_Morceaux.h>
#include <Champ_Uniforme.h>
#include <Champ_Don_lu.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Equation_base.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Dirichlet_entree_fluide.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>

Implemente_instanciable(Champ_Face,"Champ_Face",Champ_Inc_base);

// printOn


Sortie& Champ_Face::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

// readOn


Entree& Champ_Face::readOn(Entree& s)
{
  lire_donnees (s) ;
  return s ;
}

/////////////////////////////////////////////////////////////////////
//
//   Implementation des fonctions de la classe Champ_Face
//
//////////////////////////////////////////////////////////////////////

const Zone_dis_base& Champ_Face::zone_dis_base() const
{
  return la_zone_VDF.valeur();
}

void Champ_Face::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  la_zone_VDF=ref_cast(Zone_VDF, z_dis);
}

int Champ_Face::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  assert(nb_noeuds == zone_vdf().nb_faces());
  const MD_Vector& md = zone_vdf().md_vector_faces();
  // Probleme: nb_comp vaut 2 mais on ne veut qu'une dimension !!!
  // HACK :
  int old_nb_compo = nb_compo_;
  nb_compo_ = 1;
  creer_tableau_distribue(md);
  nb_compo_ = old_nb_compo;
  return nb_noeuds;
}

void Champ_Face::dimensionner_tenseur_Grad()
{
  tau_diag_.resize(la_zone_VDF->nb_elem(),dimension);
  tau_croises_.resize(la_zone_VDF->nb_aretes(),2);
}

Champ_base& Champ_Face::affecter_(const Champ_base& ch)
{
  const DoubleTab& v = ch.valeurs();
  DoubleTab& val = valeurs();
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_faces = zone_VDF.nb_faces();
  const IntVect& orientation = zone_VDF.orientation();
  int ori, n0, n1;

  if (sub_type(Champ_Uniforme,ch))
    {
      for (int num_face=0; num_face<nb_faces; num_face++)
        val(num_face) = v(0,orientation(num_face));
    }
  else if ( (sub_type(Champ_Uniforme_Morceaux,ch))
            || (sub_type(Champ_Don_lu,ch)) )
    {
      int ndeb_int = zone_VDF.premiere_face_int();
      const IntTab& face_voisins = zone_VDF.face_voisins();
      int num_face;

      for (num_face=0; num_face<ndeb_int; num_face++)
        {
          ori = orientation(num_face);
          n0 = face_voisins(num_face,0);
          if (n0 != -1)
            val(num_face) = v(n0,ori);
          else
            val(num_face) = v(face_voisins(num_face,1),ori);
        }

      for (num_face=ndeb_int; num_face<nb_faces; num_face++)
        {
          ori = orientation(num_face);
          n0 = face_voisins(num_face,0);
          n1 = face_voisins(num_face,1);
          val(num_face) = 0.5*(v(n0,ori)+v(n1,ori));
        }
    }
  else
    {
      //      int ndeb_int = zone_VDF.premiere_face_int();
      //      const IntTab& face_voisins = zone_VDF.face_voisins();
      DoubleTab positionX(zone_VDF.nb_faces_X() ,dimension);
      DoubleVect U(zone_VDF.nb_faces_X());
      DoubleTab positionY(zone_VDF.nb_faces_Y() ,dimension);
      DoubleVect V(zone_VDF.nb_faces_Y());
      DoubleTab positionZ(zone_VDF.nb_faces_Z() ,dimension);
      DoubleVect W(zone_VDF.nb_faces_Z());
      const DoubleTab& xv=zone_VDF.xv();
      int nbx=0;
      int nby=0;
      int nbz=0;
      int num_face, k;

      for (num_face=0; num_face<nb_faces; num_face++)
        {
          ori = orientation(num_face);
          switch(ori)
            {
            case 0:
              for(k=0; k<dimension; k++)
                positionX(nbx, k)=xv(num_face, k);
              nbx++;
              break;
            case 1:
              for(k=0; k<dimension; k++)
                positionY(nby, k)=xv(num_face, k);
              nby++;
              break;
            case 2:
              for(k=0; k<dimension; k++)
                positionZ(nbz, k)=xv(num_face, k);
              nbz++;
              break;
            }
        }

      ch.valeur_aux_compo(positionX, U, 0);
      ch.valeur_aux_compo(positionY, V, 1);
      if(dimension == 3)
        ch.valeur_aux_compo(positionZ, W, 2);
      nbx=nby=nbz=0;
      for (num_face=0; num_face<nb_faces; num_face++)
        {
          ori = orientation(num_face);
          switch(ori)
            {
            case 0:
              val(num_face) = U(nbx++);
              break;
            case 1:
              val(num_face) = V(nby++);
              break;
            case 2:
              val(num_face) = W(nbz++);
              break;
            }
        }
    }
  return *this;
}

const Champ_Proto& Champ_Face::affecter(const double x1,const double x2)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  DoubleTab& val = valeurs();
  for (int num_face=0; num_face<val.size(); num_face++)
    {
      int ori = orientation(num_face);
      switch(ori)
        {
        case 0 :
          val(num_face)=x1;
          break;
        case 1 :
          val(num_face)=x2;
          break;
        }
    }
  return *this;
}

const Champ_Proto& Champ_Face::affecter(const double x1,const double x2,
                                        const double x3)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  DoubleTab& val = valeurs();
  for (int num_face=0; num_face<val.size(); num_face++)
    {
      int ori = orientation(num_face);
      switch(ori)
        {
        case 0 :
          val(num_face)=x1;
          break;
        case 1 :
          val(num_face)=x2;
          break;
        case 2 :
          val(num_face)=x3;
          break;
        }
    }
  return *this;
}

const Champ_Proto& Champ_Face::affecter(const DoubleTab& v)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntVect& orientation = zone_VDF.orientation();
  DoubleTab& val = valeurs();

  if (v.nb_dim() == 2)
    {
      if (v.dimension(1) == dimension)
        {
          if (v.dimension(0) == val.size())
            for (int num_face=0; num_face<val.size(); num_face++)
              val(num_face)=v(num_face,orientation(num_face));
          else
            {
              Cerr << "Erreur TRUST dans Champ_Face::affecter(const DoubleTab& )" << finl;
              Cerr << "les dimensions du DoubleTab passe en parametre sont incompatibles " << finl;
              Cerr << "avec celles du Champ_Face " << finl;
              exit();
            }
        }
      else
        {
          Cerr << "Erreur TRUST dans Champ_Face::affecter(const DoubleTab& )" << finl;
          Cerr << "les dimensions du DoubleTab passe en parametre sont incompatibles " << finl;
          Cerr << "avec celles du Champ_Face " << finl;
          exit();
        }
    }
  return *this;
}

//-Cas CL periodique : assure que les valeurs sur des faces periodiques
// en vis a vis sont identiques. Pour cela on prend la demi somme des deux valeurs.
void Champ_Face::verifie_valeurs_cl()
{
  const Zone_Cl_dis_base& zcl = zone_Cl_dis().valeur();
  int nb_cl = zcl.nb_cond_lim();
  DoubleTab& ch_tab = valeurs();
  int ndeb,nfin,num_face;

  for (int i=0; i<nb_cl; i++)
    {
      const Cond_lim_base& la_cl = zcl.les_conditions_limites(i).valeur();
      if (sub_type(Periodique,la_cl))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          int voisine;
          double moy;

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              voisine = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
              if (ch_tab[num_face] != ch_tab[voisine])
                {
                  moy = 0.5*(ch_tab[num_face] + ch_tab[voisine]);
                  ch_tab[num_face] = moy;
                  ch_tab[voisine] = moy;
                }
            }
        }
    }
  ch_tab.echange_espace_virtuel();
}

// Description :
// Renvoie la valeur que devrait avoir le champ sur une face de bord,
// si on en croit les conditions aux limites. Le numero est compte
// dans la liste des faces de bord reelles. Le temps considere est le
// present du Champ_Face
// L'implementation a change : ces valeurs ne sont plus stockees dans le champ.

double Champ_Face::val_imp_face_bord_private(int face,int comp) const
{
  const Zone_Cl_VDF& zclo=ref_cast(Zone_Cl_VDF,equation().zone_Cl_dis().valeur());
  return Champ_Face_get_val_imp_face_bord_sym(valeurs(),temps(), face,comp,  zclo);
}


double Champ_Face_get_val_imp_face_bord(const double temp,int face,int comp, const Zone_Cl_VDF& zclo)
{
  const Zone_VDF& zone_vdf=zclo.zone_VDF();
  int face_globale,face_locale;

  face_globale=face+zone_vdf.premiere_face_bord(); // Maintenant numero dans le tableau global des faces.
  const Zone_Cl_dis_base& zcl = zclo; //equation().zone_Cl_dis().valeur();
  // On recupere la CL associee a la face et le numero local de la face dans la frontiere.
  //assert(equation().zone_Cl_dis().valeur()==zclo);

  const Cond_lim_base& cl=(face<zone_vdf.nb_faces()) ?
                          zcl.condition_limite_de_la_face_reelle(face_globale,face_locale) :
                          zcl.condition_limite_de_la_face_virtuelle(face_globale,face_locale);

  int ori = zone_vdf.orientation()(face_globale);

  const DoubleTab& vals=cl.champ_front()->valeurs_au_temps(temp);

  int face_de_vals=vals.dimension(0)==1 ? 0 : face_locale;

  if(sub_type(Symetrie,cl))
    {
      if (comp == ori)
        return 0;
      else
        {
          Cerr<<"You should call Champ_Face_get_val_imp_face_bord_sym and not  Champ_Face_get_val_imp_face_bord"<<finl;
          Process::exit();
          return 1e9;
        }
    }

  else if ( sub_type(Dirichlet_entree_fluide,cl) )
    {
      return vals(face_de_vals,comp);
    }

  else if ( sub_type(Dirichlet_paroi_fixe,cl) )
    {
      return 0;
    }

  else if ( sub_type(Dirichlet_paroi_defilante,cl) )
    {
      return vals(face_de_vals,comp);
    }

  return 0; // All other cases

}
double Champ_Face_get_val_imp_face_bord_sym(const DoubleTab& tab_valeurs, const double temp,int face,int comp, const Zone_Cl_VDF& zclo)
{
  const Zone_VDF& zone_vdf=zclo.zone_VDF();
  int face_globale,face_locale;

  face_globale=face+zone_vdf.premiere_face_bord(); // Maintenant numero dans le tableau global des faces.
  const Zone_Cl_dis_base& zcl = zclo; //equation().zone_Cl_dis().valeur();
  // On recupere la CL associee a la face et le numero local de la face dans la frontiere.
  //assert(equation().zone_Cl_dis().valeur()==zclo);

  const Cond_lim_base& cl=(face<zone_vdf.nb_faces()) ?
                          zcl.condition_limite_de_la_face_reelle(face_globale,face_locale) :
                          zcl.condition_limite_de_la_face_virtuelle(face_globale,face_locale);

  const IntTab& face_voisins = zone_vdf.face_voisins();
  const IntTab& elem_faces = zone_vdf.elem_faces();
  const DoubleVect& porosite = zone_vdf.porosite_face();
  int ori = zone_vdf.orientation()(face_globale);

  const DoubleTab& vals=cl.champ_front()->valeurs_au_temps(temp);

  int face_de_vals=vals.dimension(0)==1 ? 0 : face_locale;

  if(sub_type(Symetrie,cl))
    {
      if (comp == ori)
        return 0;
      else
        {
          int elem=0;
          if (face_voisins(face_globale,0) != -1)
            elem = face_voisins(face_globale,0);
          else
            elem = face_voisins(face_globale,1);
          int comp2=comp+Objet_U::dimension;
          return (tab_valeurs(elem_faces(elem,comp))*porosite[elem_faces(elem,comp)]
                  + tab_valeurs(elem_faces(elem,comp2))*porosite[elem_faces(elem,comp2)])
                 /    (porosite[elem_faces(elem,comp)] + porosite[elem_faces(elem,comp2)]) ;
        }
    }

  else if ( sub_type(Dirichlet_entree_fluide,cl) )
    {
      return vals(face_de_vals,comp);
    }

  else if ( sub_type(Dirichlet_paroi_fixe,cl) )
    {
      return 0;
    }

  else if ( sub_type(Dirichlet_paroi_defilante,cl) )
    {
      return vals(face_de_vals,comp);
    }

  return 0; // All other cases

}


// WEC : jamais appele !!
double Champ_Face::val_imp_face_bord_private(int face,int comp1,int comp2) const
{
  Cerr << "Champ_Face::val_imp_face_bord(,,) exit" << finl;
  exit();
  return 0; // For compilers
}

double Champ_Face_get_val_imp_face_bord( const double temp,int face,int comp, int comp2, const Zone_Cl_VDF& zclo)
{
  Cerr << "Champ_Face::val_imp_face_bord(,,) exit" << finl;
  Process::exit();
  return 0; // For compilers
}
// Cette fonction retourne :
//   1 si le fluide est sortant sur la face num_face
//   0 si la face correspond a une reentree de fluide

int Champ_Face::compo_normale_sortante(int num_face) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int signe =1;
  double vit_norm;
  // signe vaut -1 si face_voisins(num_face,0) est a l'exterieur
  // signe vaut  1 si face_voisins(num_face,1) est a l'exterieur
  if (zone_VDF.face_voisins(num_face,0) == -1)
    signe = -1;
  vit_norm= (*this)(num_face)*signe;
  return (vit_norm > 0);
}

DoubleTab& Champ_Face::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps, int distant) const
{
  return Champ_Face_implementation::trace(fr, valeurs(tps), x,distant);
}

void Champ_Face::mettre_a_jour(double un_temps)
{
  Champ_Inc_base::mettre_a_jour(un_temps);
}
