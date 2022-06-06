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

#include <Terme_Source_Constituant.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>

// Description:
//    Lit le terme de puissance a partir
//    d'un flot d'entree.
//    Lit uniquement un champ donne representant la_puissance.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Terme_Source_Constituant::lire_donnees(Entree& is)
{
  Cerr << "Lecture du Champ Source pour le terme source du/des constituants" << finl;
  is >> la_source_constituant;
}

int Terme_Source_Constituant::completer(const Champ_Inc_base& inco)
{
  colw_ = -1;
  for (int i = 0; i < inco.nb_comp(); ++i)
    colw_ = std::max(colw_, inco.noms_compo()[i].longueur());
  return colw_;
}

void Terme_Source_Constituant::ouvrir_fichier(const Equation_base& eq, const Nom& out, const Nom& qsj, const Nom& description, SFichier& os,const Nom& type, const int flag) const
{
  // flag nul on n'ouvre pas le fichier
  if (flag==0)
    return ;

  const Probleme_base& pb = eq.probleme();
  const Schema_Temps_base& sch = pb.schema_temps();

  Nom nomfichier(out);
  if (type!="") nomfichier+=(Nom)"_"+type;
  nomfichier+=".out";

  // On cree le fichier a la premiere impression avec l'en tete
  const int wcol = std::max(colw_, sch.wcol());
  os.set_col_width(wcol);
  if (sch.nb_impr()==1 && !pb.reprise_effectuee())
    {
      const int gnuplot_header = sch.gnuplot_header();
      os.ouvrir(nomfichier);
      SFichier& fic=os;
      fic << (Nom)"# Printing of the source term " + qsj + " of the equation "+ eq.que_suis_je() +" of the problem "+ eq.probleme().le_nom() << finl;
      fic << "# " << description << finl;
      if (!gnuplot_header) fic << "#";
      os.set_col_width(wcol - !gnuplot_header);
      fic.add_col("Time");
      os.set_col_width(wcol);
      for (int i = 0; i < eq.inconnue()->nb_comp(); ++i)
        fic.add_col(eq.inconnue()->noms_compo()[i].getChar());
      fic << finl;
    }
  // Sinon on l'ouvre
  else
    {
      os.ouvrir(nomfichier,ios::app);
    }
  os.precision(sch.precision_impr());
  os.setf(ios::scientific);
}
