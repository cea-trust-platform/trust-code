/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Source_PDF_base.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Incompressible/Sources
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_PDF_base.h>
#include <Zone_Cl_dis.h>

#include <Equation_base.h>
#include <Debog.h>
#include <Param.h>
#include <Champ.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <SFichier.h>
#include <Interpolation_IBM_base.h>


Implemente_base(Source_PDF_base,"Source_PDF_base",Source_dep_inco_base);
// XD source_pdf_base source_base source_pdf_base 1 Base class of the source term for the Immersed Boundary Penalized Direct Forcing method (PDF)

/*##################################################################################################
####################################################################################################
################################# READON AND PRINTON ###############################################
####################################################################################################
##################################################################################################*/

Entree& Source_PDF_base::readOn(Entree& s)
{
  Param param(que_suis_je());
  param.ajouter("aire", &champ_aire_lu_,Param::REQUIRED); // XD_ADD_P field_base volumic field: a boolean for the cell (0 or 1) indicating if the obstacle is in the cell
  param.ajouter("rotation", &champ_rotation_lu_,Param::REQUIRED); // XD_ADD_P field_base volumic field with 9 components representing the change of basis on cells (local to global). Used for rotating cases for example.
  param.ajouter_flag("transpose_rotation", &transpose_rotation_,Param::OPTIONAL); // XD_ADD_P rien  whether to transpose the basis change matrix.
  param.ajouter("modele",&modele_lu_,Param::REQUIRED);   // XD_ADD_P bloc_pdf_model model used for the Penalized Direct Forcing
  param.ajouter("interpolation",&interpolation_lue_,Param::OPTIONAL); // XD_ADD_P interpolation_ibm_base interpolation method

  param.lire_avec_accolades(s);
  interpolation_bool_ = false;
  if ((&interpolation_lue_)->non_nul())
    {
      if (!(interpolation_lue_.valeur().que_suis_je() == "Interpolation_IBM_aucune"))
        {
          interpolation_bool_ = true;
        }
    }
  return s;
}

Sortie& Source_PDF_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

/*##################################################################################################
####################################################################################################
################################# PROBLEM ASSOCIATION AND ZONES ####################################
####################################################################################################
##################################################################################################*/

void Source_PDF_base::associer_pb(const Probleme_base& pb)
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

void Source_PDF_base::compute_vitesse_imposee_projete(const DoubleTab& marqueur, const DoubleTab& points, double val, double eps)
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

void Source_PDF_base::rotate_imposed_velocity(DoubleTab& vitesse_imposee)
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

void Source_PDF_base::associer_zones(const Zone_dis& zone_dis,
                                     const Zone_Cl_dis& zone_Cl_dis)
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

double Source_PDF_base::fonct_coeff(const double& rho_m, const double& aire, const double& dt) const
{
  double val_coeff = 0.;
  if (aire<=0.)
    {
      return val_coeff;
    }

  double inv_dt = 1./dt;
  // Cerr<<"dt pour Source_PDF_base::fonct_coeff : "<< dt <<finl;
  val_coeff = rho_m * inv_dt;
  const double tps_cour = equation().probleme().schema_temps().temps_courant();
  if (temps_relax_ != 1.0e+12)
    {
      // double bco = 100.*(1.-exp(-0.1/echelle_relax_)); // tps_cour_intersection = 0.1 * temps_relax_
      double deltrel = tps_cour - temps_relax_;
      double coeff_relax = (1.+tanh(deltrel/(echelle_relax_ * temps_relax_)))/2.;
      // double coeff_relax = min (1., std::max(1.-bco*deltrel*deltrel/temps_relax_/temps_relax_, exp(deltrel/(echelle_relax_ * temps_relax_))));
      // double coeff_relax = 1.0 - exp(-tps_cour / temps_relax_);
      // double coeff_relax = tanh(tps_cour / temps_relax_);
      val_coeff *= coeff_relax ;
      // Cerr<< "coeff. relax. pour PDF = "<<coeff_relax<<" val_coeff = "<<val_coeff <<endl;
    }
  return val_coeff;
}

DoubleVect Source_PDF_base::diag_coeff_elem(ArrOfDouble& vitesse_elem, const DoubleTab& rotation, int num_elem) const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
  return DoubleVect();
}

DoubleTab Source_PDF_base::compute_coeff_elem() const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
  return DoubleTab();
}

DoubleTab Source_PDF_base::compute_coeff_matrice_pression() const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
  return DoubleTab();
}

void Source_PDF_base::multiply_coeff_volume(DoubleTab& coeff) const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

/*##################################################################################################
####################################################################################################
################################# TENSOR CALCULATION ###############################################
####################################################################################################
##################################################################################################*/

ArrOfDouble Source_PDF_base::get_tuvw_local() const
{
  assert(Objet_U::dimension==3);
  ArrOfDouble tuvw(dimension) ;
  tuvw(0) = 1.0 / modele_lu_.eta_;
  tuvw(1) = 1.0 / modele_lu_.eta_;
  tuvw(2) = 1.0 / modele_lu_.eta_;
  return tuvw ;
}

/*##################################################################################################
####################################################################################################
################################# AJOUTER_ #########################################################
####################################################################################################
##################################################################################################*/

/*
This function redirects toward the ajouter_ which correspond to the model chosen.
*/

DoubleTab& Source_PDF_base::ajouter_(const DoubleTab& vitesse, DoubleTab& resu) const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
  return resu;
}

DoubleTab& Source_PDF_base::ajouter_(const DoubleTab& vitesse, DoubleTab& resu, const int i_traitement_special) const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
  return resu;
}

/*##################################################################################################
####################################################################################################
################################# CONTRIBUER_A_AVEC ################################################
####################################################################################################
##################################################################################################*/

/*
This function redirects toward the contribuer_avec_ which correspond to the model chosen.
*/

void  Source_PDF_base::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

DoubleTab& Source_PDF_base::calculer(DoubleTab& resu) const
{
  resu = 0;
  const DoubleTab& vitesse=equation().inconnue().valeurs();
  return ajouter_(vitesse,resu);
}

DoubleTab& Source_PDF_base::calculer(DoubleTab& resu, const int i_traitement_special) const
{
  resu = 0;
  const DoubleTab& vitesse=equation().inconnue().valeurs();
  return ajouter_(vitesse,resu, i_traitement_special);
}

void Source_PDF_base::calculer_vitesse_imposee_elem_fluid()
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

void Source_PDF_base::calculer_vitesse_imposee_mean_grad()
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

void Source_PDF_base::calculer_vitesse_imposee_hybrid()
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

void Source_PDF_base::calculer_vitesse_imposee()
{
  if (interpolation_bool_)
    {
      if (interpolation_lue_.valeur().que_suis_je() == "Interpolation_IBM_element_fluide")
        {
          calculer_vitesse_imposee_elem_fluid();
        }
      else if (interpolation_lue_.valeur().que_suis_je() == "Interpolation_IBM_gradient_moyen")
        {
          calculer_vitesse_imposee_mean_grad();
        }
      else if (interpolation_lue_.valeur().que_suis_je() == "Interpolation_IBM_hybride")
        {
          calculer_vitesse_imposee_hybrid();
        }
    }
}

DoubleTab& Source_PDF_base::calculer_pdf(DoubleTab& resu) const
{
  resu = 0;
  ajouter_(vitesse_imposee_,resu);
  return resu;
}

void Source_PDF_base::mettre_a_jour(double temps)
{
  //la_source->mettre_a_jour(temps);
}

void Source_PDF_base::correct_incr_pressure(const DoubleTab& coeff_node, DoubleTab& correction_en_pression) const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

void Source_PDF_base::correct_pressure(const DoubleTab& coeff_node, DoubleTab& pression, const DoubleTab& correction_en_pression) const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

void Source_PDF_base::correct_vitesse(const DoubleTab& coeff_node, DoubleTab& vitesse) const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
}

int Source_PDF_base::impr(Sortie& os) const
{
  Cerr << "Source_PDF_base: Not implemented for current discretisation. Aborting..." << finl;
  abort();
  return 0;
}

void Source_PDF_base::ouvrir_fichier(SFichier& os, const Nom& type, const int& flag=1) const
{
  // flag nul on n'ouvre pas le fichier
  if (flag==0)
    return ;

  const Probleme_base& pb=equation().probleme();
  const Schema_Temps_base& sch=pb.schema_temps();
  const int& precision=sch.precision_impr();
  Nom nomfichier(out_);
  if (type!="") nomfichier+=(Nom)"_"+type;
  nomfichier+=".out";

  // On cree le fichier a la premiere impression avec l'en tete
  if (sch.nb_impr()==1 && !pb.reprise_effectuee())
    {
      os.ouvrir(nomfichier);
      SFichier& fic=os;
      Nom espace="\t\t";
      fic << (Nom)"# Printing of the source term "+que_suis_je()+" of the equation "+equation().que_suis_je()+" of the problem "+equation().probleme().le_nom() << finl;
      fic << "# " << description() << finl;
      fic << "# Time" << espace << "Fx" << espace << "Fy" << espace << "Fz";
      fic << finl;
    }
  // Sinon on l'ouvre
  else
    {
      os.ouvrir(nomfichier,ios::app);
    }
  os.precision(precision);
  os.setf(ios::scientific);
}

void Source_PDF_base::updateChampRho()
{
  if (equation().probleme().que_suis_je() == "Pb_Melange")
    {
      champ_rho_.valeur().affecter(equation().probleme().get_champ("masse_volumique_melange"));
    }
  else
    {
      champ_rho_.valeur().affecter(equation().probleme().get_champ("masse_volumique"));
    }
}
