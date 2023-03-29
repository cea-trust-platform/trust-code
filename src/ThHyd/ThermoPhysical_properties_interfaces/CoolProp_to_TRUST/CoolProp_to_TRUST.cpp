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

#include <CoolProp_to_TRUST.h>

CoolProp_to_TRUST::~CoolProp_to_TRUST()
{
#ifdef HAS_COOLPROP
  if (fluide) delete fluide;
#endif
}

double CoolProp_to_TRUST::tppi_get_p_min() const
{
#ifdef HAS_COOLPROP
  return fluide->p_triple();
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_p_max() const
{
#ifdef HAS_COOLPROP
  return fluide->pmax();
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_h_min() const
{
#ifdef HAS_COOLPROP
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " is not implemented ... Call 911 for help ! " << finl;
  throw;
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_h_max() const
{
#ifdef HAS_COOLPROP
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " is not implemented ... Call 911 for help ! " << finl;
  throw;
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_T_min() const
{
#ifdef HAS_COOLPROP
  return fluide->Tmin();
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_T_max() const
{
#ifdef HAS_COOLPROP
  return fluide->Tmax();
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_rho_min() const
{
#ifdef HAS_COOLPROP
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " is not implemented ... Call 911 for help ! " << finl;
  throw;
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_rho_max() const
{
#ifdef HAS_COOLPROP
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " is not implemented ... Call 911 for help ! " << finl;
  throw;
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_p_crit() const
{
#ifdef HAS_COOLPROP
  return fluide->p_critical();
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_h_crit() const
{
#ifdef HAS_COOLPROP
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " is not implemented ... Call 911 for help ! " << finl;
  throw;
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_T_crit() const
{
#ifdef HAS_COOLPROP
  return fluide->T_critical();
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}

double CoolProp_to_TRUST::tppi_get_rho_crit() const
{
#ifdef HAS_COOLPROP
  return fluide->rhomass_critical();
#else
  Cerr << "CoolProp_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the CoolProp library !!! " << finl;
  throw;
#endif
}














