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

#include <Op_Grad_EF.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Navier_Stokes_std.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <EcrFicPartage.h>
#include <SFichier.h>
#include <Champ_P0_EF.h>
#include <Check_espace_virtuel.h>
#include <Debog.h>
#include <Discretisation_base.h>
#include <EChaine.h>

#include <interface_INITGAUSS.h>
#include <interface_CALCULSI.h>
#include <Domaine.h>

Implemente_instanciable(Op_Grad_EF,"Op_Grad_EF",Operateur_Grad_base);


//// printOn
//

Sortie& Op_Grad_EF::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Grad_EF::readOn(Entree& s)
{
  return s ;
}

void Op_Grad_EF::mettre_a_jour(double temps)
{
  Operateur_Grad_base::mettre_a_jour(temps);
}

#undef lire_valimp_dans_med_

/*! @brief
 *
 */
void Op_Grad_EF::associer(const Domaine_dis& domaine_dis,
                          const Domaine_Cl_dis& domaine_Cl_dis,
                          const Champ_Inc& inc)
{
  const Domaine_EF& zEF = ref_cast(Domaine_EF, domaine_dis.valeur());
  const Domaine_Cl_EF& zclEF = ref_cast(Domaine_Cl_EF, domaine_Cl_dis.valeur());
  le_dom_EF = zEF;
  la_zcl_EF = zclEF;
  //  const IntTab& face_sommets=zEF.face_sommets();
#ifdef  lire_valimp_dans_med_
//  assert(face_sommets.dimension(1)==4);
// if (face_sommets.dimension(1)==4)
  {
    zclEF.equation().probleme().discretisation().discretiser_champ("CHAMP_SOMMETS",zEF,"int_P_bord","?",dimension,0.,int_P_bord_);


    {
      Champ_Don champ_lu;
      EChaine ee("champ_fonc_med last_time termes.med MAILLAGE_GENEPI terme_pression_limite_qdm_10 som 0.");
      ee>>champ_lu;
      int_P_bord_.valeur().affecter(champ_lu);
    }
  }
#endif

}




static int bid=0;
void test(const DoubleTab& pression, DoubleTab& grad,const Op_Grad_EF&  op)
{
  if (bid==0)
    {
      bid=1;
      DoubleTab pe(pression);
      pe=1;
      op.ajouter(pe,grad);
      Cerr<<"ici max grad "<<grad.mp_max_abs_vect()<<finl;
      //Cerr<<grad<<finl;
    }

}




void ajouter_bord(DoubleTab& resu,const Domaine_EF& domaine_EF,const  Domaine_Cl_EF& domaine_Cl_EF, const Champ_Don& int_P_bord_ )
{
  // const IntTab& face_voisins = domaine_EF.face_voisins();
  const DoubleTab& face_normales = domaine_EF.face_normales();

  const IntTab& face_sommets=domaine_EF.face_sommets();
  int nb_som_face=domaine_EF.nb_som_face();

  if (int_P_bord_.non_nul())
    {
      resu+= int_P_bord_.valeurs();
    }
  else

    {
      const DoubleVect& porosite_sommet = domaine_EF.porosite_sommet();

      if (face_sommets.dimension(1)==4)
        {
          ArrOfInt filter(4) ;


          filter[0] = 0 ;
          filter[1] = 2 ;
          filter[2] = 3 ;
          filter[3] = 1 ;
          interface_INITGAUSS init_gauss;
          int npgau=9;
          int nbnn=4;
          DoubleTab xgau(npgau,3),frgau(npgau,nbnn),dfrgau(npgau,nbnn,(int)3),poigau(npgau);
          int dim=2;
          //	  init_gauss.compute(&dim,&nbnn,&npgau,xgau.addr(),frgau.addr(),dfrgau.addr(),poigau.addr());
          init_gauss.Compute(dim,nbnn,npgau,xgau,frgau,dfrgau,poigau);
          //ArrOfInt num(nbnn);
          interface_CALCULSI CALCULSI;
          DoubleTab xl(3,8),poro(4),detj(npgau), cnorm(3),si(4);
          int ip=0;
          const DoubleTab& coord=domaine_EF.domaine().coord_sommets();

          //const DoubleTab& face_normales = domaine_EF.face_normales();

          ArrOfInt num(4);
          for ( int n_bord=0; n_bord<domaine_EF.nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl = domaine_Cl_EF.les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

              int nfin = le_bord.nb_faces_tot();
              if ( sub_type(Neumann_sortie_libre,la_cl.valeur()) )
                {
                  const Neumann_sortie_libre& la_cl_typee =
                    ref_cast(Neumann_sortie_libre, la_cl.valeur());

                  for (int ind_face=0; ind_face<nfin; ind_face++)
                    {
                      int face=le_bord.num_face(ind_face);
                      for (int i=0; i<3; i++)
                        cnorm(i)=face_normales(face,i);
                      cnorm/=norme_array(cnorm);
                      //	      int elem = face_voisins(face,0);
                      double val_imp= la_cl_typee.flux_impose(ind_face);

                      for (int i=0; i<nbnn; i++)
                        {
                          num[i]=face_sommets(face,filter[i]);
                        }

                      for (int i=0; i<nbnn; i++)
                        {
                          for (int d=0; d<Objet_U::dimension; d++)
                            xl(d,i)=coord(num[i],d);
                          poro(i)=porosite_sommet[num[i]];
                        }

                      ip=1;
                      CALCULSI.Compute(nbnn, xl,
                                       poro,ip,
                                       npgau, xgau, frgau, dfrgau,
                                       poigau,detj, cnorm,si);
                      for (int i=0; i<nbnn; i++)
                        {

                          for (int d=0; d<Objet_U::dimension; d++)
                            {
                              int som=num[(i)];
                              resu(som,d)+=si(i)*cnorm(d)*val_imp;
                            }
                        }
                    }
                }
            }
        }
      else
        for ( int n_bord=0; n_bord<domaine_EF.nb_front_Cl(); n_bord++)
          {
            const Cond_lim& la_cl = domaine_Cl_EF.les_conditions_limites(n_bord);
            const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

            int nfin = le_bord.nb_faces_tot();
            if ( sub_type(Neumann_sortie_libre,la_cl.valeur()) )
              {
                const Neumann_sortie_libre& la_cl_typee =
                  ref_cast(Neumann_sortie_libre, la_cl.valeur());

                for (int ind_face=0; ind_face<nfin; ind_face++)
                  {
                    int face=le_bord.num_face(ind_face);
                    //	      int elem = face_voisins(face,0);
                    double val_imp= la_cl_typee.flux_impose(ind_face);
                    val_imp/=nb_som_face;
                    for (int s=0; s<nb_som_face; s++)
                      {
                        int som=face_sommets(face,s);
                        for (int dir=0; dir<Objet_U::dimension; dir++)
                          resu(som,dir)+=val_imp*face_normales(face,dir)*porosite_sommet(som);
                      }
                  }
              }
          }
    }
  domaine_Cl_EF.imposer_symetrie(resu);
}


DoubleTab& Op_Grad_EF::ajouter(const DoubleTab& pression, DoubleTab& grad) const
{

  test(pression,grad,(*this));
  // Gradient d'un champ scalaire localise aux centres des elements

  // On initialise grad a zero
  grad = 0;
  assert_espace_virtuel_vect(pression);
  Debog::verifier("pression dans Op_Grad_EF",pression);
  const Domaine_EF& domaine_ef=ref_cast(Domaine_EF,equation().domaine_dis().valeur());


  const DoubleTab& Bij_thilde=domaine_ef.Bij_thilde();
  int nb_elem_tot=domaine_ef.domaine().nb_elem_tot();
  int nb_som_elem=domaine_ef.domaine().nb_som_elem();
  const IntTab& elems=domaine_ef.domaine().les_elems() ;

  for (int elem=0; elem<nb_elem_tot; elem++)
    {
      double pe=pression(elem);
      for (int s=0; s<nb_som_elem; s++)
        {
          int som=elems(elem,s);
          for (int i=0; i<dimension; i++)
            {
              grad(som,i)-=Bij_thilde(elem,s,i)*pe;
            }
        }
    }

  const IntTab& faces_sommets=domaine_ef.face_sommets();
  int nb_som_face=faces_sommets.dimension(1);
  // const IntTab& face_voisins = domaine_ef.face_voisins();
  const DoubleTab& face_normales = domaine_ef.face_normales();
  const Domaine_Cl_EF& domaine_Cl_EF = la_zcl_EF.valeur();



  // prise en compte des " cl " ajout du temr - int P sur le bord
  // test ...
  for (int n_bord=0; n_bord<domaine_ef.nb_front_Cl()*1; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_EF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre,la_cl.valeur()) )
        continue;

      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = 0;
      int num2 = + le_bord.nb_faces_tot();
      if (sub_type(Dirichlet,la_cl.valeur()) )
        // ca ne sert a rien
        for (int ind_face=num1; ind_face<num2; ind_face++)
          {
            int face=le_bord.num_face(ind_face);
            for (int s=0; s<nb_som_face; s++)
              {
                int som=faces_sommets(face,s);
                for (int comp=0; comp<dimension; comp++)
                  grad(som,comp) = 0;
              }
          }


      else
        for (int ind_face=num1; ind_face<num2*0; ind_face++)
          {
            int face=le_bord.num_face(ind_face);
            //	  int elem1 = face_voisins(face,0);
            //	  double diff =  - pression[elem1]/nb_som_face;

            for (int s=0; s<nb_som_face; s++)
              {
                int som=faces_sommets(face,s);
                // on calcule grad.S /S2
                double grad_n=0;
                for (int comp=0; comp<dimension; comp++)
                  grad_n+=grad(som,comp)*face_normales(face,comp);
                grad_n/=(domaine_ef.surface(face)*domaine_ef.surface(face));
                for (int comp=0; comp<dimension; comp++)
                  grad(som,comp) -= grad_n*face_normales(face,comp);
              }
          }
    }
  ajouter_bord(grad,domaine_ef,la_zcl_EF.valeur(),int_P_bord_);
  grad.echange_espace_virtuel();
  return grad;
}

DoubleTab& Op_Grad_EF::calculer(const DoubleTab& pre, DoubleTab& grad) const
{
  grad = 0;
  return ajouter(pre,grad);

}

void Op_Grad_EF::calculer_flux_bords() const
{
  const Domaine_Cl_EF& domaine_Cl_EF = la_zcl_EF.valeur();
  const Domaine_EF& domaine_EF = le_dom_EF.valeur();
  const IntTab& face_voisins = domaine_EF.face_voisins();
  const DoubleTab& face_normales = domaine_EF.face_normales();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,equation());
  const Champ_P0_EF& la_pression_P0 = ref_cast(Champ_P0_EF,eqn_hydr.pression_pa().valeur());
  const DoubleTab& pression_P0 = la_pression_P0.valeurs();
  if (flux_bords_.size_array()==0) flux_bords_.resize(domaine_EF.nb_faces_bord(),dimension);
  flux_bords_ = 0.;
  for (int n_bord=0; n_bord<domaine_EF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_EF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      for (int face=ndeb; face<nfin; face++)
        {
          int elem = face_voisins(face,0);
          for (int i=0; i<dimension; i++)
            flux_bords_(face,i) = pression_P0(elem)*face_normales(face,i);
        }
    }
}

int Op_Grad_EF::impr(Sortie& os) const
{
  const int impr_mom=le_dom_EF->domaine().moments_a_imprimer();
  const int impr_sum=(le_dom_EF->domaine().bords_a_imprimer_sum().est_vide() ? 0:1);
  const int impr_bord=(le_dom_EF->domaine().bords_a_imprimer().est_vide() ? 0:1);
  const Schema_Temps_base& sch = equation().probleme().schema_temps();
  double temps = sch.temps_courant();
  const Domaine_Cl_EF& domaine_Cl_EF = la_zcl_EF.valeur();
  const Domaine_EF& domaine_EF = le_dom_EF.valeur();
  int n_bord ;
  int face;
  const ArrOfDouble& c_grav=le_dom_EF->domaine().cg_moments();
  int flag=je_suis_maitre();
  //SFichier Flux_grad;
  if (!Flux_grad.is_open()) ouvrir_fichier(Flux_grad,"",flag);
  //SFichier Flux_grad_moment;
  if (!Flux_grad_moment.is_open()) ouvrir_fichier( Flux_grad_moment,"moment",impr_mom&&flag);
  //SFichier Flux_grad_sum;
  if (!Flux_grad_sum.is_open()) ouvrir_fichier(Flux_grad_sum,"sum",impr_sum&&flag);
  EcrFicPartage Flux_grad_face;
  ouvrir_fichier_partage(Flux_grad_face,"",impr_bord);
  if (je_suis_maitre())
    {
      Flux_grad.add_col(temps);
      if (impr_mom) Flux_grad_moment.add_col(temps);
      if (impr_sum) Flux_grad_sum.add_col(temps);
    }
  for ( n_bord=0; n_bord<domaine_EF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_EF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      double fluxx_s = 0. ;
      double fluxy_s = 0. ;
      double fluxz_s = 0. ;
      double fluxx_sum_s = 0. ;
      double fluxy_sum_s = 0. ;
      double fluxz_sum_s = 0. ;
      double moment_x = 0.;
      double moment_y = 0.;
      double moment_z = 0.;
      for (face=ndeb; face<nfin; face++)
        {
          if (dimension == 2)
            {
              fluxx_s += flux_bords_(face,0) ;
              fluxy_s += flux_bords_(face,1) ;

              // Calcul du moment exerce par le fluide sur le bord (OM/\F)
              if (impr_mom) moment_z+=flux_bords_(face,1)*c_grav[0]-flux_bords_(face,0)*c_grav[1];

              if (domaine_EF.domaine().bords_a_imprimer_sum().contient(le_bord.le_nom()))
                {
                  fluxx_sum_s += flux_bords_(face,0) ;
                  fluxy_sum_s += flux_bords_(face,1) ;
                }
            }
          else if (dimension == 3)
            {
              fluxx_s += flux_bords_(face,0) ;
              fluxy_s += flux_bords_(face,1) ;
              fluxz_s += flux_bords_(face,2) ;

              // Calcul du moment exerce par le fluide sur le bord (OM/\F)
              moment_x+=flux_bords_(face,2)*c_grav[1]-flux_bords_(face,1)*c_grav[2];
              moment_y+=flux_bords_(face,0)*c_grav[2]-flux_bords_(face,2)*c_grav[0];
              moment_z+=flux_bords_(face,1)*c_grav[0]-flux_bords_(face,0)*c_grav[1];
              if (domaine_EF.domaine().bords_a_imprimer_sum().contient(le_bord.le_nom()))
                {
                  fluxx_sum_s += flux_bords_(face,0) ;
                  fluxy_sum_s += flux_bords_(face,1) ;
                  fluxz_sum_s += flux_bords_(face,2) ;
                }
            }
        }
      if (dimension == 2)
        {
          fluxx_s=Process::mp_sum(fluxx_s);
          fluxy_s=Process::mp_sum(fluxy_s);
          fluxx_sum_s=Process::mp_sum(fluxx_sum_s);
          fluxy_sum_s=Process::mp_sum(fluxy_sum_s);
          moment_z=Process::mp_sum(moment_z);
          if (je_suis_maitre())
            {
              Flux_grad.add_col(fluxx_s);
              Flux_grad.add_col(fluxy_s);
              if (impr_sum)
                {
                  Flux_grad_sum.add_col(fluxx_sum_s);
                  Flux_grad_sum.add_col(fluxy_sum_s);
                }
              if (impr_mom) Flux_grad_moment.add_col(moment_z);
            }
        }
      if (dimension == 3)
        {
          fluxx_s=Process::mp_sum(fluxx_s);
          fluxy_s=Process::mp_sum(fluxy_s);
          fluxz_s=Process::mp_sum(fluxz_s);
          fluxx_sum_s=Process::mp_sum(fluxx_sum_s);
          fluxy_sum_s=Process::mp_sum(fluxy_sum_s);
          fluxz_sum_s=Process::mp_sum(fluxz_sum_s);
          moment_x=Process::mp_sum(moment_x);
          moment_y=Process::mp_sum(moment_y);
          moment_z=Process::mp_sum(moment_z);
          if(je_suis_maitre())
            {
              Flux_grad.add_col(fluxx_s);
              Flux_grad.add_col(fluxy_s);
              Flux_grad.add_col(fluxz_s);
              if (impr_sum)
                {
                  Flux_grad_sum.add_col(fluxx_sum_s);
                  Flux_grad_sum.add_col(fluxy_sum_s);
                  Flux_grad_sum.add_col(fluxz_sum_s);
                }
              if (impr_mom)
                {
                  Flux_grad_moment.add_col(moment_x);
                  Flux_grad_moment.add_col(moment_y);
                  Flux_grad_moment.add_col(moment_z);
                }
            }
        }
    }
  if (je_suis_maitre())
    {
      Flux_grad << finl;
      if (impr_mom) Flux_grad_moment<<finl;
      if (impr_sum) Flux_grad_sum<<finl;
    }
  for ( n_bord=0; n_bord<domaine_EF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_EF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      if (domaine_EF.domaine().bords_a_imprimer().contient(le_bord.le_nom()))
        {
          if (je_suis_maitre()) Flux_grad_face << "# Force par face sur " << le_bord.le_nom() << " au temps " << temps << " : " << finl;
          for (face=ndeb; face<nfin; face++)
            {
              if (dimension==2)
                Flux_grad_face << "# Face a x= " << domaine_EF.xv(face,0) << " y= " << domaine_EF.xv(face,1) << " flux_x= " << flux_bords_(face,0) << " flux_y= " << flux_bords_(face,1) << finl;
              else if (dimension == 3)
                Flux_grad_face << "# Face a x= " << domaine_EF.xv(face,0) << " y= " << domaine_EF.xv(face,1) << " z= " << domaine_EF.xv(face,3) << " flux_x= " << flux_bords_(face,0) << " flux_y= " << flux_bords_(face,1) << " flux_z= " << flux_bords_(face,2) << finl;
            }
          Flux_grad_face.syncfile();
        }
    }

  return 1;
}

