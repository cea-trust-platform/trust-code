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
// File:        Cal_std.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////


//// calculer_flux_faces_echange_externe_impose
//

inline int CLASSNAME::calculer_flux_faces_echange_externe_impose() const
{
  return 1;
}


//// calculer_flux_faces_echange_global_impose
//

inline int CLASSNAME::calculer_flux_faces_echange_global_impose() const
{
  return 1;
}


//// calculer_flux_faces_entree_fluide
//

inline int CLASSNAME::calculer_flux_faces_entree_fluide() const
{
  return 1;
}


//// calculer_flux_faces_paroi
//

inline int CLASSNAME::calculer_flux_faces_paroi() const
{
  return 1;
}


//// calculer_flux_faces_paroi_adiabatique
//

inline int CLASSNAME::calculer_flux_faces_paroi_adiabatique() const
{
  return 0;
}


//// calculer_flux_faces_paroi_defilante
//

inline int CLASSNAME::calculer_flux_faces_paroi_defilante() const
{
  return 0;
}


//// calculer_flux_faces_paroi_fixe
//

inline int CLASSNAME::calculer_flux_faces_paroi_fixe() const
{
  return 1;
}


//// calculer_flux_faces_sortie_libre
//

inline int CLASSNAME::calculer_flux_faces_sortie_libre() const
{
  return 0;
}


//// calculer_flux_faces_symetrie
//

inline int CLASSNAME::calculer_flux_faces_symetrie() const
{
  return 0;
}


//// calculer_flux_faces_periodique
//

inline int CLASSNAME::calculer_flux_faces_periodique() const
{
  return 1;
}

//// calculer_flux_faces_NSCBC
//

inline int CLASSNAME::calculer_flux_faces_NSCBC() const
{
  return 0;
}


