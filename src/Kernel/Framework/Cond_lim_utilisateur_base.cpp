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

#include <Cond_lim_utilisateur_base.h>
#include <Discretisation_base.h>
#include <Probleme_Couple.h>
#include <Entree_complete.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Interprete.h>
#include <SFichier.h>

Implemente_base_sans_constructeur(Cond_lim_utilisateur_base,"Cond_lim_utilisateur_base",Cond_lim_base);

Sortie& Cond_lim_utilisateur_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Entree& Cond_lim_utilisateur_base::readOn(Entree& s ) { return s ; }

void  Cond_lim_utilisateur_base::ecrire(const Nom& ajout)
{
  if (je_suis_maitre())
    {
      SFichier conv("convert_jdd",ios::app);
      conv<<(*this)<<" # "<<ajout<<finl;
    }
}

void Cond_lim_utilisateur_base::lire(Entree& s ,Equation_base& mon_eq,const Nom& nom_bord)
{
  nom_bord_=nom_bord;
  mon_equation=mon_eq;
  la_cl_=new Cond_lim;
  Nom ajout("");
  complement(ajout);
#ifndef NDEBUG
  ecrire(ajout);
#endif
  Entree_complete s_complete(ajout,s);

  s_complete>>*(la_cl_);
  Cerr<<"end reading condmimutil"<<finl;
}

Cond_lim_utilisateur_base:: Cond_lim_utilisateur_base():la_cl_(0) { }

Cond_lim& Cond_lim_utilisateur_base::la_cl()
{
  return *(la_cl_);
}


void Cond_lim_utilisateur_base::complement(Nom& ajout)
{
  Cerr<<"Cond_lim_utilisateur_base::complement(Nom& toto) does nothing"<<finl;
}

int Cond_lim_utilisateur_base::is_pb(std::string dis)
{
  const Discretisation_base& discr=mon_equation->discretisation();
  Nom nom_discr=discr.que_suis_je();
  return int(nom_discr == dis);
}

int Cond_lim_utilisateur_base::is_pb_VEF()
{
  Cerr<<"Cond_lim_utilisateur_base::is_pb_VEF not coded" <<finl;
  Process::exit();
  return 0;
}

/*! @brief renvoit 0 si le pb n'est pas rayonnant 1 si il est semi_transp
 *
 *                      2 si il est transparent
 *
 */
int Cond_lim_utilisateur_base::is_pb_rayo()
{
  Probleme_base& pb = mon_equation->probleme();
  Milieu_base& milieu = ref_cast(Milieu_base, pb.milieu());

  if (milieu.is_rayo_transp())
    return 2;
  else if (milieu.is_rayo_semi_transp())
    return 1;
  else
    return 0;
}

/*
 * Classes filles
 */
Implemente_instanciable(cl_timp,"Paroi_Temperature_imposee",Cond_lim_utilisateur_base);
Implemente_instanciable(paroi_adiabatique,"paroi_adiabatique",Cond_lim_utilisateur_base);
Implemente_instanciable(paroi_flux_impose,"paroi_flux_impose",Cond_lim_utilisateur_base);
Implemente_instanciable(paroi_contact,"paroi_contact",Cond_lim_utilisateur_base);
Implemente_instanciable(paroi_contact_rayo,"paroi_contact_rayo",Cond_lim_utilisateur_base);
Implemente_instanciable(paroi_contact_fictif,"paroi_contact_fictif",Cond_lim_utilisateur_base);
Implemente_instanciable(paroi_contact_fictif_rayo,"paroi_contact_fictif_rayo",Cond_lim_utilisateur_base);

Sortie& cl_timp::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Sortie& paroi_adiabatique::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Sortie& paroi_flux_impose::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Sortie& paroi_contact::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << nom_autre_pb << " " << nom_autre_bord;
}

Sortie& paroi_contact_rayo::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << nom_autre_pb << " " << nom_autre_bord << " " << type_rayo;
}

Sortie& paroi_contact_fictif::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << nom_autre_pb << " " << nom_autre_bord << " " << conduct_fictif << " " << ep_fictif;
}

Sortie& paroi_contact_fictif_rayo::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << nom_autre_pb << " " << nom_autre_bord << " " << conduct_fictif << " " << ep_fictif << " " << type_rayo;
}

Entree& cl_timp::readOn(Entree& s ) { return s; }
Entree& paroi_adiabatique::readOn(Entree& s ) { return s; }
Entree& paroi_flux_impose::readOn(Entree& s ) { return s; }

Entree& paroi_contact::readOn(Entree& s )
{
  s >> nom_autre_pb;
  s >> nom_autre_bord;
  return s ;
}

Entree& paroi_contact_rayo::readOn(Entree& s )
{
  s >> nom_autre_pb;
  s >> nom_autre_bord;
  s >> type_rayo; /* pour l'autre probleme */

  if (type_rayo != "TRANSP" && type_rayo != "SEMI_TRANSP")
    {
      Cerr << "type_rayo should be TRANSP or SEMI_TRANSP and not " << type_rayo << finl;
      Process::exit();
    }

  return s ;
}

Entree& paroi_contact_fictif::readOn(Entree& s )
{
  s >> nom_autre_pb;
  s >> nom_autre_bord;
  s >> conduct_fictif;
  s >> ep_fictif;
  return s ;
}

Entree& paroi_contact_fictif_rayo::readOn(Entree& s )
{
  s >> nom_autre_pb;
  s >> nom_autre_bord;
  s >> conduct_fictif;
  s >> ep_fictif;
  s >> type_rayo; /* pour l'autre probleme */

  if (type_rayo != "TRANSP" && type_rayo != "SEMI_TRANSP")
    {
      Cerr << "type_rayo should be TRANSP or SEMI_TRANSP and not " << type_rayo << finl;
      Process::exit();
    }

  return s ;
}

void paroi_adiabatique::complement(Nom& ajout)
{
  int rayo = is_pb_rayo();
  if (rayo == 1)
    {

      if (is_pb_VDF())
        ajout = "paroi_flux_impose_rayo_semi_transp_VDF champ_front_uniforme 1 0";
      else
        ajout = "paroi_flux_impose_rayo_semi_transp_VEF champ_front_uniforme 1 0";
      //ajout="Neumann_Paroi_adiabatique";
    }
  else if (rayo == 2)
    ajout = "paroi_flux_impose_rayo_transp champ_front_uniforme 1 0";
  else
    ajout = "Neumann_Paroi_adiabatique";
}

void paroi_flux_impose::complement(Nom& ajout)
{
  int rayo = is_pb_rayo();
  if (rayo == 1)
    {
      if (is_pb_VDF())
        ajout = "paroi_flux_impose_rayo_semi_transp_VDF";
      else
        ajout = "paroi_flux_impose_rayo_semi_transp_VEF";
    }
  else if (rayo == 2)
    ajout = "paroi_flux_impose_rayo_transp";
  else
    ajout = "Neumann_Paroi";
}

void cl_timp::complement(Nom& ajout)
{
  int rayo = is_pb_rayo();
  Nom Nrayo;
  if (rayo == 1)
    Nrayo = "_rayo_semi_transp";
  if (rayo == 2)
    Nrayo = "_rayo_transp";
  if (is_pb_VDF())
    {
      ajout = "paroi_echange_externe_impose";
      if (rayo)
        ajout += Nrayo;
      Nom cl(" h_imp champ_front_uniforme ");
      const int N = mon_equation->inconnue().valeurs().dimension(1);
      cl += N;
      for (int n = 0; n < N; n++) cl += " 1e10";
      cl += " t_ext";
      ajout += cl;
    }
  else
    {
      ajout = "temperature_imposee_paroi";
      if (rayo)
        {
          ajout = "paroi_temperature_imposee";
          ajout += Nrayo;
        }

    }
}

void paroi_contact::complement(Nom& ajout)
{
  const Nom& nom_mon_pb = mon_equation->probleme().le_nom();
  if (nom_mon_pb == nom_autre_pb)
    {
      Cerr << "Error in " << que_suis_je() << " the name of the other problem is the same as my problem :" << nom_mon_pb << " on the boundary :" << nom_bord_ << finl;
      Process::exit();
    }

  if (is_pb_VDF() || is_pb_PolyMAC() || is_pb_PolyMAC_P0P1NC() || is_pb_PolyMAC_P0())
    {
      if (is_pb_VDF()) ajout = "paroi_echange_contact_VDF ";
      else ajout = is_pb_PolyMAC_P0P1NC() ? "paroi_echange_contact_PolyMAC_P0P1NC " :
                     is_pb_PolyMAC_P0() ? "paroi_echange_contact_PolyMAC_P0 " : "paroi_echange_contact_PolyMAC ";

      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord + " ";
      ajout += mon_equation->inconnue().le_nom() + " 1.e10";
    }
  else
    {
      ajout = "scalaire_impose_paroi ";
      ajout += "Champ_Front_contact_VEF ";
      ajout += nom_mon_pb;
      ajout += " ";
      ajout += nom_bord_;
      ajout += " ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
    }
}

void paroi_contact_rayo::complement(Nom& ajout)
{
  int rayo = is_pb_rayo();
  if (!rayo)
    {
      if (type_rayo == "TRANSP") rayo = 2;
      else if (type_rayo == "SEMI_TRANSP") rayo = 1;
      else throw;
    }

  const Nom& nom_mon_pb = mon_equation->probleme().le_nom();
  if (nom_mon_pb == nom_autre_pb)
    {
      Cerr << "Error in " << que_suis_je() << " the name of the other problem is the same as my problem :" << nom_mon_pb << " on the boundary :" << nom_bord_ << finl;
      Process::exit();
    }

  if (is_pb_VDF() || is_pb_PolyMAC() || is_pb_PolyMAC_P0P1NC() || is_pb_PolyMAC_P0())
    {
      if (is_pb_VDF())
        {
          if (rayo == 2) ajout = "Echange_contact_Rayo_transp_VDF ";
          if (rayo == 1) ajout = "Paroi_Echange_contact_rayo_semi_transp_VDF ";
        }
      else ajout = is_pb_PolyMAC_P0P1NC() ? "paroi_echange_contact_PolyMAC_P0P1NC " :
                     is_pb_PolyMAC_P0() ? "paroi_echange_contact_PolyMAC_P0 " : "paroi_echange_contact_PolyMAC ";

      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord + " ";
      ajout += mon_equation->inconnue().le_nom() + " 1.e10";
    }
  else
    {
      if (rayo == 1)
        {
          ajout = "paroi_temperature_imposee_rayo_semi_transp ";
          ajout += "Champ_Front_contact_rayo_semi_transp_VEF ";
        }
      else if (rayo == 2)
        {
          ajout = "paroi_temperature_imposee_rayo_transp ";
          ajout += "Champ_Front_contact_rayo_transp_VEF ";
        }
      ajout += nom_mon_pb;
      ajout += " ";
      ajout += nom_bord_;
      ajout += " ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
    }
}

void paroi_contact_fictif::complement(Nom& ajout)
{
  const Nom& nom_mon_pb = mon_equation->probleme().le_nom();
  if (is_pb_VDF())
    {
      ajout = "paroi_echange_contact_VDF ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " temperature ";
      ajout += Nom(conduct_fictif / ep_fictif, "%e");
    }
  else if (is_pb_PolyMAC())
    {
      ajout = "paroi_echange_contact_PolyMAC ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " temperature ";
      ajout += Nom(conduct_fictif / ep_fictif, "%e");
    }
  else if (is_pb_PolyMAC_P0P1NC())
    {
      ajout = "paroi_echange_contact_PolyMAC_P0P1NC ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " temperature ";
      ajout += Nom(conduct_fictif / ep_fictif, "%e");
    }
  else if (is_pb_PolyMAC_P0())
    {
      ajout = "paroi_echange_contact_PolyMAC_P0 ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " temperature ";
      ajout += Nom(conduct_fictif / ep_fictif, "%e");
    }
  else
    {
      ajout = "temperature_imposee_paroi ";
      ajout += "Champ_Front_contact_fictif_VEF ";
      ajout += nom_mon_pb;
      ajout += " ";
      ajout += nom_bord_;
      ajout += " ";
      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " ";
      ajout += Nom(conduct_fictif, "%e");
      ajout += " ";
      ajout += Nom(ep_fictif, "%e");
    }
}

void paroi_contact_fictif_rayo::complement(Nom& ajout)
{
  int rayo = is_pb_rayo();
  if (!rayo)
    {
      if (type_rayo == "TRANSP") rayo = 2;
      else if (type_rayo == "SEMI_TRANSP") rayo = 1;
      else throw;
    }

  if (is_pb_VDF())
    {
      if (rayo == 2) ajout = "Echange_contact_Rayo_transp_VDF ";
      if (rayo == 1) ajout = "Paroi_Echange_contact_rayo_semi_transp_VDF ";

      ajout += nom_autre_pb;
      ajout += " ";
      ajout += nom_autre_bord;
      ajout += " temperature ";
      ajout += Nom(conduct_fictif / ep_fictif, "%e");
    }
  else
    {
      Cerr << que_suis_je() << "coded only in non-radiating in VEF/PolyMAC_P0P1NC" << finl;
      Process::exit();
    }
}

