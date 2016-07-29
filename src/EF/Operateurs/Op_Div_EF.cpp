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
// File:        Op_Div_EF.cpp
// Directory:   $TRUST_ROOT/src/EF/Operateurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Div_EF.h>
#include <Zone_Cl_EF.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Schema_Temps_base.h>
#include <Check_espace_virtuel.h>
#include <EcrFicPartage.h>
#include <SFichier.h>

Implemente_instanciable(Op_Div_EF,"Op_Div_EF",Operateur_Div_base);


//// printOn
//

Sortie& Op_Div_EF::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Div_EF::readOn(Entree& s)
{
  return s ;
}



// Description:
void Op_Div_EF::associer(const Zone_dis& zone_dis,
                         const Zone_Cl_dis& zone_Cl_dis,
                         const Champ_Inc&)
{
  const Zone_EF& zEF = ref_cast(Zone_EF, zone_dis.valeur());
  const Zone_Cl_EF& zclEF = ref_cast(Zone_Cl_EF, zone_Cl_dis.valeur());
  la_zone_EF = zEF;
  la_zcl_EF = zclEF;
}

DoubleTab& Op_Div_EF::ajouter(const DoubleTab& vit, DoubleTab& div) const
{
  const Zone_EF& zone_ef=ref_cast(Zone_EF,equation().zone_dis().valeur());


  const DoubleTab& Bij_thilde=zone_ef.Bij_thilde();
  int nb_elem=zone_ef.zone().nb_elem();
  int nb_som_elem=zone_ef.zone().nb_som_elem();
  const IntTab& elems=zone_ef.zone().les_elems() ;

  for (int elem=0; elem<nb_elem; elem++)
    {
      for (int s=0; s<nb_som_elem; s++)
        {
          int som=elems(elem,s);
          for (int i=0; i<dimension; i++)
            {
              div(elem)+=Bij_thilde(elem,s,i)*vit(som,i);
            }
          // Cerr<<finl;
        }
    }
  // L'espace virtuel du tableau div n'est pas mis a jour par l'operateur,
  //  assert(invalide_espace_virtuel(div));
  declare_espace_virtuel_invalide(div);
  // calcul de flux bord

  const IntTab& face_sommets=zone_ef.face_sommets();
  int nb_som_face=zone_ef.nb_som_face();
  const DoubleTab& face_normales = zone_ef.face_normales();
  DoubleTab& tab_flux_bords = ref_cast(DoubleTab,flux_bords_);
  const DoubleVect& porosite_sommet=zone_ef.porosite_sommet();
  tab_flux_bords.resize(zone_ef.nb_faces_bord(),1);
  tab_flux_bords=0;
  int premiere_face_int=zone_ef.premiere_face_int();
  for (int face=0; face<premiere_face_int; face++)
    {
      for (int s=0; s<nb_som_face; s++)
        {
          int som=face_sommets(face,s);
          for (int dir=0; dir<Objet_U::dimension; dir++)
            tab_flux_bords(face,0)+=porosite_sommet(som)*vit(som,dir)*face_normales(face,dir)/nb_som_face;
        }
    }


  return div;
}

DoubleTab& Op_Div_EF::calculer(const DoubleTab& vit, DoubleTab& div) const
{
  div = 0;
  return ajouter(vit,div);

}

int Op_Div_EF::impr(Sortie& os) const
{

  Nom espace=" \t";
  const int impr_bord=(la_zone_EF->zone().Bords_a_imprimer().est_vide() ? 0:1);
  SFichier Flux_div;
  ouvrir_fichier(Flux_div,"",je_suis_maitre());
  EcrFicPartage Flux_face;
  ouvrir_fichier_partage(Flux_face,"",impr_bord);
  double temps=equation().probleme().schema_temps().temps_courant();
  if (je_suis_maitre()) Flux_div<< temps;

  int nb_compo=flux_bords_.dimension(1);
  // On parcours les frontieres pour sommer les flux par frontiere dans le tableau flux_bord
  DoubleVect flux_bord(nb_compo);
  DoubleVect bilan(nb_compo);
  bilan = 0;
  for (int num_cl=0; num_cl<la_zone_EF->nb_front_Cl(); num_cl++)
    {
      flux_bord=0;
      const Cond_lim& la_cl = la_zcl_EF->les_conditions_limites(num_cl);
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
              Flux_div<< espace << flux_bord(k);
              bilan(k)+=flux_bord(k);
            }
        }
    }

  if(je_suis_maitre())
    {
      for(int k=0; k<nb_compo; k++)
        {
          Flux_div<< espace << bilan(k) ;
        }
      Flux_div << finl;
    }

  for (int num_cl=0; num_cl<la_zone_EF->nb_front_Cl(); num_cl++)
    {
      const Frontiere_dis_base& la_fr = la_zcl_EF->les_conditions_limites(num_cl).frontiere_dis();
      const Cond_lim& la_cl = la_zcl_EF->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      if (la_zone_EF->zone().Bords_a_imprimer().contient(la_fr.le_nom()))
        {
          Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps " << temps << " : " << finl;
          for (int face=ndeb; face<nfin; face++)
            {
              if (dimension==2)
                Flux_face << "# Face a x= " << la_zone_EF->xv(face,0) << " y= " << la_zone_EF->xv(face,1) << " flux=" ;
              else if (dimension==3)
                Flux_face << "# Face a x= " << la_zone_EF->xv(face,0) << " y= " << la_zone_EF->xv(face,1) << " z= " << la_zone_EF->xv(face,2) << " flux=" ;
              for(int k=0; k<nb_compo; k++)
                Flux_face << " " << flux_bords_(face, k);
              Flux_face << finl;
            }
          Flux_face.syncfile();
        }
    }

  return 1;
}


void Op_Div_EF::volumique(DoubleTab& div) const
{
  const Zone_EF& zone_EF = la_zone_EF.valeur();
  const DoubleVect& vol = zone_EF.volumes();
  int nb_elem=zone_EF.zone().nb_elem_tot();
  int num_elem;

  for(num_elem=0; num_elem<nb_elem; num_elem++)
    div(num_elem)/=vol(num_elem);
}
