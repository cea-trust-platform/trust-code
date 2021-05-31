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
#include <FichierHDFPar.h>

extern void convert_to(const char *s, double& ob);
Implemente_instanciable(Champ_Fonc_reprise,"Champ_Fonc_reprise",Champ_Fonc_base);

// XD champ_fonc_reprise champ_don_base champ_fonc_reprise 0 This field is used to read a data field in a save file (.xyz or .sauv) at a specified time. It is very useful, for example, to run a thermohydraulic calculation with velocity initial condition read into a save file from a previous hydraulic calculation.
// XD  attr format chaine(into=["binaire","formatte","xyz","single_hdf"]) format 1 Type of file (the file format). If xyz format is activated, the .xyz file from the previous calculation will be given for filename, and if formatte or binaire is choosen, the .sauv file of the previous calculation will be specified for filename. In the case of a parallel calculation, if the mesh partition does not changed between the previous calculation and the next one, the binaire format should be preferred, because is faster than the xyz format. If single_hdf is used, the same constraints/advantages as binaire apply, but a single (HDF5) file is produced on the filesystem instead of having one file per processor.
// XD attr filename chaine filename 0 Name of the save file.
// XD attr pb_name ref_Pb_base pb_name 0 Name of the problem.
// XD attr champ chaine champ 0 Name of the problem unknown. It may also be the temporal average of a problem unknown (like moyenne_vitesse, moyenne_temperature,...)
// XD attr fonction fonction_champ_reprise fonction 1 Optional keyword to apply a function on the field being read in the save file (e.g. to read a temperature field in Celsius units and convert it for the calculation on Kelvin units, you will use: fonction 1 273.+val )
// XD attr temps chaine time 0 Time of the saved field in the save file or last_time. If you give the keyword last_time instead, the last time saved in the save file will be used.

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
  Cerr<<"Usage : Champ_Fonc_reprise [xyz|formatte|binaire|single_hdf] fichier.xyz nom_pb nom_inco [fonction n f1(val) f2(val) ... fn(val)] [temps|last_time]"<<finl;
  Nom nom_fic,nom_pb,nom_champ;
  Nom nom_champ_inc;
  Motcle format_rep("xyz");
  int last_time=0;
  double un_temps=-1;

  // Lecture

  s>>nom_fic;
  if ((nom_fic==Motcle("xyz"))||(nom_fic==Motcle("binaire"))||(nom_fic==Motcle("single_hdf"))||(nom_fic==Motcle("formatte")))
    {
      format_rep=nom_fic;
      s>>nom_fic;
    }
  s>>nom_pb>>nom_champ;

  if (format_rep == "single_hdf" && Process::nproc() == 1)
    {
      Cerr << "Resuming with a single_hdf option is only available for a parallel calculation !" << finl;
      Process::exit();
    }

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
      Process::exit();
    }

  // Ouverture du fichier
  statistiques().begin_count(temporary_counter_);
  Nom ident_lu;
  Nom type_objet_lu;

  int mode_lec_sa= EcritureLectureSpecial::mode_lec;
  DERIV(Entree_Fichier_base) fic_rep;

#ifdef MPI_
  Entree_Brute input_data;
  FichierHDFPar fic_hdf; //FichierHDF fic_hdf;
#endif

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
      else if (format_rep == "single_hdf")
        {
#ifdef MPI_
          LecFicDiffuse test;
          if (!test.ouvrir(nom_fic))
            {
              Cerr << "Error! " << nom_fic << " file not found ! " << finl;
              Process::exit();
            }
          fic_hdf.open(nom_fic, true);
          fic_hdf.read_dataset("/sauv", Process::me(),input_data);
#endif
        }
      else
        fic_rep.typer("LecFicDistribue");
      Cerr << "Opening file " << nom_fic << " (LecFicDistribueBin)" << finl;
    }

  if (format_rep != "single_hdf")
    {
      fic_rep->ouvrir(nom_fic);

      if(fic_rep->fail())
        {
          Cerr<<"Error while opening the file of resumption : " <<nom_fic<<finl;
          Process::exit();
        }

      if (fic_rep->eof())
        {
          Cerr << "Error in Champ_Fonc_reprise::reprendre" << finl;
          Cerr << "The resumption file does not exist" << finl;
          Cerr << "or could not be opened correctly." << finl;
          Process::exit();
        }
    }

  if (last_time)
    {
      if(format_rep == "single_hdf")
        {
#ifdef MPI_
          un_temps = get_last_time(input_data);
          fic_hdf.read_dataset("/sauv", Process::me(), input_data);
#endif
        }
      else
        {
          un_temps = get_last_time(fic_rep.valeur());
          fic_rep->close();
          fic_rep->ouvrir(nom_fic);
        }
      Cerr << "In the " << nom_fic << " file, we find the last time: " << un_temps << " and read the fields." << finl;
    }

  // Depuis la 1.5.5, lecture du format de sauvegarde
  if(format_rep != "single_hdf")
    fic_rep.valeur() >> ident_lu;
  else
    {
#ifdef MPI_
      input_data >> ident_lu;
#endif
    }

  int format_sauvegarde=0;
  if (ident_lu=="format_sauvegarde:")
    {
      if(format_rep != "single_hdf")
        fic_rep.valeur() >> format_sauvegarde; // Read the format
      else
        {
#ifdef MPI_
          input_data >> format_sauvegarde;
#endif
        }
    }
  else
    {
      // Version anterieure, on referme et on reouvre
      if (format_rep=="xyz")
        {
          fic_rep->close();
          fic_rep->ouvrir(nom_fic);
        }
      else
        {
          Cerr<<"This .sauv file is too old and the format is not supported anymore."<<finl;
          Process::exit();
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
  const Champ_Inc_base& ch_inc=ref_cast(Champ_Inc_base,ref_ch.valeur());
  vrai_champ_.typer(ch_inc.que_suis_je());
  vrai_champ_->associer_zone_dis_base(pb.domaine_dis().zone_dis(0));
  //vrai_champ_->fixer_nb_valeurs_temporelles(2);
  vrai_champ_->nommer(ch_inc.le_nom());
  vrai_champ_->fixer_nb_comp(ch_inc.nb_comp());
  //vrai_champ_->fixer_nb_valeurs_nodales(ch_inc.nb_valeurs_nodales());
  vrai_champ_->valeurs() = ch_inc.valeurs();

  nb_compo_ = ch_inc.nb_comp();
  if (ch=="FONCTION")
    if (fxyz.size()!=ch_inc.nb_comp())
      {
        Cerr<<"The number of components read " <<fxyz.size()<<" for the resume function"<<finl;
        Cerr<<"does not corresponds to the number of components "<<ch_inc.nb_comp()<<" of field target"<<finl;
        Process::exit();
      }

  // Lecture du fichier
  if (format_rep != "single_hdf")
    avancer_fichier(fic_rep.valeur(), nom_ident);
  else
    {
#ifdef MPI_
      avancer_fichier(input_data, nom_ident);
#endif
    }

  if (reprend_champ_moyen)
    {
      double tdeb,tfin;
      int n; // Nombre d'operateurs statistiques
      if (format_rep != "single_hdf")
        {
          fic_rep.valeur() >> n;
          fic_rep.valeur() >> tdeb;
          fic_rep.valeur() >> tfin;
          avancer_fichier(fic_rep.valeur(),nom_ident_champ_stat);
        }
      else
        {
#ifdef MPI_
          input_data >> n;
          input_data >> tdeb;
          input_data >> tfin;
          avancer_fichier(input_data,nom_ident_champ_stat);
#endif
        }

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
      if (format_rep != "single_hdf")
        champ_moyen.reprendre(fic_rep.valeur());
      else
        {
#ifdef MPI_
          champ_moyen.reprendre(input_data);
#endif
        }
      // On remplit le champ
      le_champ().valeurs() = champ_moyen.calculer_valeurs();
    }
  else if (reprend_modele_k_eps)
    {
      if (format_rep != "single_hdf")
        {
          avancer_fichier(fic_rep.valeur(), nom_ident_champ_keps);
          le_champ().reprendre(fic_rep.valeur());
        }
      else
        {
#ifdef MPI_
          avancer_fichier(input_data, nom_ident_champ_keps);
          le_champ().reprendre(input_data);
#endif
        }

    }
  else
    {
      if (format_rep != "single_hdf")
        le_champ().reprendre(fic_rep.valeur());
      else
        {
#ifdef MPI_
          le_champ().reprendre(input_data);
#endif
        }

    }

  if(format_rep == "single_hdf")
    {
#ifdef MPI_
      fic_hdf.close();
#endif
    }

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
              Process::exit();
            }
        }
      else if (nb_compo!=tab_valeurs.dimension_tot(1))
        {
          Cerr << "Error in the number of components of the function read:" << nb_compo << finl;
          Cerr << "The number of components of field " << le_champ().le_nom() << " is:" << tab_valeurs.dimension_tot(1) << finl;
          Process::exit();
        }
      for (int i=0; i<sz; i++)
        {
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
