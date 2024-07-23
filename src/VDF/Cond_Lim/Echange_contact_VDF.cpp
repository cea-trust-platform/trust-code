/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Modele_turbulence_scal_base.h>
#include <Schema_Euler_Implicite.h>
#include <Turbulence_paroi_scal.h>
#include <Echange_contact_VDF.h>
#include <Champ_front_calc.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Schema_Comm.h>
#include <Milieu_base.h>
#include <Domaine_VDF.h>
#include <TRUST_Ref.h>
#include <TRUSTTrav.h>
#include <ArrOfBit.h>

Implemente_instanciable(Echange_contact_VDF,"Paroi_Echange_contact_VDF",Echange_global_impose);
// XD paroi_echange_contact_vdf condlim_base paroi_echange_contact_vdf -1 Boundary condition type to model the heat flux between two problems. Important: the name of the boundaries in the two problems should be the same.
// XD attr autrepb ref_Pb_base autrepb 0 Name of other problem.
// XD attr nameb chaine nameb 0 Name of bord.
// XD attr temp chaine temp 0 Name of field.
// XD attr h floattant h 0 Value assigned to a coefficient (expressed in W.K-1m-2) that characterises the contact between the two mediums. In order to model perfect contact, h must be taken to be infinite. This value must obviously be the same in both the two problems blocks. NL2 The surface thermal flux exchanged between the two mediums is represented by : NL2 fi = h (T1-T2) where 1/h = d1/lambda1 + 1/val_h_contact + d2/lambda2 NL2 where di : distance between the node where Ti and the wall is found.


int meme_point2(const DoubleVect& a,const DoubleVect& b);

Sortie& Echange_contact_VDF::printOn(Sortie& s ) const { return s << que_suis_je() << finl; }

Entree& Echange_contact_VDF::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Thermique"), Motcle("Neutronique"), Motcle("fraction_massique"), Motcle("indetermine") };

  s >> nom_autre_pb_ >> nom_bord >> nom_champ >> h_paroi;
  T_autre_pb().typer("Champ_front_calc");
  T_ext().typer("Ch_front_var_instationnaire_dep");
  T_ext()->fixer_nb_comp(1);
  return s;
}

void Echange_contact_VDF::completer()
{
  Echange_global_impose::completer();
  int nb_cases=domaine_Cl_dis().equation().schema_temps().nb_valeurs_temporelles();
  T_autre_pb().associer_fr_dis_base(T_ext().frontiere_dis());
  T_autre_pb()->completer();
  T_autre_pb()->fixer_nb_valeurs_temporelles(nb_cases);
}

/*! @brief Change le i-eme temps futur de la CL.
 *
 */
void Echange_contact_VDF::changer_temps_futur(double temps,int i)
{
  Cond_lim_base::changer_temps_futur(temps,i);
  T_autre_pb()->changer_temps_futur(temps,i);
}

/*! @brief Tourne la roue de la CL
 *
 */
int Echange_contact_VDF::avancer(double temps)
{
  int ok=Cond_lim_base::avancer(temps);
  ok = ok && T_autre_pb()->avancer(temps);
  return ok;
}

/*! @brief Tourne la roue de la CL
 *
 */
int Echange_contact_VDF::reculer(double temps)
{
  int ok=Cond_lim_base::reculer(temps);
  ok = ok && T_autre_pb()->reculer(temps);
  return ok;
}


void calculer_h_local(DoubleTab& tab,const Equation_base& une_eqn,const Domaine_VDF& zvdf_2,const Front_VF& front_vf,const Milieu_base& le_milieu,double invhparoi,int opt)
{
  DoubleVect e;
  const IntTab& face_voisins = zvdf_2.face_voisins();
  int i;
  int nb_faces=front_vf.nb_faces();
  int nb_comp = le_milieu.conductivite()->nb_comp();
  int ndeb = front_vf.num_premiere_face();
  int nfin = ndeb + nb_faces;
  bool dequiv=false;

  e.resize(front_vf.nb_faces());

  const RefObjU& modele_turbulence = une_eqn.get_modele(TURBULENCE);

  if (modele_turbulence.non_nul() && sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()) && opt!=1 )
    {
      const Modele_turbulence_scal_base& mod_turb_scal = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
      const Turbulence_paroi_scal& loi_par = mod_turb_scal.loi_paroi();
      if( loi_par->use_equivalent_distance() )
        {
          int boundary_index=-1;
          dequiv=true;
          // Looking for the good boundary
          int nb_boundaries=zvdf_2.domaine().nb_front_Cl();
          for (int n_bord=0; n_bord<nb_boundaries; n_bord++)
            {
              if (zvdf_2.front_VF(n_bord).le_nom() == front_vf.le_nom())
                boundary_index=n_bord;
            }
          for (int ind_face=0; ind_face<nb_faces; ind_face++)
            {
              e(ind_face)=loi_par->equivalent_distance(boundary_index,ind_face);
            }
        }
    }
  if( ! dequiv )
    {
      for (int face=ndeb; face<nfin; face++)
        {








          e(face-ndeb) = zvdf_2.dist_norm_bord(face);
        }
    }

  // Calcul de tab = 1/(e/lambda + 1/h_paroi) =1/(e/lambda+invhparoi)
  if(!sub_type(Champ_Uniforme,le_milieu.conductivite().valeur()))
    {
      const DoubleTab& lambda = le_milieu.conductivite().valeurs();
      for (int face=ndeb; face<nfin; face++)
        {
          int elem = face_voisins(face,0);
          if (elem == -1)
            elem = face_voisins(face,1);
          for(i=0; i<nb_comp; i++)
            {
              assert(le_milieu.conductivite()(elem,i)!=0.);
              tab(face-ndeb,i) = 1./(e(face-ndeb)/lambda(elem,i)+invhparoi);
            }
        }
    }
  else  // la conductivite est un Champ uniforme
    {
      for (int face=ndeb; face<nfin; face++)
        {
          for(i=0; i<nb_comp; i++)
            {
              assert(le_milieu.conductivite()(0,i)!=0.);
              tab(face-ndeb,i) = 1./(e(face-ndeb)/le_milieu.conductivite()(0,i)+invhparoi);
            }
        }
    }

} // fin du cas Raccord_local_homogene


void calculer_h_distant(DoubleTab& tab,const Equation_base& une_eqn,const Domaine_VDF& zvdf_2,const Front_VF& front_vf,const    Milieu_base& le_milieu,double invhparoi,int opt,const Nom& nom_racc2)
{
  DoubleVect e;
  int i;
  int nb_comp = le_milieu.conductivite()->nb_comp();
  int nb_faces_raccord1 =tab.dimension(0); //= domaine_dis1.domaine().raccord(nom_racc1)->nb_faces();
  bool dequiv=false;

  const RefObjU& modele_turbulence = une_eqn.get_modele(TURBULENCE);
  if (modele_turbulence.non_nul() && sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()) && opt!=1 )
    {

      const Modele_turbulence_scal_base& mod_turb_scal = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
      const Turbulence_paroi_scal& loi_par = mod_turb_scal.loi_paroi();
      if( loi_par->use_equivalent_distance() )
        {
          dequiv=true;
          DoubleVect d_equiv_tmp;
          front_vf.frontiere().trace_face_distant( loi_par->equivalent_distance_name(d_equiv_tmp,nom_racc2),e);
          // const Paroi_scal_hyd_base_VDF& loip = ref_cast(Paroi_scal_hyd_base_VDF,loi_par.valeur());
          // front_vf.frontiere().trace_face(loip.d_equiv_nom(d_equiv_tmp,nom_racc2),e);
        }
    }
  if( ! dequiv )
    {
      DoubleVect dist;
      front_vf.frontiere().trace_face_distant(zvdf_2.dist_norm_bord(dist,nom_racc2),e);
    }

  // Calcul de tab = 1/(e/lambda + 1/h_paroi)=1/(e/lambda + invhparoi)

  if(!sub_type(Champ_Uniforme,le_milieu.conductivite().valeur()))
    {
      DoubleTab lambda;
      front_vf.frontiere().trace_elem_distant(le_milieu.conductivite().valeurs(),lambda);
      for (int face=0; face<nb_faces_raccord1; face++)
        for(i=0; i<nb_comp; i++)
          {
            assert(lambda(face,i)!=0.);
            tab(face,i) = 1./(e(face)/lambda(face,i)+invhparoi);
          }
    }
  else  // la conductivite est un Champ uniforme
    {
      const DoubleTab& lambda = le_milieu.conductivite().valeurs();
      for (int face=0; face<nb_faces_raccord1; face++)
        for(i=0; i<nb_comp; i++)
          {
            assert(lambda(0,i)!=0.);
            tab(face,i) = 1./(e(face)/lambda(0,i)+invhparoi);
          }
    }
}        // fin du cas Raccord_distant_homogene


void Echange_contact_VDF::calculer_h_mon_pb(DoubleTab& tab,double invhparoi,int opt)
{
  // forcement local
  const Equation_base& mon_eqn = domaine_Cl_dis().equation();
  const Milieu_base& mon_milieu = mon_eqn.milieu();
  const Domaine_VDF& ma_zvdf = ref_cast(Domaine_VDF,domaine_Cl_dis().domaine_dis().valeur());
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  calculer_h_local(tab,mon_eqn,ma_zvdf,ma_front_vf,mon_milieu,invhparoi,opt);
}

void Echange_contact_VDF::calculer_h_autre_pb(DoubleTab& tab,double invhparoi,int opt)
{
  Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  const Milieu_base& le_milieu=ch.milieu();
  const Domaine_VDF& zvdf_2=ref_cast(Domaine_VDF, ch.domaine_dis());
  const Front_VF& front_vf=ref_cast(Front_VF, ch.front_dis());
  const Equation_base& une_eqn=ch.equation();
  int nb_comp = le_milieu.conductivite()->nb_comp();
  assert(nb_comp==1);

  Domaine_dis_base& domaine_dis1 = domaine_Cl_dis().domaine_dis().valeur();
  Nom nom_racc1=frontiere_dis().frontiere().le_nom();
  Nom nom_racc2=ch.nom_bord_oppose();

  int nb_faces_raccord1 = domaine_dis1.domaine().raccord(nom_racc1)->nb_faces();
  if (tab.dimension(0)!=nb_faces_raccord1
      ||tab.dimension(1)!=nb_comp)
    {
      Cerr << "Dans Echange_contact_VDF::calculer_h_autre_pb, tab n'est plus dimensionne." << finl;
      Cerr << "Le dimensionnement est fait dans Echange_contact_VDF::initialiser." << finl;
      exit();
    }
  if (domaine_dis1.domaine().raccord(nom_racc1)->que_suis_je() =="Raccord_distant_homogene")
    {
      calculer_h_distant(tab,une_eqn,zvdf_2,front_vf,le_milieu,invhparoi,opt,nom_racc2);
    }
  else // Raccord_local_homogene
    {
      calculer_h_local(tab,une_eqn,zvdf_2,front_vf,le_milieu,invhparoi,opt);
    }
}

int Echange_contact_VDF::initialiser(double temps)
{
  if (!Echange_global_impose::initialiser(temps))
    return 0;

  // XXX : On rempli les valeurs ici et pas dans le readOn car le milieu de pb2 ets pas encore lu !!!
  Champ_front_calc& ch = ref_cast(Champ_front_calc, T_autre_pb().valeur());
  ch.creer(nom_autre_pb_, nom_bord, nom_champ);

  const Equation_base& o_eqn = ch.equation();
  const Front_VF& fvf = ref_cast(Front_VF, frontiere_dis()), o_fvf = ref_cast(Front_VF, ch.front_dis());
  const Domaine_VDF& o_domaine = ref_cast(Domaine_VDF, ch.domaine_dis());
  const IntTab& o_f_e = o_domaine.face_voisins();
  const Milieu_base& le_milieu = ch.milieu();
  int nb_comp = le_milieu.conductivite()->nb_comp();
  Nom nom_racc1 = frontiere_dis().frontiere().le_nom();
  Domaine_dis_base& domaine_dis1 = domaine_Cl_dis().domaine_dis().valeur();
  int nb_faces_raccord1 = domaine_dis1.domaine().raccord(nom_racc1)->nb_faces();

  h_imp_.typer("Champ_front_fonc");
  h_imp_->fixer_nb_comp(nb_comp);
  h_imp_.valeurs().resize(nb_faces_raccord1,nb_comp);

  if (domaine_dis1.domaine().raccord(nom_racc1)->que_suis_je() !="Raccord_distant_homogene")
    verifier_correspondance();

  autre_h.resize(nb_faces_raccord1,nb_comp);

  ch.initialiser(temps,domaine_Cl_dis().equation().inconnue());

  monolithic = sub_type(Schema_Euler_Implicite, o_eqn.schema_temps()) ?
               ref_cast(Schema_Euler_Implicite, o_eqn.schema_temps()).resolution_monolithique(o_eqn.domaine_application()) : 0;
  if (!monolithic) return 1; //pas besoin du reste
  o_domaine.init_virt_e_map();

  /* src(i) = (proc, j) : source de l'item i de mdv_elem */
  IntTab src(0, 2);
  MD_Vector_tools::creer_tableau_distribue(o_domaine.domaine().md_vector_elements(), src);
  for (int i = 0; i < src.dimension_tot(0); i++) src(i, 0) = Process::me(), src(i, 1) = i;
  src.echange_espace_virtuel();

  /* o_proc, o_item -> processeur/item de l'element pour chaque face de la frontiere */
  DoubleTrav o_proc, o_item, proc, l_item;
  o_domaine.creer_tableau_faces(o_proc), o_domaine.creer_tableau_faces(o_item);
  fvf.frontiere().creer_tableau_faces(proc), fvf.frontiere().creer_tableau_faces(l_item);
  for (int i = 0; i < o_fvf.nb_faces(); i++)
    {
      int f = o_fvf.num_face(i), e = o_f_e(f, 0) == -1 ? o_f_e(f, 1) : o_f_e(f, 0);
      o_proc(f) = src(e, 0), o_item(f) = src(e, 1); //element
    }

  //projection sur la frontiere locale
  if (o_fvf.frontiere().que_suis_je() == "Raccord_distant_homogene")
    o_fvf.frontiere().trace_face_distant(o_proc, proc), o_fvf.frontiere().trace_face_distant(o_item, l_item);
  else o_fvf.frontiere().trace_face_local(o_proc, proc), o_fvf.frontiere().trace_face_local(o_item, l_item);

  //remplissage
  item.resize(fvf.nb_faces()), item = -1;
  for (int i = 0; i < fvf.nb_faces(); i++)
    if (l_item(i) >= 0)
      {
        if (proc(i) == Process::me())
          item(i) = (int)std::lrint(l_item(i));                     //item local (reel)
        else
          {
            if (o_domaine.virt_e_map.count({{ (int) proc(i), (int) l_item(i) }}))   //item local (virtuel)
            item(i) = o_domaine.virt_e_map.at({{ (int) proc(i),  (int) l_item(i) }});
            else Process::exit(Nom("Echange_contact_VDF : missing element opposite face ") + Nom(fvf.num_face(i)) + " in " + fvf.le_nom() + " ! Have you used Decouper_multi?");
          }
      }
  return 1;
}

void Echange_contact_VDF::mettre_a_jour(double temps)
{
  Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  const Milieu_base& le_milieu=ch.milieu();
  int nb_comp = le_milieu.conductivite()->nb_comp();
  assert(nb_comp==1);

  T_autre_pb().mettre_a_jour(temps);

  int is_pb_fluide=0;

  DoubleTab& mon_h= h_imp_->valeurs();

  int opt=0;
  assert(h_paroi!=0.);
  double invhparoi=1./h_paroi;
  calculer_h_autre_pb( autre_h, invhparoi, opt);
  calculer_h_mon_pb(mon_h,0.,opt);

  calculer_Teta_paroi(T_wall_,mon_h,autre_h,is_pb_fluide,temps);
  calculer_Teta_equiv(T_ext()->valeurs_au_temps(temps),mon_h,autre_h,is_pb_fluide,temps);
  // on a calculer Teta paroi, on peut calculer htot dans himp (= mon_h)
  int taille=mon_h.dimension(0);
  for (int ii=0; ii<taille; ii++)
    for (int jj=0; jj<nb_comp; jj++)
      {
        mon_h(ii,jj)=1./(1./autre_h(ii,jj)+1./mon_h(ii,jj));
      }
  Echange_global_impose::mettre_a_jour(temps);
}


/*! @brief remplit Teta_eq utilise T_autre_pb au temps passe en parametre
 *
 */
void Echange_contact_VDF::calculer_Teta_equiv(DoubleTab& Teta_eq,const DoubleTab& mon_h,const DoubleTab& lautre_h,int i,double temps)
{
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  int nb_faces_bord = ma_front_vf.nb_faces();
  assert(Teta_eq.dimension(0)==nb_faces_bord);
  assert(Teta_eq.dimension(1)==1);
  DoubleTab& t_autre=T_autre_pb()->valeurs_au_temps(temps);
  for (int numfa=0; numfa<nb_faces_bord; numfa++)
    {
      Teta_eq(numfa,0) = t_autre(numfa,0);
    }
  Teta_eq.echange_espace_virtuel();
}

/*! @brief remplit Teta_p utilise T_autre_pb au temps passe en parametre
 *
 */
void Echange_contact_VDF::calculer_Teta_paroi(DoubleTab& Teta_p,const DoubleTab& mon_h,const DoubleTab& lautre_h,int i, double temps)
{
  const Equation_base& mon_eqn = domaine_Cl_dis().equation();
  const DoubleTab& mon_inco=mon_eqn.inconnue().valeurs();
  const Domaine_VDF& ma_zvdf = ref_cast(Domaine_VDF,domaine_Cl_dis().domaine_dis().valeur());
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  int ndeb = ma_front_vf.num_premiere_face();
  int nb_faces_bord = ma_front_vf.nb_faces();
  int ind_fac,elem;
  Teta_p.resize(nb_faces_bord,1);
  DoubleTab& t_autre=T_autre_pb()->valeurs_au_temps(temps);
  for (int numfa=0; numfa<nb_faces_bord; numfa++)
    {
      ind_fac = numfa+ndeb;
      if (ma_zvdf.face_voisins(ind_fac,0)!= -1)
        elem = ma_zvdf.face_voisins(ind_fac,0);
      else
        elem = ma_zvdf.face_voisins(ind_fac,1);

      Teta_p(numfa,0) = (mon_h(numfa,0)*mon_inco(elem) + lautre_h(numfa,0)*t_autre(numfa,0))/(mon_h(numfa,0)+lautre_h(numfa,0));
    }
}

// En VDF, les faces de deux raccords doivent etre numerotes de la meme facon
int Echange_contact_VDF::verifier_correspondance() const
{
  const Champ_front_calc& ch=ref_cast(Champ_front_calc, T_autre_pb().valeur());
  const Domaine_VDF& ma_zvdf = ref_cast(Domaine_VDF,domaine_Cl_dis().domaine_dis().valeur());
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  const Domaine_VDF& zvdf_2=ref_cast(Domaine_VDF, ch.domaine_dis());
  const Front_VF& front_vf=ref_cast(Front_VF, ch.front_dis());
  int ndeb = ma_front_vf.num_premiere_face();
  int ndeb2 = front_vf.num_premiere_face();
  int nb_faces_bord = ma_front_vf.nb_faces();
  int ind_fac,ind_fac_2;
  const DoubleTab& xv=ma_zvdf.xv();
  const DoubleTab& xv2=zvdf_2.xv();

  DoubleVect x1(dimension);
  DoubleVect x2(dimension);
  for (int numfa=0; numfa<nb_faces_bord; numfa++)
    {
      ind_fac = numfa+ndeb;
      ind_fac_2=numfa+ndeb2;
      for (int k=0; k<dimension; k++)
        {
          x1[k] = xv(ind_fac,k);
          x2[k] = xv2(ind_fac_2,k);
        }
      if (!meme_point2(x1,x2))
        {
          Cerr<<finl;
          Cerr<<"Correspondence problem: the two 'Raccords' named "<<ma_front_vf.le_nom() <<" are not facing."<<finl;
          Cerr<<"Problem on the following faces:" << finl;
          if ( dimension==2 )
            {
              Cerr << "Coordinates of the face 1: (" << x1[0] << " " << x1[1] << ") on the domain " << ma_zvdf.domaine().le_nom() << finl;
              Cerr << "Coordinates of the face 2: (" << x2[0] << " " << x2[1] << ") on the domain " << zvdf_2.domaine().le_nom()  << finl;
            }
          else
            {
              Cerr << "Coordinates of the face 1: (" << x1[0] << " " << x1[1] << x1[2] << ") on the domain " << ma_zvdf.domaine().le_nom() << finl;
              Cerr << "Coordinates of the face 2: (" << x2[0] << " " << x2[1] << x2[2] << ") on the domain " << zvdf_2.domaine().le_nom()  << finl;
            }
          Cerr << "\nIn VDF, numbering of faces on each 'Raccord' must be identical (to a constant)." << finl;
          Cerr << "The faces of each sub frontier of the boundary " << ma_front_vf.le_nom() << " must be created in the same order on each domain, this process is sensitive to the reading sense of the faces." << finl;
          exit();
          return 0;
        }
    }
  return 1;
}

int meme_point2(const DoubleVect& x,const DoubleVect& y)
{
  int dim = x.size();
  assert(dim==y.size());
  int ind =1;
  for (int k=0; k<dim; k++)
    if (std::fabs(x(k)-y(k))>=Objet_U::precision_geom)
      {
        ind = 0;
        return ind;
      }
  return ind;
}
