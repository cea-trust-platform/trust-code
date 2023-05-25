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

#ifndef Matrice_Grossiere_H_TPP
#define Matrice_Grossiere_H_TPP

#include <Matrice_Morse_Sym.h>

template <typename _TYPE_, typename _TYPE_ARRAY_>
void Matrice_Grossiere::build_matrix(const IJK_Field_template<_TYPE_,_TYPE_ARRAY_>&   coeffs_face)
{
  const IJK_Splitting& splitting = coeffs_face.get_splitting();

  int i, j, k;
  const int ni = splitting.get_nb_elem_local(DIRECTION_I);
  const int nj = splitting.get_nb_elem_local(DIRECTION_J);
  const int nk = splitting.get_nb_elem_local(DIRECTION_K);
  {
    renum_.resize(nk+2, nj+2, ni+2, Array_base::NOCOPY_NOINIT);
    renum_ = -1; // init a -1
    // plusieur vecteur renum pour le cas shear periodic ou une case peut renvoyer vers plusieurs
    // + 4 autres vecteur contenant les ponderation associee pour interpolation 4th order
    renum_m2_.resize(nk+2, nj+2, ni+2, Array_base::NOCOPY_NOINIT);
    renum_m1_.resize(nk+2, nj+2, ni+2, Array_base::NOCOPY_NOINIT);
    renum_p1_.resize(nk+2, nj+2, ni+2, Array_base::NOCOPY_NOINIT);
    renum_p2_.resize(nk+2, nj+2, ni+2, Array_base::NOCOPY_NOINIT);
    renum_m2_ = -1; // init a -1
    renum_m1_ = -1; // init a -1
    renum_p1_ = -1; // init a -1
    renum_p2_ = -1; // init a -1
    ponderation_shear_m2_.resize(nk+2, nj+2, ni+2, Array_base::NOCOPY_NOINIT);
    ponderation_shear_m1_.resize(nk+2, nj+2, ni+2, Array_base::NOCOPY_NOINIT);
    ponderation_shear_0_.resize(nk+2, nj+2, ni+2, Array_base::NOCOPY_NOINIT);
    ponderation_shear_p1_.resize(nk+2, nj+2, ni+2, Array_base::NOCOPY_NOINIT);
    ponderation_shear_p2_.resize(nk+2, nj+2, ni+2, Array_base::NOCOPY_NOINIT);
    ponderation_shear_m2_ = -1.; // init a -1
    ponderation_shear_m1_ = -1.; // init a -1
    ponderation_shear_0_ = -1.; // init a -1
    ponderation_shear_p1_ = -1.; // init a -1
    ponderation_shear_p2_ = -1.; // init a -1

    int count = 0;
    for (k = 0; k < nk; k++)
      for (j = 0; j < nj; j++)
        for (i = 0; i < ni; i++)
          renum(i,j,k) = count++;

    // initialisation du tableau d'indice

    ArrOfInt pe_voisins(6);
    pe_voisins.set_smart_resize(1);
    VECT(ArrOfInt) items_to_send(6);
    VECT(ArrOfInt) items_to_recv(6);
    VECT(ArrOfInt) blocs_to_recv(6);
    int npe = 0;

    const int pe_imin = splitting.get_neighbour_processor(0 /* left */, DIRECTION_I);
    const int pe_jmin = splitting.get_neighbour_processor(0 /* left */, DIRECTION_J);
    const int pe_kmin = splitting.get_neighbour_processor(0 /* left */, DIRECTION_K);
    const int pe_imax = splitting.get_neighbour_processor(1 /* right */, DIRECTION_I);
    const int pe_jmax = splitting.get_neighbour_processor(1 /* right */, DIRECTION_J);
    const int pe_kmax = splitting.get_neighbour_processor(1 /* right */, DIRECTION_K);
    int z_index = splitting.get_local_slice_index(2);
    int z_index_min = 0;
    int z_index_max = splitting.get_nprocessor_per_direction(2) - 1;

    // duCluzeau
    // Ici, on doit permuter les indices du vecteur solution de pression pour rendre compte des
    // conditions de periodicites (ou de shear-periodicité)

    int pe = pe_kmin;
    if (pe >= 0) // si voisin du bas existe
      {
        pe_voisins[npe] = pe;
        if (pe != pe_kmax) // On a au moins 3 proc sur z
          {
            // add_virt_bloc --> stock l'emplacement des indices de l'espace fantome de count à count+ni*nj dans blocs_to_recv
            add_virt_bloc(pe, count, 0,0,-1, ni,nj,0, blocs_to_recv[npe], splitting);

            // add_dist_bloc --> stock lespace reel en 0 dans items_to_send[0] pour lespace virtuel en nk du proc k-1
            // pour les conditions de shear-periodicité, besoin d'un changement si c est le premier proc en z pour prendre en compte l'offset
            if( z_index == z_index_min && IJK_Splitting::defilement_==1)
              {
                add_dist_bloc(pe, 0,0,0, ni,nj,1, items_to_send[npe], splitting, -1.);
              }
            else
              {
                add_dist_bloc(pe, 0,0,0, ni,nj,1, items_to_send[npe], splitting);
              }
          }
        else
          {
            // le meme processeur voisin a gauche et a droite  (par periodicite), soit 1 soit 2 proc sur z
            // attention a l'ordre des blocs:

            // si un seul proc sur k --> assure la shear periodicite
            // sinon, stock l'emplacement des indices de l'espace fantome en -1 , puis en nk+1, sans modif
            if(IJK_Splitting::defilement_==1)
              {
                add_virt_bloc(pe, count, 0,0,-1, ni,nj,0, blocs_to_recv[npe], splitting, 1.);
                add_virt_bloc(pe, count, 0,0,nk, ni,nj,nk+1, blocs_to_recv[npe], splitting, -1.);
              }
            else
              {
                add_virt_bloc(pe, count, 0,0,-1, ni,nj,0, blocs_to_recv[npe], splitting);
                add_virt_bloc(pe, count, 0,0,nk, ni,nj,nk+1, blocs_to_recv[npe], splitting);
              }

            // si un seul proc sur k --> add_dist_bloc ne fait rien, si 2 procs voisins des deux cotes :
            // stock l'espace reel en 0 et en nk-1 --> offset suivant la position du proc en z=0 ou z=zmax.
            if( z_index == z_index_max && IJK_Splitting::defilement_==1)
              {
                add_dist_bloc(pe, 0,0,nk-1, ni,nj,nk, items_to_send[npe], splitting, 1.);
              }
            else
              {
                add_dist_bloc(pe, 0,0,nk-1, ni,nj,nk, items_to_send[npe], splitting);
              }
            if( z_index == z_index_min && IJK_Splitting::defilement_==1)
              {
                add_dist_bloc(pe, 0,0,0, ni,nj,1, items_to_send[npe], splitting, -1.);
              }
            else
              {
                add_dist_bloc(pe, 0,0,0, ni,nj,1, items_to_send[npe], splitting);
              }

          }
        npe++;
      }

    pe = pe_jmin;
    if (pe >= 0)
      {
        pe_voisins[npe] = pe;
        if (pe != pe_jmax)
          {
            add_virt_bloc(pe, count, 0,-1,0, ni,0,nk, blocs_to_recv[npe], splitting);
            add_dist_bloc(pe, 0,0,0, ni,1,nk, items_to_send[npe], splitting);
          }
        else
          {
            // un processeur voisin a gauche et a droite  (par periodicite)
            // attention a l'ordre des blocs:
            add_virt_bloc(pe, count, 0,-1,0, ni,0,nk, blocs_to_recv[npe], splitting);
            add_virt_bloc(pe, count, 0,nj,0, ni,nj+1,nk, blocs_to_recv[npe], splitting);
            add_dist_bloc(pe, 0,nj-1,0, ni,nj,nk, items_to_send[npe], splitting);
            add_dist_bloc(pe, 0,0,0, ni,1,nk, items_to_send[npe], splitting);
          }
        npe++;
      }

    pe = pe_imin;
    if (pe >= 0)
      {
        pe_voisins[npe] = pe;
        if (pe != pe_imax)
          {
            add_virt_bloc(pe, count, -1,0,0, 0,nj,nk, blocs_to_recv[npe], splitting);
            add_dist_bloc(pe, 0,0,0, 1,nj,nk, items_to_send[npe], splitting);
          }
        else
          {
            // un processeur voisin a gauche et a droite  (par periodicite)
            // attention a l'ordre des blocs:
            add_virt_bloc(pe, count, -1,0,0, 0,nj,nk, blocs_to_recv[npe], splitting);
            add_virt_bloc(pe, count, ni,0,0, ni+1,nj,nk, blocs_to_recv[npe], splitting);
            add_dist_bloc(pe, ni-1,0,0, ni,nj,nk, items_to_send[npe], splitting);
            add_dist_bloc(pe, 0,0,0, 1,nj,nk, items_to_send[npe], splitting);
          }
        npe++;
      }

    pe = pe_imax;
    if (pe >= 0 && pe != pe_imin)
      {
        pe_voisins[npe] = pe;
        add_virt_bloc(pe, count, ni,0,0, ni+1,nj,nk, blocs_to_recv[npe], splitting);
        add_dist_bloc(pe, ni-1,0,0, ni,nj,nk, items_to_send[npe], splitting);
        npe++;
      }

    pe = pe_jmax;
    if (pe >= 0 && pe != pe_jmin)
      {
        pe_voisins[npe] = pe;
        add_virt_bloc(pe, count, 0,nj,0, ni,nj+1,nk, blocs_to_recv[npe], splitting);
        add_dist_bloc(pe, 0,nj-1,0, ni,nj,nk, items_to_send[npe], splitting);
        npe++;
      }

    pe = pe_kmax;
    if (pe >= 0 && pe != pe_kmin)
      {
        pe_voisins[npe] = pe;
        add_virt_bloc(pe, count, 0,0,nk, ni,nj,nk+1, blocs_to_recv[npe], splitting);
        // autre test pour le proc pe_kmax, bloc stocke pour assurer la shear periodicite.
        if( z_index == z_index_max && IJK_Splitting::defilement_==1)
          {
            add_dist_bloc(pe, 0,0,nk-1, ni,nj,nk, items_to_send[npe], splitting, 1.);
          }
        else
          {
            add_dist_bloc(pe, 0,0,nk-1, ni,nj,nk, items_to_send[npe], splitting);
          }

        npe++;
      }

    MD_Vector_std md_std(count /* nb_items_tot */,
                         ni * nj * nk /* nb_items_reels */,
                         pe_voisins,
                         items_to_send,
                         items_to_recv,
                         blocs_to_recv);

    md_.copy(md_std);
  }

  {
    const int n_reels = md_.valeur().get_nb_items_reels();
    voisins_.dimensionner(n_reels);
    coeffs_.dimensionner(n_reels);
    coeff_diag_.resize(n_reels);
    voisins_virt_.dimensionner(n_reels);
    coeffs_virt_.dimensionner(n_reels);

    for (k = 0; k < nk; k++)
      {
        for (j = 0; j < nj; j++)
          {
            for (i = 0; i < ni; i++)
              {
                ajoute_coeff(i,j,k,i,j,k-1,coeffs_face(i,j,k,2));
                ajoute_coeff(i,j,k,i,j-1,k,coeffs_face(i,j,k,1));
                ajoute_coeff(i,j,k,i-1,j,k,coeffs_face(i,j,k,0));
                ajoute_coeff(i,j,k,i+1,j,k,coeffs_face(i+1,j,k,0));
                ajoute_coeff(i,j,k,i,j+1,k,coeffs_face(i,j+1,k,1));
                ajoute_coeff(i,j,k,i,j,k+1,coeffs_face(i,j,k+1,2));
              }
          }
      }

    for (i = 0; i < n_reels; i++)
      {
        if (coeff_diag_[i] == 0.)
          coeff_diag_[i] = 1.;
      }
    int nnz = 0;
    for (i = 0; i < n_reels; i++)
      nnz += 1 + coeffs_[i].size();
    int nnz_virt = 0;
    for (i = 0; i < n_reels; i++)
      nnz_virt += coeffs_virt_[i].size();

    if (Process::me() == 0)
      coeff_diag_[0] *= 2;

    mat_.dimensionner(1, 2);
    mat_.get_bloc(0,0).typer("Matrice_Morse_Sym");
    mat_.get_bloc(0,1).typer("Matrice_Morse");
    Matrice_Morse_Sym& carre = ref_cast(Matrice_Morse_Sym ,mat_.get_bloc(0,0).valeur());
    Matrice_Morse&      rect  = ref_cast(Matrice_Morse ,    mat_.get_bloc(0,1).valeur());

    carre.dimensionner(n_reels, nnz);
    carre.remplir(voisins_, coeffs_, coeff_diag_);
    rect.dimensionner(n_reels, md_.valeur().get_nb_items_tot() - n_reels, nnz_virt);
    rect.remplir(voisins_virt_, coeffs_virt_);

    voisins_ = IntLists();
    coeffs_ = DoubleLists();
    coeff_diag_.reset();
    voisins_virt_ = IntLists();
    coeffs_virt_ = DoubleLists();
    // pour voir la matrice lisiblement
    //carre.imprimer_formatte(Cout);
  }

}

#endif
