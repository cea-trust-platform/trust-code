//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Leap_frog.h>
#include <Pred_Cor.h>
#include <RRK2.h>
#include <Schema_Adams_Bashforth_order_2.h>
#include <Schema_Adams_Bashforth_order_3.h>
#include <Schema_Adams_Moulton_order_2.h>
#include <Schema_Adams_Moulton_order_3.h>
#include <Schema_Euler_Implicite.h>
#include <Schema_Euler_explicite.h>
#include <Schema_RK_Williamson.h>
void instancie_src_Kernel_Schemas_Temps() {
Cerr << "src_Kernel_Schemas_Temps" << finl;
Leap_frog inst1;verifie_pere(inst1);
Pred_Cor inst2;verifie_pere(inst2);
RRK2 inst3;verifie_pere(inst3);
Schema_Adams_Bashforth_order_2 inst4;verifie_pere(inst4);
Schema_Adams_Bashforth_order_3 inst5;verifie_pere(inst5);
Schema_Adams_Moulton_order_2 inst6;verifie_pere(inst6);
Schema_Adams_Moulton_order_3 inst7;verifie_pere(inst7);
Schema_Euler_Implicite inst8;verifie_pere(inst8);
Schema_Euler_explicite inst9;verifie_pere(inst9);
RK2 inst10;verifie_pere(inst10);
RK3 inst11;verifie_pere(inst11);
RK4 inst12;verifie_pere(inst12);
}
