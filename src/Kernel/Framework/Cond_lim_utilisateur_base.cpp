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
// File:        Cond_lim_utilisateur_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#include <Cond_lim_utilisateur_base.h>
#include <Probleme_base.h>
#include <Entree_complete.h>
#include <Equation_base.h>
#include <Probleme_Couple.h>
#include <Discretisation_base.h>
#include <Interprete.h>
#include <Milieu_base.h>
#include <SFichier.h>

Implemente_base_sans_constructeur(Cond_lim_utilisateur_base,"Cond_lim_utilisateur_base",Cond_lim_base);
Implemente_instanciable(cl_timp,"Paroi_Temperature_imposee",Cond_lim_utilisateur_base);
Implemente_instanciable(paroi_adiabatique,"paroi_adiabatique",Cond_lim_utilisateur_base);
Implemente_instanciable(paroi_flux_impose,"paroi_flux_impose",Cond_lim_utilisateur_base);
Implemente_instanciable(paroi_contact,"paroi_contact",Cond_lim_utilisateur_base);

Implemente_instanciable(paroi_contact_fictif,"paroi_contact_fictif",Cond_lim_utilisateur_base);
// Description:
//    Ecrit le type de l'objet sur un flot de sortie
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet

Sortie& cl_timp::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}
Sortie& paroi_adiabatique::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}
Sortie& paroi_flux_impose::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}
Sortie& paroi_contact::printOn(Sortie& s ) const
{
  return s << que_suis_je()  << " "<<nom_autre_pb<<" "<<nom_autre_bord ;
}
Sortie& paroi_contact_fictif::printOn(Sortie& s ) const
{
  return s << que_suis_je()  << " "<<nom_autre_pb<<" "<<nom_autre_bord  << " "<<conduct_fictif<<" "<<ep_fictif;
}

// Description:
//    Lecture d'une condition aux limite de type Cond_lim_utilisateur_base
//    a partir d'un flot d'entree.
//
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& cl_timp::readOn(Entree& s )
{
  return s;
}
Entree& paroi_adiabatique::readOn(Entree& s )
{
  return s;
}
Entree& paroi_flux_impose::readOn(Entree& s )
{
  return s;
}

Entree& paroi_contact::readOn(Entree& s )
{
  s >> nom_autre_pb;
  s >> nom_autre_bord;
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
// Description:
//    Ecrit le type de l'objet sur un flot de sortie
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Cond_lim_utilisateur_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

// Description:
//    Lecture d'une condition aux limite de type Cond_lim_utilisateur_base
//    a partir d'un flot d'entree.
//
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Cond_lim_utilisateur_base::readOn(Entree& s )
{
  // la_cl=new Cond_lim;

  return s ;
}

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


int Cond_lim_utilisateur_base::is_pb_VDF()
{
  const Discretisation_base& discr=mon_equation->discretisation();
  Nom nom_discr=discr.que_suis_je();
  if (nom_discr=="VDF_Front_Tracking")
    {
      nom_discr="VDF";
    }
  if  (nom_discr=="VDF")
    return 1;
  else
    return 0;
}
int Cond_lim_utilisateur_base::is_pb_PolyMAC()
{
  const Discretisation_base& discr=mon_equation->discretisation();
  Nom nom_discr=discr.que_suis_je();
  if (nom_discr=="PolyMAC")
    return 1;
  else
    return 0;
}
int Cond_lim_utilisateur_base::is_pb_VEF()
{
  Cerr<<"Cond_lim_utilisateur_base::is_pb_VEF not coded" <<finl;
  exit();
  return 0;
}

// Description renvoit 0 si le pb n'est pas rayonnant
//                     1 si il est semi_transp
//                     2 si il est transparent

int Cond_lim_utilisateur_base::is_pb_rayo()
{
  Probleme_base& pb=mon_equation->probleme();
  Milieu_base& milieu = ref_cast(Milieu_base,pb.milieu());


  if (milieu.is_rayo_transp())
    return 2;
  else if (milieu.is_rayo_semi_transp())
    return 1;
  else
    return 0;

}

void paroi_adiabatique::complement(Nom& ajout)
{
  int rayo=is_pb_rayo();
  if (rayo==1)
    {

      if (is_pb_VDF())
        ajout="paroi_flux_impose_rayo_semi_transp_VDF champ_front_uniforme 1 0";
      else
        ajout="paroi_flux_impose_rayo_semi_transp_VEF champ_front_uniforme 1 0";
      //ajout="Neumann_Paroi_adiabatique";
    }
  else if (rayo==2)
    ajout="paroi_flux_impose_rayo_transp champ_front_uniforme 1 0";
  else
    ajout="Neumann_Paroi_adiabatique";
}
void paroi_flux_impose::complement(Nom& ajout)
{
  int rayo=is_pb_rayo();
  if (rayo==1)
    {
      if (is_pb_VDF())
        ajout="paroi_flux_impose_rayo_semi_transp_VDF";
      else
        ajout="paroi_flux_impose_rayo_semi_transp_VEF";
    }
  else if (rayo==2)
    ajout="paroi_flux_impose_rayo_transp";
  else
    ajout="Neumann_Paroi";
}
void cl_timp::complement(Nom& ajout)
{
  int rayo=is_pb_rayo();
  Nom Nrayo;
  if (rayo==1) Nrayo="_rayo_semi_transp";
  if (rayo==2)  Nrayo="_rayo_transp";
  if (is_pb_VDF())
    {
      ajout= "paroi_echange_externe_impose";
      if (rayo)
        ajout+=Nrayo;
      Nom cl(" h_imp champ_front_uniforme 1 1e10 t_ext");
      ajout+=cl;
      //exit();
    }
  else
    {
      ajout="temperature_imposee_paroi";
      if (rayo)
        {
          ajout="paroi_temperature_imposee";
          ajout+=Nrayo;
        }

    }
}

void paroi_contact::complement(Nom& ajout)
{
  int rayo =        is_pb_rayo();
  if (!rayo)
    {
      if (ref_cast(Probleme_base,Interprete::objet(nom_autre_pb)).milieu().is_rayo_transp())
        rayo = 2;
      else if (ref_cast(Probleme_base,Interprete::objet(nom_autre_pb)).milieu().is_rayo_semi_transp())
        rayo = 1;
    }

  const Nom& nom_mon_pb=mon_equation->probleme().le_nom();
  if (nom_mon_pb==nom_autre_pb)
    {
      Cerr<<"Error in "<<que_suis_je()<<" the name of the other problem is the same as my problem :"<<nom_mon_pb<<" on the boundary :"<< nom_bord_<<finl;
      exit();
    }

  if (is_pb_VDF())
    {
      //paroi_echange_contact_VDF pb2 Droit1 temperature 1.e10


      ajout= "paroi_echange_contact_VDF ";
      if (rayo==2)
        ajout= "Echange_contact_Rayo_transp_VDF ";
      if (rayo==1)
        ajout="Paroi_Echange_contact_rayo_semi_transp_VDF ";

      ajout+=nom_autre_pb;
      ajout+=" ";
      ajout+=nom_autre_bord;
      ajout+=" temperature 1.e10";
    }
  else if (is_pb_PolyMAC())
    {
      //paroi_echange_contact_PolyMAC pb2 Droit1 temperature 1.e10
      ajout= "paroi_echange_contact_PolyMAC ";
      ajout+=nom_autre_pb;
      ajout+=" ";
      ajout+=nom_autre_bord;
      ajout+=" temperature 1.e10";
    }
  else
    {

      if (rayo==1)
        {
          ajout="paroi_temperature_imposee_rayo_semi_transp ";
          ajout+="Champ_Front_contact_rayo_semi_transp_VEF ";
        }
      else if (rayo==2)
        {
          ajout="paroi_temperature_imposee_rayo_transp ";
          ajout+="Champ_Front_contact_rayo_transp_VEF ";
        }
      else
        {
          ajout="scalaire_impose_paroi ";
          ajout+="Champ_Front_contact_VEF ";
        }
      ajout+=nom_mon_pb;
      ajout+=" ";
      ajout+=nom_bord_;
      ajout+=" ";
      ajout+=nom_autre_pb;
      ajout+=" ";
      ajout+=nom_autre_bord;
    }
}
void paroi_contact_fictif::complement(Nom& ajout)
{
  int rayo=is_pb_rayo();
  if (!rayo)
    {
      if (ref_cast(Probleme_base,interprete().objet(nom_autre_pb)).milieu().is_rayo_transp())
        rayo = 2;
      else if (ref_cast(Probleme_base,interprete().objet(nom_autre_pb)).milieu().is_rayo_semi_transp())
        rayo = 1;
    }
  //if ((!mon_equation->probleme().equation(0).comprend_champ("vitesse"))&&(!ref_cast(Probleme_base,interprete().objet(nom_autre_pb)).equation(0).comprend_champ("vitesse")))
  //          rayo=0;
  const Nom& nom_mon_pb=mon_equation->probleme().le_nom();
  //  if (nom_mon_pb==nom_autre_pb)
  //    {
  //      Cerr<<"Erreur dans "<<que_suis_je()<<" le nom de l'autre pb est le meme que celui de mon pb :"<<nom_mon_pb<<" sur le bord :"<< nom_bord_<<finl;
  //      exit();
  //    }
  if (is_pb_VDF())
    {
      ajout= "paroi_echange_contact_VDF ";
      if (rayo==2)
        ajout= "Echange_contact_Rayo_transp_VDF ";
      if (rayo==1)
        ajout="Paroi_Echange_contact_rayo_semi_transp_VDF ";

      ajout+=nom_autre_pb;
      ajout+=" ";
      ajout+=nom_autre_bord;
      ajout+=" temperature ";
      ajout+=Nom(conduct_fictif/ep_fictif,"%e");
    }
  else if (is_pb_PolyMAC())
    {
      //paroi_echange_contact_PolyMAC pb2 Droit1 temperature conduc / ep
      ajout= "paroi_echange_contact_PolyMAC ";
      ajout+=nom_autre_pb;
      ajout+=" ";
      ajout+=nom_autre_bord;
      ajout+=" temperature ";
      ajout+=Nom(conduct_fictif/ep_fictif,"%e");
    }
  else
    {
      if (rayo)
        {
          Cerr<<que_suis_je()<< "coded only in non-radiating in VEF"<<finl;
          Process::exit();
        }

      ajout="temperature_imposee_paroi ";
      ajout+="Champ_Front_contact_fictif_VEF ";
      ajout+=nom_mon_pb;
      ajout+=" ";
      ajout+=nom_bord_;
      ajout+=" ";
      ajout+=nom_autre_pb;
      ajout+=" ";
      ajout+=nom_autre_bord;
      ajout+=" ";
      ajout+=Nom(conduct_fictif,"%e");
      ajout+=" ";
      ajout+=Nom(ep_fictif,"%e");
    }
}

