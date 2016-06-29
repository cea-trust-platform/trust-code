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
// File:        Champ_Q1NC_impl.h
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Q1NC_impl_included
#define Champ_Q1NC_impl_included

#include <Champ_implementation.h>
#include <Frontiere_dis_base.h>
#include <Frontiere.h>

class Zone_VEF;
class IntTab;


class Champ_Q1NC_impl: public Champ_implementation
{

public:

  virtual ~Champ_Q1NC_impl() {}
  inline double fonction_forme_2D(double x, double y, int le_poly,
                                  int face);

  inline static double fonction_forme_2D_normalise(double x, double y, int face);

  inline double fonction_forme_3D(double x, double y, double z,
                                  int le_poly, int face);
  inline static double fonction_forme_3D_normalise(double x, double y, double z, int face);

  static DoubleTab& Derivee_fonction_forme_2D_normalise(double u, double v, DoubleTab& DF);

  static DoubleTab& Derivee_fonction_forme_3D_normalise(double u, double v, double w, DoubleTab& DF);

  DoubleVect& valeur_a_elem(const DoubleVect& position,
                            DoubleVect& val,
                            int le_poly) const;
  double calcule_valeur_a_elem_compo(double xs, double ys, double zs,
                                     int le_poly, int ncomp) const;
  double valeur_a_elem_compo(const DoubleVect& position,
                             int le_poly, int ncomp) const;
  double valeur_a_sommet_compo(int num_som, int le_poly, int ncomp) const;
  DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                              const IntVect& les_polys,
                              DoubleTab& valeurs) const;
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleVect& valeurs,
                                     int ncomp) const ;
  DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const;
  DoubleVect& valeur_aux_sommets_compo(const Domaine&,
                                       DoubleVect&, int) const;
  DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const;
  int remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                    IntVect& polys) const;
  void transforme_coord2D();
  void transforme_coord3D();

protected :
  virtual const Zone_VEF& zone_vef() const=0;
  inline DoubleTab& trace(const Frontiere_dis_base& fr, const DoubleTab& y, DoubleTab& x,int distant) const;
  //  virtual void dimensionner_array() = 0;
  //  DoubleVect dummy;
  DoubleTab tab_param;

};

inline double Champ_Q1NC_impl::fonction_forme_2D(double x, double y, int le_poly,  int face)
{
  double ksi,eta;
  ksi=tab_param(le_poly,0)*x+tab_param(le_poly,1)*y+tab_param(le_poly,2);
  eta=tab_param(le_poly,3)*x+tab_param(le_poly,4)*y+tab_param(le_poly,5);
  double fonction_de_forme=1.;
  fonction_de_forme*=fonction_forme_2D_normalise(ksi,eta,face);
  // Cerr << "fonction_de_forme " << fonction_de_forme << finl;
  return fonction_de_forme;
}

inline double Champ_Q1NC_impl::fonction_forme_3D(double x, double y, double z,
                                                 int le_poly, int face)
{
  double ksi,eta,psi;

  ksi=tab_param(le_poly,0)*x+tab_param(le_poly,1)*y+tab_param(le_poly,2)*z+tab_param(le_poly,3);
  eta=tab_param(le_poly,4)*x+tab_param(le_poly,5)*y+tab_param(le_poly,6)*z+tab_param(le_poly,7);
  psi=tab_param(le_poly,8)*x+tab_param(le_poly,9)*y+tab_param(le_poly,10)*z+tab_param(le_poly,11);
  double fonction_de_forme=1.;

  fonction_de_forme*=fonction_forme_3D_normalise(ksi,eta,psi,face);


  // Cerr << "ksi " << ksi << " eta " << eta << " psi " << psi << finl;
  return fonction_de_forme;

}

inline double Champ_Q1NC_impl::fonction_forme_2D_normalise(double ksi, double eta, int face )
{
  // la fonction de forme est calculer dans la base (ksi), (eta).
  // aux milieu des faces.
  // Psi1(x,y) = 0.25 - 0.5ksi + 0.25(ksi^2 - eta^2)
  // Psi2(x,y) = 0.25 - 0.5eta - 0.25(ksi^2 - eta^2)
  // Psi3(x,y) = 0.25 + 0.5ksi + 0.25(ksi^2 - eta^2)
  // Psi4(x,y) = 0.25 + 0.5eta - 0.25(ksi^2 - eta^2)

  double fonction_de_forme_normalisee, carre_ksi=ksi*ksi, carre_eta=eta*eta ;

  switch(face)
    {
    case 0:
      {
        fonction_de_forme_normalisee = 0.25 - 0.5*ksi + 0.25*(carre_ksi - carre_eta);
        break;
      }
    case 1:
      {
        fonction_de_forme_normalisee = 0.25 - 0.5*eta - 0.25*(carre_ksi - carre_eta);
        break;
      }
    case 2:
      {
        fonction_de_forme_normalisee = 0.25 + 0.5*ksi + 0.25*(carre_ksi - carre_eta);
        break;
      }
    case 3:
      {
        fonction_de_forme_normalisee = 0.25 + 0.5*eta - 0.25*(carre_ksi - carre_eta);
        break;
      }
    default :
      {
        Cerr << "Erreur dans Champ_Q1NC_impl::fonction_forme_2D : " << finl;
        Cerr << "Un quadrangle n'a pas " << face << " faces" << finl;
        Process::exit();
        fonction_de_forme_normalisee=-1;
      }
    }
  return fonction_de_forme_normalisee;
}

inline double Champ_Q1NC_impl::fonction_forme_3D_normalise(double ksi, double eta, double psi, int face )
{
  // la fonction de forme est calculer dans la base (ksi), (eta), (psi).
  // aux milieu des faces.

  // Psi0(x,y,z) = 1/6. - 0.5ksi + 1/3.(ksi^2 - eta^2) + 1/6.(eta^2 - psi^2)
  // Psi1(x,y,z) = 1/6. - 0.5eta - 1/6.(ksi^2 - eta^2) + 1/6.(eta^2 - psi^2)

  // Psi2(x,y,z) = 1/6. - 0.5psi - 1/6.(ksi^2 - eta^2) + 1/6.(eta^2 - psi^2)
  // Psi3(x,y,z) = 1/6. + 0.5ksi + 1/3.(ksi^2 - eta^2) + 1/6.(eta^2 - psi^2)

  // Psi4(x,y,z) = 1/6. + 0.5eta - 1/6.(ksi^2 - eta^2) + 1/6.(eta^2 - psi^2)
  // Psi5(x,y,z) = 1/6. + 0.5psi - 1/6.(ksi^2 - eta^2) + 1/6.(eta^2 - psi^2)


  double fonction_de_forme_normalisee ;
  double six=1./6., tier=1./3., carre_ksi=ksi*ksi, carre_eta=eta*eta, carre_psi=psi*psi;
  switch(face)
    {
    case 0:
      {
        fonction_de_forme_normalisee = six - 0.5*ksi + tier*(carre_ksi - carre_eta) + six*(carre_eta - carre_psi);
        break;
      }
    case 1:
      {
        fonction_de_forme_normalisee = six - 0.5*eta - six*(carre_ksi - carre_eta) + six*(carre_eta - carre_psi);
        break;
      }
    case 2:
      {
        fonction_de_forme_normalisee = six - 0.5*psi - six*(carre_ksi - carre_eta) - tier*(carre_eta - carre_psi);
        break;
      }
    case 3:
      {
        fonction_de_forme_normalisee = six + 0.5*ksi + tier*(carre_ksi - carre_eta) + six*(carre_eta - carre_psi);
        break;
      }
    case 4:
      {
        fonction_de_forme_normalisee = six + 0.5*eta - six*(carre_ksi - carre_eta) + six*(carre_eta - carre_psi);
        break;
      }
    case 5:
      {
        fonction_de_forme_normalisee = six + 0.5*psi - six*(carre_ksi - carre_eta) - tier*(carre_eta - carre_psi);
        break;
      }
    default :
      {
        Cerr << "Erreur dans Champ_Q1NC_impl::fonction_forme_2D : " << finl;
        Cerr << "Un quadrangle n'a pas " << face << " faces" << finl;
        fonction_de_forme_normalisee=-1;
        Process::exit();
      }
    }
  return fonction_de_forme_normalisee;
}

inline DoubleTab& Champ_Q1NC_impl::trace(const Frontiere_dis_base& fr, const DoubleTab& y, DoubleTab& x,int distant) const
{
  if (distant)
    fr.frontiere().trace_face_distant(y, x);
  else
    fr.frontiere().trace_face_local(y, x);
  // useless ? x.echange_espace_virtuel();
  return x;
}

#endif
