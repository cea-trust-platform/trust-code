//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Leap_frog.h>
#include <Pred_Cor.h>
#include <RK3.h>
#include <RK4_D3P.h>
#include <RRK2.h>
#include <Schema_Adams_Bashforth_order_2.h>
#include <Schema_Adams_Bashforth_order_3.h>
#include <Schema_Adams_Moulton_order_2.h>
#include <Schema_Adams_Moulton_order_3.h>
#include <Schema_Euler_Implicite.h>
#include <Schema_Euler_explicite.h>
void instancie_src_Kernel_Schemas_Temps() {
Cerr << "src_Kernel_Schemas_Temps" << finl;
Leap_frog inst1;verifie_pere(inst1);
Pred_Cor inst2;verifie_pere(inst2);
RK3 inst3;verifie_pere(inst3);
RK4_D3P inst4;verifie_pere(inst4);
RRK2 inst5;verifie_pere(inst5);
Schema_Adams_Bashforth_order_2 inst6;verifie_pere(inst6);
Schema_Adams_Bashforth_order_3 inst7;verifie_pere(inst7);
Schema_Adams_Moulton_order_2 inst8;verifie_pere(inst8);
Schema_Adams_Moulton_order_3 inst9;verifie_pere(inst9);
Schema_Euler_Implicite inst10;verifie_pere(inst10);
Schema_Euler_explicite inst11;verifie_pere(inst11);
}
