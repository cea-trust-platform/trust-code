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
// File:        med_nul.h
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////
#ifndef MED_H
#define MED_H

#define PCLINUX

#define MED_NULL       (void *) NULL
#define MED_MAX_PARA        20

#define MED_TAILLE_DESC 200
#define MED_TAILLE_IDENT  8
#define MED_TAILLE_NOM   32
#define MED_TAILLE_LNOM  80
#define MED_TAILLE_PNOM   8

typedef enum {MED_FULL_INTERLACE,
              MED_NO_INTERLACE
             }  med_mode_switch;

typedef enum {MED_LECT,MED_ECRI,MED_REMP} med_mode_acces;

typedef enum {MED_MAILLE, MED_FACE, MED_ARETE, MED_NOEUD} med_entite_maillage;

typedef enum {MED_COOR, MED_CONN, MED_NOM, MED_NUM, MED_FAM} med_table;

typedef enum {MED_REEL64=6, MED_INT32=24,MED_INT64=26, MED_INT} med_type_champ;

#define MED_NBR_GEOMETRIE_MAILLE 15
#define MED_NBR_GEOMETRIE_FACE 4
#define MED_NBR_GEOMETRIE_ARETE 2
typedef enum {MED_POINT1=1, MED_SEG2=102, MED_SEG3=103, MED_TRIA3=203,
              MED_QUAD4=204, MED_TRIA6=206,MED_QUAD8=208, MED_TETRA4=304,
              MED_PYRA5=305, MED_PENTA6=306, MED_HEXA8=308, MED_TETRA10=310,
              MED_PYRA13=313, MED_PENTA15=315, MED_HEXA20=320,  MED_POLYGONE=400, MED_POLYEDRE=500, MED_NONE=0
             }
med_geometrie_element;

typedef enum {MED_NOD, MED_DESC} med_connectivite ;

typedef enum {MED_CART, MED_CYL, MED_SPHER} med_repere;

typedef enum {MED_FAUX, MED_VRAI} med_booleen ;

typedef enum {MED_GROUPE, MED_ATTR, MED_FAMILLE} med_dim_famille;

typedef enum {MED_COMP, MED_DTYPE} med_dim_champ;

typedef enum {MED_HDF_VERSION, MED_VERSION, MED_FICH_DES} med_fich_info;

#define MED_NOPG   1                   /* -> pas de point de Gauss                    */
#define MED_NOPFL  ""                  /* -> pas de profils utilisateur               */
#define MED_NOPFLi "                                "  /* Variable Interne                      */
#define MED_NOPF   0                   /* -> pas de profils pour _MEDdataseNnumEcrire */
#define MED_NOPDT -1                   /* rem: pas de pas de temps negatifs           */
#define MED_NONOR -1                   /* rem: pas de numero ordre negatif                 */
#define MED_DIM1   1                   /* PAS */
#define MED_ALL    0

#if defined(SUN4SOL2) || defined(PCLINUX) || defined(OSF1) || defined(IRIX64_32) || defined(RS6000)
/* interface C/FORTRAN */
/* this true only with g77 and gcc : we must change it to use directly NOMF_... and INT32 or INT64 - it will be more simple to understand and to use ! */
#define NOMF_POST_UNDERSCORE
/*
  piratage typedef hdf
*/
typedef unsigned long      hsize_t;
typedef signed long      hssize_t;
typedef int hid_t;
typedef int herr_t;



/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef int            med_int;
typedef double         med_float;
#endif

#if defined(HP9000)
/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef int            med_int;
typedef double         med_float;
#endif

#if defined(IRIX64)
#define NOMF_POST_UNDERSCORE

/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef long           med_int;
typedef double         med_float;
#endif


#if defined(PPRO_NT)
/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef int               med_int;
typedef double         med_float;
#endif


#if defined(NOMF_PRE_UNDERSCORE) && defined(NOMF_POST_UNDERSCORE)
#   define NOMF(x)     _##x##_
#endif
#if defined(NOMF_PRE_UNDERSCORE) && !defined(NOMF_POST_UNDERSCORE)
#   define NOMF(x)     _##x
#endif
#if !defined(NOMF_PRE_UNDERSCORE) && defined(NOMF_POST_UNDERSCORE)
#   define NOMF(x)     x##_
#endif
#if !defined(NOMF_PRE_UNDERSCORE) && !defined(NOMF_POST_UNDERSCORE)
#   define NOMF(x)     x
#endif

#include <med_proto_nul.h>

#endif  /* MED_H */
