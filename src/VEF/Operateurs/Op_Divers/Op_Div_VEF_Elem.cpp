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

#include <Op_Div_VEF_Elem.h>
#include <Domaine_Cl_VEF.h>
#include <Periodique.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>

#include <EcrFicPartage.h>
#include <TRUSTTrav.h>
#include <communications.h>

Implemente_instanciable(Op_Div_VEF_Elem,"Op_Div_VEF_P1NC",Operateur_Div_base);

Sortie& Op_Div_VEF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

Entree& Op_Div_VEF_Elem::readOn(Entree& s)
{
  return s ;
}


inline void erreur()
{
  Cerr << "VEF discretization obsolete." << finl;
  Cerr << "Use VEFPreP1B discretization instead with P0 support." << finl;
  Process::exit();
}

/*! @brief
 *
 */
void Op_Div_VEF_Elem::associer(const Domaine_dis& domaine_dis,
                               const Domaine_Cl_dis& domaine_Cl_dis,
                               const Champ_Inc&)
{
  const Domaine_VEF& zvef = ref_cast(Domaine_VEF, domaine_dis.valeur());
  const Domaine_Cl_VEF& zclvef = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis.valeur());
  le_dom_vef = zvef;
  la_zcl_vef = zclvef;
}

DoubleTab& Op_Div_VEF_Elem::calculer(const DoubleTab& vit, DoubleTab& div) const
{
  div = 0;
  return ajouter(vit,div);

}

void Op_Div_VEF_Elem::volumique(DoubleTab& div) const
{
  // PQ : 04/03
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const DoubleVect& vol = domaine_VEF.volumes();
  int nb_elem=domaine_VEF.domaine().nb_elem_tot();
  bool kernelOnDevice = div.isKernelOnDevice(vol, "Op_Div_VEF_Elem::volumique(x)");
  const double * vol_addr = kernelOnDevice ? mapToDevice(vol) : vol.addr();
  double * div_addr = kernelOnDevice ? computeOnTheDevice(div) : div.addr();
  #pragma omp target teams distribute parallel for if (kernelOnDevice && Objet_U::computeOnDevice)
  for(int num_elem=0; num_elem<nb_elem; num_elem++)
    div_addr[num_elem]/=vol_addr[num_elem];
}

int Op_Div_VEF_Elem::impr(Sortie& os) const
{
  const int impr_bord=(le_dom_vef->domaine().bords_a_imprimer().est_vide() ? 0:1);
  const Schema_Temps_base& sch = equation().probleme().schema_temps();
  double temps = sch.temps_courant();

  int nb_compo=flux_bords_.dimension(1);
  // On parcours les frontieres pour sommer les flux par frontiere dans le tableau flux_bord
  DoubleVect bilan(nb_compo);
  bilan = 0;
  int nb_cl = le_dom_vef->nb_front_Cl();
  // flux_bords contains the sum of flux on each boundary:
  DoubleTrav tab_flux_bords(3,nb_cl,nb_compo);
  for (int num_cl=0; num_cl<nb_cl; num_cl++)
    {
      const Cond_lim& la_cl = la_zcl_vef->les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      int perio = (sub_type(Periodique,la_cl.valeur())?1:0);
      for (int face=ndeb; face<nfin; face++)
        for(int k=0; k<nb_compo; k++)
          {
            tab_flux_bords(0,num_cl,k)+=flux_bords_(face, k);
            if(perio)
              {
                if(face<(ndeb+frontiere_dis.nb_faces()/2))
                  tab_flux_bords(1,num_cl,k)+=flux_bords_(face, k);
                else
                  tab_flux_bords(2,num_cl,k)+=flux_bords_(face, k);
              }
          }
    }
  // Sum on all CPUs:
  mp_sum_for_each_item(tab_flux_bords);

  // Print
  if(je_suis_maitre())
    {
      //SFichier Flux_div;
      if (!Flux_div.is_open()) ouvrir_fichier(Flux_div,"",1);
      Flux_div.add_col(temps);
      for (int num_cl=0; num_cl<nb_cl; num_cl++)
        {
          const Cond_lim& la_cl = la_zcl_vef->les_conditions_limites(num_cl);
          int perio = (sub_type(Periodique,la_cl.valeur())?1:0);
          for(int k=0; k<nb_compo; k++)
            {
              if(perio)
                {
                  Flux_div.add_col(tab_flux_bords(1,num_cl,k));
                  Flux_div.add_col(tab_flux_bords(2,num_cl,k));
                }
              else
                Flux_div.add_col(tab_flux_bords(0,num_cl,k));
              bilan(k)+=tab_flux_bords(0,num_cl,k);
            }
        }

      for(int k=0; k<nb_compo; k++)
        Flux_div.add_col(bilan(k));
      Flux_div << finl;
    }

  const LIST(Nom)& Liste_bords_a_imprimer = le_dom_vef->domaine().bords_a_imprimer();
  if (!Liste_bords_a_imprimer.est_vide())
    {
      EcrFicPartage Flux_face;
      ouvrir_fichier_partage(Flux_face,"",impr_bord);
      for (int num_cl=0; num_cl<nb_cl; num_cl++)
        {
          const Frontiere_dis_base& la_fr = la_zcl_vef->les_conditions_limites(num_cl).frontiere_dis();
          const Cond_lim& la_cl = la_zcl_vef->les_conditions_limites(num_cl);
          const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = frontiere_dis.num_premiere_face();
          int nfin = ndeb + frontiere_dis.nb_faces();
          if (le_dom_vef->domaine().bords_a_imprimer().contient(la_fr.le_nom()))
            {
              Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps " << temps << " : " << finl;
              for (int face=ndeb; face<nfin; face++)
                {
                  if (dimension==2)
                    Flux_face << "# Face a x= " << le_dom_vef->xv(face,0) << " y= " << le_dom_vef->xv(face,1) << " flux=" ;
                  else if (dimension==3)
                    Flux_face << "# Face a x= " << le_dom_vef->xv(face,0) << " y= " << le_dom_vef->xv(face,1) << " z= " << le_dom_vef->xv(face,2) << " flux=" ;
                  for(int k=0; k<nb_compo; k++)
                    Flux_face << " " << flux_bords_(face, k);
                  Flux_face << finl;
                }
              Flux_face.syncfile();
            }
        }
    }
  return 1;
}

DoubleTab& Op_Div_VEF_Elem::ajouter(const DoubleTab& vit, DoubleTab& div) const
{
  erreur();
  return div;
}

