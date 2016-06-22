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
// File:        Champ_Fonc_reprise.cpp
// Directory:   $TRUST_ROOT/src/Kernel/VF/Champs
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_reprise.h>
#include <Probleme_base.h>
#include <LecFicDiffuseBin.h>
#include <Avanc.h>
#include <EcritureLectureSpecial.h>
#include <Op_Moyenne.h>
#include <Champ_Generique.h>
#include <Entree_complete.h>
#include <Vect_Parser_U.h>
#include <LecFicDistribueBin.h>
#include <EFichierBin.h>
#include <stat_counters.h>
#include <Deriv_Entree_Fichier_base.h>

extern void convert_to(const char *s, double& ob);
Implemente_instanciable(Champ_Fonc_reprise,"Champ_Fonc_reprise",Champ_Fonc_base);


//     printOn()
/////

Sortie& Champ_Fonc_reprise::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//
Entree& Champ_Fonc_reprise::readOn(Entree& s)
{
  Cerr<<"Usage : Champ_Fonc_reprise [xyz|formatte|binaire] fichier.xyz nom_pb nom_inco [fonction n f1(val) f2(val) ... fn(val)] [temps|last_time]"<<finl;
  Nom nom_fic,nom_pb,nom_champ;
  Nom nom_champ_inc;
  Motcle format_rep("xyz");
  int last_time=0;
  double un_temps=-1;

  // Lecture

  s>>nom_fic;
  if ((nom_fic==Motcle("xyz"))||(nom_fic==Motcle("binaire"))||(nom_fic==Motcle("formatte")))
    {
      format_rep=nom_fic;
      s>>nom_fic;
    }
  s>>nom_pb>>nom_champ;

  if (Motcle(nom_champ).debute_par("MOYENNE_"))
    nom_champ_inc=((Motcle)nom_champ).suffix("MOYENNE_");

  Motcle ch, time;
  s>>ch;
  VECT(Parser_U) fxyz;
  if (ch=="FONCTION")
    {
      int nb_compo;
      s >> nb_compo;
      fxyz.dimensionner(nb_compo);
      Nom fonction;
      for (int i = 0; i<nb_compo; i++)
        {
          s >> fonction;
          fxyz[i].setNbVar(1);
          fxyz[i].setString(fonction);
          fxyz[i].addVar("val");
          fxyz[i].parseString();
        }
      s >> time;
      if (time=="LAST_TIME")
        last_time=1;
      else
        convert_to(time,un_temps);
    }
  else
    {
      if (ch=="LAST_TIME")
        last_time=1;
      else
        convert_to(ch,un_temps);
    }
  // On recupere le pb, puis ensuite on cherche le champ; on recupere la zone_dis
  const Probleme_base& pb =ref_cast(Probleme_base,Interprete::objet(nom_pb));
  REF(Champ_base) ref_ch;
  int reprend_champ_moyen=0;
  int reprend_modele_k_eps=0;

  if (nom_champ_inc=="??")
    nom_champ_inc=nom_champ;

  // Cas des champs moyens
  if (((Motcle)nom_champ).debute_par("MOYENNE_"))
    {
      nom_champ+="_natif_";
      nom_champ+=pb.domaine().le_nom();
      reprend_champ_moyen=1;
    }
  // Cas du K-Epsilon
  if (((Motcle)nom_champ_inc)=="K_EPS")
    {
      reprend_modele_k_eps=1;
    }
  ref_ch = pb.get_champ(Motcle(nom_champ_inc));
  if sub_type(Champ_Inc_base,ref_ch.valeur())
    Cerr << nom_champ_inc << " is an unknown of problem " << nom_pb << finl;
  else
    {
      Cerr << nom_champ_inc << " is not an unknown of problem " << nom_pb << finl;
      exit();
    }

  // Ouverture du fichier
  statistiques().begin_count(temporary_counter_);
  Nom ident_lu;
  Nom type_objet_lu;

  int mode_lec_sa= EcritureLectureSpecial::mode_lec;
  DERIV(Entree_Fichier_base) fic_rep;
  if (format_rep == "xyz")
    {
      EcritureLectureSpecial::mode_lec=1;
      fic_rep.typer(EcritureLectureSpecial::Input);
      Cerr << "Opening xyz file " << nom_fic << " (" << EcritureLectureSpecial::Input << ")" << finl;
    }
  else
    {
      EcritureLectureSpecial::mode_lec=0;
      if (format_rep == "binaire")
        fic_rep.typer("LecFicDistribueBin");
      else
        fic_rep.typer("LecFicDistribue");
      Cerr << "Opening file " << nom_fic << " (LecFicDistribueBin)" << finl;
    }
  fic_rep->ouvrir(nom_fic);
  if(fic_rep->fail())
    {
      Cerr<<"Error while opening the file of resumption : " <<nom_fic<<finl;
      exit();
    }

  EFichier& fich=ref_cast(EFichier,fic_rep.valeur());

  if (fich.eof())
    {
      Cerr << "Error in Champ_Fonc_reprise::reprendre" << finl;
      Cerr << "The resumption file does not exist" << finl;
      Cerr << "or could not be opened correctly." << finl;
      exit();
    }

  if (last_time)
    {
      un_temps = get_last_time(fich);
      fich.close();
      fich.ouvrir(nom_fic);
    }

  // Depuis la 1.5.5, lecture du format de sauvegarde
  fich >> ident_lu;
  int format_sauvegarde=0;
  if (ident_lu=="format_sauvegarde:")
    fich >> format_sauvegarde; // Read the format
  else
    {
      // Version anterieure, on referme et on reouvre
      if (format_rep=="xyz")
        {
          fich.close();
          fich.ouvrir(nom_fic);
        }
      else
        {
          Cerr<<"This .sauv file is too old and the format is not supported anymore."<<finl;
          exit();
        }
    }

  // Creation des identifiants
  Nom nom_ident;
  Nom nom_ident_champ_stat;
  Nom nom_ident_champ_keps;
  Nom type=ref_ch.valeur().que_suis_je();
  Nom nom_temps=Nom(un_temps,time_format_from(format_sauvegarde));
  if (reprend_champ_moyen)
    {
      nom_ident="Operateurs_Statistique_tps";
      nom_ident+=pb.domaine().le_nom();
      nom_ident+=nom_temps;
      nom_ident_champ_stat=nom_champ;

      nom_ident_champ_stat+="Champ_Fonc";
      type.suffix("Champ");
      nom_ident_champ_stat+=type;
      nom_ident_champ_stat+=pb.domaine().le_nom();
      nom_ident_champ_stat+=nom_temps;
    }
  else if (reprend_modele_k_eps)
    {
      nom_ident="Modele_turbulence_hyd_K_Epsilon";
      nom_ident+=pb.domaine().le_nom();
      nom_ident+=nom_temps;
      nom_ident_champ_keps=nom_champ_inc;
      nom_ident_champ_keps+=type;
      nom_ident_champ_keps+=pb.domaine().le_nom();
      nom_ident_champ_keps+=nom_temps;
    }
  else
    {
      nom_ident=nom_champ_inc;
      nom_ident+=type;
      nom_ident+=pb.domaine().le_nom();
      nom_ident+=nom_temps;
    }

  associer_zone_dis_base(pb.domaine_dis().zone_dis(0));
  // on cree un champ comme le ch_ref;
  vrai_champ_.typer(ref_ch.valeur().que_suis_je());
  const Champ_Inc_base& ch_inc=ref_cast(Champ_Inc_base,ref_ch.valeur());
  Champ_Inc_base& v_champ=vrai_champ_.valeur();
  le_champ().associer_zone_dis_base(pb.domaine_dis().zone_dis(0));

  v_champ.fixer_nb_valeurs_temporelles(2);
  v_champ.nommer(ch_inc.le_nom());
  v_champ.fixer_nb_comp(ch_inc.nb_comp());
  v_champ.fixer_nb_valeurs_nodales(ch_inc.nb_valeurs_nodales());

  nb_compo_ = ch_inc.nb_comp();
  if (ch=="FONCTION")
    if (fxyz.size()!=ch_inc.nb_comp())
      {
        Cerr<<"The number of components read " <<fxyz.size()<<" for the resume function"<<finl;
        Cerr<<"does not corresponds to the number of components "<<ch_inc.nb_comp()<<" of field target"<<finl;
        exit();
      }

  // Lecture du fichier
  avancer_fichier(fich, nom_ident);

  if (reprend_champ_moyen)
    {
      int n; // Nombre d'operateurs statistiques
      fich >> n;
      double tdeb,tfin;
      fich >> tdeb;
      fich >> tfin;
      avancer_fichier(fich,nom_ident_champ_stat);
      // On cree un operateur_statistique qui va nous permettre de relire le champ moyen
      Op_Moyenne champ_moyen;
      //On construit un Champ_Generique_refChamp pour pouvoir associer un Champ_Generique_base
      //a l operateur
      Champ_Generique champ;
      Nom ajout("");
      ajout += " refChamp { Pb_champ ";
      ajout += pb.le_nom();
      ajout += " ";
      ajout += le_champ().le_nom();
      ajout += " }";
      Entree_complete s_complete(ajout,s);
      s_complete>>champ;

      champ_moyen.associer(pb.domaine_dis().zone_dis(0),champ.valeur(),tdeb,tfin);
      champ_moyen.completer(pb);
      champ_moyen.fixer_tstat_deb(tdeb,tfin);
      champ_moyen.reprendre(fich);
      // On remplit le champ
      le_champ().valeurs() = champ_moyen.calculer_valeurs();
    }
  else if (reprend_modele_k_eps)
    {
      avancer_fichier(fich, nom_ident_champ_keps);
      le_champ().reprendre(fich);
    }
  else
    le_champ().reprendre(fich);

  EcritureLectureSpecial::mode_lec=mode_lec_sa;
  statistiques().end_count(temporary_counter_);
  Cerr << "End of resuming the file " << nom_fic << " after " << statistiques().last_time(temporary_counter_) << " s" << finl;

  ////////////////////////////////////////
  // Transformation eventuelle du champ lu
  ////////////////////////////////////////
  if (fxyz.size())
    {
      DoubleTab& tab_valeurs=le_champ().valeurs();
      // Boucle sur les noeuds du champs
      int sz=tab_valeurs.dimension_tot(0);
      int nb_compo=fxyz.size();
      if (tab_valeurs.nb_dim()==1)
        {
          if (nb_compo!=1)
            {
              Cerr << "Error in the function read because the field " << le_champ().le_nom() << " has a single dimension." << finl;
              exit();
            }
        }
      else if (nb_compo!=tab_valeurs.dimension_tot(1))
        {
          Cerr << "Error in the number of components of the function read:" << nb_compo << finl;
          Cerr << "The number of components of field " << le_champ().le_nom() << " is:" << tab_valeurs.dimension_tot(1) << finl;
          exit();
        }
      for (int i=0; i<sz; i++)
        {
          if (nb_compo==1)
            {
              fxyz[0].setVar("val",tab_valeurs(i));
              // On fait la transformation des valeurs du champs
              tab_valeurs(i)=fxyz[0].eval();
            }
          else
            // On boucle sur les composantes du champ
            for (int j=0; j<nb_compo; j++)
              {
                fxyz[j].setVar("val",tab_valeurs(i,j));
                // On fait la transformation des valeurs du champs
                tab_valeurs(i,j)=fxyz[j].eval();
              }
        }
    }
  return s ;
}

void Champ_Fonc_reprise::mettre_a_jour(double t)
{

  Champ_Fonc_base::mettre_a_jour(t);

}

const Zone_dis_base& Champ_Fonc_reprise::zone_dis_base() const
{
  return zone_dis;
}
