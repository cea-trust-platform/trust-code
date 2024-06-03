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
#ifndef Raccord_distant_homogene_included
#define Raccord_distant_homogene_included

#include <Raccord.h>
class Domaine_dis_base;
class Raccord_distant_homogene : public Raccord_distant
{
  Declare_instanciable_sans_constructeur(Raccord_distant_homogene);
public:
  Raccord_distant_homogene();
  inline IntTab& Tab_Envoi();
  inline ArrOfInt& Tab_Recep();
  inline Nom& nom_frontiere_voisine();
  inline int& est_initialise()
  {
    return est_initialise_;
  };
  inline double& e() ;
  inline const int& est_initialise() const;
  inline const double& e() const;
  void trace_elem_distant(const DoubleTab&, DoubleTab&) const override;
  void trace_face_distant(const DoubleTab&, DoubleTab&) const override;
  void trace_face_distant(const DoubleVect&, DoubleVect&) const override;
  void completer();
  void initialise(const Frontiere&, const Domaine_dis_base&, const Domaine_dis_base&);
private :
  inline const IntTab& Tab_Envoi() const;
  inline const ArrOfInt& Tab_Recep() const;
  IntTab tab_envoi;
  ArrOfInt tab_recep;
  Nom nom_frontiere_voisine_;
  int est_initialise_;
  double e_;
  // Tableaux de travail:
  ArrOfInt send_pe_list_;
  ArrOfInt recv_pe_list_;
};
/*! @brief
 *
 */
inline IntTab& Raccord_distant_homogene::Tab_Envoi()
{
  return tab_envoi;
}
inline const IntTab& Raccord_distant_homogene::Tab_Envoi() const
{
  return tab_envoi;
}


/*! @brief
 *
 */
inline ArrOfInt& Raccord_distant_homogene::Tab_Recep()
{
  return tab_recep;
}
inline const ArrOfInt& Raccord_distant_homogene::Tab_Recep() const
{
  return tab_recep;
}


/*! @brief
 *
 */
inline Nom& Raccord_distant_homogene::nom_frontiere_voisine()
{
  return nom_frontiere_voisine_;
}

inline double& Raccord_distant_homogene::e()
{
  return e_;
}
inline const int& Raccord_distant_homogene::est_initialise() const
{
  return est_initialise_;
}

inline const double& Raccord_distant_homogene::e() const
{
  return e_;
}
#endif
