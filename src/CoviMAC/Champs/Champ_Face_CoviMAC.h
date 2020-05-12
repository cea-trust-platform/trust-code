/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champ_Face_CoviMAC.h
// Directory:   $TRUST_ROOT/src/CoviMAC/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Face_CoviMAC_included
#define Champ_Face_CoviMAC_included

#include <Champ_Inc_base.h>
#include <Zone_VF.h>
#include <Ref_Zone_VF.h>
#include <Matrice_Morse.h>
#include <SolveurSys.h>

/////////////////////////////////////////////////////////////////////////////
// .NAME        : Champ_Face_CoviMAC
// .DESCRIPTION : class Champ_Face_CoviMAC
//
// Champ correspondant a une inconnue decrite par ses tangentes aux faces duales (ligne amont-aval, type vitesse)
// Les flux aux faces sont accessibles par les methodes valeurs_normales(), avec synchro automatique
// Degres de libertes : composantes tangentielles aux faces duales
/////////////////////////////////////////////////////////////////////////////

class Champ_Face_CoviMAC : public Champ_Inc_base
{

  Declare_instanciable(Champ_Face_CoviMAC) ;

protected :
  virtual       Champ_base& le_champ(void)      ;
  virtual const Champ_base& le_champ(void) const;

public :

  void    associer_zone_dis_base(const Zone_dis_base&);
  virtual const Zone_dis_base& zone_dis_base() const
  {
    return ref_zone_vf_.valeur();
  } ;

  /* methodes de Champ_Inc_Base reimplementees pour tenir compte de valeurs_normales */
  virtual int fixer_nb_valeurs_temporelles(int i);
  virtual int fixer_nb_valeurs_nodales(int n);
  virtual void creer_tableau_distribue(const MD_Vector& md, Array_base::Resize_Options opt = Array_base::COPY_INIT);
  virtual DoubleTab& valeurs();
  virtual const DoubleTab& valeurs() const
  {
    return Champ_Inc_base::valeurs();
  } ;
  virtual DoubleTab& valeurs(double tps);
  virtual const DoubleTab&   valeurs(double temps) const
  {
    return Champ_Inc_base::valeurs(temps);
  } ;
  virtual DoubleTab& futur(int i);
  virtual const DoubleTab& futur(int i=1) const
  {
    return Champ_Inc_base::futur(i);
  } ;
  virtual DoubleTab& passe(int i);
  virtual const DoubleTab& passe(int i=1) const
  {
    return Champ_Inc_base::passe(i);
  } ;
  virtual Champ_Inc_base& avancer(int i);
  virtual Champ_Inc_base& reculer(int i);
  virtual double changer_temps_futur(const double& t, int i);
  virtual double changer_temps_passe(const double& t, int i);
  virtual operator DoubleTab& ();
  virtual double changer_temps(const double& t);

  /* valeurs normales */
  const DoubleTab& valeurs_normales() const;
  const DoubleTab& valeurs_normales(double tps);

  virtual DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& result, int poly) const;
  virtual double valeur_a_elem_compo(const DoubleVect& position, int poly, int ncomp) const;
  virtual DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& polys, DoubleTab& result) const;
  virtual DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& polys, DoubleVect& result, int ncomp) const;

  virtual DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const;
  virtual int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const;

  virtual DoubleTab& valeur_aux_faces(DoubleTab& result) const;
  DoubleVect& calcul_S_barre_sans_contrib_paroi(const DoubleTab& vitesse, DoubleVect& SMA_barre) const;
  DoubleVect& calcul_S_barre(const DoubleTab& vitesse,DoubleVect& SMA_barre) const;
  DoubleTab& trace(const Frontiere_dis_base& , DoubleTab& , double, int distant ) const;

  Champ_base& affecter_(const Champ_base& );

  const Zone_VF& zone_vf() const
  {
    return ref_zone_vf_.valeur();
  };


  //tableaux de correspondance lies aux CLs
  //types de CL : 0 -> pas de CL
  //              1 -> Neumann ou Neumann_homogene
  //              2 -> Symetrie
  //              3 -> Dirichlet
  //              4 -> Dirichlet_homogene
  void init_cl() const;
  mutable IntTab icl; // icl(f, .) = { type de CL, num de la CL, indice de la face dans la CL }

  //integrale de la vitesse sur le bord des aretes duales -> pour calculer la vorticite
  void init_ra() const;
  mutable IntTab radeb, raji, rajf; //reconstruction du rotationnel par (raji, raci)[radeb(a, 0), radeb(a + 1, 0)[ (vitesses aux faces)
  mutable DoubleTab raci, racf;     //                                + (rajf, racf)[radeb(a, 1), radeb(a + 1, 1)[ (val_imp aux faces de bord)

  //interpolation aux aretes de la vitesse (dans le plan normal a chaque arete)
  void init_va() const;
  mutable IntTab vadeb, vaji, vajf;   //reconstruction de la vitesse par (vaji, vaci)[vadeb(a, 0), vadeb(a + 1, 0)[ (vitesses aux faces)
  mutable DoubleTab vaci, vacf;       // + (vajf, vacf)[vadeb(a, 1), vadeb(a + 1, 1)[ (val_imp aux faces de bord) + (vaja, vaca)[vadeb(., 2)] (val_imp aux aretes)

  //interpolations aux elements : vitesse val(e, i) = v_i, gradient vals(e, i, j) = dv_i / dx_j
  void interp_ve (const DoubleTab& inco, DoubleTab& val) const;
  void interp_gve(const DoubleTab& inco, DoubleTab& vals) const;

  /* interpolation faces duales -> faces */
  void interp_vfn(const DoubleTab& src, DoubleTab& dst) const;

protected:
  mutable Roue_ptr valeurs_normales_, valeurs_normales_a_jour_;
  REF(Zone_VF) ref_zone_vf_;
};


#endif /* Champ_Face_CoviMAC_included */
