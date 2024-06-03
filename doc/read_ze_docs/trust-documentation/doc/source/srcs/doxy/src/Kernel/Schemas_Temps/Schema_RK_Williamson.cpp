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

#include <Schema_RK_Williamson.h>

// XD runge_kutta_ordre_2 schema_temps_base runge_kutta_ordre_2 -1 This is a low-storage Runge-Kutta scheme of second order that uses 2 integration points. The method is presented by Williamson (case 1) in https://www.sciencedirect.com/science/article/pii/0021999180900339
Implemente_instanciable(RK2,"Runge_Kutta_ordre_2",TRUSTSchema_RK<Ordre_RK::DEUX_WILLIAMSON>);
Sortie& RK2::printOn(Sortie& s) const { return  TRUSTSchema_RK<Ordre_RK::DEUX_WILLIAMSON>::printOn(s); }
Entree& RK2::readOn(Entree& s) { return TRUSTSchema_RK<Ordre_RK::DEUX_WILLIAMSON>::readOn(s) ; }

// XD runge_kutta_ordre_3 schema_temps_base runge_kutta_ordre_3 -1 This is a low-storage Runge-Kutta scheme of third order that uses 3 integration points. The method is presented by Williamson (case 7) in https://www.sciencedirect.com/science/article/pii/0021999180900339
Implemente_instanciable(RK3,"Runge_Kutta_ordre_3",TRUSTSchema_RK<Ordre_RK::TROIS_WILLIAMSON>);
Sortie& RK3::printOn(Sortie& s) const { return  TRUSTSchema_RK<Ordre_RK::TROIS_WILLIAMSON>::printOn(s); }
Entree& RK3::readOn(Entree& s) { return TRUSTSchema_RK<Ordre_RK::TROIS_WILLIAMSON>::readOn(s) ; }

// XD runge_kutta_ordre_4 schema_temps_base runge_kutta_ordre_4 -1 This is a low-storage Runge-Kutta scheme of fourth order that uses 3 integration points. The method is presented by Williamson (case 17) in https://www.sciencedirect.com/science/article/pii/0021999180900339
Implemente_instanciable(RK4, "Runge_Kutta_ordre_4|Runge_Kutta_ordre_4_D3P", TRUSTSchema_RK<Ordre_RK::QUATRE_WILLIAMSON>);
Sortie& RK4::printOn(Sortie& s) const { return TRUSTSchema_RK<Ordre_RK::QUATRE_WILLIAMSON>::printOn(s); }
Entree& RK4::readOn(Entree& s) { return TRUSTSchema_RK<Ordre_RK::QUATRE_WILLIAMSON>::readOn(s); }
