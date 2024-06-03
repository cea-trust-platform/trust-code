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

#ifndef Eval_Amont_PolyMAC_Elem_included
#define Eval_Amont_PolyMAC_Elem_included

#include <Eval_Conv_PolyMAC.h>
#include <Eval_PolyMAC_Elem.h>

/*! @brief class Eval_Amont_PolyMAC_Elem
 *
 *  Evaluateur PolyMAC pour la convection
 *  Le champ convecte est scalaire (Champ_Elem_PolyMAC)
 *  Schema de convection Amont
 *  Rq:Les evaluateurs de flux convectifs calculent en fait le terme
 *  convectif qui figure au second membre de l'equation d'evolution
 *  c.a.d l'oppose du flux convectif pour la methode EXPLICITE.
 *
 *  Dans le cas de la methode IMPLICITE les evaluateurs calculent la quantite qui figure
 *  dans le premier membre de l'equation, nous ne prenons pas par consequent l'oppose en
 *  ce qui concerne les termes pour la matrice, par contre pour le second membre nous
 *  procedons comme en explicite mais en ne fesant intervenir que les valeurs fournies
 *  par les conditions limites.
 *
 *
 */
class Eval_Amont_PolyMAC_Elem: public Eval_Conv_PolyMAC, public Eval_PolyMAC_Elem
{

public:
  Eval_Amont_PolyMAC_Elem() { }

  inline int calculer_flux_faces_echange_externe_impose() const override { return 0; }
  inline int calculer_flux_faces_echange_global_impose() const override { return 0; }
  inline int calculer_flux_faces_entree_fluide() const override { return 1; }
  inline int calculer_flux_faces_paroi() const override { return 0; }
  inline int calculer_flux_faces_paroi_adiabatique() const override { return 0; }
  inline int calculer_flux_faces_paroi_defilante() const override { return 0; }
  inline int calculer_flux_faces_paroi_fixe() const override { return 0; }
  inline int calculer_flux_faces_sortie_libre() const override { return 1; }
  inline int calculer_flux_faces_symetrie() const override { return 0; }
  inline int calculer_flux_faces_periodique() const override { return 1; }

  // Fonctions qui servent a calculer le flux de grandeurs scalaires
  // Elles sont de type double et renvoient le flux

  inline double flux_face(const DoubleTab&, int, const Dirichlet_entree_fluide&, int) const override;
  inline double flux_face(const DoubleTab&, int, const Dirichlet_paroi_defilante&, int) const override { return 0; }
  inline double flux_face(const DoubleTab&, int, const Dirichlet_paroi_fixe&, int) const override { return 0; }
  inline double flux_face(const DoubleTab&, int, int, int, const Echange_externe_impose&, int) const override { return 0; }
  inline double flux_face(const DoubleTab&, int, const Echange_global_impose&, int) const override { return 0; }
  inline double flux_face(const DoubleTab&, int, const Neumann_paroi&, int) const override { return 0; }
  inline double flux_face(const DoubleTab&, int, const Neumann_paroi_adiabatique&, int) const override { return 0; }
  inline double flux_face(const DoubleTab&, int, const Neumann_sortie_libre&, int) const override;
  inline double flux_face(const DoubleTab&, int, const Symetrie&, int) const override { return 0; }
  inline double flux_face(const DoubleTab&, int, const Periodique&, int) const override;
  inline double flux_faces_interne(const DoubleTab&, int) const override;

  // Fonctions qui servent a calculer le flux de grandeurs vectorielles
  // Elles sont de type void et remplissent le tableau flux

  inline void flux_face(const DoubleTab&, int, const Symetrie&, int, DoubleVect& flux) const override { }
  inline void flux_face(const DoubleTab&, int, const Periodique&, int, DoubleVect& flux) const override;
  inline void flux_face(const DoubleTab&, int, const Neumann_sortie_libre&, int, DoubleVect& flux) const override;
  inline void flux_face(const DoubleTab&, int, const Dirichlet_entree_fluide&, int, DoubleVect& flux) const override;
  inline void flux_face(const DoubleTab&, int, const Dirichlet_paroi_fixe&, int, DoubleVect& flux) const override { }
  inline void flux_face(const DoubleTab&, int, const Dirichlet_paroi_defilante&, int, DoubleVect& flux) const override { }
  inline void flux_face(const DoubleTab&, int, const Neumann_paroi_adiabatique&, int, DoubleVect& flux) const override { }
  inline void flux_face(const DoubleTab&, int, const Neumann_paroi&, int, DoubleVect& flux) const override { }
  inline void flux_face(const DoubleTab&, int, int, int, const Echange_externe_impose&, int, DoubleVect& flux) const override { }
  inline void flux_face(const DoubleTab&, int, const Echange_global_impose&, int, DoubleVect& flux) const override { }

  inline void flux_faces_interne(const DoubleTab&, int, DoubleVect& flux) const override;

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // scalaires dans le cas implicite.

  inline void coeffs_face(int, int, const Symetrie&, double& aii, double& ajj) const override { }
  inline void coeffs_face(int, int, const Neumann_sortie_libre&, double& aii, double& ajj) const override;
  inline void coeffs_face(int, int, const Dirichlet_entree_fluide&, double& aii, double& ajj) const override;
  inline void coeffs_face(int, int, const Dirichlet_paroi_fixe&, double& aii, double& ajj) const override { }
  inline void coeffs_face(int, int, const Dirichlet_paroi_defilante&, double& aii, double& ajj) const override { }
  inline void coeffs_face(int, int, const Neumann_paroi_adiabatique&, double& aii, double& ajj) const override { }
  inline void coeffs_face(int, int, const Neumann_paroi&, double& aii, double& ajj) const override { }
  inline void coeffs_face(int, int, int, int, const Echange_externe_impose&, double& aii, double& ajj) const override { }
  inline void coeffs_face(int, int, const Echange_global_impose&, double& aii, double& ajj) const override { }
  inline void coeffs_face(int, int, const Periodique&, double& aii, double& ajj) const override;
  inline void coeffs_faces_interne(int, double& aii, double& ajj) const override;

  // contribution de la derivee en vitesse d'une equation scalaire
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int, const Dirichlet_entree_fluide&, int) const override;
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int, const Dirichlet_paroi_defilante&, int) const override { return 0; }
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int, const Dirichlet_paroi_fixe&, int) const override { return 0; }
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int, int, int, const Echange_externe_impose&, int) const override { return 0; }
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int, const Echange_global_impose&, int) const override { return 0; }
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int, const Neumann_paroi&, int) const override { return 0; }
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int, const Neumann_paroi_adiabatique&, int) const override { return 0; }
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int, const Neumann_sortie_libre&, int) const override;
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int, const Symetrie&, int) const override { return 0; }
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int, const Periodique&, int) const override;
  inline double coeffs_faces_interne_bloc_vitesse(const DoubleTab&, int) const override;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite pour les grandeurs scalaires.

  inline double secmem_face(int, const Symetrie&, int) const override { return 0; }
  inline double secmem_face(int, const Neumann_sortie_libre&, int) const override;
  inline double secmem_face(int, const Dirichlet_entree_fluide&, int) const override;
  inline double secmem_face(int, const Dirichlet_paroi_fixe&, int) const override { return 0; }
  inline double secmem_face(int, const Dirichlet_paroi_defilante&, int) const override { return 0; }
  inline double secmem_face(int, const Neumann_paroi_adiabatique&, int) const override { return 0; }
  inline double secmem_face(int, const Neumann_paroi&, int) const override { return 0; }
  inline double secmem_face(int, int, int, const Echange_externe_impose&, int) const override { return 0; }
  inline double secmem_face(int, const Echange_global_impose&, int) const override { return 0; }
  inline double secmem_face(int, const Periodique&, int) const override { return 0; }
  inline double secmem_faces_interne(int) const override { return 0; }

  // Fonctions qui servent a calculer les coefficients de la matrice pour des grandeurs
  // vectorielles dans le cas implicite.

  inline void coeffs_face(int, int, const Symetrie&, DoubleVect& aii, DoubleVect& ajj) const override { }
  inline void coeffs_face(int, int, const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj) const override;
  inline void coeffs_face(int, int, const Dirichlet_entree_fluide&, DoubleVect& aii, DoubleVect& ajj) const override;
  inline void coeffs_face(int, int, const Dirichlet_paroi_fixe&, DoubleVect& aii, DoubleVect& ajj) const override { }
  inline void coeffs_face(int, int, const Dirichlet_paroi_defilante&, DoubleVect& aii, DoubleVect& ajj) const override { }
  inline void coeffs_face(int, int, const Neumann_paroi_adiabatique&, DoubleVect& aii, DoubleVect& ajj) const override { }
  inline void coeffs_face(int, int, const Neumann_paroi&, DoubleVect& aii, DoubleVect& ajj) const override { }
  inline void coeffs_face(int, int, int, int, const Echange_externe_impose&, DoubleVect& aii, DoubleVect& ajj) const override { }
  inline void coeffs_face(int, int, const Echange_global_impose&, DoubleVect& aii, DoubleVect& ajj) const override { }
  inline void coeffs_face(int, int, const Periodique&, DoubleVect& aii, DoubleVect& ajj) const override;

  inline void coeffs_faces_interne(int, DoubleVect& aii, DoubleVect& ajj) const override;

  // Fonctions qui servent a calculer la contribution des conditions limites
  // au second membre pour l'implicite pour les grandeurs vectorielles.

  inline void secmem_face(int, const Symetrie&, int, DoubleVect&) const override { }
  inline void secmem_face(int, const Neumann_sortie_libre&, int, DoubleVect&) const override;
  inline void secmem_face(int, const Dirichlet_entree_fluide&, int, DoubleVect&) const override;
  inline void secmem_face(int, const Dirichlet_paroi_fixe&, int, DoubleVect&) const override { }
  inline void secmem_face(int, const Dirichlet_paroi_defilante&, int, DoubleVect&) const override { }
  inline void secmem_face(int, const Neumann_paroi_adiabatique&, int, DoubleVect&) const override { }
  inline void secmem_face(int, const Neumann_paroi&, int, DoubleVect&) const override { }
  inline void secmem_face(int, int, int, const Echange_externe_impose&, int, DoubleVect&) const override { }
  inline void secmem_face(int, const Echange_global_impose&, int, DoubleVect&) const override { }
  inline void secmem_face(int, const Periodique&, int, DoubleVect&) const override { }
  inline void secmem_faces_interne(int, DoubleVect& flux) const override { }
};

inline double Eval_Amont_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face, const Dirichlet_entree_fluide& la_cl, int num1) const
{
  int n0 = elem_(face, 0);
  int n1 = elem_(face, 1);
  double psc = dt_vitesse[face] * surface[face] * porosite[face];
  double flux;

  double val_imp = la_cl.val_imp(face - num1);

  if (n0 != -1)
    {
      if (psc > 0)
        flux = psc * inco[n0];
      else
        flux = psc * val_imp;
    }
  else   // n1 != -1
    {
      if (psc > 0)
        flux = psc * val_imp;
      else
        flux = psc * inco[n1];
    }
  return -flux;
}

inline void Eval_Amont_PolyMAC_Elem::coeffs_face(int face, int, const Dirichlet_entree_fluide& la_cl, double& aii, double& ajj) const
{
  int i = elem_(face, 0);
  //  int j = elem(face,1);
  double psc = dt_vitesse[face] * surface[face] * porosite[face];

  if (i != -1)
    {
      if (psc > 0)
        {
          aii = psc;
          ajj = 0;
        }
      else
        {
          aii = 0;
          ajj = 0;
        }
    }
  else   // j != -1
    {
      if (psc < 0)
        {
          ajj = -psc;
          aii = 0;
        }
      else
        {
          aii = 0;
          ajj = 0;
        }
    }
}

inline double Eval_Amont_PolyMAC_Elem::coeffs_face_bloc_vitesse(const DoubleTab& inco, int face, const Dirichlet_entree_fluide& la_cl, int num1) const
{
  int n0 = elem_(face, 0);
  int n1 = elem_(face, 1);
  double psc = surface[face] * porosite[face];
  double flux;

  double val_imp = la_cl.val_imp(face - num1);

  if (n0 != -1)
    {
      if (dt_vitesse[face] > 0)
        flux = psc * inco[n0];
      else
        flux = psc * val_imp;
    }
  else   // n1 != -1
    {
      if (dt_vitesse[face] > 0)
        flux = psc * val_imp;
      else
        flux = psc * inco[n1];
    }
  return flux;
}

inline double Eval_Amont_PolyMAC_Elem::secmem_face(int face, const Dirichlet_entree_fluide& la_cl, int num1) const
{
  int i = elem_(face, 0);
  //  int j = elem(face,1);
  double psc = dt_vitesse[face] * surface[face] * porosite[face];
  double flux;

  if (i != -1)
    {
      if (psc < 0)
        flux = psc * la_cl.val_imp(face - num1);
      else
        flux = 0;
    }
  else   // j != -1
    {
      if (psc > 0)
        flux = psc * la_cl.val_imp(face - num1);
      else
        flux = 0;
    }
  return -flux;
}

inline double Eval_Amont_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face, const Neumann_sortie_libre& la_cl, int num1) const
{
  double flux;
  int n0 = elem_(face, 0);
  int n1 = elem_(face, 1);
  double psc = dt_vitesse[face] * surface(face) * porosite(face);

  double val_ext = la_cl.val_ext(face - num1);

  if (n0 != -1)
    {
      if (psc > 0)
        flux = psc * inco[n0];
      else
        flux = psc * val_ext;
    }
  else   // n1 != -1
    {
      if (psc > 0)
        flux = psc * val_ext;
      else
        flux = psc * inco[n1];
    }
  return -flux;
}

inline void Eval_Amont_PolyMAC_Elem::coeffs_face(int face, int, const Neumann_sortie_libre& la_cl, double& aii, double& ajj) const
{
  int i = elem_(face, 0);
  //  int j = elem(face,1);
  double psc = dt_vitesse[face] * surface(face) * porosite(face);

  if (i != -1)
    {
      if (psc > 0)
        {
          aii = psc;
          ajj = 0;
        }
      else
        {
          aii = 0;
          ajj = 0;
        }
    }
  else   // j != -1
    {
      if (psc < 0)
        {
          ajj = -psc;
          aii = 0;
        }
      else
        {
          aii = 0;
          ajj = 0;
        }
    }
}

inline double Eval_Amont_PolyMAC_Elem::coeffs_face_bloc_vitesse(const DoubleTab& inco, int face, const Neumann_sortie_libre& la_cl, int num1) const
{
  double flux;
  int n0 = elem_(face, 0);
  int n1 = elem_(face, 1);
  double psc = surface(face) * porosite(face);

  double val_ext = la_cl.val_ext(face - num1);

  if (n0 != -1)
    {
      if (dt_vitesse[face] > 0)
        flux = psc * inco[n0];
      else
        flux = psc * val_ext;
    }
  else   // n1 != -1
    {
      if (dt_vitesse[face] > 0)
        flux = psc * val_ext;
      else
        flux = psc * inco[n1];
    }
  return flux;
}

inline double Eval_Amont_PolyMAC_Elem::secmem_face(int face, const Neumann_sortie_libre& la_cl, int num1) const
{
  double flux;
  int i = elem_(face, 0);
  //  int j = elem(face,1);
  double psc = dt_vitesse[face] * surface(face) * porosite(face);
  if (i != -1)
    {
      if (psc < 0)
        flux = psc * la_cl.val_ext(face - num1);
      else
        flux = 0;
    }
  else   // n1 != -1
    {
      if (psc > 0)
        flux = psc * la_cl.val_ext(face - num1);
      else
        flux = 0;
    }
  return -flux;
}

inline double Eval_Amont_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face, const Periodique& la_cl, int) const
{
  double psc = dt_vitesse[face] * surface(face) * porosite(face);
  double flux;
  if (psc > 0)
    flux = psc * inco(elem_(face, 0));
  else
    flux = psc * inco(elem_(face, 1));
  return -flux;
}

inline void Eval_Amont_PolyMAC_Elem::coeffs_face(int face, int, const Periodique& la_cl, double& aii, double& ajj) const
{
  double psc = dt_vitesse[face] * surface[face] * porosite[face];

  if (psc > 0)
    {
      aii = psc;
      ajj = 0;
    }

  else
    {
      ajj = -psc;
      aii = 0;
    }
}

inline double Eval_Amont_PolyMAC_Elem::coeffs_face_bloc_vitesse(const DoubleTab& inco, int face, const Periodique& la_cl, int) const
{
  double psc = surface(face) * porosite(face);
  double flux;
  if (dt_vitesse[face] > 0)
    flux = psc * inco(elem_(face, 0));
  else
    flux = psc * inco(elem_(face, 1));
  return flux;
}

inline double Eval_Amont_PolyMAC_Elem::flux_faces_interne(const DoubleTab& inco, int face) const
{
  double psc = dt_vitesse[face] * surface(face) * porosite(face);
  double flux;
  if (psc > 0)
    flux = psc * inco(elem_(face, 0));
  else
    flux = psc * inco(elem_(face, 1));
  return -flux;
}

inline void Eval_Amont_PolyMAC_Elem::coeffs_faces_interne(int face, double& aii, double& ajj) const
{
  double psc = dt_vitesse[face] * surface[face] * porosite[face];
  if (psc > 0)
    {
      aii = psc;
      ajj = 0;
    }
  else
    {
      ajj = -psc;
      aii = 0;
    }
}

inline double Eval_Amont_PolyMAC_Elem::coeffs_faces_interne_bloc_vitesse(const DoubleTab& inco, int face) const
{
  double psc = surface(face) * porosite(face);
  double flux;
  if (dt_vitesse[face] > 0)
    flux = psc * inco(elem_(face, 0));
  else
    flux = psc * inco(elem_(face, 1));
  return flux;
}

inline void Eval_Amont_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face, const Periodique& la_cl, int num1, DoubleVect& flux) const
{
  int k;
  double psc = dt_vitesse[face] * surface(face) * porosite(face);
  int elem1 = elem_(face, 0);
  int elem2 = elem_(face, 1);

  if (psc > 0)
    for (k = 0; k < flux.size(); k++)
      flux(k) = -psc * inco(elem1, k);
  else
    for (k = 0; k < flux.size(); k++)
      flux(k) = -psc * inco(elem2, k);
}

inline void Eval_Amont_PolyMAC_Elem::coeffs_face(int face, int, const Periodique& la_cl, DoubleVect& aii, DoubleVect& ajj) const
{
  int k;
  double psc = dt_vitesse[face] * surface(face) * porosite(face);
  if (psc > 0)
    {
      for (k = 0; k < aii.size(); k++)
        aii(k) = psc;
      for (k = 0; k < ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      for (k = 0; k < ajj.size(); k++)
        ajj(k) = -psc;
      for (k = 0; k < aii.size(); k++)
        aii(k) = 0;
    }
}

inline void Eval_Amont_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face, const Neumann_sortie_libre& la_cl, int num1, DoubleVect& flux) const
{
  int k;
  int n0 = elem_(face, 0);
  int n1 = elem_(face, 1);
  double psc = dt_vitesse[face] * surface(face) * porosite(face);
  if (n0 != -1)
    {
      if (psc > 0)
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * inco(n0, k);
      else
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * la_cl.val_ext(face - num1, k);
    }
  else   // n1 != -1
    {
      if (psc > 0)
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * la_cl.val_ext(face - num1, k);
      else
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * inco(n1, k);
    }
}

inline void Eval_Amont_PolyMAC_Elem::coeffs_face(int face, int, const Neumann_sortie_libre& la_cl, DoubleVect& aii, DoubleVect& ajj) const
{
  int k;
  int i = elem_(face, 0);
  double psc = dt_vitesse[face] * surface(face) * porosite(face);

  if (i != -1)
    {
      if (psc > 0)
        {
          for (k = 0; k < aii.size(); k++)
            aii(k) = psc;
          for (k = 0; k < ajj.size(); k++)
            ajj(k) = 0;
        }
      else
        {
          for (k = 0; k < aii.size(); k++)
            aii(k) = 0;
          for (k = 0; k < ajj.size(); k++)
            ajj(k) = 0;
        }
    }
  else   // j != -1
    {
      if (psc < 0)
        {
          for (k = 0; k < ajj.size(); k++)
            ajj(k) = -psc;
          for (k = 0; k < aii.size(); k++)
            aii(k) = 0;
        }
      else
        {
          for (k = 0; k < ajj.size(); k++)
            ajj(k) = 0;
          for (k = 0; k < aii.size(); k++)
            aii(k) = 0;
        }
    }
}

inline void Eval_Amont_PolyMAC_Elem::secmem_face(int face, const Neumann_sortie_libre& la_cl, int num1, DoubleVect& flux) const
{
  int k;
  int i = elem_(face, 0);
  double psc = dt_vitesse[face] * surface(face) * porosite(face);
  if (i != -1)
    {
      if (psc < 0)
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * la_cl.val_ext(face - num1, k);
      else
        for (k = 0; k < flux.size(); k++)
          flux(k) = 0;
    }
  else   // n1 != -1
    {
      if (psc > 0)
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * la_cl.val_ext(face - num1, k);
      else
        for (k = 0; k < flux.size(); k++)
          flux(k) = 0;
    }
}

inline void Eval_Amont_PolyMAC_Elem::flux_face(const DoubleTab& inco, int face, const Dirichlet_entree_fluide& la_cl, int num1, DoubleVect& flux) const
{
  int k;
  int n0 = elem_(face, 0);
  int n1 = elem_(face, 1);
  double psc = dt_vitesse[face] * surface[face] * porosite[face];
  if (n0 != -1)
    {
      if (psc > 0)
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * inco(n0, k);
      else
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * la_cl.val_imp(face - num1, k);
    }
  else   // n1 != -1
    {
      if (psc > 0)
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * la_cl.val_imp(face - num1, k);
      else
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * inco(n1, k);
    }
}

inline void Eval_Amont_PolyMAC_Elem::coeffs_face(int face, int, const Dirichlet_entree_fluide& la_cl, DoubleVect& aii, DoubleVect& ajj) const
{
  int k;
  int i = elem_(face, 0);
  double psc = dt_vitesse[face] * surface(face) * porosite(face);
  if (i != -1)
    {
      if (psc > 0)
        {
          for (k = 0; k < aii.size(); k++)
            aii(k) = psc;
          for (k = 0; k < ajj.size(); k++)
            ajj(k) = 0;
        }
      else
        {
          for (k = 0; k < aii.size(); k++)
            aii(k) = 0;
          for (k = 0; k < ajj.size(); k++)
            ajj(k) = 0;
        }
    }
  else   // j != -1
    {
      if (psc < 0)
        {
          for (k = 0; k < ajj.size(); k++)
            ajj(k) = -psc;
          for (k = 0; k < aii.size(); k++)
            aii(k) = 0;
        }
      else
        {
          for (k = 0; k < ajj.size(); k++)
            ajj(k) = 0;
          for (k = 0; k < aii.size(); k++)
            aii(k) = 0;
        }
    }
}

inline void Eval_Amont_PolyMAC_Elem::secmem_face(int face, const Dirichlet_entree_fluide& la_cl, int num1, DoubleVect& flux) const
{
  int k;
  int i = elem_(face, 0);
  double psc = dt_vitesse[face] * surface(face) * porosite(face);
  if (i != -1)
    {
      if (psc < 0)
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * la_cl.val_imp(face - num1, k);
      else
        for (k = 0; k < flux.size(); k++)
          flux(k) = 0;
    }
  else   // n1 != -1
    {
      if (psc > 0)
        for (k = 0; k < flux.size(); k++)
          flux(k) = -psc * la_cl.val_imp(face - num1, k);
      else
        for (k = 0; k < flux.size(); k++)
          flux(k) = 0;
    }
}

inline void Eval_Amont_PolyMAC_Elem::flux_faces_interne(const DoubleTab& inco, int face, DoubleVect& flux) const
{
  int k;
  int n0 = elem_(face, 0);
  int n1 = elem_(face, 1);
  double psc = dt_vitesse[face] * surface(face) * porosite(face);
  if (psc > 0)
    for (k = 0; k < flux.size(); k++)
      flux(k) = -psc * inco(n0, k);
  else
    for (k = 0; k < flux.size(); k++)
      flux(k) = -psc * inco(n1, k);
}

inline void Eval_Amont_PolyMAC_Elem::coeffs_faces_interne(int face, DoubleVect& aii, DoubleVect& ajj) const
{
  int k;
  double psc = dt_vitesse[face] * surface[face] * porosite[face];
  if (psc > 0)
    {
      for (k = 0; k < aii.size(); k++)
        aii(k) = psc;
      for (k = 0; k < ajj.size(); k++)
        ajj(k) = 0;
    }
  else
    {
      for (k = 0; k < ajj.size(); k++)
        ajj(k) = -psc;
      for (k = 0; k < aii.size(); k++)
        aii(k) = 0;
    }
}

#endif /* Eval_Amont_PolyMAC_Elem_included */
