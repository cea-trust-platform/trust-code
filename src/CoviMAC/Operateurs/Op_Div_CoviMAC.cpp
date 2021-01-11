/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Op_Div_CoviMAC.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Div_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
//#include <Les_Cl.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Check_espace_virtuel.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <Matrice_Morse.h>
#include <Matrix_tools.h>
#include <Array_tools.h>
#include <Debog.h>

Implemente_instanciable(Op_Div_CoviMAC,"Op_Div_CoviMAC",Operateur_Div_base);


//// printOn
//

Sortie& Op_Div_CoviMAC::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Div_CoviMAC::readOn(Entree& s)
{
  return s ;
}



// Description:
void Op_Div_CoviMAC::associer(const Zone_dis& zone_dis,
                              const Zone_Cl_dis& zone_Cl_dis,
                              const Champ_Inc&)
{
  const Zone_CoviMAC& zCoviMAC = ref_cast(Zone_CoviMAC, zone_dis.valeur());
  const Zone_Cl_CoviMAC& zclCoviMAC = ref_cast(Zone_Cl_CoviMAC, zone_Cl_dis.valeur());
  la_zone_CoviMAC = zCoviMAC;
  la_zcl_CoviMAC = zclCoviMAC;
}

void Op_Div_CoviMAC::dimensionner(Matrice_Morse& matrice) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const IntTab& f_e = zone.face_voisins();
  int i, e, f, n, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), N = equation().inconnue().valeurs().line_size(), D = dimension;
  ch.init_cl();

  IntTab stencil(0,2);
  stencil.set_smart_resize(1);
  for (f = 0; f < zone.nb_faces(); f++) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
      for (n = 0; n < N; n++) stencil.append_line(N * e + n, N * f + n);

  tableau_trier_retirer_doublons(stencil);
  Matrix_tools::allocate_morse_matrix(N * ne_tot, N * (nf_tot + D * ne_tot), stencil, matrice);
}

DoubleTab& Op_Div_CoviMAC::ajouter(const DoubleTab& vit, DoubleTab& div) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face();
  const IntTab& f_e = zone.face_voisins();
  int i, e, f, n, N = vit.line_size();
  assert(div.line_size() == N);
  ch.init_cl();

  DoubleTab& tab_flux_bords = ref_cast(DoubleTab,flux_bords_);
  tab_flux_bords.resize(zone.nb_faces_bord(),1);
  tab_flux_bords=0;

  for (f = 0; f < zone.nb_faces(); f++)
    {
      for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) for (n = 0; n < N; n++)
          div(e, n) += (i ? -1 : 1) * fs(f) * pf(f) * vit(f, n);
      if (f < zone.premiere_face_int()) for (n = 0; n < N; n++) tab_flux_bords(f, n) = fs(f) * pf(f) * vit(f, n);
    }

  div.echange_espace_virtuel();

  return div;
}
void Op_Div_CoviMAC::contribuer_a_avec(const DoubleTab&,Matrice_Morse& mat) const
{
  const Zone_CoviMAC& zone = la_zone_CoviMAC.valeur();
  const Champ_Face_CoviMAC& ch = ref_cast(Champ_Face_CoviMAC, equation().inconnue().valeur());
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face();
  const IntTab& f_e = zone.face_voisins();
  int i, e, f, n, N = ch.valeurs().line_size();
  ch.init_cl();

  for (f = 0; f < zone.nb_faces(); f++) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++)
      for (n = 0; n < N; n++) mat(N * e + n, N * f + n) += (i ? 1 : -1) * fs(f) * pf(f);
}

DoubleTab& Op_Div_CoviMAC::calculer(const DoubleTab& vit, DoubleTab& div) const
{
  div = 0;
  return ajouter(vit,div);

}

int Op_Div_CoviMAC::impr(Sortie& os) const
{

  const int impr_bord=(la_zone_CoviMAC->zone().Bords_a_imprimer().est_vide() ? 0:1);
  SFichier Flux_div;
  ouvrir_fichier(Flux_div,"",je_suis_maitre());
  EcrFicPartage Flux_face;
  ouvrir_fichier_partage(Flux_face,"",impr_bord);
  const Schema_Temps_base& sch = equation().probleme().schema_temps();
  double temps = sch.temps_courant();
  if (je_suis_maitre()) Flux_div.add_col(temps);

  int nb_compo=flux_bords_.dimension(1);
  // On parcours les frontieres pour sommer les flux par frontiere dans le tableau flux_bord
  DoubleVect flux_bord(nb_compo);
  DoubleVect bilan(nb_compo);
  bilan = 0;
  for (int num_cl=0; num_cl<la_zone_CoviMAC->nb_front_Cl(); num_cl++)
    {
      flux_bord=0;
      const Cond_lim& la_cl = la_zcl_CoviMAC->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      for (int face=ndeb; face<nfin; face++)
        for(int k=0; k<nb_compo; k++)
          flux_bord(k)+=flux_bords_(face, k);
      for(int k=0; k<nb_compo; k++)
        flux_bord(k)=Process::mp_sum(flux_bord(k));

      if(je_suis_maitre())
        {
          for(int k=0; k<nb_compo; k++)
            {
              //Ajout pour impression sur fichiers separes
              Flux_div.add_col(flux_bord(k));
              bilan(k)+=flux_bord(k);
            }
        }
    }

  if(je_suis_maitre())
    {
      for(int k=0; k<nb_compo; k++)
        {
          Flux_div.add_col(bilan(k));
        }
      Flux_div << finl;
    }

  for (int num_cl=0; num_cl<la_zone_CoviMAC->nb_front_Cl(); num_cl++)
    {
      const Frontiere_dis_base& la_fr = la_zcl_CoviMAC->les_conditions_limites(num_cl).frontiere_dis();
      const Cond_lim& la_cl = la_zcl_CoviMAC->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      if (la_zone_CoviMAC->zone().Bords_a_imprimer().contient(la_fr.le_nom()))
        {
          Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps " << temps << " : " << finl;
          for (int face=ndeb; face<nfin; face++)
            {
              if (dimension==2)
                Flux_face << "# Face a x= " << la_zone_CoviMAC->xv(face,0) << " y= " << la_zone_CoviMAC->xv(face,1) << " flux=" ;
              else if (dimension==3)
                Flux_face << "# Face a x= " << la_zone_CoviMAC->xv(face,0) << " y= " << la_zone_CoviMAC->xv(face,1) << " z= " << la_zone_CoviMAC->xv(face,2) << " flux=" ;
              for(int k=0; k<nb_compo; k++)
                Flux_face << " " << flux_bords_(face, k);
              Flux_face << finl;
            }
          Flux_face.syncfile();
        }
    }

  return 1;
}


void Op_Div_CoviMAC::volumique(DoubleTab& div) const
{
  const Zone_CoviMAC& zone_CoviMAC = la_zone_CoviMAC.valeur();
  const DoubleVect& vol = zone_CoviMAC.volumes();
  int nb_elem=zone_CoviMAC.zone().nb_elem_tot();
  int num_elem;

  for(num_elem=0; num_elem<nb_elem; num_elem++)
    div(num_elem)/=vol(num_elem);
}
