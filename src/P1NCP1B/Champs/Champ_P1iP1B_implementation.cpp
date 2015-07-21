/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Champ_P1iP1B_implementation.cpp
// Directory:   $TRUST_ROOT/src/P1NCP1B/Champs
// Version:     /main/40
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_P1iP1B_implementation.h>
#include <Champ_implementation_P1.h>
#include <Domaine.h>
#include <Zone_VEF_PreP1b.h>
#include <Zone_Cl_VEFP1B.h>
#include <Debog.h>
#include <IntList.h>
#include <Matrice_Morse_Sym.h>
#include <IntLists.h>
#include <DoubleLists.h>
#include <SolveurSys.h>
#include <Solv_GCP.h>
#include <SSOR.h>
#include <Check_espace_virtuel.h>
#include <Neumann_sortie_libre.h>
#include <Champ_P0_VEF.h>
#include <ConstDoubleTab_parts.h>
#include <Frontiere_dis_base.h>

DoubleVect& Champ_P1iP1B_implementation::
valeur_a_elem(const DoubleVect& position,
              DoubleVect& val,
              int le_poly) const
{
  int dimension=Objet_U::dimension;
  DoubleTab vals(1,1);
  IntVect les_polys(1);
  DoubleTab positions(1,dimension);

  les_polys(0) = le_poly;
  for (int i=0; i<dimension; i++)
    positions(0,i) = position(i);

  valeur_aux_elems(positions, les_polys, vals);

  val(0) = vals(0,0);
  return val;
}

double Champ_P1iP1B_implementation::
valeur_a_elem_compo(const DoubleVect& position,
                    int le_poly, int ncomp) const
{
  double val=0;
  int dimension=Objet_U::dimension;

  DoubleVect vals(1);
  IntVect les_polys(1);
  DoubleTab positions(1,dimension);

  les_polys(0) = le_poly;
  for (int i=0; i<dimension; i++)
    positions(0,i) = position(i);

  valeur_aux_elems_compo(positions, les_polys, vals, ncomp);

  val = vals(0);

  return val;
}

// Recupere un tableau positions contenant des coordonnees de points
// contenus dans les elements ranges dans le tableau les_polys
// Renvoie le tableau val contenant les valeurs du champ aux points
DoubleTab& Champ_P1iP1B_implementation::
valeur_aux_elems(const DoubleTab& positions,
                 const IntVect& les_polys,
                 DoubleTab& val) const
{
  int som;
  double xs,ys,zs=0;
  // zs bien initialise en 3D

  const Zone_VEF_PreP1b& zvef = zone_vef();
  const Zone& zone_geom = zvef.zone();
  const DoubleTab& coord = zone_geom.domaine().coord_sommets();
  const IntTab& sommet_poly = zone_geom.les_elems();
  int prs=zvef.numero_premier_sommet();
  int nb_compo_=le_champ().nb_comp();
  int dimension=Objet_U::dimension;

  if (val.nb_dim() == 1)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(nb_compo_ == 1);
    }
  else if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo_);
    }
  else
    {
      Cerr << "Erreur TRUST dans Champ_P1NC::valeur_aux()\n";
      Cerr << "Le DoubleTab val a plus de 2 entrees\n";
      Process::exit();
    }

  // Filtrage du champ dans le tableau champ_filtre_
  champ_filtre_=filtrage(zvef,le_champ());

  if (nb_compo_ == 1)
    {
      int les_polys_size=les_polys.size();
      for(int rang_poly=0; rang_poly<les_polys_size; rang_poly++)
        {
          // On initialise
          if (val.nb_dim() == 1)
            val(rang_poly) = 0.;
          else
            val(rang_poly,0) = 0.;

          // On prend le_poly
          int le_poly=les_polys(rang_poly);
          if (le_poly != -1)
            {
              // Contribution P0
              if (zvef.get_alphaE())
                {
                  if (val.nb_dim() == 1)
                    val(rang_poly) += champ_filtre_(le_poly);
                  else
                    val(rang_poly,0) += champ_filtre_(le_poly);
                }
              // Contribution P1
              if (zvef.get_alphaS())
                {
                  xs = positions(rang_poly,0);
                  ys = positions(rang_poly,1);
                  if (dimension == 3)
                    zs = positions(rang_poly,2);

                  // Calcul par les fonctions de forme P1 du champ filtre
                  // au point (xs,yz,zs)
                  for (int i=0; i<dimension+1; i++)
                    {
                      som = prs+sommet_poly(le_poly,i);
                      double champ_filtre_som=champ_filtre_(som);
                      double li=0.;

                      if (dimension == 2)
                        li=coord_barycentrique_P1_triangle(sommet_poly,
                                                           coord, xs, ys,
                                                           le_poly, i);
                      else if (dimension == 3)
                        li=coord_barycentrique_P1_tetraedre(sommet_poly,
                                                            coord, xs, ys, zs,
                                                            le_poly, i);
                      if (val.nb_dim() == 1)
                        val(rang_poly) += champ_filtre_som * li;
                      else
                        val(rang_poly,0) += champ_filtre_som * li;
                    }
                }
            }
        }
    }
  else // nb_compo_ > 1
    {
      Cerr << "Vous en etes deja la vous ?" << finl;
      Process::exit();
    }
  return val;
}

// Idem que valeur_aux_elems mais pour une composante d'un champ vecteur
DoubleVect& Champ_P1iP1B_implementation::
valeur_aux_elems_compo(const DoubleTab& positions,
                       const IntVect& les_polys,
                       DoubleVect& val,int ncomp) const
{
  Cerr << "Champ_P1iP1B_implementation::valeur_aux_elems_compo non code." << finl;
  // Factoriser le code avec la methode valeur_aux_elems()
  Process::exit();
  return val;
}


// Recupere un domaine
// Renvoie un tableau contenant les valeurs du champ aux sommets du domaine
DoubleTab& Champ_P1iP1B_implementation::
valeur_aux_sommets(const Domaine& dom,
                   DoubleTab& val) const
{
  const Zone_VEF_PreP1b& zvef = zone_vef();
  int nb_compo_=le_champ().nb_comp();

  if (val.nb_dim() == 1)
    {
      assert(nb_compo_ == 1);
    }
  else if (val.nb_dim() == 2)
    {
      assert(val.dimension(1) == nb_compo_);
    }
  else
    {
      Cerr << "Erreur TRUST dans Champ_P1iP1B_implementation:::valeur_aux_sommets()\n";
      Cerr << "Le DoubleTab val a plus de 2 entrees\n";
      Process::exit();
    }

  // Filtrage du champ pour le postraitement (contenu dans le tableau champ_filtre_)
  champ_filtre_=filtrage(zvef,le_champ());

  if (zvef.get_alphaE()) // Support P0
    {
      Champ_P0_VEF tmp;
      tmp.associer_zone_dis_base(zvef);
      tmp.fixer_nb_comp(1);
      tmp.fixer_nb_valeurs_nodales(zvef.nb_elem());
      DoubleVect& valeurs=tmp.valeurs();
      valeurs.ref_data(champ_filtre_.addr(),zvef.nb_elem_tot());
      tmp.valeur_aux_sommets(zvef.zone().domaine(), val);
    }

  if (zvef.get_alphaS()) // Support P1
    {
      int nbs=zvef.nb_som();
      int prs=zvef.numero_premier_sommet();
      if (nb_compo_ == 1)
        {
          int som,num_som;
          for (num_som = 0; num_som<nbs; num_som++)
            {
              som = prs+num_som;
              if (val.nb_dim() == 1)
                val(num_som) += champ_filtre_(som);
              else
                val(num_som,0) += champ_filtre_(som);
            }
        }
      else // nb_compo_ > 1
        {
          Cerr << "Vous en etes deja la vous ?" << endl;
          Process::exit();
        }
    }
  return val;
}


//
DoubleVect& Champ_P1iP1B_implementation::
valeur_aux_sommets_compo(const Domaine& dom,
                         DoubleVect& val,
                         int ncomp) const
{
  Cerr << "Champ_P1iP1B_implementation::valeur_aux_sommets_compo pas code." << finl;
  // Factoriser avec la methode valeur_aux_sommets()
  Process::exit();
  return val;
}

DoubleTab& Champ_P1iP1B_implementation::
remplir_coord_noeuds(DoubleTab& coord) const
{
  const Zone_VEF_PreP1b& zvef=zone_vef();
  const Zone& la_zone=zvef.zone();
  const Domaine& dom=la_zone.domaine();
  const DoubleTab& coord_sommets=dom.coord_sommets();
  int nbe=zvef.nb_elem_tot();
  int nbs=zvef.nb_som_tot();
  int pre=zvef.numero_premier_element();
  int prs=zvef.numero_premier_sommet();
  int dimension=Objet_U::dimension;

  const DoubleTab& xg = zvef.xp();

  int k=pre;
  coord.resize(nbe+nbs,dimension);
  {
    for(int i=0; i<nbe; i++)
      {
        for(int j=0; j<dimension; j++)
          coord(k,j)=xg(i,j);
        k++;
      }
  }
  k=prs;
  {
    for(int i=0; i<nbs; i++)
      {
        for(int j=0; j<dimension; j++)
          coord(k,j)=coord_sommets(i,j);
        k++;
      }
  }
  return coord;
}

int Champ_P1iP1B_implementation::
remplir_coord_noeuds_et_polys(DoubleTab& positions,
                              IntVect& polys) const
{
  Cerr << "Pas code " << finl;
  assert(0);
  Process::exit();
  return 1;
}

void assembler(const Zone_VEF_PreP1b& zone_VEF, Matrice& matrice)
{
  matrice.typer("Matrice_Morse_Sym");
  Matrice_Morse_Sym& MatPoisson=ref_cast(Matrice_Morse_Sym, matrice.valeur());
  int nb_som_tot = zone_VEF.zone().nb_som_tot();
  int nb_arete_tot = zone_VEF.zone().nb_aretes_tot();
  int nnz=nb_som_tot+nb_arete_tot;
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& renum_arete_perio=zone_VEF.get_renum_arete_perio();
  const Domaine& dom=zone_VEF.zone().domaine();

  IntLists voisins(nb_som_tot);
  DoubleLists coeffs(nb_som_tot);
  DoubleVect diag(nb_som_tot);
  // On parcourt toutes les aretes non periodiques:
  for(int arete=0; arete<nb_arete_tot; arete++)
    {
      if (renum_arete_perio(arete)==arete)
        {
          int som1=dom.get_renum_som_perio(aretes_som(arete, 0));
          int som2=dom.get_renum_som_perio(aretes_som(arete, 1));
          if(som1>som2)
            {
              int tmp=som1;
              som1=som2;
              som2=tmp;
            }
          voisins[som1].add(som2);
          coeffs[som1].add(1);
          diag(som1)++;
          diag(som2)++;
        }
    }
  for (int i=0; i<nb_som_tot; i++)
    {
      if (diag(i)==0)
        {
          assert(i!=dom.get_renum_som_perio(i));
          diag(i)=1; // Sommets periodiques
        }
    }
  MatPoisson.dimensionner(nb_som_tot, nnz) ;
  MatPoisson.remplir(voisins, coeffs, diag) ;
  MatPoisson.compacte() ;
  MatPoisson.set_est_definie(1);
  //Cerr << "Matrice P1";MatPoisson.imprimer(Cerr);
}

static double coeff=4./5.;
static double coeff_inv=1/coeff;
double second_membre(const Zone_VEF_PreP1b& zone_VEF, ArrOfDouble& Pa, DoubleVect& secmem)
{
  int nb_arete_tot = zone_VEF.zone().nb_aretes_tot();
  const IntTab& aretes_som=zone_VEF.zone().aretes_som();
  const ArrOfInt& renum_arete_perio=zone_VEF.get_renum_arete_perio();
  const Domaine& dom=zone_VEF.zone().domaine();
  secmem=0;
  // On parcourt toutes les aretes non periodiques
  for(int arete=0; arete<nb_arete_tot; arete++)
    {
      if (renum_arete_perio(arete)==arete)
        {
          double x=Pa(arete);
          int som1=dom.get_renum_som_perio(aretes_som(arete, 0));
          int som2=dom.get_renum_som_perio(aretes_som(arete, 1));
          secmem(som1)+=x;
          secmem(som2)+=x;
        }
    }
  secmem*=coeff;
  secmem.echange_espace_virtuel();
  double norme=mp_norme_vect(secmem);
  //Cerr << "||somme Pa||= " << norme << finl;
  return norme;
}

void corriger(const Zone_VEF_PreP1b& zone_VEF, DoubleTab& champ_filtre_, Matrice& matrice, const int& Condition_Neumann_imposee_)
{
  int nb_elem=zone_VEF.nb_elem();
  int nb_som=zone_VEF.nb_som();

  // Tableaux d'acces aux valeurs Pk et Ps
  if (!zone_VEF.get_alphaE() || !zone_VEF.get_alphaS())
    {
      Cerr << "Erreur dans Champ_P1iP1B_impl.cpp: corriger(...), le champ doit avoir une partie sommets et elements" << finl;
      Process::exit();
    }
  DoubleTab_parts parties_P(champ_filtre_);
  DoubleVect& Pk = parties_P[0];  // partie elements
  DoubleVect& Ps = parties_P[1];  // partie sommets

  // Filtrage si support arete
  if (zone_VEF.get_alphaA())
    {
      DoubleVect& Pa = parties_P[2];  // partie aretes

      // Si premier passage on assemble la matrice
      if (!matrice.non_nul()) assembler(zone_VEF, matrice);

      // Construction du second membre
      DoubleVect secmem;
      zone_VEF.zone().domaine().creer_tableau_sommets(secmem);
      second_membre(zone_VEF, Pa, secmem);

      // Calcul de la correction
      DoubleVect solution;
      solution.copy(secmem, ArrOfDouble::NOCOPY_NOINIT); // Copie de la structure
      solution = 0.;

      SolveurSys solveur;
      solveur.typer("Solv_GCP");
      Precond p;
      p.typer("SSOR");
      ref_cast(Solv_GCP,solveur.valeur()).set_precond(p);
      solveur.nommer("Pa_filter_solver");
      solveur.resoudre_systeme(matrice, secmem, solution);

      // Application de la periodicite sur la solution:
      const Domaine& dom=zone_VEF.zone().domaine();
      for(int i=0; i<nb_som; i++)
        {
          int som=dom.get_renum_som_perio(i);
          if (som!=i)
            solution(i)=solution(som);
        }

      // Correction de Ps:
      for(int som=0; som<nb_som; som++)
        Ps(som) += solution(som);

      // Correction de Pa:
      int nb_arete = zone_VEF.zone().nb_aretes();
      const IntTab& aretes_som=zone_VEF.zone().aretes_som();
      const ArrOfInt& ok_arete=zone_VEF.get_ok_arete();
      for(int arete=0; arete<nb_arete; arete++)
        {
          if(!ok_arete(arete) && Pa(arete)!=0)
            {
              Cerr << "Pa(arete_superflue)!=0 dans Champ_P1iP1B_implementation::corriger" << finl;
              Cerr << "Contacter le support TRUST." << finl;
              Cerr << "S'il s'agit d'une reprise au format xyz d'un calcul" << finl;
              Cerr << "alors il faut ecraser le fichier .ok_arete de votre calcul de reprise" << finl;
              Cerr << "par le fichier .ok_arete de votre calcul precedant afin d'avoir les" << finl;
              Cerr << "memes aretes superflues." << finl;
              Process::exit();
            }
          int som1=aretes_som(arete, 0);
          int som2=aretes_som(arete, 1);
          double sigma=solution(som1)+solution(som2);
          Pa(arete)-=sigma*coeff_inv;
        }

      // Correction de Pk:
      const IntTab& les_elems = zone_VEF.zone().les_elems();
      int nb_som_par_elem=les_elems.dimension_tot(1);
      for(int elem=0; elem<nb_elem; elem++)
        {
          double sigma=0;
          for (int som=0; som<nb_som_par_elem; som++)
            sigma+=solution(les_elems(elem,som));
          Pk(elem)-=sigma/4;
        }
    }

  // Filtrage L2:
  double moyenne_K = mp_moyenne_vect(Pk);                // Calcul de la moyenne du champ aux elements
  Pk -= moyenne_K;                                 // Correction des elements du champ_filtre_
  Ps += moyenne_K;                                 // Correction des sommets du champ_filtre_

  // On retranche a Ps sa moyenne si pas de Cl de Neumann pour avoir une moyenne nulle
  // Il faudrait peut etre mieux faire cela dans Assembleur_P_VEFPreP1B::modifier_solution
  // si cela est possible...
  assert(Condition_Neumann_imposee_!=-1);

  if (!Condition_Neumann_imposee_)
    {
      // L'espace virtuel de Ps n'a pas besoin d'etre a jour pour calculer correctement la moyenne
      Ps -= mp_moyenne_vect(Ps); // Correction des sommets du champ_filtre_
    }

  champ_filtre_.echange_espace_virtuel();         // Mise a jour des espaces virtuels
}

DoubleTab& Champ_P1iP1B_implementation::filtrage(const Zone_VEF_PreP1b& zvef, const Champ_base& un_champ) const
{
  // Filtrage si supports element et sommet presents au moins
  if (zvef.get_alphaE() && zvef.get_alphaS())
    {
      // Pas de filtrage si deja fait sur ce champ:
      if (un_champ.valeurs().addr()==adresse_champ_filtre_ && un_champ.temps()==temps_filtrage_)
        return champ_filtre_;

      // On copie le champ a filtrer dans le tableau qui contiendra le champ filtre
      champ_filtre_=un_champ.valeurs();
      temps_filtrage_=un_champ.temps();
      adresse_champ_filtre_=un_champ.valeurs().addr();
      //Cout << "Filtrage du champ " << un_champ.le_nom() << " au temps " << un_champ.temps() << finl;

      // Correction du champ
      corriger(zvef, champ_filtre_, matrice_filtrage_, Condition_Neumann_imposee_);
      Debog::verifier("champ apres filtre=", champ_filtre_);
      return champ_filtre_;
    }
  else
    {
      champ_filtre_=un_champ.valeurs();
      return champ_filtre_;
    }
}

// Fixe Condition_Neumann_imposee_ a 1 si il existe une CL de Neumann 0 sinon
// Si pas de condition de Neumann, on imposera une moyenne nulle dans le filtrage du champ
// pour avoir le meme champ en sequentiel et en parallele
void Champ_P1iP1B_implementation::completer(const Zone_Cl_dis_base& zcl)
{
  const Conds_lim& les_cl = zcl.les_conditions_limites();
  Condition_Neumann_imposee_ = 0;
  for(int i=0; i<les_cl.size(); i++)
    if (sub_type(Neumann_sortie_libre,les_cl[i].valeur()))
      Condition_Neumann_imposee_ = 1;
}

DoubleTab& Champ_P1iP1B_implementation::trace(const Frontiere_dis_base& fr, const DoubleTab& y, DoubleTab& x) const
{
  const Front_VF& fr_vf = ref_cast(Front_VF, fr);
  const Zone_VEF_PreP1b& zone_dis = zone_vef();
  const DoubleTab& cg_faces = zone_dis.xv();
  int nb_faces_fr = fr_vf.nb_faces();
  int face, elem;

  assert(x.dimension(0)==fr_vf.nb_faces());

  IntVect elem_voisins(nb_faces_fr);
  elem_voisins = -1;
  DoubleTab cg_faces_fr(nb_faces_fr,Objet_U::dimension);
  DoubleTab val_interp(nb_faces_fr);

  //On recupere les elements voisins des faces de la frontiere
  for (int i=0; i<nb_faces_fr; i++)
    {
      face = fr_vf.num_premiere_face()+i;
      elem = zone_dis.face_voisins(face,0);
      if (elem==-1)
        elem = zone_dis.face_voisins(face,1);
      elem_voisins(i) = elem;
    }

  //Remplissage du tableau contenant les coordonnees
  //du centre de gravite des faces de la frontiere
  for (int num_face=0; num_face<nb_faces_fr; num_face++)
    for (int dim=0; dim<Objet_U::dimension; dim++)
      {
        face = fr_vf.num_premiere_face()+num_face;
        cg_faces_fr(num_face,dim) = cg_faces(face,dim);
      }

  //Interpolation des valeurs du champ au centre de gravite des faces frontieres
  valeur_aux_elems(cg_faces_fr,elem_voisins,val_interp);

  for (int i=0; i<nb_faces_fr; i++)
    {
      x(i,0) = val_interp(i);
    }
  // Useless ? x.echange_espace_virtuel();
  return x;
}
