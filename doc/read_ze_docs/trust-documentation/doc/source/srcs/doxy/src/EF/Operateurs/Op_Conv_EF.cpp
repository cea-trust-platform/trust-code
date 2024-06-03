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

#include <Schema_Temps_base.h>
#include <Op_Conv_EF.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Op_Conv_EF,"Op_Conv_Generic_EF",Op_Conv_EF_base);

Op_Conv_EF::Op_Conv_EF(): hourglass(0.),hourglass_impl_(0),btd_impl_(0),centre_impl_(0), hourglass_hors_conv_(0),btd_hors_conv_(0),f_lu_(-1.),calcul_dt_stab_(0) { }

Implemente_instanciable_sans_constructeur(Op_Conv_BTD_EF,"Op_Conv_BTD_EF",Op_Conv_EF);
Op_Conv_BTD_EF::Op_Conv_BTD_EF() : Op_Conv_EF()
{
  calcul_dt_stab_=2;
  type_op = amont;
  hourglass=1;
  hourglass_impl_=1;
  btd_impl_=1;
  centre_impl_=1;
  btd_hors_conv_=1;
  hourglass_hors_conv_=1;
}
void Op_Conv_BTD_EF::completer()
{
  // pour ne pas mettre les flags automatiquement
  Op_Conv_EF_base::completer();

}
Sortie& Op_Conv_BTD_EF::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}
Entree& Op_Conv_BTD_EF::readOn(Entree& s )
{
  Param param(que_suis_je()); // XD convection_btd convection_deriv btd 1 Only for EF discretization.
  param.ajouter("btd",&btd_,Param::REQUIRED); // XD_ADD_P double not_set
  param.ajouter("facteur",&facteur_,Param::REQUIRED);  // XD_ADD_P double not_set

  param.lire_avec_accolades(s);
  return s;
}

double Op_Conv_BTD_EF::coefficient_btd() const
{
  if (equation().equation_non_resolue())
    calculer_dt_stab();
  double f=dt_stab_conv()*facteur_*btd_/2.;
  return f;
}

Sortie& Op_Conv_EF::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

Entree& Op_Conv_EF::readOn(Entree& s )
{
  btd_=3.;
  Motcle type_op_lu;
  s >> type_op_lu;

  if (!(type_op_lu=="amont") &&  !(type_op_lu=="centre")  &&  !(type_op_lu=="amont3")&&!(type_op_lu=="btd"))
    {
      Cerr << type_op_lu << " n'est pas compris par " << que_suis_je() << finl;
      Cerr << " choisir parmi : amont -btd - centre " << finl;
      exit();
    }


  if (type_op_lu=="centre")
    {
      type_op = centre;

    }

  if ((type_op_lu=="amont")||(type_op_lu=="amont3")||(type_op_lu=="btd"))
    {
      type_op = amont;
    }


  if ((type_op==amont)
      &&  (type_op_lu!="amont2"))
    {
      f_lu_=0.036585;
      hourglass=1;
      hourglass_impl_=1;
      btd_impl_=1;
      centre_impl_=1;
    }
  if (type_op_lu=="amont3")
    {
      s >> f_lu_;
      // decommente  les 2 lognes
      // centre_impl_=0;
      //	calcul_dt_stab_=1;
    }
  if  (type_op_lu=="btd")
    f_lu_=-1;


  return s ;
}
void Op_Conv_EF::completer()
{
  Op_Conv_EF_base::completer();
  if (sub_type(Op_Diff_EF,equation().operateur(0).l_op_base()))
    {
      if (equation().schema_temps().diffusion_implicite())
        {
          btd_hors_conv_=1;
          hourglass_hors_conv_=1;
          calcul_dt_stab_=2;
        }
    }
  else
    {
      btd_hors_conv_=0;
      hourglass_hors_conv_=0;
    }

}

DoubleTab& Op_Conv_EF::ajouter(const DoubleTab& transporte, DoubleTab& resu) const
{
  return ajouter_sous_cond(transporte, resu, btd_hors_conv_, hourglass_hors_conv_, 0);
}

DoubleTab& Op_Conv_EF::ajouter_a_la_diffusion(const DoubleTab& transporte, DoubleTab& resu) const
{
  return ajouter_sous_cond(transporte, resu, !btd_hors_conv_, !hourglass_hors_conv_, 1);
}

double Op_Conv_EF::coefficient_btd() const
{
  double dt=equation().schema_temps().pas_de_temps();
  double f=(dimension)*dt/(dimension-1);
  if (f_lu_>=0) f=f_lu_;
  return f;
}

DoubleTab& Op_Conv_EF::ajouter_sous_cond_gen(const DoubleTab& transporte, DoubleTab& resu, int btd_impl, int hourglass_impl, int centre_impl) const
{
  return ajouter_sous_cond_template<AJOUTE_COND::GEN>(transporte,resu,btd_impl,hourglass_impl,centre_impl);
}

DoubleTab& Op_Conv_EF::ajouter_sous_cond_dim3_nbn8_nbdim2(const DoubleTab& transporte, DoubleTab& resu, int btd_impl, int hourglass_impl, int centre_impl) const
{
  return ajouter_sous_cond_template<AJOUTE_COND::D3_82>(transporte,resu,btd_impl,hourglass_impl,centre_impl);
}

DoubleTab& Op_Conv_EF::ajouter_sous_cond_dim3_nbn8_nbdim1(const DoubleTab& transporte, DoubleTab& resu, int btd_impl, int hourglass_impl, int centre_impl) const
{
  return ajouter_sous_cond_template<AJOUTE_COND::D3_81>(transporte,resu,btd_impl,hourglass_impl,centre_impl);
}

DoubleTab& Op_Conv_EF::ajouter_sous_cond_dim2_nbn4_nbdim2(const DoubleTab& transporte, DoubleTab& resu, int btd_impl, int hourglass_impl, int centre_impl) const
{
  return ajouter_sous_cond_template<AJOUTE_COND::D2_42>(transporte,resu,btd_impl,hourglass_impl,centre_impl);
}

DoubleTab& Op_Conv_EF::ajouter_sous_cond_dim2_nbn4_nbdim1(const DoubleTab& transporte, DoubleTab& resu, int btd_impl, int hourglass_impl, int centre_impl) const
{
  return ajouter_sous_cond_template<AJOUTE_COND::D2_41>(transporte,resu,btd_impl,hourglass_impl,centre_impl);
}

DoubleTab& Op_Conv_EF::ajouter_sous_cond(const DoubleTab& transporte, DoubleTab& resu, int btd_impl, int hourglass_impl, int centre_impl) const
{
  const Domaine_EF& domaine_ef = ref_cast(Domaine_EF, equation().domaine_dis().valeur());
  int nb_som_elem = domaine_ef.domaine().nb_som_elem();
  int nb_compo = transporte.line_size();

  if ((dimension == 3) && (nb_som_elem == 8))
    {
      if (nb_compo == 1)
        return ajouter_sous_cond_dim3_nbn8_nbdim1(transporte, resu, btd_impl, hourglass_impl, centre_impl);
      else
        return ajouter_sous_cond_dim3_nbn8_nbdim2(transporte, resu, btd_impl, hourglass_impl, centre_impl);
    }
  return ajouter_sous_cond_gen(transporte, resu, btd_impl, hourglass_impl, centre_impl);
}

void Op_Conv_EF::ajouter_contribution_sous_cond(const DoubleTab& transporte, Matrice_Morse& matrice,int btd_impl,int hourglass_impl,int centre_impl ) const
{

  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const DoubleTab& G=la_vitesse.valeurs();

  int transport_rhou=0;
  if (vitesse_->le_nom()=="rho_u") transport_rhou=1;

  const DoubleTab& rho_elem=(transport_rhou==1 ? equation().probleme().get_champ("masse_volumique_melange").valeurs() : \
                             equation().probleme().get_champ("masse_volumique").valeurs());
  int is_not_rho_unif = (rho_elem.size() == 1 ? 0 : 1);

  const Domaine_EF& domaine_ef=ref_cast(Domaine_EF,equation().domaine_dis().valeur());
  int nb_comp = transporte.line_size();
  const DoubleVect& volumes_thilde= domaine_ef.volumes_thilde();
  const DoubleVect& volumes= domaine_ef.volumes();
  const DoubleTab& IPhi_thilde=domaine_ef.IPhi_thilde();

  const DoubleTab& bij=domaine_ef.Bij();
  int nb_elem_tot=domaine_ef.domaine().nb_elem_tot();
  int nb_som_elem=domaine_ef.domaine().nb_som_elem();
  const IntTab& elems=domaine_ef.domaine().les_elems() ;
  ArrOfDouble G_e(dimension);

  double f=coefficient_btd();
  //  DoubleTab transp_loc(nb_som_elem,nb_comp);
  // A DEPLACER !!!!!
  const DoubleTab& lambda=ref_cast(Operateur_Diff_base,equation().operateur(0).l_op_base()).diffusivite().valeurs();
  int is_not_lambda_unif=1;
  if (lambda.size()==1)
    is_not_lambda_unif=0;

  int mcoef3d[8]= {1,-1,-1,1,-1,1,1,-1};
  int sommetoppose[8]= {7,6,5,4,3,2,1,0};
  for (int elem=0; elem<nb_elem_tot; elem++)
    if (elem_contribue(elem))
      {
        G_e=0;
        for (int i1=0; i1<nb_som_elem; i1++)
          {
            int glob=elems(elem,i1);
            for (int b=0; b<dimension; b++)
              G_e[b]+=G(glob,b);
          }
        G_e/=nb_som_elem;

        double pond2=volumes_thilde(elem)/volumes(elem)/volumes(elem);

        if (transport_rhou) pond2 /= (is_not_rho_unif ? rho_elem(elem) : rho_elem(0,0));

        if ((hourglass)&&(nb_som_elem==8)&&(hourglass_impl==1))
          {
            double pond3=f*dotproduct_array(G_e,G_e);
            if (transport_rhou) pond3 /= (is_not_rho_unif ? rho_elem(elem) : rho_elem(0,0));
            if (is_not_lambda_unif)
              pond3+=lambda(elem);
            else
              pond3+=lambda(0,0);
            pond3*=volumes_thilde(elem)/volumes(elem)*pow(volumes(elem),0.3333333333333333);
            pond3*=hourglass;

            for (int a=0; a<nb_comp; a++)
              {
                double coef2d=0.042*pond3;
                double coef3d=coef2d*0.5;

                for (int i1=0; i1<8; i1++)
                  {
                    int n1=elems(elem,i1)*nb_comp+a;
                    for (int i2=0; i2<8; i2++)
                      {
                        int n2=elems(elem,i2)*nb_comp+a;
                        matrice.coef(n1,n2)+=mcoef3d[i1]*mcoef3d[i2]*coef3d-coef2d;
                      }
                    matrice.coef(n1,n1)+=coef2d*4.;
                    matrice.coef(n1,elems(elem,sommetoppose[i1])*nb_comp+a)+=coef2d*4.;
                  }
              }
          }

        for (int i1=0; i1<nb_som_elem; i1++)
          {
            int glob=elems(elem,i1);

            if (((btd_impl==1)&&(type_op==amont))||(centre_impl==1))
              {
                double cb=0;
                for (int b=0; b<dimension; b++)
                  cb+=G_e[b]*bij(elem,i1,b);
                cb*=(f*pond2);
                for (int i2=0; i2<nb_som_elem; i2++)
                  {
                    int glob2=elems(elem,i2);

                    double ca=0;
                    if (centre_impl==1)
                      {
                        for (int b=0; b<dimension; b++)
                          ca+=G_e[b]*bij(elem,i2,b);
                        double pond=IPhi_thilde(elem,i1)/volumes(elem);

                        ca*=pond;
                      }
                    double cc=0;
                    if ((btd_impl==1)&&(type_op==amont))
                      {
                        for (int c=0; c<dimension; c++)
                          cc+=G_e[c]*bij(elem,i2,c);
                        cc*=cb;
                      }
                    //resu(glob)-=cc*cb*transp_loc(i2,0);
                    for (int a=0; a<nb_comp; a++)
                      {
                        int n1=glob*nb_comp+a;
                        int n2=glob2*nb_comp+a;
                        matrice.coef(n1,n2)+=cc+ca;

                      }
                  }
              }
          }
      }
  return;
}

void Op_Conv_EF::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const
{
  return ajouter_contribution_sous_cond(transporte,matrice,(btd_impl_)&&(btd_hors_conv_==0),(hourglass_impl_)&&(hourglass_hors_conv_==0),centre_impl_);
}

void Op_Conv_EF::ajouter_contribution_a_la_diffusion(const DoubleTab& transporte, Matrice_Morse& matrice ) const
{
  ajouter_contribution_sous_cond(transporte,matrice,(btd_impl_)&&(btd_hors_conv_==1),(hourglass_impl_)&&(hourglass_hors_conv_==1),0);
}

double Op_Conv_EF::calculer_dt_stab() const
{
  if (calcul_dt_stab_==0) return  Operateur_base::calculer_dt_stab();

  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const DoubleTab& G=la_vitesse.valeurs();

  int transport_rhou=0;
  if (vitesse_->le_nom()=="rho_u") transport_rhou=1;

  const DoubleTab& rho_elem=(transport_rhou==1 ? equation().probleme().get_champ("masse_volumique_melange").valeurs() : \
                             equation().probleme().get_champ("masse_volumique").valeurs());
  int is_not_rho_unif = (rho_elem.size() == 1 ? 0 : 1);

  const Domaine_EF& domaine_ef=ref_cast(Domaine_EF,equation().domaine_dis().valeur());
  const DoubleVect& valeurs_diffusivite = ref_cast(Operateur_Diff_base,equation().operateur(0).l_op_base()).diffusivite().valeurs();
  int is_not_lambda_unif = (valeurs_diffusivite.size() == 1 ? 0 : 1);

  int autre_eq=0;
  const DoubleVect& valeurs_diffusivite_p = ref_cast(Operateur_Diff_base,equation().probleme().equation(autre_eq).operateur(0).l_op_base()).diffusivite().valeurs();
  if (calcul_dt_stab_==2)
    {
      if (&valeurs_diffusivite_p == &valeurs_diffusivite)
        autre_eq=1;
    }
  const DoubleVect& valeurs_diffusivite_2 = ref_cast(Operateur_Diff_base,equation().probleme().equation(autre_eq).operateur(0).l_op_base()).diffusivite().valeurs();

  const DoubleTab& coord=domaine_ef.domaine().les_sommets();

  int nb_elem_tot=domaine_ef.domaine().nb_elem_tot();
  int nb_som_elem=domaine_ef.domaine().nb_som_elem();
  const IntTab& elems=domaine_ef.domaine().les_elems() ;
  ArrOfDouble G_e(dimension);
  double Max=0;
  double dt_l=0,dt2=0;
  for (int elem=0; elem<nb_elem_tot; elem++)
    if (elem_contribue(elem))
      {
        G_e=0;
        for (int i1=0; i1<nb_som_elem; i1++)
          {
            int glob=elems(elem,i1);
            for (int b=0; b<dimension; b++)
              G_e[b]+=G(glob,b);
          }
        G_e/=nb_som_elem;
        //if (transport_rhou) 	G_e/=rho_elem(elem);
        double ml=0;
        double dx2;
        // pas de temps de diffusion !!
        double ml2=0;
        double vx[3];
        for (int d=0; d<3; d++)
          {
            dx2=std::fabs(coord(elems(elem,0),d)-coord(elems(elem,7),d));
            dx2+=std::fabs(coord(elems(elem,1),d)-coord(elems(elem,6),d));
            dx2+=std::fabs(coord(elems(elem,2),d)-coord(elems(elem,5),d));
            dx2+=std::fabs(coord(elems(elem,3),d)-coord(elems(elem,4),d));
            dx2*=0.25;
            dx2*=dx2;

            ml2+=dx2;
            //double p=G_e(d)*G_e(d)/dx2;
            ml+=G_e[d]*G_e[d]/dx2;
            vx[d]=G_e[d]*G_e[d];
          }

        double diffu=(is_not_lambda_unif?valeurs_diffusivite(elem):valeurs_diffusivite(0));

        if (calcul_dt_stab_==2)
          {
            double diffu2=(is_not_lambda_unif?valeurs_diffusivite_2(elem):valeurs_diffusivite_2(0));
            if (diffu2<diffu)
              diffu=diffu2;
          }
        diffu /= (is_not_rho_unif ? rho_elem(elem) : rho_elem(0,0));

        // horreur pour NR G2 cas clotaire
        // diffu*=0.5;
        //if (elem==0) Cerr<<"uu "<<diffu<<finl;
        if (transport_rhou)
          {
            double rho_e = (is_not_rho_unif ? rho_elem(elem) : rho_elem(0,0));
            double inv_rho2= rho_e*rho_e;
            inv_rho2=1./inv_rho2;
            ml*=inv_rho2;
            for (int d=0; d<dimension; d++)
              vx[d]*=inv_rho2;
          }
        // le .5* c'est pour avoir le meme dt que g2 mais faux
        double dxe=sqrt(ml2);
        if (dxe<min_dx_)
          min_dx_=dxe;
        ml2=diffu/ml2;
        ml*=btd_;
        ml=sqrt(ml2*ml2+ml)+ml2+DMINFLOAT;
        /*      if (ml2>0)
        ml=ml/(sqrt(ml2*ml2+ml)-ml2);
        else ml=-1.;
        */
        if (ml > Max) Max=ml;
        double ue=sqrt(vx[0]+vx[1]+vx[2]);
        if (ue>max_ue_) max_ue_=ue;

        dt_l= 1./(Max+DMINFLOAT);

        if (elem==0) dt2=dt_l;
        if (btd_!=3.)
          {
            double dt0=dt2;
            double s=1.;
            while(s>0)
              {
                s=-1;
                for (int d=0; d<dimension; d++)
                  {
                    double c=vx[d]*dt0;
                    if (std::fabs(c)>1e-16)
                      s+=c/(2.*diffu  +c*btd_);
                  }
                if (s>0)
                  {
                    dt0*=0.9;
                    Cout<<elem<<" " <<dt_l<<" "<<s<<" "<<dt0<<finl;
                    exit();
                  }
              }
            dt2=std::min(dt2,dt0);
            dt_l=std::min(dt2,dt_l);
          }
      }
  min_dx_=mp_min(min_dx_);
  max_ue_=mp_max(max_ue_);
  coefficient_correcteur_supg_.dimensionner(1,1);
  coefficient_correcteur_supg_(0,0)=min_dx_/max_ue_;
  double dt_stab=dt_l;
  dt_stab = Process::mp_min(dt_stab);
  // astuce pour contourner le type const de la methode
  Op_Conv_EF_base& op = ref_cast_non_const(Op_Conv_EF_base,*this);
  op.fixer_dt_stab_conv(dt_stab);

  return dt_stab;
}

void Op_Conv_EF::contribue_au_second_membre(DoubleTab& resu ) const
{
  // on a rien implicite
  ajouter_sous_cond(equation().inconnue().valeurs(),resu,((btd_hors_conv_==1)||btd_impl_),((hourglass_hors_conv_==1)||hourglass_impl_),centre_impl_);
}

void Op_Conv_EF::contribue_au_second_membre_a_la_diffusion(DoubleTab& resu ) const
{
  ajouter_sous_cond(equation().inconnue().valeurs(),resu,((btd_hors_conv_==0)||btd_impl_),((hourglass_hors_conv_==0)||hourglass_impl_),1);
}

const Champ_base& Op_Conv_EF::get_champ(const Motcle& nom) const
{
  if (nom=="coefficient_correcteur_supg")
    return coefficient_correcteur_supg_;
  return Op_Conv_EF_base::get_champ(nom);
}
