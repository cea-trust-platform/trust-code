/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Frontiere.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Frontiere_included
#define Frontiere_included


#include <Faces.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Frontiere.
//    Une Frontiere decrit une partie de la frontiere d'une Zone,
//    elle possede un Nom, elle porte des Faces et a une Zone associee.
//    Une Frontiere peut etre specialisee en un Bord, un Joint un
//    Raccord ou une Face Interne.
// .SECTION voir aussi
//    Zone Bord Joint Raccord Faces_Interne
//////////////////////////////////////////////////////////////////////////////
class Frontiere : public Objet_U
{
  Declare_base(Frontiere);

public:

  // Entree& lire(Entree&);
  // Sortie& ecrire(Sortie&) const;
  void associer_zone(const Zone&);
  const Zone& zone() const;
  Zone& zone();
  void nommer(const Nom&);
  void ajouter_faces(const IntTab&);
  void typer_faces(const Motcle&);
  void typer_faces(const Type_Face&);
  inline const Faces& faces() const;
  inline Faces& faces();
  IntTab& les_sommets_des_faces();
  const IntTab& les_sommets_des_faces() const;
  inline int nb_faces() const;
  inline void dimensionner(int);
  inline const Nom& le_nom() const;
  void renum(const IntVect&);
  void add(const Frontiere& );
  inline int nb_faces_virt(int i) const;
  inline int face_virt(int i) const;

  inline int num_premiere_face() const;
  inline void fixer_num_premiere_face(int );
  inline const ArrOfInt& get_faces_virt() const;
  inline ArrOfInt& get_faces_virt();
  inline const double& get_aire() const
  {
    return aire_;
  };
  inline void set_aire(double& aire)
  {
    aire_ = aire;
  };

  virtual void creer_tableau_faces(Array_base&, Array_base::Resize_Options opt = Array_base::COPY_INIT) const;
  virtual void trace_elem(const DoubleTab&, DoubleTab&) const;
  virtual void trace_face(const DoubleTab&, DoubleTab&) const;
  virtual void trace_face(const DoubleVect&, DoubleVect&) const;
private :

  Nom nom;
  Faces les_faces;
  REF(Zone) la_zone;
  ArrOfInt faces_virt;
  int num_premiere_face_;
  double aire_;
};


// Description:
//    Renvoie le nombre de faces de la frontiere.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de faces de la frontiere
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Frontiere::nb_faces() const
{
  return les_faces.nb_faces();
}


// Description:
//    Dimensionne la frontiere, i.e. fixe
//    son nombre de faces.
// Precondition:
// Parametre: int i
//    Signification: le nombre de faces a donner a la frontiere
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la frontiere a i faces.
inline void Frontiere::dimensionner(int i)
{
  les_faces.dimensionner(i);
}


// Description:
//    Renvoie le nom de la frontiere.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom de la frontiere
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Nom& Frontiere::le_nom() const
{
  return nom;
}

// Description:
//    Renvoie les faces de la frontiere
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Faces&
//    Signification: les faces de la frontiere
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Faces& Frontiere::faces() const
{
  return les_faces;
}

// Description:
//    Renvoie les faces de la frontiere
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Faces&
//    Signification: les faces de la frontiere
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Faces& Frontiere::faces()
{
  return les_faces;
}

inline int Frontiere::nb_faces_virt(int i) const
{
  return faces_virt.size_array();
}
inline int Frontiere::face_virt(int i) const
{
  return faces_virt(i);
}

inline int Frontiere::num_premiere_face() const
{
  return num_premiere_face_ ;
}
inline void Frontiere::fixer_num_premiere_face(int i)
{
  num_premiere_face_ = i;
}

inline const ArrOfInt& Frontiere::get_faces_virt() const
{
  return faces_virt;
}

inline ArrOfInt& Frontiere::get_faces_virt()
{
  return faces_virt;
}
#endif
