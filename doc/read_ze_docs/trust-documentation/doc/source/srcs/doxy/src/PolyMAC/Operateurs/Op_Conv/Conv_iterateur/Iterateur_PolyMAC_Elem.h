/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Iterateur_PolyMAC_Elem_included
#define Iterateur_PolyMAC_Elem_included

#include <Equation_base.h>
#include <Milieu_base.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Operateur_base.h>
#include <Operateur_Diff_base.h>
#include <Op_Conv_PolyMAC_iterateur_base.h>
#include <EcrFicPartage.h>
#include <communications.h>
#include <TRUSTTrav.h>

template <class _TYPE_>
class Iterateur_PolyMAC_Elem : public Iterateur_PolyMAC_base
{
  inline int duplique() const override
  {
    Iterateur_PolyMAC_Elem* xxx = new  Iterateur_PolyMAC_Elem(*this);
    if(!xxx) Process::exit("Not enough memory ");
    return xxx->numero();
  }

  inline unsigned taille_memoire() const override { throw; }

public:
  inline Iterateur_PolyMAC_Elem() { } ;
  inline Iterateur_PolyMAC_Elem(const Iterateur_PolyMAC_Elem<_TYPE_>& );
  inline Evaluateur_PolyMAC& evaluateur() override;
  inline const Evaluateur_PolyMAC& evaluateur() const override;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const override;
  DoubleTab& ajouter(const DoubleTab&, DoubleTab& ) const override;
  void calculer_flux_bord(const DoubleTab&) const override;
  void contribuer_au_second_membre(DoubleTab& ) const override;
  void ajouter_contribution(const DoubleTab&, Matrice_Morse& ) const override;
  void ajouter_contribution_vitesse(const DoubleTab&, Matrice_Morse& ) const override;
  inline void completer_() override;
  int impr(Sortie& os) const override;
  void modifier_flux() const;

protected:
  _TYPE_ flux_evaluateur;
  DoubleTab& ajouter_bords(const DoubleTab& , DoubleTab& ) const;
  DoubleTab& ajouter_bords(const DoubleTab& , DoubleTab& , int ) const;
  DoubleTab& ajouter_interne(const DoubleTab& , DoubleTab& ) const;
  DoubleTab& ajouter_interne(const DoubleTab& , DoubleTab& , int ) const;
  void contribuer_au_second_membre_bords(DoubleTab& ) const;
  void contribuer_au_second_membre_bords(DoubleTab& , int ) const;
  void contribuer_au_second_membre_interne(DoubleTab& ) const;
  void contribuer_au_second_membre_interne(DoubleTab&, int ) const;
  void ajouter_contribution_bords(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_bords(const DoubleTab&, Matrice_Morse&, int ) const;
  void ajouter_contribution_interne(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_interne(const DoubleTab&, Matrice_Morse&, int ) const;
  void ajouter_contribution_interne_vitesse(const DoubleTab&, Matrice_Morse& ) const;
  void ajouter_contribution_bords_vitesse(const DoubleTab&, Matrice_Morse& ) const;
  const Milieu_base& milieu() const;
  IntTab elem;
  mutable SFichier Flux; // Impression .out
};

template <class _TYPE_> inline Iterateur_PolyMAC_Elem<_TYPE_>::Iterateur_PolyMAC_Elem(const Iterateur_PolyMAC_Elem<_TYPE_>& iter) :Iterateur_PolyMAC_base(iter), flux_evaluateur(iter.flux_evaluateur)
{
  elem.ref(iter.elem);
}

template <class _TYPE_> inline Evaluateur_PolyMAC& Iterateur_PolyMAC_Elem<_TYPE_>::evaluateur()
{
  Evaluateur_PolyMAC& eval = (Evaluateur_PolyMAC&) flux_evaluateur;
  return eval;
}
template <class _TYPE_> inline const Evaluateur_PolyMAC& Iterateur_PolyMAC_Elem<_TYPE_>::evaluateur() const
{
  Evaluateur_PolyMAC& eval = (Evaluateur_PolyMAC&) flux_evaluateur;
  return eval;
}

template <class _TYPE_> inline void Iterateur_PolyMAC_Elem<_TYPE_>::completer_()
{
  elem.ref(la_domaine->face_voisins());
}

template <class _TYPE_>
const Milieu_base& Iterateur_PolyMAC_Elem<_TYPE_>::milieu() const
{
  return (la_zcl->equation()).milieu();
}

template <class _TYPE_>
DoubleTab& Iterateur_PolyMAC_Elem<_TYPE_>::ajouter(const DoubleTab& donne,
                                                   DoubleTab& resu) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(donne.nb_dim() < 3);
  assert(la_zcl.non_nul());
  assert(la_domaine.non_nul());
  int ncomp=1;
  if (donne.nb_dim() == 2)
    ncomp=donne.dimension(1);
  DoubleTab& flux_bords=op_base->flux_bords();
  flux_bords.resize(la_domaine->nb_faces_bord(),ncomp);
  flux_bords=0;
  /* modif b.m.: on va faire += sur des items virtuels, initialiser les cases */
  /* sinon risque que les cases soient invalides ou non initialisees */
  // {
  //   int n = resu.size_array() - resu.size();
  //   double *data = resu.addr() + resu.size();
  //   for (; n; n--, data++)
  //     *data = 0.;
  // }
  if( ncomp == 1) /* cas scalaire */
    {
      ajouter_bords(donne, resu) ;
      ajouter_interne(donne, resu) ;
    }
  else /* cas vectoriel */
    {
      ajouter_bords(donne, resu, ncomp) ;
      ajouter_interne(donne, resu, ncomp) ;
    }
  modifier_flux() ;
  return resu;
}
template <class _TYPE_>  DoubleTab& Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_bords(const DoubleTab& donnee,
                                                                                  DoubleTab& resu) const
{
  int elem1, elem2;
  int ndeb, nfin;
  int face;
  int num_cl=0;
  double flux;
  int nb_front_Cl=la_domaine->nb_front_Cl();
  DoubleTab& flux_bords=op_base->flux_bords();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      /* pour chaque Condition Limite on regarde son type */
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      /* Test en bidim axi */
      if (bidim_axi && !sub_type(Symetrie,la_cl.valeur()))
        {
          if (nfin>ndeb && est_egal(la_domaine.valeur().face_surfaces()[ndeb],0))
            {
              Cerr << "Error in the definition of the boundary conditions." << finl;
              Cerr << "The axis of revolution for this 2D calculation is along Y." << finl;
              Cerr << "So you must specify symmetry for the boundary " << frontiere_dis.le_nom() << finl;
              exit();
            }
        }
      switch(type_cl(la_cl))
        {
        case symetrie :
          if (flux_evaluateur.calculer_flux_faces_symetrie())
            {
              const Symetrie& cl =(const Symetrie&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case sortie_libre :
          if (flux_evaluateur.calculer_flux_faces_sortie_libre())
            {
              const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case entree_fluide :
          if (flux_evaluateur.calculer_flux_faces_entree_fluide())
            {
              const Dirichlet_entree_fluide& cl =(const Dirichlet_entree_fluide&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case paroi_fixe :
          if (flux_evaluateur.calculer_flux_faces_paroi_fixe())
            {
              const Dirichlet_paroi_fixe& cl =(const Dirichlet_paroi_fixe&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case paroi_defilante :
          if (flux_evaluateur.calculer_flux_faces_paroi_defilante())
            {
              const Dirichlet_paroi_defilante& cl =(const Dirichlet_paroi_defilante&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case paroi_adiabatique :
          if (flux_evaluateur.calculer_flux_faces_paroi_adiabatique())
            {
              const Neumann_paroi_adiabatique& cl =(const Neumann_paroi_adiabatique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  /* on initialise elem1 elem2 et on fait planter */
                  elem1=-1;
                  elem2=-1;
                  assert(0);
                  exit();
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  resu[elem1]+=flux;
                  flux_bords(face,0)+=flux;
                  resu[elem2]-=flux;
                  flux_bords(face,0)-=flux;
                }
            }
          break;
        case paroi :
          if (flux_evaluateur.calculer_flux_faces_paroi())
            {
              const Neumann_paroi& cl =(const Neumann_paroi&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case echange_externe_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_externe_impose())
            {
              const Echange_externe_impose& cl =(const Echange_externe_impose&) (la_cl.valeur());

              int boundary_index=-1;
              if (la_domaine.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom())
                boundary_index=num_cl;

              for (face=ndeb; face<nfin; face++)
                {
                  int local_face=la_domaine.valeur().front_VF(boundary_index).num_local_face(face);
                  flux = flux_evaluateur.flux_face(donnee, boundary_index,face,local_face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case echange_global_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_global_impose())
            {
              const Echange_global_impose& cl =(const Echange_global_impose&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case periodique :
          if (flux_evaluateur.calculer_flux_faces_periodique())
            {
              const Periodique& cl =(const Periodique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=0.5*flux;
                      if ( face < (ndeb+frontiere_dis.nb_faces()/2) )
                        flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=0.5*flux;
                      if ( (ndeb+frontiere_dis.nb_faces()/2) <= face )
                        flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case scalaire_impose_paroi :
          break;
          /*
                                                                            case nouvelle_Cl_PolyMAC :
                                                                            if (flux_evaluateur.calculer_flux_faces_echange_global_impose()){
                                                                            const Nouvelle_Cl_PolyMAC& cl =(const Nouvelle_Cl_PolyMAC&) (la_cl.valeur());
                                                                            for (face=ndeb; face<nfin; face++) {
                                                                            if ( (elem1=elem(face,0)) > -1)
                                                                            resu[elem1]+=flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                                                                            if ( (elem2=elem(face,1)) > -1)
                                                                            resu[elem2]-=flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                                                                            }
                                                                            }
                                                                            break;
          */
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Cerr << "Dans Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_bords"<<finl;
          exit();
          break;
        }
    }
  return resu;
}
template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::calculer_flux_bord(const DoubleTab& donnee) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(donnee.nb_dim() < 3);
  assert(la_zcl.non_nul());
  assert(la_domaine.non_nul());
  int ncomp=1;
  if (donnee.nb_dim() == 2)
    ncomp=donnee.dimension(1);
  DoubleTab& flux_bords=op_base->flux_bords();
  flux_bords.resize(la_domaine->nb_faces_bord(),ncomp);
  flux_bords=0;

  if( ncomp != 1) Process ::exit();/* cas scalaire */


  int ndeb, nfin;
  int face;
  int num_cl=0;
  double flux;
  int nb_front_Cl=la_domaine->nb_front_Cl();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      /* pour chaque Condition Limite on regarde son type */
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      /* Test en bidim axi */
      if (bidim_axi && !sub_type(Symetrie,la_cl.valeur()))
        {
          if (nfin>ndeb && est_egal(la_domaine.valeur().face_surfaces()[ndeb],0))
            {
              Cerr << "Error in the definition of the boundary conditions." << finl;
              Cerr << "The axis of revolution for this 2D calculation is along Y." << finl;
              Cerr << "So you must specify symmetry for the boundary " << frontiere_dis.le_nom() << finl;
              exit();
            }
        }
      switch(type_cl(la_cl))
        {
        case symetrie :
          if (flux_evaluateur.calculer_flux_faces_symetrie())
            {
              const Symetrie& cl =(const Symetrie&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem(face,0)) > -1)
                    {
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem(face,1)) > -1)
                    {
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case sortie_libre :
          if (flux_evaluateur.calculer_flux_faces_sortie_libre())
            {
              const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem(face,0)) > -1)
                    {
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem(face,1)) > -1)
                    {
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case entree_fluide :
          if (flux_evaluateur.calculer_flux_faces_entree_fluide())
            {
              const Dirichlet_entree_fluide& cl =(const Dirichlet_entree_fluide&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem(face,0)) > -1)
                    {
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem(face,1)) > -1)
                    {
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case paroi_fixe :
          if (flux_evaluateur.calculer_flux_faces_paroi_fixe())
            {
              const Dirichlet_paroi_fixe& cl =(const Dirichlet_paroi_fixe&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem(face,0)) > -1)
                    {
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem(face,1)) > -1)
                    {
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case paroi_defilante :
          if (flux_evaluateur.calculer_flux_faces_paroi_defilante())
            {
              const Dirichlet_paroi_defilante& cl =(const Dirichlet_paroi_defilante&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem(face,0)) > -1)
                    {
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem(face,1)) > -1)
                    {
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case paroi_adiabatique :
          if (flux_evaluateur.calculer_flux_faces_paroi_adiabatique())
            {
              const Neumann_paroi_adiabatique& cl =(const Neumann_paroi_adiabatique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  assert(0);
                  exit();
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  flux_bords(face,0)+=flux;
                  flux_bords(face,0)-=flux;
                }
            }
          break;
        case paroi :
          if (flux_evaluateur.calculer_flux_faces_paroi())
            {
              const Neumann_paroi& cl =(const Neumann_paroi&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem(face,0)) > -1)
                    {
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem(face,1)) > -1)
                    {
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case echange_externe_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_externe_impose())
            {
              const Echange_externe_impose& cl =(const Echange_externe_impose&) (la_cl.valeur());

              int boundary_index=-1;
              if (la_domaine.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom())
                boundary_index=num_cl;

              for (face=ndeb; face<nfin; face++)
                {
                  int local_face=la_domaine.valeur().front_VF(boundary_index).num_local_face(face);
                  flux = flux_evaluateur.flux_face(donnee, boundary_index,face,local_face, cl, ndeb);
                  if ( (elem(face,0)) > -1)
                    {
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem(face,1)) > -1)
                    {
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case echange_global_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_global_impose())
            {
              const Echange_global_impose& cl =(const Echange_global_impose&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem(face,0)) > -1)
                    {
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem(face,1)) > -1)
                    {
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case periodique :
          if (flux_evaluateur.calculer_flux_faces_periodique())
            {
              const Periodique& cl =(const Periodique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                  if ( (elem(face,0)) > -1)
                    {
                      if ( face < (ndeb+frontiere_dis.nb_faces()/2) )
                        flux_bords(face,0)+=flux;
                    }
                  if ( (elem(face,1)) > -1)
                    {
                      if ( (ndeb+frontiere_dis.nb_faces()/2) <= face )
                        flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case scalaire_impose_paroi :
          break;
          /*
                                                                            case nouvelle_Cl_PolyMAC :
                                                                            if (flux_evaluateur.calculer_flux_faces_echange_global_impose()){
                                                                            const Nouvelle_Cl_PolyMAC& cl =(const Nouvelle_Cl_PolyMAC&) (la_cl.valeur());
                                                                            for (face=ndeb; face<nfin; face++) {
                                                                            if ( (int elem1=elem(face,0)) > -1)
                                                                            resu[elem1]+=flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                                                                            if ( (int elem2=elem(face,1)) > -1)
                                                                            resu[elem2]-=flux_evaluateur.flux_face(donnee, face, cl, ndeb);
                                                                            }
                                                                            }
                                                                            break;
          */
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          Cerr << "Dans Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_bords"<<finl;
          exit();
          break;
        }
    }
  modifier_flux() ;
}

template <class _TYPE_>  DoubleTab& Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_bords(const DoubleTab& donnee,
                                                                                  DoubleTab& resu,int ncomp) const
{
  int elem1, elem2;
  int ndeb, nfin;
  int face,k;
  DoubleVect flux(ncomp);
  int num_cl=0;
  int nb_front_Cl=la_domaine->nb_front_Cl();
  DoubleTab& flux_bords=op_base->flux_bords();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      /* Test en bidim axi */
      if (bidim_axi && !sub_type(Symetrie,la_cl.valeur()))
        {
          if (nfin>ndeb && est_egal(la_domaine.valeur().face_surfaces()[ndeb],0))
            {
              Cerr << "Error in the definition of the boundary conditions." << finl;
              Cerr << "The axis of revolution for this 2D calculation is along Y." << finl;
              Cerr << "So you must specify symmetry for the boundary " << frontiere_dis.le_nom() << finl;
              exit();
            }
        }
      switch(type_cl(la_cl))
        {
        case symetrie :
          if (flux_evaluateur.calculer_flux_faces_symetrie())
            {
              const Symetrie& cl =(const Symetrie&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case sortie_libre :
          if (flux_evaluateur.calculer_flux_faces_sortie_libre())
            {
              const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case entree_fluide :
          if (flux_evaluateur.calculer_flux_faces_entree_fluide())
            {
              const Dirichlet_entree_fluide& cl =(const Dirichlet_entree_fluide&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case paroi_fixe :
          if (flux_evaluateur.calculer_flux_faces_paroi_fixe())
            {
              const Dirichlet_paroi_fixe& cl =(const Dirichlet_paroi_fixe&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case paroi_defilante :
          if (flux_evaluateur.calculer_flux_faces_paroi_defilante())
            {
              const Dirichlet_paroi_defilante& cl =(const Dirichlet_paroi_defilante&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case paroi_adiabatique :
          if (flux_evaluateur.calculer_flux_faces_paroi_adiabatique())
            {
              const Neumann_paroi_adiabatique& cl =(const Neumann_paroi_adiabatique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case paroi :
          if (flux_evaluateur.calculer_flux_faces_paroi())
            {
              const Neumann_paroi& cl =(const Neumann_paroi&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
                  /* on initialise elem1 elem2 et on fait planter */
                  elem1=-1;
                  elem2=-1;
                  assert(0);
                  exit();
                  for (k=0; k<ncomp; k++)
                    {
                      resu(elem1,k) +=flux(k);
                      flux_bords(face,k)+=flux(k);
                    }
                  for (k=0; k<ncomp; k++)
                    {
                      resu(elem2,k) -=flux(k);
                      flux_bords(face,k)-=flux(k);
                    }
                }
            }
          break;
        case echange_externe_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_externe_impose())
            {
              const Echange_externe_impose& cl =(const Echange_externe_impose&) (la_cl.valeur());

              int boundary_index=-1;
              if (la_domaine.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom())
                boundary_index=num_cl;

              for (face=ndeb; face<nfin; face++)
                {
                  int local_face=la_domaine.valeur().front_VF(boundary_index).num_local_face(face);
                  flux_evaluateur.flux_face(donnee, boundary_index, face, local_face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case echange_global_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_global_impose())
            {
              const Echange_global_impose& cl =(const Echange_global_impose&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case periodique :
          if (flux_evaluateur.calculer_flux_faces_periodique())
            {
              const Periodique& cl =(const Periodique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=0.5*flux(k);
                        if ( face < (ndeb+frontiere_dis.nb_faces()/2) )
                          flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=0.5*flux(k);
                        if ( (ndeb+frontiere_dis.nb_faces()/2) <= face )
                          flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
          /*
                                                                            case nouvelle_Cl_PolyMAC :
                                                                            if (flux_evaluateur.calculer_flux_faces_echange_global_impose()){
                                                                            const Nouvelle_Cl_PolyMAC& cl =(const Nouvelle_Cl_PolyMAC&) (la_cl.valeur());
                                                                            for (face=ndeb; face<nfin; face++) {
                                                                            flux_evaluateur.flux_face(donnee, face, cl, ndeb, flux);
                                                                            if ( (elem1=elem(face,0)) > -1)
                                                                            for (k=0; k<ncomp; k++)
                                                                            resu(elem1,k) +=flux(k);
                                                                            if ( (elem2=elem(face,1)) > -1)
                                                                            for (k=0; k<ncomp; k++)
                                                                            resu(elem2,k) -=flux(k);
                                                                            }
                                                                            }
                                                                            break;
          */
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          exit();
          break;
        }
    }
  return resu;
}

template <class _TYPE_>  DoubleTab& Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_interne(const DoubleTab& donnee,
                                                                                    DoubleTab& resu) const
{
  const Domaine_PolyMAC& domaine_PolyMAC = la_domaine.valeur();
  double flux;
  int face;
  int ndeb = domaine_PolyMAC.premiere_face_int();
  int nfin = domaine_PolyMAC.nb_faces();
  for (face=ndeb; face<nfin; face++)
    {
      flux=flux_evaluateur.flux_faces_interne(donnee, face);
      resu[elem(face,0)]+=flux;
      resu[elem(face,1)]-=flux;
    }
  return resu;
}
template <class _TYPE_>  DoubleTab& Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_interne(const DoubleTab& donnee,
                                                                                    DoubleTab& resu,int ncomp) const
{
  const Domaine_PolyMAC& domaine_PolyMAC = la_domaine.valeur();
  DoubleVect flux(ncomp);
  int face,k;
  int elem0,elem1;
  int ndeb = domaine_PolyMAC.premiere_face_int();
  int nfin = domaine_PolyMAC.nb_faces();
  for (face=ndeb; face<nfin; face++)
    {
      flux_evaluateur.flux_faces_interne(donnee, face, flux);
      elem0 = elem(face,0);
      elem1 = elem(face,1);
      for (k=0; k<ncomp; k++)
        {
          resu(elem0,k)+=flux(k);
          resu(elem1,k)-=flux(k);
        }
    }
  return resu;
}
template <class _TYPE_>  DoubleTab& Iterateur_PolyMAC_Elem<_TYPE_>::calculer(const DoubleTab& inco, DoubleTab& resu) const
{
  operator_egal(resu, 0., VECT_REAL_ITEMS);
  return ajouter(inco,resu);
}
template <class _TYPE_>  void  Iterateur_PolyMAC_Elem<_TYPE_>::modifier_flux() const
{
  if (op_base->equation().inconnue().le_nom()=="temperature"
      && !( sub_type(Operateur_Diff_base,op_base.valeur()) && ref_cast(Operateur_Diff_base,op_base.valeur()).diffusivite().le_nom() == "conductivite" ) )
    {
      DoubleTab& flux_bords=op_base->flux_bords();
      const Domaine_PolyMAC& la_domaine_vdf=ref_cast(Domaine_PolyMAC,op_base->equation().domaine_dis().valeur());
      const Champ_base& rho = (op_base->equation()).milieu().masse_volumique().valeur();
      const Champ_Don& Cp = (op_base->equation()).milieu().capacite_calorifique();
      const IntTab& face_voisins=la_domaine_vdf.face_voisins();
      int rho_uniforme=(sub_type(Champ_Uniforme,rho) ? 1:0);
      int cp_uniforme=(sub_type(Champ_Uniforme,Cp.valeur()) ? 1:0);
      int is_rho_u=op_base->equation().probleme().is_dilatable();
      if (is_rho_u)
        {
          const Operateur_base& op=op_base.valeur();
          is_rho_u=0;
          if (sub_type(Op_Conv_PolyMAC_iterateur_base,op))
            if (ref_cast(Op_Conv_PolyMAC_iterateur_base,op).vitesse().le_nom()=="rho_u")
              is_rho_u=1;
        }
      double Cp_=0,rho_=0;
      const int nb_faces_bords=la_domaine_vdf.nb_faces_bord();
      for (int face=0; face<nb_faces_bords; face++)
        {
          int num_elem=face_voisins(face,0);
          if (num_elem == -1) num_elem = face_voisins(face,1);
          if (cp_uniforme) Cp_=Cp(0,0);
          else if (Cp.nb_comp()==1) Cp_=Cp(num_elem);
          else Cp_=Cp(num_elem,0);
          if (rho_uniforme) rho_=rho(0,0);
          else if (rho.nb_comp()==1) rho_=rho(num_elem);
          else rho_=rho(num_elem,0);
          /* si on est en QC temperature on a calcule div(rhou * T) */
          /* il ne faut pas remultiplier par rho */
          if (is_rho_u) rho_=1;
          flux_bords(face,0) *= (rho_*Cp_);
        }
    }
}

template <class _TYPE_>  int Iterateur_PolyMAC_Elem<_TYPE_>::impr(Sortie& os) const
{
  const Domaine& madomaine=la_domaine->domaine();
  const Domaine_PolyMAC& zpoly=ref_cast(Domaine_PolyMAC,op_base->equation().domaine_dis().valeur());
  const int impr_bord=(madomaine.bords_a_imprimer().est_vide() ? 0:1);
  const Schema_Temps_base& sch = la_zcl->equation().probleme().schema_temps();
  double temps = sch.temps_courant();
  const DoubleTab& inco = la_zcl->equation().inconnue().valeurs();
  DoubleTab& flux_bords=op_base->flux_bords();
  int ncomp=1;
  if (inco.nb_dim() == 2)
    ncomp=inco.dimension(1);
  flux_bords.resize(zpoly.premiere_face_int(),ncomp);
  DoubleVect bilan(flux_bords.dimension(1));
  int k,face;
  int nb_front_Cl=la_domaine->nb_front_Cl();
  DoubleTrav flux_bords2( 3, nb_front_Cl , flux_bords.dimension(1)) ;
  flux_bords2=0;
  /*flux_bord(k)          ->   flux_bords2(0,num_cl,k) */
  /*flux_bord_perio1(k)   ->   flux_bords2(1,num_cl,k) */
  /*flux_bord_perio2(k)   ->   flux_bords2(2,num_cl,k) */
  for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      int periodicite = (type_cl(la_cl)==periodique?1:0);
      for (face=ndeb; face<nfin; face++)
        for(k=0; k<flux_bords.dimension(1); k++)
          {
            flux_bords2(0,num_cl,k)+=flux_bords(face, k);
            if(periodicite)
              {
                if( face < (ndeb+frontiere_dis.nb_faces()/2) )
                  flux_bords2(1,num_cl,k)+=flux_bords(face, k);
                else
                  flux_bords2(2,num_cl,k)+=flux_bords(face, k);
              }
          }
    } /* fin for num_cl */
  mp_sum_for_each_item(flux_bords2);
  if (je_suis_maitre())
    {
      //SFichier Flux;
      if (!Flux.is_open()) op_base->ouvrir_fichier(Flux,"",1);
      Flux.add_col(temps);
      for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
        {
          const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
          int periodicite = (type_cl(la_cl)==periodique?1:0);
          for(k=0; k<flux_bords.dimension(1); k++)
            {
              bilan(k)+=flux_bords2(0,num_cl,k);
              if(periodicite)
                {
                  Flux.add_col(flux_bords2(1,num_cl,k));
                  Flux.add_col(flux_bords2(2,num_cl,k));
                }
              else
                Flux.add_col(flux_bords2(0,num_cl,k));
            }
        }
      for(k=0; k<flux_bords.dimension(1); k++)
        Flux.add_col(bilan(k));
      Flux << finl;
    }
  const LIST(Nom)& Liste_Bords_a_imprimer = la_domaine->domaine().bords_a_imprimer();
  if (!Liste_Bords_a_imprimer.est_vide())
    {
      EcrFicPartage Flux_face;
      op_base->ouvrir_fichier_partage(Flux_face,"",impr_bord);
      for (int num_cl=0; num_cl<nb_front_Cl; num_cl++)
        {
          const Frontiere_dis_base& la_fr = la_zcl->les_conditions_limites(num_cl).frontiere_dis();
          const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
          const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = frontiere_dis.num_premiere_face();
          int nfin = ndeb + frontiere_dis.nb_faces();
          if (madomaine.bords_a_imprimer().contient(la_fr.le_nom()))
            {
              Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps " << temps << " : " << finl;
              for (face=ndeb; face<nfin; face++)
                {
                  if (dimension == 2)
                    Flux_face << "# Face a x= " << la_domaine->xv(face,0) << " y= " << la_domaine->xv(face,1) << " : ";
                  else if (dimension == 3)
                    Flux_face << "# Face a x= " << la_domaine->xv(face,0) << " y= " << la_domaine->xv(face,1) << " z= " << la_domaine->xv(face,2) << " : ";
                  for(k=0; k<flux_bords.dimension(1); k++)
                    Flux_face << flux_bords(face, k) << " ";
                  Flux_face << finl;
                }
              Flux_face.syncfile();
            }
        }
    }
  return 1;
}

template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::contribuer_au_second_membre(DoubleTab& resu) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(resu.nb_dim() < 3);
  assert(la_zcl.non_nul());
  assert(la_domaine.non_nul());
  int ncomp=1;
  if (resu.nb_dim() == 2)
    ncomp=resu.dimension(1);
  assert(op_base->flux_bords().dimension(0)==la_domaine->nb_faces_bord()); /* resize deja fait */
  if( ncomp == 1) /* cas scalaire */
    {
      contribuer_au_second_membre_bords(resu) ;
      contribuer_au_second_membre_interne(resu) ;
    }
  else /* cas vectoriel */
    {
      contribuer_au_second_membre_bords(resu, ncomp) ;
      contribuer_au_second_membre_interne(resu, ncomp) ;
    }
}
template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::contribuer_au_second_membre_bords(DoubleTab& resu) const
{
  int elem1, elem2;
  int ndeb, nfin;
  int face;
  int num_cl=0;
  double flux;
  int nb_front_Cl=la_domaine->nb_front_Cl();
  DoubleTab& flux_bords=op_base->flux_bords();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      /* pour chaque Condition Limite on regarde son type */
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          if (flux_evaluateur.calculer_flux_faces_symetrie())
            {
              const Symetrie& cl =(const Symetrie&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.secmem_face(face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case sortie_libre :
          if (flux_evaluateur.calculer_flux_faces_sortie_libre())
            {
              const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.secmem_face(face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case entree_fluide :
          if (flux_evaluateur.calculer_flux_faces_entree_fluide())
            {
              const Dirichlet_entree_fluide& cl =(const Dirichlet_entree_fluide&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.secmem_face(face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case paroi_fixe :
          if (flux_evaluateur.calculer_flux_faces_paroi_fixe())
            {
              const Dirichlet_paroi_fixe& cl =(const Dirichlet_paroi_fixe&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.secmem_face(face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case paroi_defilante :
          if (flux_evaluateur.calculer_flux_faces_paroi_defilante())
            {
              const Dirichlet_paroi_defilante& cl =(const Dirichlet_paroi_defilante&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.secmem_face(face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case paroi_adiabatique :
          if (flux_evaluateur.calculer_flux_faces_paroi_adiabatique())
            {
              const Neumann_paroi_adiabatique& cl =(const Neumann_paroi_adiabatique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.secmem_face(face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case paroi :
          if (flux_evaluateur.calculer_flux_faces_paroi())
            {
              const Neumann_paroi& cl =(const Neumann_paroi&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.secmem_face(face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case echange_externe_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_externe_impose())
            {
              const Echange_externe_impose& cl =(const Echange_externe_impose&) (la_cl.valeur());

              int boundary_index=-1;
              if (la_domaine.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom())
                boundary_index=num_cl;

              for (face=ndeb; face<nfin; face++)
                {
                  int local_face=la_domaine.valeur().front_VF(boundary_index).num_local_face(face);
                  flux = flux_evaluateur.secmem_face(boundary_index,face,local_face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case echange_global_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_global_impose())
            {
              const Echange_global_impose& cl =(const Echange_global_impose&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux = flux_evaluateur.secmem_face(face, cl, ndeb);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      resu[elem1]+=flux;
                      flux_bords(face,0)+=flux;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      resu[elem2]-=flux;
                      flux_bords(face,0)-=flux;
                    }
                }
            }
          break;
        case periodique :
          if (flux_evaluateur.calculer_flux_faces_periodique())
            {
              const Periodique& cl =(const Periodique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  if ( (elem1=elem(face,0)) > -1)
                    resu[elem1]+=0.5*flux_evaluateur.secmem_face(face, cl, ndeb);
                  if ( (elem2=elem(face,1)) > -1)
                    resu[elem2]-=0.5*flux_evaluateur.secmem_face(face, cl, ndeb);
                }
            }
          break;
        case scalaire_impose_paroi :
          break;
          /*
                                                                            case nouvelle_Cl_PolyMAC :
                                                                            if (flux_evaluateur.calculer_flux_faces_echange_global_impose()){
                                                                            const Nouvelle_Cl_PolyMAC& cl =(const Nouvelle_Cl_PolyMAC&) (la_cl.valeur());
                                                                            for (face=ndeb; face<nfin; face++) {
                                                                            if ( (elem1=elem(face,0)) > -1)
                                                                            resu[elem1]+=flux_evaluateur.secmem_face(face, cl, ndeb);
                                                                            if ( (elem2=elem(face,1)) > -1)
                                                                            resu[elem2]-=flux_evaluateur.secmem_face(face, cl, ndeb);
                                                                            }
                                                                            }
                                                                            break;
          */
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          exit();
          break;
        }
    }
}

template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::contribuer_au_second_membre_bords(DoubleTab& resu,int ncomp) const
{
  int elem1, elem2;
  int ndeb, nfin;
  int face,k;
  DoubleVect flux(ncomp);
  int num_cl=0;
  int nb_front_Cl=la_domaine->nb_front_Cl();
  DoubleTab& flux_bords=op_base->flux_bords();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          if (flux_evaluateur.calculer_flux_faces_symetrie())
            {
              const Symetrie& cl =(const Symetrie&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.secmem_face(face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case sortie_libre :
          if (flux_evaluateur.calculer_flux_faces_sortie_libre())
            {
              const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.secmem_face(face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case entree_fluide :
          if (flux_evaluateur.calculer_flux_faces_entree_fluide())
            {
              const Dirichlet_entree_fluide& cl =(const Dirichlet_entree_fluide&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.secmem_face(face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case paroi_fixe :
          if (flux_evaluateur.calculer_flux_faces_paroi_fixe())
            {
              const Dirichlet_paroi_fixe& cl =(const Dirichlet_paroi_fixe&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.secmem_face(face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case paroi_defilante :
          if (flux_evaluateur.calculer_flux_faces_paroi_defilante())
            {
              const Dirichlet_paroi_defilante& cl =(const Dirichlet_paroi_defilante&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.secmem_face(face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case paroi_adiabatique :
          if (flux_evaluateur.calculer_flux_faces_paroi_adiabatique())
            {
              const Neumann_paroi_adiabatique& cl =(const Neumann_paroi_adiabatique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.secmem_face(face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case paroi :
          if (flux_evaluateur.calculer_flux_faces_paroi())
            {
              const Neumann_paroi& cl =(const Neumann_paroi&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.secmem_face(face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case echange_externe_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_externe_impose())
            {
              const Echange_externe_impose& cl =(const Echange_externe_impose&) (la_cl.valeur());

              int boundary_index=-1;
              if (la_domaine.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom())
                boundary_index=num_cl;

              for (face=ndeb; face<nfin; face++)
                {
                  int local_face=la_domaine.valeur().front_VF(boundary_index).num_local_face(face);
                  flux_evaluateur.secmem_face(boundary_index,face,local_face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case echange_global_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_global_impose())
            {
              const Echange_global_impose& cl =(const Echange_global_impose&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.secmem_face(face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=flux(k);
                        flux_bords(face,k)+=flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=flux(k);
                        flux_bords(face,k)-=flux(k);
                      }
                }
            }
          break;
        case periodique :
          if (flux_evaluateur.calculer_flux_faces_periodique())
            {
              const Periodique& cl =(const Periodique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.secmem_face(face, cl, ndeb, flux);
                  if ( (elem1=elem(face,0)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem1,k) +=0.5*flux(k);
                        flux_bords(face,k)+=0.5*flux(k);
                      }
                  if ( (elem2=elem(face,1)) > -1)
                    for (k=0; k<ncomp; k++)
                      {
                        resu(elem2,k) -=0.5*flux(k);
                        flux_bords(face,k)-=0.5*flux(k);
                      }
                }
            }
          break;
          /*
                                                                            case nouvelle_Cl_PolyMAC :
                                                                            if (flux_evaluateur.calculer_flux_faces_echange_global_impose()){
                                                                            const Nouvelle_Cl_PolyMAC& cl =(const Nouvelle_Cl_PolyMAC&) (la_cl.valeur());
                                                                            for (face=ndeb; face<nfin; face++) {
                                                                            flux_evaluateur.secmem_face(face, cl, ndeb, flux);
                                                                            if ( (elem1=elem(face,0)) > -1)
                                                                            for (k=0; k<ncomp; k++)
                                                                            resu(elem1,k) +=flux(k);
                                                                            if ( (elem2=elem(face,1)) > -1)
                                                                            for (k=0; k<ncomp; k++)
                                                                            resu(elem2,k) -=flux(k);
                                                                            }
                                                                            }
                                                                            break;
          */
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          exit();
          break;
        }
    }
}

template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::contribuer_au_second_membre_interne(DoubleTab& resu) const
{
  const Domaine_PolyMAC& domaine_PolyMAC = la_domaine.valeur();
  double flux;
  int face;
  int ndeb=domaine_PolyMAC.premiere_face_int();
  int nfin=domaine_PolyMAC.nb_faces();
  for (face=ndeb; face<nfin; face++)
    {
      flux=flux_evaluateur.secmem_faces_interne(face);
      resu[elem(face,0)]+=flux;
      resu[elem(face,1)]-=flux;
    }
}
template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::contribuer_au_second_membre_interne( DoubleTab& resu,int ncomp) const
{
  const Domaine_PolyMAC& domaine_PolyMAC = la_domaine.valeur();
  DoubleVect flux(ncomp);
  int face,k;
  int elem0,elem1;
  int ndeb=domaine_PolyMAC.premiere_face_int();
  int nfin=domaine_PolyMAC.nb_faces();
  for (face=ndeb; face<nfin; face++)
    {
      flux_evaluateur.secmem_faces_interne(face, flux);
      elem0 = elem(face,0);
      elem1 = elem(face,1);
      for (k=0; k<ncomp; k++)
        {
          resu(elem0,k)+=flux(k);
          resu(elem1,k)-=flux(k);
        }
    }
}

template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_contribution(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(inco.nb_dim() < 3);
  assert(la_zcl.non_nul());
  assert(la_domaine.non_nul());
  int ncomp=1;
  if (inco.nb_dim() == 2)
    ncomp=inco.dimension(1);
  DoubleTab& flux_bords=op_base->flux_bords();
  flux_bords.resize(la_domaine->nb_faces_bord(),ncomp);
  flux_bords=0;
  if( ncomp == 1) /* cas scalaire */
    {
      ajouter_contribution_bords(inco, matrice) ;
      ajouter_contribution_interne(inco, matrice) ;
    }
  else /* cas vectoriel */
    {
      ajouter_contribution_bords(inco, matrice, ncomp) ;
      ajouter_contribution_interne(inco, matrice, ncomp) ;
    }
}
template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_contribution_bords(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  int elem1, elem2;
  double aii=0, ajj=0;
  int ndeb, nfin;
  int face;
  int num_cl=0;
  int nb_front_Cl=la_domaine->nb_front_Cl();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      /* pour chaque Condition Limite on regarde son type */
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          if (flux_evaluateur.calculer_flux_faces_symetrie())
            {
              const Symetrie& cl =(const Symetrie&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      matrice(elem1,elem1)+=aii;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      matrice(elem2,elem2)+=ajj;
                    }
                }
            }
          break;
        case sortie_libre :
          if (flux_evaluateur.calculer_flux_faces_sortie_libre())
            {
              const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      matrice(elem1,elem1)+=aii;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      matrice(elem2,elem2)+=ajj;
                    }
                }
            }
          break;
        case entree_fluide :
          if (flux_evaluateur.calculer_flux_faces_entree_fluide())
            {
              const Dirichlet_entree_fluide& cl =(const Dirichlet_entree_fluide&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      matrice(elem1,elem1)+=aii;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      matrice(elem2,elem2)+=ajj;
                    }
                }
            }
          break;
        case paroi_fixe :
          if (flux_evaluateur.calculer_flux_faces_paroi_fixe())
            {
              const Dirichlet_paroi_fixe& cl =(const Dirichlet_paroi_fixe&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      matrice(elem1,elem1)+=aii;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      matrice(elem2,elem2)+=ajj;
                    }
                }
            }
          break;
        case paroi_defilante :
          if (flux_evaluateur.calculer_flux_faces_paroi_defilante())
            {
              const Dirichlet_paroi_defilante& cl =(const Dirichlet_paroi_defilante&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      matrice(elem1,elem1)+=aii;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      matrice(elem2,elem2)+=ajj;
                    }
                }
            }
          break;
        case paroi_adiabatique :
          if (flux_evaluateur.calculer_flux_faces_paroi_adiabatique())
            {
              const Neumann_paroi_adiabatique& cl =(const Neumann_paroi_adiabatique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      matrice(elem1,elem1)+=aii;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      matrice(elem2,elem2)+=ajj;
                    }
                }
            }
          break;
        case paroi :
          if (flux_evaluateur.calculer_flux_faces_paroi())
            {
              const Neumann_paroi& cl =(const Neumann_paroi&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      matrice(elem1,elem1)+=aii;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      matrice(elem2,elem2)+=ajj;
                    }
                }
            }
          break;
        case echange_externe_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_externe_impose())
            {
              const Echange_externe_impose& cl =(const Echange_externe_impose&) (la_cl.valeur());

              int boundary_index=-1;
              if (la_domaine.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom())
                boundary_index=num_cl;

              for (face=ndeb; face<nfin; face++)
                {
                  int local_face=la_domaine.valeur().front_VF(boundary_index).num_local_face(face);
                  flux_evaluateur.coeffs_face(boundary_index,face,local_face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      matrice(elem1,elem1)+=aii;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      matrice(elem2,elem2)+=ajj;
                    }
                }
            }
          break;
        case echange_global_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_global_impose())
            {
              const Echange_global_impose& cl =(const Echange_global_impose&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      matrice(elem1,elem1)+=aii;
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      matrice(elem2,elem2)+=ajj;
                    }
                }
            }
          break;
        case periodique :
          if (flux_evaluateur.calculer_flux_faces_periodique())
            {
              const Periodique& cl =(const Periodique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  /* GF Qui a mis l'appel au coeffs faces_internes ??? . Cest faux ...*/
                  /* flux_evaluateur.coeffs_faces_interne(face, aii, ajj); */
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  matrice(elem1,elem1)+=0.5*aii;
                  matrice(elem1,elem2)-=0.5*ajj;
                  matrice(elem2,elem2)+=0.5*ajj;
                  matrice(elem2,elem1)-=0.5*aii;
                }
            }
          break;
        case scalaire_impose_paroi :
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          exit();
          break;
        }
    }
}

template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_contribution_bords(const DoubleTab& inco, Matrice_Morse& matrice ,int ncomp) const
{
  int elem1, elem2;
  DoubleVect aii(ncomp), ajj(ncomp);
  int ndeb, nfin;
  int face,i;
  int num_cl=0;
  int nb_front_Cl=la_domaine->nb_front_Cl();
  for (; num_cl<nb_front_Cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          if (flux_evaluateur.calculer_flux_faces_symetrie())
            {
              const Symetrie& cl =(const Symetrie&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem1*ncomp+i,elem1*ncomp+i)+=aii(i);
                        }
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem2*ncomp+i,elem2*ncomp+i)+=ajj(i);
                        }
                    }
                }
            }
          break;
        case sortie_libre :
          if (flux_evaluateur.calculer_flux_faces_sortie_libre())
            {
              const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem1*ncomp+i,elem1*ncomp+i)+=aii(i);
                        }
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem2*ncomp+i,elem2*ncomp+i)+=ajj(i);
                        }
                    }
                }
            }
          break;
        case entree_fluide :
          if (flux_evaluateur.calculer_flux_faces_entree_fluide())
            {
              const Dirichlet_entree_fluide& cl =(const Dirichlet_entree_fluide&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem1*ncomp+i,elem1*ncomp+i)+=aii(i);
                        }
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem2*ncomp+i,elem2*ncomp+i)+=ajj(i);
                        }
                    }
                }
            }
          break;
        case paroi_fixe :
          if (flux_evaluateur.calculer_flux_faces_paroi_fixe())
            {
              const Dirichlet_paroi_fixe& cl =(const Dirichlet_paroi_fixe&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem1*ncomp+i,elem1*ncomp+i)+=aii(i);
                        }
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem2*ncomp+i,elem2*ncomp+i)+=ajj(i);
                        }
                    }
                }
            }
          break;
        case paroi_defilante :
          if (flux_evaluateur.calculer_flux_faces_paroi_defilante())
            {
              const Dirichlet_paroi_defilante& cl =(const Dirichlet_paroi_defilante&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem1*ncomp+i,elem1*ncomp+i)+=aii(i);
                        }
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem2*ncomp+i,elem2*ncomp+i)+=ajj(i);
                        }
                    }
                }
            }
          break;
        case paroi_adiabatique :
          if (flux_evaluateur.calculer_flux_faces_paroi_adiabatique())
            {
              const Neumann_paroi_adiabatique& cl =(const Neumann_paroi_adiabatique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem1*ncomp+i,elem1*ncomp+i)+=aii(i);
                        }
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem2*ncomp+i,elem2*ncomp+i)+=ajj(i);
                        }
                    }
                }
            }
          break;
        case paroi :
          if (flux_evaluateur.calculer_flux_faces_paroi())
            {
              const Neumann_paroi& cl =(const Neumann_paroi&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem1*ncomp+i,elem1*ncomp+i)+=aii(i);
                        }
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem2*ncomp+i,elem2*ncomp+i)+=ajj(i);
                        }
                    }
                }
            }
          break;
        case echange_externe_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_externe_impose())
            {
              const Echange_externe_impose& cl =(const Echange_externe_impose&) (la_cl.valeur());

              int boundary_index=-1;
              if (la_domaine.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom())
                boundary_index=num_cl;

              for (face=ndeb; face<nfin; face++)
                {
                  int local_face=la_domaine.valeur().front_VF(boundary_index).num_local_face(face);
                  flux_evaluateur.coeffs_face(boundary_index,face,local_face,ndeb, cl, aii, ajj);
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem1*ncomp+i,elem1*ncomp+i)+=aii(i);
                        }
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem2*ncomp+i,elem2*ncomp+i)+=aii(i);
                        }
                    }
                }
            }
          break;
        case echange_global_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_global_impose())
            {
              const Echange_global_impose& cl =(const Echange_global_impose&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  if ( (elem1=elem(face,0)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem1*ncomp+i,elem1*ncomp+i)+=aii(i);
                        }
                    }
                  if ( (elem2=elem(face,1)) > -1)
                    {
                      for (i=0; i<ncomp; i++)
                        {
                          matrice(elem2*ncomp+i,elem2*ncomp+i)+=ajj(i);
                        }
                    }
                }
            }
          break;
        case periodique :
          if (flux_evaluateur.calculer_flux_faces_periodique())
            {
              const Periodique& cl =(const Periodique&) (la_cl.valeur());
              for (face=ndeb; face<nfin; face++)
                {
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  elem1 = elem(face,0);
                  elem2 = elem(face,1);
                  flux_evaluateur.coeffs_face(face,ndeb, cl, aii, ajj);
                  {
                    for (i=0; i<ncomp; i++)
                      {
                        int n1=elem1*ncomp+i;
                        int n2=elem2*ncomp+i;
                        matrice(n1,n1)+=0.5*aii(i);
                        matrice(n1,n2)-=0.5*ajj(i);
                        matrice(n2,n2)+=0.5*ajj(i);
                        matrice(n2,n1)-=0.5*aii(i);
                      }
                  }
                }
            }
          break;
        case scalaire_impose_paroi :
          // on n'a rien a convecter
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          exit();
          break;
        }
    }
}

template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_contribution_interne(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  const Domaine_PolyMAC& domaine_PolyMAC = la_domaine.valeur();
  int face;
  double aii=0, ajj=0;
  int elem1,elem2;
  int ndeb=domaine_PolyMAC.premiere_face_int();
  int nfin=domaine_PolyMAC.nb_faces();
  for (face=ndeb; face<nfin; face++)
    {
      elem1 = elem(face,0);
      elem2 = elem(face,1);
      flux_evaluateur.coeffs_faces_interne(face, aii, ajj);
      matrice(elem1,elem1)+=aii;
      matrice(elem1,elem2)-=ajj;
      matrice(elem2,elem2)+=ajj;
      matrice(elem2,elem1)-=aii;
    }
}
template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_contribution_interne(const DoubleTab& inco, Matrice_Morse& matrice ,int ncomp) const
{
  const Domaine_PolyMAC& domaine_PolyMAC = la_domaine.valeur();
  int face,i;
  DoubleVect aii(ncomp), ajj(ncomp);
  int elem1,elem2;
  int ndeb=domaine_PolyMAC.premiere_face_int();
  int nfin=domaine_PolyMAC.nb_faces();
  for (face=ndeb; face<nfin; face++)
    {
      elem1 = elem(face,0);
      elem2 = elem(face,1);
      flux_evaluateur.coeffs_faces_interne(face, aii, ajj);
      for (i=0; i<ncomp; i++)
        {
          int i0=elem1*ncomp+i,j0=elem2*ncomp+i;
          matrice(i0,i0)+=aii(i) ;
          matrice(i0,j0)-=ajj(i) ;
          matrice(j0,j0)+=ajj(i) ;
          matrice(j0,i0)-=aii(i) ;
        }
    }
}

template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_contribution_vitesse(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  ((_TYPE_&) flux_evaluateur).mettre_a_jour();
  assert(inco.nb_dim() < 3);
  assert(la_zcl.non_nul());
  assert(la_domaine.non_nul());
  int ncomp=1;
  if (inco.nb_dim() == 2)
    ncomp=inco.dimension(1);
  // DoubleTab& flux_bords=op_base->flux_bords();
  // flux_bords.resize(la_domaine->nb_faces_bord(),ncomp);
  // flux_bords=0;
  if( ncomp == 1) /* cas scalaire */
    {
      ajouter_contribution_bords_vitesse(inco, matrice);
      ajouter_contribution_interne_vitesse(inco, matrice);
    }
  else /* cas vectoriel */
    {
      abort();
      // ajouter_contribution_bords(inco, matrice, ncomp) ;
      // ajouter_contribution_interne(inco, matrice, ncomp) ;
    }
}

template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_contribution_interne_vitesse(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  const Domaine_PolyMAC& domaine_PolyMAC = la_domaine.valeur();
  double aef = 0;
  const int ndeb = domaine_PolyMAC.premiere_face_int();
  const int nfin = domaine_PolyMAC.nb_faces();
  for (int f = ndeb; f < nfin; f++)
    {
      const int e1 = elem(f, 0);
      const int e2 = elem(f, 1);
      aef = flux_evaluateur.coeffs_faces_interne_bloc_vitesse(inco, f);
      matrice(e1, f) += aef;
      matrice(e2, f) -= aef;
    }
}

template <class _TYPE_>  void Iterateur_PolyMAC_Elem<_TYPE_>::ajouter_contribution_bords_vitesse(const DoubleTab& inco, Matrice_Morse& matrice ) const
{
  int e1, e2;
  double aef=0;
  int ndeb, nfin;
  int nb_front_Cl=la_domaine->nb_front_Cl();
  for (int num_cl = 0; num_cl<nb_front_Cl; num_cl++)
    {
      /* pour chaque Condition Limite on regarde son type */
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      ndeb = frontiere_dis.num_premiere_face();
      nfin = ndeb + frontiere_dis.nb_faces();
      switch(type_cl(la_cl))
        {
        case symetrie :
          if (flux_evaluateur.calculer_flux_faces_symetrie())
            {
              const Symetrie& cl =(const Symetrie&) (la_cl.valeur());
              for (int f = ndeb; f < nfin; f++)
                {
                  aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
                  if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
                  if ( (e2 = elem(f, 1)) > -1) matrice(e2, f) -= aef;
                }
            }
          break;
        case sortie_libre :
          if (flux_evaluateur.calculer_flux_faces_sortie_libre())
            {
              const Neumann_sortie_libre& cl =(const Neumann_sortie_libre&) (la_cl.valeur());
              for (int f = ndeb; f < nfin; f++)
                {
                  aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
                  if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
                  if ( (e2 = elem(f, 1)) > -1) matrice(e2, f) -= aef;
                }
            }
          break;
        case entree_fluide :
          if (flux_evaluateur.calculer_flux_faces_entree_fluide())
            {
              const Dirichlet_entree_fluide& cl =(const Dirichlet_entree_fluide&) (la_cl.valeur());
              for (int f = ndeb; f < nfin; f++)
                {
                  aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
                  if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
                  if ( (e2 = elem(f, 1)) > -1) matrice(e2, f) -= aef;
                }
            }
          break;
        case paroi_fixe :
          if (flux_evaluateur.calculer_flux_faces_paroi_fixe())
            {
              const Dirichlet_paroi_fixe& cl =(const Dirichlet_paroi_fixe&) (la_cl.valeur());
              for (int f = ndeb; f < nfin; f++)
                {
                  aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
                  if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
                  if ( (e2 = elem(f, 1)) > -1) matrice(e2, f) -= aef;
                }
            }
          break;
        case paroi_defilante :
          if (flux_evaluateur.calculer_flux_faces_paroi_defilante())
            {
              const Dirichlet_paroi_defilante& cl =(const Dirichlet_paroi_defilante&) (la_cl.valeur());
              for (int f = ndeb; f < nfin; f++)
                {
                  aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
                  if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
                  if ( (e2 = elem(f, 1)) > -1) matrice(e2, f) -= aef;
                }
            }
          break;
        case paroi_adiabatique :
          if (flux_evaluateur.calculer_flux_faces_paroi_adiabatique())
            {
              const Neumann_paroi_adiabatique& cl =(const Neumann_paroi_adiabatique&) (la_cl.valeur());
              for (int f = ndeb; f < nfin; f++)
                {
                  aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
                  if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
                  if ( (e2 = elem(f, 1)) > -1) matrice(e2, f) -= aef;
                }
            }
          break;
        case paroi :
          if (flux_evaluateur.calculer_flux_faces_paroi())
            {
              const Neumann_paroi& cl =(const Neumann_paroi&) (la_cl.valeur());
              for (int f = ndeb; f < nfin; f++)
                {
                  aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
                  if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
                  if ( (e2 = elem(f, 1)) > -1) matrice(e2, f) -= aef;
                }
            }
          break;
        case echange_externe_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_externe_impose())
            {
              const Echange_externe_impose& cl =(const Echange_externe_impose&) (la_cl.valeur());
              int boundary_index=-1;
              if (la_domaine.valeur().front_VF(num_cl).le_nom() == frontiere_dis.le_nom())
                boundary_index=num_cl;

              for (int f = ndeb; f < nfin; f++)
                {
                  int local_face=la_domaine.valeur().front_VF(boundary_index).num_local_face(f);
                  aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, boundary_index, f, local_face, cl, ndeb);
                  if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
                  if ( (e2 = elem(f, 1)) > -1) matrice(e1, f) -= aef;
                }
            }
          break;
        case echange_global_impose :
          if (flux_evaluateur.calculer_flux_faces_echange_global_impose())
            {
              const Echange_global_impose& cl =(const Echange_global_impose&) (la_cl.valeur());
              for (int f = ndeb; f < nfin; f++)
                {
                  aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
                  if ( (e1 = elem(f, 0)) > -1) matrice(e1, f) += aef;
                  if ( (e2 = elem(f, 1)) > -1) matrice(e2, f) -= aef;
                }
            }
          break;
        case periodique :
          if (flux_evaluateur.calculer_flux_faces_periodique())
            {
              const Periodique& cl =(const Periodique&) (la_cl.valeur());
              for (int f = ndeb; f < nfin; f++)
                {
                  aef = flux_evaluateur.coeffs_face_bloc_vitesse(inco, f, cl, ndeb);
                  if ( (e1 = elem(f, 0)) > -1)
                    {
                      if ( f < (ndeb+frontiere_dis.nb_faces()/2) )
                        matrice(e1, f) += aef;
                    }
                  if ( (e2 = elem(f, 1)) > -1)
                    {
                      if ( (ndeb+frontiere_dis.nb_faces()/2) <= f )
                        matrice(e2, f) -= aef;
                    }
                }
            }
          break;
        default :
          Cerr << "On ne reconnait pas la condition limite : " << la_cl.valeur();
          exit();
          break;
        }

    }
}

#endif /* Iterateur_PolyMAC_Elem_included */
