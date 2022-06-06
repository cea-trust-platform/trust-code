/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Champ_front_contact_VEF.h>
#include <Probleme_base.h>
#include <Mod_turb_hyd_base.h>
#include <Modele_turbulence_scal_base.h>
#include <Raccord_distant_homogene.h>
#include <Champ_Uniforme.h>
#include <distances_VEF.h>
#include <Interprete.h>
#include <Pb_Conduction.h>
#include <Zone_Cl_VEF.h>
#include <Domaine.h>
#include <Champ_Fonc_P0_VEF.h>
#include <Format_Post_Med.h>
#include <Champ_front_contact_fictif_VEF.h>
#include <Schema_Comm.h>
#include <ArrOfBit.h>
#include <SFichier.h>
#include <Ref_Objet_U.h>
#include <Constituant.h>
#include <Convection_Diffusion_Concentration.h>


// WEC : Attention. D'autres tableaux que le champ_front meriteraient davoir plusieurs valeurs temporelles !
// En particulier les gradients...

Implemente_instanciable(Champ_front_contact_VEF,"Champ_front_contact_VEF",Ch_front_var_instationnaire_dep);


Sortie& Champ_front_contact_VEF::printOn(Sortie& os) const
{
  return os;
}


Entree& Champ_front_contact_VEF::readOn(Entree& is)
{

  Motcle nom_inco;
  nom_inco = "temperature";

  is >> nom_pb1 >> nom_bord1 >> nom_pb2 >> nom_bord2 ;

  creer(nom_pb1, nom_bord1, nom_pb2, nom_bord2, nom_inco);

  Cerr << "Champ_front_contact_VEF::readOn : " << nom_pb1 << nom_bord1 << nom_inco << finl;
  Cerr << "      connecte a                : " << nom_pb2 << nom_bord2 << nom_inco << finl;
  fixer_nb_comp(1); // prevu pour la temperature.
  if (Process::nproc() > 1)
    connect_est_remplit = 1;
  else
    connect_est_remplit = 0;
  remplir_elems_voisin_bord();

  return is;
}

void Champ_front_contact_VEF::completer()
{
  // Check/initialize Raccord boundaries in parallel:
  if (Process::nproc()>1)
    {
      nom_bord = nom_bord2;
      associer_ch_inc_base(l_inconnue2.valeur()) ;
      const Zone_dis_base& zone_dis_opposee = front_dis().zone_dis();
      const Zone_dis_base& zone_dis_locale = frontiere_dis().zone_dis();
      const Frontiere& frontiere_opposee = front_dis().frontiere();
      const Frontiere& frontiere_locale = frontiere_dis().frontiere();
      if (!sub_type(Raccord_distant_homogene, frontiere_opposee))
        {
          const Nom& nom_domaine_oppose = zone_dis_opposee.zone().domaine().le_nom();
          Cerr << "Error, the boundary " << frontiere_opposee.le_nom() << " should be a Raccord." << finl;
          Cerr << "Add in your data file between the definition and the partition of the domain " << nom_domaine_oppose << " :" << finl;
          Cerr << "Modif_bord_to_raccord " << nom_domaine_oppose << " " << frontiere_opposee.le_nom() << finl;
          exit();
        }
      Raccord_distant_homogene& raccord_distant = ref_cast_non_const(Raccord_distant_homogene, frontiere_opposee);
      raccord_distant.initialise(frontiere_locale, zone_dis_locale, zone_dis_opposee);
    }
}

int Champ_front_contact_VEF::initialiser(double temps, const Champ_Inc_base& inco)
{
  if (!Ch_front_var_instationnaire_dep::initialiser(temps,inco))
    return 0;

  // WEC : code repris de calculer_coeffs_echange qui faisait des
  // iniialisations au premier pas de temps.

  // remplissage du tableau de connectivite des numeros de faces de bord
  // entre les deux problemes couples.
  if (connect_est_remplit==0)
    {
      remplir_connect_bords();
      connect_est_remplit = 1;
      //Les elems_voisin_bord_ ont ete remplis pour chaque frontiere portant un Champ_Front_contact_VEF
      //On peut remplir faces_coin
      remplir_faces_coin();
    }
  verification_faces_coin=0;

  // On initialise les references au ch_fr_autre_pb et fr_vf_autre_pb
  associer_front_vf_et_ch_front_autre_pb();

  // Dimensionnement des tableaux
  int nb_faces=frontiere_dis().frontiere().nb_faces();
  gradient_num_transf.resize(nb_faces);
  gradient_fro_transf.resize(nb_faces);
  Scal_moy.resize(nb_faces);
  gradient_num_local.resize(nb_faces);
  gradient_fro_local.resize(nb_faces);
  coeff_amort_num.resize(nb_faces);
  coeff_amort_denum.resize(nb_faces);

  // Est-on dans le probleme conduction ?
  // (utilise en rayo trans et semi_transp => factorisation)
  const Probleme_base& pb = l_inconnue1->equation().probleme();
  if (sub_type(Pb_Conduction,pb))
    is_conduction=1;
  else
    is_conduction=0;

  // Initialisation des tableaux de gradients
  calculer_coeffs_echange(temps); // sur instant present
  return 1;
}

void Champ_front_contact_VEF::creer(const Nom& nompb1, const Nom& nom1,
                                    const Nom& nompb2, const Nom& nom2,
                                    const Motcle& nom_inco)
{
  nom_bord1 = nom1;
  nom_bord2 = nom2;
  Nom nom_inco1, nom_inco2;
  nom_inco1 = nom_inco;
  nom_inco2 = nom_inco;

  REF(Champ_base) rch1;
  Probleme_base& pb1=ref_cast(Probleme_base, Interprete::objet(nompb1));

  if (!sub_type(Pb_Conduction,pb1))
    if (pb1.equation(1).inconnue().le_nom()!="temperature")
      nom_inco1 = "concentration";

  rch1 = pb1.get_champ(nom_inco1);
  l_inconnue1=ref_cast(Champ_Inc_base, rch1.valeur()) ;

  REF(Champ_base) rch2 ;
  Probleme_base& pb2=ref_cast(Probleme_base, Interprete::objet(nompb2));

  if (!sub_type(Pb_Conduction,pb2))
    if (pb2.equation(1).inconnue().le_nom()!="temperature")
      nom_inco2 = "concentration";

  rch2 = pb2.get_champ(nom_inco2);
  l_inconnue2=ref_cast(Champ_Inc_base, rch2.valeur()) ;

  if (nom_inco1!=nom_inco2)
    {
      Cerr<<"Une condition limite de contact est mal specifiee"<<finl;
      Cerr<<"Il existe une incoherence sur l'inconnue mise en jeu de part et d'autre de la frontiere de contact"<<finl;
      exit();
    }

}


void Champ_front_contact_VEF::associer_ch_inc_base(const Champ_Inc_base& inco)
{
  l_inconnue = inco;
}



Champ_front_base& Champ_front_contact_VEF::affecter_(const Champ_front_base& ch)
{
  return *this;
}


void Champ_front_contact_VEF::calcul_grads_transf(double temps)
{
  // Dans le cas de raccords homogenes, c'est a dire parfaitement coincidents
  // au niveau du maillage, gradient_num_local = gradient_num_transf et
  // gradient_fro_local = gradient_fro_transf

  gradient_num_transf.copy_array(gradient_num_local);
  gradient_fro_transf.copy_array(gradient_fro_local);
}

void Champ_front_contact_VEF::calcul_grads_locaux(double temps)
{
  // collection information problem local
  nom_bord = nom_bord1 ;
  associer_ch_inc_base(l_inconnue1.valeur()) ;
  const Champ_Inc_base& inco = l_inconnue.valeur();
  const DoubleTab& inco_valeurs = l_inconnue->valeurs(temps);
  //const Equation_base& eq=l_inconnue->equation();
  assert (nom_bord != "??") ;
  const Zone_VEF& la_zone_dis=ref_cast(Zone_VEF, zone_dis());
  const Front_VF& la_front_vf=ref_cast(Front_VF, front_dis());
  const DoubleVect& vol = la_zone_dis.volumes();
  const IntTab& face_voisins = la_zone_dis.face_voisins();
  const IntTab& elem_faces = la_zone_dis.elem_faces();
  const DoubleTab& face_normales = la_zone_dis.face_normales();
  int ndeb = la_front_vf.num_premiere_face();
  const Milieu_base& le_milieu = milieu();
  DoubleTab coeff_lam, coeff_turb;

  // On test si le probleme local est un probleme thermohydraulique turbulent
  // Et si le modele de turbulence utilise necessite l'utilisation d'une loi de paroi
  int ind_loi_paroi=0;
  DoubleVect d_equiv;
  //DoubleTab positions_Pf;

  if (!sub_type(Convection_Diffusion_Concentration,inco.equation()))
    coeff_lam = le_milieu.conductivite().valeurs();
  else
    coeff_lam = ref_cast(Constituant,le_milieu).diffusivite_constituant().valeurs();
  int dim = coeff_lam.nb_dim();

  const RefObjU& mod = inco.equation().get_modele(TURBULENCE);
  if (mod.non_nul())
    {
      const Modele_turbulence_scal_base& mod_turb_scal = ref_cast(Modele_turbulence_scal_base,mod.valeur());
      coeff_turb = mod_turb_scal.conductivite_turbulente().valeurs();

      const Turbulence_paroi_scal& loipar = mod_turb_scal.loi_paroi();

      if (loipar.non_nul())
        {
          if( loipar.valeur().use_equivalent_distance() )
            {
              ind_loi_paroi=1;
              // const Paroi_scal_hyd_base_VEF& paroi_vef = ref_cast(Paroi_scal_hyd_base_VEF,loipar.valeur());
              int nb_faces = la_front_vf.nb_faces();
              d_equiv.resize(nb_faces);
              // Recherche du bord correspondant
              int i_bord=-1;
              for (int n_bord=0; n_bord<la_zone_dis.zone().nb_front_Cl(); n_bord++)
                if (la_zone_dis.front_VF(n_bord).le_nom() == la_front_vf.le_nom()) i_bord=n_bord;
              // Copie des donnees d'un bord dans les tableaux locaux temporaires positions_Pf et d_equiv
              for (int ind_face=0; ind_face<nb_faces; ind_face++)
                {
                  // d_equiv(ind_face) = paroi_vef.equivalent_distance(i_bord,ind_face);
                  d_equiv(ind_face) = loipar.valeur().equivalent_distance(i_bord,ind_face);
                }
            }
        }

    }

  int i, j, num, fac_glob ;
  int fac_front, fac_loc;
  double coeff;
  int signe;
  double surface_pond, surface_face;
  int nb_faces_elem = la_zone_dis.zone().nb_faces_elem();
  const DoubleTab& face_normale = la_zone_dis.face_normales();

  // calcul de grad_num_local et grad_fro_local
  int nb_faces = la_front_vf.nb_faces();
  for ( fac_front = 0 ; fac_front<nb_faces ; fac_front++)
    {
      gradient_num_local(fac_front) = 0.;
      gradient_fro_local(fac_front) = 0.;

      fac_glob = fac_front +  ndeb ;
      num = face_voisins(fac_glob,0);
      if (num < 0 ) num = face_voisins(fac_glob,1);

      if(dim!=1) coeff = coeff_lam(0,0) ;
      else       coeff = coeff_lam(num) ;

      //GF PQ
      // ajout de lambda_t cote fluide egalite des flux entre lambda_s *DT_s/dy =(lambda+lambda_t)_f *DT_f/dy

      if (mod.non_nul())
        coeff += coeff_turb(num);

      Scal_moy(fac_front) = 0. ;
      double ratio = 0. ;
      for (i=0; i<dimension; i++) ratio += (face_normales(fac_glob,i) * face_normales(fac_glob,i));
      ratio = sqrt(ratio) ;
      int fac;
      surface_face=la_zone_dis.face_surfaces(fac_glob);

      //Calcul de la temperature moyenne dans la maille
      for (i=0; i<nb_faces_elem; i++)
        {
          // On ne tient pas compte de la temperature de paroi
          // pour calculer la temperature moyenne dans la maille.
          if ( (j= elem_faces(num,i)) != fac_glob )
            {
              //On pondere la moyenne par la surface
              surface_pond = 0.;
              for (int kk=0; kk<dimension; kk++)
                surface_pond -= (face_normale(j,kk)*la_zone_dis.oriente_normale(j,num)*face_normale(fac_glob,kk)*
                                 la_zone_dis.oriente_normale(fac_glob,num))/(surface_face*surface_face);
              Scal_moy(fac_front) += inco_valeurs(j)*surface_pond;
            }
        }


      for (fac=0; fac<dimension+1; fac++)
        for (i=0; i<dimension; i++)
          {
            fac_loc = elem_faces(num,fac) ;
            signe = la_zone_dis.oriente_normale(fac_loc, num);
            if (fac_loc != fac_glob) gradient_num_local(fac_front) += (signe * face_normales(fac_glob,i) / ratio *
                                                                         face_normales(fac_loc,i)*inco_valeurs(fac_loc)) ;
            else                 gradient_fro_local(fac_front) += (signe * face_normales(fac_loc,i) / ratio *
                                                                     face_normales(fac_loc,i)) ;
          }

      gradient_num_local(fac_front) *= (coeff / vol(num));
      gradient_fro_local(fac_front) *= (coeff / vol(num));

      // On recalcul gradient_num_local et gradient_fro_local si l'ecoulement dans le domaine local est turbulent
      if (ind_loi_paroi==1)
        {
          gradient_num_local(fac_front) = 0.;
          gradient_fro_local(fac_front) = 0.;

          gradient_num_local(fac_front) = -Scal_moy(fac_front);
          double coeff_equiv = coeff/d_equiv(fac_front);
          gradient_num_local(fac_front) *=coeff_equiv;
          gradient_fro_local(fac_front) = coeff_equiv;
        }
    }
}

void Champ_front_contact_VEF::mettre_a_jour(double temps )
{
  if (!verification_faces_coin)
    {
      test_faces_coin();
      verification_faces_coin=1;
    }

  const Frontiere& la_front=la_frontiere_dis->frontiere();
  int nb_faces=la_front.nb_faces();
  DoubleTab& tab=valeurs_au_temps(temps);

  // On recupere les coefficients gradient_num_transf et gradient_fro_transf de l'autre probleme
  DoubleVect gradient_num_transf_autre_pb(nb_faces);
  DoubleVect gradient_fro_transf_autre_pb(nb_faces);
  if (!ch_fr_autre_pb.non_nul())
    {
      Cerr << "Attention: Vous utilisez une condition de contact Champ_front_contact_VEF sur le bord " << nom_bord1 << " sur le probleme "<< nom_pb1 <<" " << finl;
      Cerr << "Vous devez avoir un Champ_front_contact_VEF equivalent sur le bord " << nom_bord2 << " du probleme "<<nom_pb2<<" " << finl;
      exit();
    }
  trace_face_raccord(fr_vf_autre_pb.valeur(),ch_fr_autre_pb->gradient_num_transf,gradient_num_transf_autre_pb);
  trace_face_raccord(fr_vf_autre_pb.valeur(),ch_fr_autre_pb->gradient_fro_transf,gradient_fro_transf_autre_pb);
  //
  //
  //  gradient_fro_ h
  //  gradient_num_ -h*T
  // Calcul de la temperature imposee
  for (int fac_front=0; fac_front<nb_faces; fac_front++)
    {
      tab(fac_front,0)= -(gradient_num_local(fac_front) + gradient_num_transf_autre_pb(fac_front))
                        / ( gradient_fro_transf_autre_pb(fac_front) + gradient_fro_local(fac_front)) ;
    }
  // Verification de la coherence des temperatures de bord calculees
  //verifier_scalaire_bord();
}


void Champ_front_contact_VEF::verifier_scalaire_bord(double temps)
{
  // Cette methode a pour but de verifier que la temperature de bord calculee pour chaque
  // face num_face est bien comprise entre les temperatures moyennes sur les elements entourant
  // num_face dans chaque milieu.
  const Frontiere& la_front=la_frontiere_dis->frontiere();
  int nb_faces=la_front.nb_faces();

  DoubleVect Scal_moy_autre_pb(nb_faces);
  trace_face_raccord(fr_vf_autre_pb.valeur(),ch_fr_autre_pb->Scal_moy,Scal_moy_autre_pb);

  DoubleTab& tab=valeurs_au_temps(temps);

  // Verification sur les temperatures
  for (int fac_front=0; fac_front<nb_faces; fac_front++)
    {
      if (Scal_moy(fac_front) > Scal_moy_autre_pb(fac_front))
        {
          if ((tab(fac_front,0)>Scal_moy(fac_front)) || (tab(fac_front,0)<Scal_moy_autre_pb(fac_front)))
            {
              if (tab(fac_front,0)>Scal_moy(fac_front))
                {
                  Cerr<<"Attention, Tp n'est pas compris entre les temperatures"<<finl;
                  Cerr<<"fluide et solide au voisinage de la paroi"<<finl;
                  Cerr<<"Tp("<<fac_front<<") = "<<tab(fac_front,0)<<" est trop elevee"<<finl;
                  Cerr<<"de DT = "<<tab(fac_front,0)-Scal_moy(fac_front)<<" K"<<finl;
                }
              else
                {
                  Cerr<<"Attention, Tp n'est pas compris entre les temperatures"<<finl;
                  Cerr<<"fluide et solide au voisinage de la paroi"<<finl;
                  Cerr<<"Tp("<<fac_front<<") = "<<tab(fac_front,0)<<" est trop basse "<<finl;
                  Cerr<<"de DT = "<<Scal_moy_autre_pb(fac_front)-tab(fac_front,0)<<" K"<<finl;
                }
            }
        }
      else
        {
          if ((tab(fac_front,0)<Scal_moy(fac_front)) || (tab(fac_front,0)>Scal_moy_autre_pb(fac_front)))
            {
              if (tab(fac_front,0)<Scal_moy(fac_front))
                {
                  Cerr<<"Attention, Tp n'est pas compris entre les temperatures"<<finl;
                  Cerr<<"fluide et solide au voisinage de la paroi"<<finl;
                  Cerr<<"Tp("<<fac_front<<") = "<<tab(fac_front,0)<<" est trop basse"<<finl;
                  Cerr<<"de DT = "<<Scal_moy(fac_front)-tab(fac_front,0)<<" K"<<finl;
                }
              else
                {
                  Cerr<<"Attention, Tp n'est pas compris entre les temperatures"<<finl;
                  Cerr<<"fluide et solide au voisinage de la paroi"<<finl;
                  Cerr<<"Tp("<<fac_front<<") = "<<tab(fac_front,0)<<" est trop elevee"<<finl;
                  Cerr<<"de DT = "<<tab(fac_front,0)-Scal_moy_autre_pb(fac_front)<<" K"<<finl;
                }
            }
        }
    }
}


void Champ_front_contact_VEF::calculer_coeffs_echange(double temps)
{
  // Calcul de la temperature imposee
  calcul_grads_locaux(temps);
  calcul_grads_transf(temps);
}


const Champ_Inc_base& Champ_front_contact_VEF::inconnue() const
{
  return l_inconnue.valeur();
}


const Nom& Champ_front_contact_VEF::nom_bord_oppose() const
{
  return nom_bord;
}


const Equation_base& Champ_front_contact_VEF::equation() const
{
  if (l_inconnue.non_nul()==0)
    {
      Cerr << "\nError in Champ_front_contact_VEF::equation() : not able to return the equation !" << finl;
      Process::exit();
    }
  return inconnue().equation();
}


const Milieu_base& Champ_front_contact_VEF::milieu() const
{
  return equation().milieu();
}


const Zone_dis_base& Champ_front_contact_VEF::zone_dis() const
{
  return inconnue().zone_dis_base();
}


const Zone_Cl_dis_base& Champ_front_contact_VEF::zone_Cl_dis() const
{
  return equation().zone_Cl_dis().valeur();
}


const Frontiere_dis_base& Champ_front_contact_VEF::front_dis() const
{
  return zone_dis().frontiere_dis(nom_bord);
}


void Champ_front_contact_VEF::calcul_coeff_amort()
{
  const Zone_VEF& la_zone_dis=ref_cast(Zone_VEF, l_inconnue1->equation().zone_dis().valeur());
  int rang_front = la_zone_dis.rang_frontiere(nom_bord1);
  const Front_VF& la_front_vf=ref_cast(Front_VF, la_zone_dis.frontiere_dis(rang_front));
  const DoubleVect& vol = la_zone_dis.volumes();
  const IntTab& face_voisins = la_zone_dis.face_voisins();
  const DoubleTab& face_normales = la_zone_dis.face_normales();
  const Milieu_base& le_milieu = l_inconnue1->equation().milieu();

  int nb_faces = la_front_vf.nb_faces();
  int ndeb = la_front_vf.num_premiere_face();

  int fac_front;
  for (fac_front = 0; fac_front<nb_faces; fac_front++)
    {
      int fac = fac_front + ndeb;
      int num = face_voisins(fac,0);
      if (num < 0) num = face_voisins(fac,1);

      // calcul de la surface de la face courante
      double ratio = 0.;
      for (int i=0; i<dimension; i++) ratio += (face_normales(fac,i) * face_normales(fac,i));
      ratio = sqrt(ratio);

      double alpha;
      if(sub_type(Champ_Uniforme,le_milieu.diffusivite().valeur()))
        alpha=le_milieu.diffusivite()(0,0);
      else
        {
          const DoubleTab& tab_alpha = le_milieu.diffusivite().valeurs();
          alpha=tab_alpha(num);
        }

      double volume = vol(num);

      coeff_amort_num(fac_front) = volume/ratio;
      coeff_amort_denum(fac_front) = (alpha*ratio)/volume;
    }
}


void Champ_front_contact_VEF::associer_front_vf_et_ch_front_autre_pb()
{
  const Champ_Inc_base& temp_autre = l_inconnue2.valeur();
  const Equation_base& eqn = temp_autre.equation();
  const Zone_Cl_dis_base& zcl = eqn.zone_Cl_dis().valeur();

  int num_cl;
  for (num_cl = 0; num_cl<zcl.nb_cond_lim(); num_cl++)
    {
      const Cond_lim_base& la_cl = zcl.les_conditions_limites(num_cl);
      const Nom& le_nom_cl = la_cl.frontiere_dis().le_nom();
      if (le_nom_cl == nom_bord2)
        {
          if (sub_type(Champ_front_contact_VEF,la_cl.champ_front().valeur()))
            {
              ch_fr_autre_pb = ref_cast(Champ_front_contact_VEF,la_cl.champ_front().valeur());
              // Test de securite pour verifier que l'on a bien saisit le champ
              // front_contact_VEF.
              if (ch_fr_autre_pb->nom_pb1 == nom_pb1)
                if (!(sub_type(Champ_front_contact_fictif_VEF,(*this))))
                  {
                    Cerr<<"Attention, erreur lors de la lecture du Champ_Front_Contact_VEF"<<finl;
                    Cerr<<"La syntaxe a utiliser est : nom_pb_local nom_bord_local nom_pb_distant nom_bord_distant"<<finl;
                    exit();
                  }
              fr_vf_autre_pb=ref_cast(Front_VF, la_cl.frontiere_dis());
            }
        }
    }
}

DoubleVect& Champ_front_contact_VEF::trace_face_raccord(const Front_VF& fr_vf,const DoubleVect& y,DoubleVect& x)
{
  // On verifie que la frontiere est de type Raccord_distant_homogene
  if (sub_type(Raccord_distant_homogene,fr_vf.frontiere()))
    fr_vf.frontiere().trace_face_distant(y,x);
  else
    {
      if (Process::nproc()>1)
        {
          // On teste si on a bien un raccord distant homogene car sinon le tableau connect n'est pas rempli en parallele
          // En effet remplir_connect_bords n'est fait qu'en sequentiel
          Cerr << "The boundary named " << fr_vf.frontiere().le_nom() << " is not a 'Raccord Distant Homogene'" << finl;
          Cerr << "Use keyword modif_bord_to_raccord to change the boundary of kind Paroi into a boundary of kind Raccord" << finl;
          Cerr << "after you read the meshes during the partitioning." << finl;
          exit();
        }
      // On traite un raccord local
      int nb_fac_front=x.size();
      for(int fac_front=0; fac_front<nb_fac_front; fac_front++)
        x(fac_front) = y(connect_bords(fac_front));
    }

  return x;
}


void Champ_front_contact_VEF::remplir_connect_bords()
{
  // Par convention, les grandeurs notees sans indice ainsi que celle notees
  // avec un indice 1 correspondent au probleme courant, celle notees avec
  // un indice 2 correspondent aux grandeurs du probleme couple avec le probleme
  // courant
  const Frontiere& la_front = la_frontiere_dis->frontiere();
  int nb_faces = la_front.nb_faces();

  //
  // collection des informations du probleme 1
  //
  nom_bord = nom_bord1 ;
  associer_ch_inc_base(l_inconnue1.valeur()) ;

  assert (nom_bord != "??") ;
  const Zone_VEF& la_zone_dis1=ref_cast(Zone_VEF, l_inconnue1->equation().zone_dis().valeur());

  int rang_front1 = la_zone_dis1.rang_frontiere(nom_bord1);
  const Front_VF& la_front_vf1=ref_cast(Front_VF, la_zone_dis1.frontiere_dis(rang_front1));
  const DoubleTab& xv1 = la_zone_dis1.xv();
  const IntTab& face_voisin=la_zone_dis1.face_voisins();

  const IntTab& elem_faces = la_zone_dis1.elem_faces();

  int ndeb1 = la_front_vf1.num_premiere_face();

  //
  // collection des informations du probleme 2
  //
  nom_bord = nom_bord2 ;
  associer_ch_inc_base(l_inconnue2.valeur()) ;

  assert (nom_bord != "??") ;
  const Zone_VEF& la_zone_dis2=ref_cast(Zone_VEF, l_inconnue2->equation().zone_dis().valeur());

  int rang_front2 = la_zone_dis2.rang_frontiere(nom_bord2);
  const Front_VF& la_front_vf2=ref_cast(Front_VF, la_zone_dis2.frontiere_dis(rang_front2));
  const DoubleTab& xv2 = la_zone_dis2.xv();
  int ndeb2 = la_front_vf2.num_premiere_face();

  // On verifie que les numerotations des faces sur ce bord sont bien compatibles
  // pour les deux problemes consideres.
  int i,j,k,temoin,temoin_tot;

  double erreur = 0.;
  int ii;
  for (ii=0; ii<dimension; ii++)
    if (std::fabs(xv1(ndeb1,ii) - xv1(ndeb1+nb_faces,ii)) > erreur )
      erreur = std::fabs(xv1(ndeb1,ii) - xv1(ndeb1+nb_faces,ii));
  //
  // L'erreur est 1.e-5 fois la distance maximale entre deux faces de bord
  erreur *=1.e-5;

  //GF la reference pour calculer l'erreur ne correspondait a rien
  // distance entre la premiere face du bord et la face apres le bord ?????




  // Version PAT
  // erreur = Objet_U::precision_geom
  connect_bords.resize(nb_faces);

  //assert(la_front_vf2.nb_faces()==la_front_vf1.nb_faces());
  //Cout << "nb faces = " << la_front_vf2.nb_faces() << finl;
  //Cout << "nb faces = " << la_front_vf1.nb_faces() << finl;

  // PQ : 06/08/07

  int nb_faces1 = la_front_vf1.nb_faces();
  int nb_faces2 = la_front_vf2.nb_faces();

  if(nb_faces1!=nb_faces2)
    {
      Cerr << "Warning, the number of faces (" << nb_faces1 << ") on the boundary " << la_front_vf1.le_nom();
      Cerr << " of the domain " << la_zone_dis1.zone().domaine().le_nom() << finl;
      Cerr << "is different of the number of faces (" << nb_faces2 << ") on the boundary " << la_front_vf2.le_nom();
      Cerr << " of the domain " << la_zone_dis2.zone().domaine().le_nom() << " ..." << finl;
      Cerr << "The exchange condition with the paroi_contact can't succeed!" << finl;
      Nom fichier="connectivity_failed_";
      Nom fichier_med="connectivity_failed_";
      Nom nom_pb;
      if(nb_faces1>nb_faces2)
        {
          nom_pb=nom_pb2;
          fichier+=nom_bord2;
          fichier_med+=nom_pb;
          fichier_med+=".med";
          connectivity_failed(la_zone_dis1, nb_faces1, ndeb1, la_zone_dis2, nb_faces2, ndeb2, fichier, fichier_med);
        }
      else
        {
          nom_pb=nom_pb1;
          fichier+=nom_bord1;
          fichier_med+=nom_pb;
          fichier_med+=".med";
          connectivity_failed(la_zone_dis2, nb_faces2, ndeb2, la_zone_dis1, nb_faces1, ndeb1, fichier, fichier_med);
        }
      Cerr << "Boundaries " << nom_bord1 << " from problem " << nom_pb1 << finl;
      Cerr << "       and " << nom_bord2 << " from problem " << nom_pb2 << finl;
      Cerr << "have not the same number of faces." << finl;
      Cerr << "Coincidence between meshes may be recovered " << finl;
      Cerr << "by cutting mesh associated to " << nom_pb << finl;
      Cerr << "thanks to decouper_bord_coincident keyword or HOMARD tool (see reference manual)." << finl;
      exit();
    }



  // PQ : 03/03 : redefinition du tableau de connectivite
  // processus "optimise" et applicable a tout type de configuration

  temoin_tot=0;

  for (i=0; i<nb_faces; i++)
    {

      // on redefinit erreur pour chaque face comme etant:
      // on cherche l'elt associe a la face, on calcule le min du centre
      // de gravite de l'elem a ses faces -> d1
      // d1*1e-2
      int elem_voisin=face_voisin(ndeb1+i,0);
      if (elem_voisin==-1)
        elem_voisin=face_voisin(ndeb1+i,1);
      int nbf=elem_faces.dimension(1);
      double d1=DMAXFLOAT;
      for (int f=0; f<nbf; f++)
        {
          int f2=elem_faces(elem_voisin,f);
          if (dimension==2)
            d1=std::min(d1,distance_2D(f2,elem_voisin,la_zone_dis1));
          else
            d1=std::min(d1,distance_3D(f2,elem_voisin,la_zone_dis1));
        }
      erreur=d1*1e-2;
      temoin=0;
      for (j=i; j<nb_faces; j++)
        {
          // O.C. : dimension plutot que dim1 dans la boucle suivante :
          //          for (k=0 ; k<dim1 ; k++)
          for (k=0 ; k<dimension ; k++)
            if (std::fabs(xv1(ndeb1+i,k) - xv2(ndeb2+j,k)) > erreur) break;
          // Si on a passe le test ci-dessus, c'est que les dim coordonnees de la face
          // sont les memes (d == dim)
          if (k==dimension)
            {

              /** if (j==1)
                  {
                  Cout << "connect i j " << i << " " << j << finl;
                  Cout << "xv i j " << xv1(ndeb1+i,0) << " " << xv2(ndeb2+j,0) << finl;
                  Cout << "xv i j " << xv1(ndeb1+i,1) << " " << xv2(ndeb2+j,1) << finl;
                  Cout << "xv i j " << xv1(ndeb1+i,2) << " " << xv2(ndeb2+j,2) << finl;
                  Cout << "err i j " << std::fabs(xv1(ndeb1+i,0) - xv2(ndeb2+j,0)) << finl;
                  Cout << "err i j " << std::fabs(xv1(ndeb1+i,1) - xv2(ndeb2+j,1)) << finl;
                  Cout << "err i j " << std::fabs(xv1(ndeb1+i,2) - xv2(ndeb2+j,2)) << finl;
                  Cout << "erreur " << erreur << finl;
                  }*/
              connect_bords(i)=j;
              temoin=1;
              break;
            }
        }
      if(temoin==0)
        for (j=0; j<i; j++)
          {
            for (k=0 ; k<dimension ; k++)
              if (std::fabs(xv1(ndeb1+i,k) - xv2(ndeb2+j,k)) > erreur) break;

            // Si on a passe le test ci-dessus, c'est que les dim coordonnees de la face
            // sont les memes (d == dim)
            if (k==dimension)
              {
                /**  if (j==1)
                     {
                     Cout << "connect i j " << i << " " << j << finl;
                     Cout << "xv i j " << xv1(ndeb1+i,0) << " " << xv2(ndeb2+j,0) << finl;
                     Cout << "xv i j " << xv1(ndeb1+i,1) << " " << xv2(ndeb2+j,1) << finl;
                     Cout << "xv i j " << xv1(ndeb1+i,2) << " " << xv2(ndeb2+j,2) << finl;
                     Cout << "err i j " << std::fabs(xv1(ndeb1+i,0) - xv2(ndeb2+j,0)) << finl;
                     Cout << "err i j " << std::fabs(xv1(ndeb1+i,1) - xv2(ndeb2+j,1)) << finl;
                     Cout << "err i j " << std::fabs(xv1(ndeb1+i,2) - xv2(ndeb2+j,2)) << finl;
                     Cout << "erreur " << erreur << finl;
                     }*/
                connect_bords(i)=j;
                temoin=1;
                break;
              }
          }
      if(temoin==0)
        {
          temoin_tot=1;
          if(dimension==2)
            Cerr<<" no connectivity found for face : "<<ndeb1+i<<" located at : x= "<<xv1(ndeb1+i,0)<<" y= "<<xv1(ndeb1+i,1)<<finl;
          else
            Cerr<<" no connectivity found for face : "<<ndeb1+i<<" located at : x= "<<xv1(ndeb1+i,0)<<" y= "<<xv1(ndeb1+i,1)<< " z= "<<xv1(ndeb1+i,2)<<finl;
        }
    }


  if(temoin_tot==1)
    {
      Cerr<<" Process stopped at Champ_front_contact_VEF::remplir_connect_bords() "<<finl;
      exit();
    }
  // on verifie que l'on a bien cree une bijection
  ArrOfInt inv_connect_bords;
  inv_connect_bords.resize_array(nb_faces, Array_base::NOCOPY_NOINIT);
  inv_connect_bords=-1;
  for (i=0; i<nb_faces; i++)
    {
      inv_connect_bords[connect_bords(i)]=i;
    }
  if (min_array(inv_connect_bords) < 0)
    {
      Cerr<<"Pb a la creation de connect_bord, ce n'est pas une bijection"<<finl;
      Cerr<<"tableau inverse "<<inv_connect_bords<<finl;
      Cerr<<"tableau connect "<<connect_bords<<finl;
      exit();
    }
}

void Champ_front_contact_VEF::remplir_faces_coin()
{
  const Zone_Cl_VEF& la_zcl_dis1=ref_cast(Zone_Cl_VEF, l_inconnue1->equation().zone_Cl_dis().valeur());
  const Conds_lim& conds_lim = la_zcl_dis1.les_conditions_limites();
  int size_cl = conds_lim.size();
  int size = elems_voisin_bord_.size();
  int elem_vois, elem_vois_autre_fr;

  //Parcours des faces de la frontiere

  int elem_vois_face_1, elem_vois_face_2;
  faces_coin.resize(size);

  for (int ind_face_1=0; ind_face_1<size; ind_face_1++)
    {
      elem_vois_face_1 = elems_voisin_bord_[ind_face_1];
      for (int ind_face_2 =0; ind_face_2<size; ind_face_2++)
        {
          if (ind_face_1!=ind_face_2)
            {
              elem_vois_face_2 = elems_voisin_bord_[ind_face_2];
              if ((elem_vois_face_1==elem_vois_face_2) && (elem_vois_face_1!=-1))
                faces_coin(ind_face_1)=1;

            }
        }
    }

  //On tient compte ensuite des faces appaartenant a d autres fontieres
  //pouvant appartenir a un element coin

  for (int num_cl=0; num_cl<size_cl; num_cl++)
    {
      const Champ_front& ch_front = conds_lim[num_cl]->champ_front();
      if (sub_type(Champ_front_contact_VEF,ch_front.valeur()))
        {
          const Champ_front_contact_VEF& ch_front_contact = ref_cast(Champ_front_contact_VEF,ch_front.valeur());
          const Nom& nom_bord_autre_fr = ch_front_contact.get_nom_bord1();

          if (nom_bord1!=nom_bord_autre_fr)
            {
              const IntVect& elems_autre_fr =  ch_front_contact.get_elems_voisin_bord();
              int size_autre_fr = elems_autre_fr.size();
              for (int ind_face =0; ind_face<size; ind_face++)
                {
                  elem_vois = elems_voisin_bord_[ind_face];
                  for (int ind_face_autre_fr =0; ind_face_autre_fr<size_autre_fr; ind_face_autre_fr++)
                    {
                      elem_vois_autre_fr = elems_autre_fr[ind_face_autre_fr];
                      if ((elem_vois==elem_vois_autre_fr) && (elem_vois!=-1))
                        faces_coin(ind_face)=1;
                    }
                }
            }
        }
    }
}

void Champ_front_contact_VEF::remplir_elems_voisin_bord()
{
  const Zone_VEF& la_zone_dis1=ref_cast(Zone_VEF, l_inconnue1->zone_dis_base());
  int rang_front1 = la_zone_dis1.rang_frontiere(nom_bord1);
  const Front_VF& la_front_vf1=ref_cast(Front_VF, la_zone_dis1.frontiere_dis(rang_front1));
  const IntTab& face_voisins1 = la_zone_dis1.face_voisins();
  int nb_faces = la_front_vf1.nb_faces();
  int ndeb = la_front_vf1.num_premiere_face();

  int voisin_0, voisin_1;
  int elem_vois;
  elems_voisin_bord_.resize(nb_faces);
  elems_voisin_bord_=-1;

  for (int face=0; face<nb_faces; face++)
    {
      elem_vois=-1;
      voisin_0 = face_voisins1(ndeb+face,0);
      voisin_1 = face_voisins1(ndeb+face,1);

      if (voisin_0!=-1)
        elem_vois=voisin_0;
      else
        elem_vois=voisin_1;
      elems_voisin_bord_(face) = elem_vois;
    }

}
void Champ_front_contact_VEF::test_faces_coin()
{
  const Zone_VEF& la_zone_dis1=ref_cast(Zone_VEF, l_inconnue1->zone_dis_base());
  const DoubleTab& xv1 = la_zone_dis1.xv();
  const Zone_Cl_VEF& la_zcl_dis1=ref_cast(Zone_Cl_VEF, l_inconnue1->equation().zone_Cl_dis().valeur());
  const Conds_lim& conds_lim = la_zcl_dis1.les_conditions_limites();
  int size_cl = conds_lim.size();

  const Nom& nom_dom = l_inconnue1->equation().probleme().domaine().le_nom();
  int erreur = 0;
  for (int num_cl=0; num_cl<size_cl; num_cl++)
    {
      const Champ_front& ch_front = conds_lim[num_cl]->champ_front();
      if (sub_type(Champ_front_contact_VEF,ch_front.valeur()))
        {
          const Champ_front_contact_VEF& ch_front_contact = ref_cast(Champ_front_contact_VEF,ch_front.valeur());
          const Nom& nom_bord_fr = ch_front_contact.get_nom_bord1();
          int rang_front_fr = la_zone_dis1.rang_frontiere(nom_bord_fr);
          const Front_VF& la_front_vf_fr = ref_cast(Front_VF, la_zone_dis1.frontiere_dis(rang_front_fr));
          int ndeb_fr = la_front_vf_fr.num_premiere_face();
          const IntVect& faces_coin_fr = ch_front_contact.get_faces_coin();

          for (int i=0; i<faces_coin_fr.size(); i++)
            {
              if (faces_coin_fr(i)!=0)
                {
                  erreur =1;
                  Cerr<<"La face suivante du bord "<<nom_bord_fr<<" est une face de coin pour le domaine "<<nom_dom<<finl;
                  for (int k=0; k<dimension; k++)
                    Cerr<<"x_"<<k<<" "<<xv1(ndeb_fr+i,k)<<finl;
                }
            }
        }
    }
  if (erreur)
    {
      Cerr<<"Le codage de Champ_front_contact_VEF n est pas disponible pour cette situation"<<finl;
      Cerr<<"Decouper la (ou les) maille concernee en utilisant l interprete VerifierCoin"<<finl;
      Cerr<<"ou en utilisant Trianguler_H en 2D ou Tetraedriser_homogene en 3D"<<finl;
      //exit();
    }
}

void Champ_front_contact_VEF::connectivity_failed(const Zone_VEF& zvef1, int& nb_faces1, int& ndeb1,
                                                  const Zone_VEF& zvef2, int& nb_faces2, int& ndeb2,
                                                  Nom& fichier, Nom& fichier_med)
{

  const IntTab& face_voisin2 = zvef2.face_voisins();
  const IntTab& elem_faces2  = zvef2.elem_faces();
  int nb_elem2 = zvef2.nb_elem();

  const DoubleTab& xv1  = zvef1.xv();

  int nbf = dimension+1; // nombre de faces par element
  int fac,elem,face1,face2,elem2,trouve;
  int j;

  // Creation du champ P0

  Champ_Fonc_P0_VEF chp;
  chp.associer_zone_dis_base(zvef2);
  chp.dimensionner(nb_elem2,1);
  chp.fixer_nb_comp(1);
  chp.nommer("Raccord_Homard");
  DoubleTab& chp_val = chp.valeurs();

  chp_val=0.;


  for (fac=0; fac<nb_faces1; fac++)
    {
      face1=fac+ndeb1;

      elem2=-1;

      if (dimension==2)  elem2 = zvef2.zone().chercher_elements(xv1(face1,0),xv1(face1,1));
      else                 elem2 = zvef2.zone().chercher_elements(xv1(face1,0),xv1(face1,1),xv1(face1,2));

      if(elem2==-1)
        {
          Cerr << "Probleme dans la recherche de l'element associe !" << finl;
          exit();
        }

      chp_val(elem2)++;
    }

  /////////////////////////////////////////////////////////
  // Ecriture du fichier Connectivity_failed pour decoupage
  // a l'aide de l'interprete  Decouper_Bord_coincident
  /////////////////////////////////////////////////////////

  SFichier fic(fichier);
  fic << nb_faces2 << finl;
  fic << nb_faces1 << finl;

  for (fac=0; fac<nb_faces2; fac++)
    {
      face2=fac+ndeb2;
      elem2=face_voisin2(face2,0);
      if (elem2==-1) elem2=face_voisin2(face2,1);

      // recherche de l'indice locale de la face
      trouve=0;
      for(j=0; j<nbf; j++)
        if(face2==elem_faces2(elem2,j))
          {
            trouve=1;
            break;
          }

      if(trouve==0)
        {
          Cerr << "Probleme dans la recherche de l'indice local !" << finl;
          exit();
        }

      fic << chp_val(elem2) << " " << fac << " " << elem2 << " " << j << finl;
    }

  fic <<(int) -1;
  // pour forcer l ecriture
  fic.close();
  /////////////////////////////////////////////////////////
  // Ecriture du fichier MED pour decoupage suivant HOMARD
  /////////////////////////////////////////////////////////

  // renormalisation de chp_val

  for (elem=0; elem<nb_elem2; elem++)
    {
      if(chp_val(elem)==(2*(dimension-1))) chp_val(elem)=1;
      else                                   chp_val(elem)=0;
    }
  Nom bis(fichier_med);
  bis+="_index";
  fichier_med.prefix(".med");
  const Domaine& dom2 = zvef2.zone().domaine();
  Noms unites_(1);
  unites_[0]="1";
  Noms nom_compos(1);
  nom_compos[0]=chp.le_nom();
  Format_Post_Med post;
  post.initialize_by_default(fichier_med);
  post.ecrire_domaine(dom2,1);
  post.ecrire_champ(dom2,unites_,nom_compos,-1,0., 0., chp.le_nom(), dom2.le_nom(), Motcle("elem"), "scalar",chp_val);
  post.ecrire_champ(dom2,unites_,nom_compos,-1,0., 0., chp.le_nom(), dom2.le_nom(), Motcle("elem"), "scalar",chp_val);
}
