def read_libs_from_makeliba():
    import os
    f1=open(os.getenv("TRUST_ENV")+"/make.liba")
    a=' '.join(f1.readlines()).replace('\\','')
    # a+=syslibs
    libtrio=[]
    listL=[]
    for mot in a.split():
        # Commence par chercher -L car parfois -l present dans le chemin (ex: /x86_64-linux/) provoque un bon bug pour -l
        if (mot.find('-L')>-1):
            P=mot[2:]
            if not P in listL:
                listL.append(P)
                pass
            pass
        elif (mot.find('-l')>-1):
            libtrio.append(mot.replace('-l',''))
        elif (mot.find('/lib')>-1)and(mot[0]!='-'):
            libtrio.append(mot[mot.rfind('/lib')+4:-2])
            P=(mot[:mot.rfind('/lib')])
            if not P in listL:
                listL.append(P)
                pass
            pass

        pass
    list1=[]
    list2=[]
    for L in listL:
        if (L.rfind("$")>-1):
            list1.append(L.replace('$(','$ENV{').replace(')','}'))
        else:
            list2.append(L)
            pass
        pass
    print("libdeptrio ",libtrio)
    out=open(os.getenv("TRUST_ENV")+"/Cmake.libs","w")

    out.write('SET (list_path_libs '+' '.join(list1)+')\n')
    out.write('SET (list_path_sys '+' '.join(list2)+')\n')
    out.write('SET (list_libs '+' '.join(libtrio)+')\n')
    for var in ['list_path_libs','list_path_sys','list_libs']:
        out.write('MARK_AS_ADVANCED('+var+')\n')
        pass
    pass


def add_library_for_dir(s2):
    strout="add_library(obj_"+s2+" OBJECT  ${srcs} )\n"
    strout += "set(listlibs ${listlibs} " +s2 +" PARENT_SCOPE    )\n"
    strout += "add_custom_target(check_sources_"+s2+"   COMMAND check_sources.sh ${CMAKE_CURRENT_SOURCE_DIR} ) #COMMENT  \"checking code validity "+s2+"\" )\n" # MAIN_DEPENDENCY ${file} DEPENDS ${file})\n"
    strout+="add_DEPENDENCIES(obj_"+s2+" check_sources_"+s2+")\n"
    # strout += "  set(listobj ${listobj}  $<TARGET_OBJECTS:obj_"+s2[1] +"> PARENT_SCOPE    )\n"
    strout +="if(COMPIL_DYN)\n"
    strout += "  add_library("+s2 +" SHARED   $<TARGET_OBJECTS:obj_"+s2+"> )\n"
    strout+="  install(TARGETS "+s2+"  DESTINATION lib)\n"
    strout +="endif(COMPIL_DYN)\n"
    return strout

# ecrit les fichiers CmakeFile a partir des fichiers make.include


def lire_make_include(file,listlib):
    f=open(file)
    g=f.readlines()
    #g=replace(g,'\\\n','')
    t=""
    for s in g:
        t+=s.replace('\\\n','').replace('\t',' ')
    t=t.split('\n')
    if  1:
        strout="file(GLOB srcs [A-Z|a-z]*.cpp [A-Z|a-z]*.c [A-Z|a-z]*.f)\n"
    else:
        import os
        strout="file(GLOB srcs "

        l1=os.listdir(os.path.dirname(file))
        for l in l1:
            if ((l[-4:]==".cpp")or (l[-2:]==".c") or (l[-2:]==".f")):
                strout+=l+" "
                pass

            pass
        strout+=" )\n"
        pass

    #  strout+="add_definitions(${ADD_CPPFLAGS})\n"

    # print t,type(t)
    for s in t:
        #print s
        if s.find("Includes")>=0:
            #print s,s.find("Includes") ,s.split('-I')
            s=s.replace('-I.','-I${CMAKE_CURRENT_SOURCE_DIR}')
            s2=s.split('-I')
            inc_dir = " ".join(s2[1:])
            strout += "include_directories(" + inc_dir.replace('(','{').replace(')','}')+ ")\n" # .replace('TRUST_ROOT','Trio_SOURCE_DIR')+")\n"
            pass
        elif s.find("Lib")>=0:
            s2=s.split("/lib")
            listlib.append(s2[1])
            strout+=add_library_for_dir(s2[1])

            pass
        pass
    return strout,listlib
    pass
def ajoute_subdir(dirmake,dirsdict):
    strout=""
    for d in dirsdict[dirmake]:
        strout+="add_subdirectory("+d+")\n"
        pass
    return strout



def generate_cmake(listdirorg,sans_subdir,atelier):
    listlib=[]
    #listinclude=""
    import os
    if ((sans_subdir==1)):
        for dirmake in listdirorg:

            cmake=os.path.join(dirmake,'CMakeLists.txt')
            if os.path.exists(cmake):
                os.remove(cmake)
                pass
            pass
        pass

    if ((sans_subdir==0)):
        for dirmake in listdirorg:
            strout=""
            make=os.path.join(dirmake,'make.include')
            if os.path.exists(make):
                strout,listlib=lire_make_include(make,listlib)
                pass
            cmake=make.replace('make.include','CMakeLists.txt')
            out=open(cmake,'w')
            out.write(strout)
            pass
        # listinclude+=dirmake.replace('.','${Trio_SOURCE_DIR}')+" "

        if 1:
            out=open(os.path.join('src/MAIN','CMakeLists.txt'),'a')
            out.write(add_library_for_dir("main"))
            out.close()
        pass
        # global

        pass

    if (atelier) :
        out=open('CMakeLists.txt.trio','w')
    else:
        out=open('src/CMakeLists.txt','w')
    out.write('''# Tell the CMake makefile generator to not have rules depend on
# themselves.  This causes extra rebuilds when the include path
# changes from turning a kit on or off.
SET(CMAKE_SKIP_RULE_DEPENDENCY 0 )

#OPTION(kernelonly "kernel only" OFF)
#OPTION(microkernel "micro kernel only" OFF)
#OPTION(microAL "micro kernel + AL" OFF)
set(kernel "full" CACHE STRING "full,standard,numeric,micro" )

OPTION(VISUAL "pour MSVC" OFF)

IF(NOT VISUAL)
 # on prend les compilos choisis par configure
 # pour retirer des blancs inutils
 set (pathcc $ENV{TRUST_CC})
 string(STRIP ${pathcc} CMAKE_CXX_COMPILER)
 set (CMAKE_C_COMPILER $ENV{TRUST_cc})
 set (CMAKE_Fortran_COMPILER $ENV{TRUST_F77})
ENDIF(NOT VISUAL)

project(TRUST)



if(EXISTS ${CMAKE_SOURCE_DIR}/cmake.deps)
   include(cmake.deps)
endif()


''')
    out.write('set(listdir '+' '.join(listdirorg)+')\n')
    out.write('''


set(listdir_standard src/Kernel/Champs src/Kernel/Champs_dis src/Kernel/Cond_Lim src/Kernel/Framework src/Kernel/Geometrie src/Kernel/Geometrie/Decoupeur src/Kernel/ICoCo src/Kernel/MEDimpl src/Kernel/Math src/Kernel/Math/Matrices src/Kernel/Math/SolvSys src/Kernel/Operateurs src/Kernel/Schemas_Temps src/Kernel/Solveurs src/Kernel/Statistiques_temps src/Kernel/Utilitaires src/Kernel/VF/Champs src/Kernel/VF/Zones  src/MAIN)

set(listdir_micro src/Kernel/Math src/Kernel/Utilitaires  src/MAIN)
set(listdir_numeric src/Kernel/Math src/Kernel/Math/Matrices  src/Kernel/Math/SolvSys src/Kernel/Utilitaires src/MAIN  )


if (NOT ${kernel} STREQUAL "full")
  MESSAGE("Kernel ${kernel}")
  set (listdirmod ${listdir_${kernel}})
  set(ajout _${kernel}_kernel)
  set(special_srcs ${CMAKE_CURRENT_BINARY_DIR}/appel_c_mod_${kernel}.cpp)
  message("special srs ${special_srcs}")
  set (oo "")
  foreach(d ${listdir})
    LIST(FIND  listdirmod ${d} trouve)
    if (${trouve} EQUAL -1)
      STRING(REPLACE "/" "_" d2 ${d})
      set(oo "${oo} void instancie_${d2}() { } ")
    endif()
  endforeach(d)
  set(oo "${oo} \n")
  if (EXISTS  ${special_srcs})
    FILE(READ  ${special_srcs} org)
    string(COMPARE NOTEQUAL ${org} ${oo} update_src)
  else()
    set(update_src 1)
  endif()
  if (update_src)
    message( "update ${special_srcs}")
    FILE(WRITE ${special_srcs} "${oo}")
  endif(update_src)

  set(listdir ${listdirmod})
endif()

CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)




IF(NOT VISUAL)

enable_language(Fortran OPTIONAL)

SET (MPI_INCLUDE $ENV{MPI_INCLUDE})
SET (CUDA_INC_PATH $ENV{CUDA_INC_PATH})
SET (TRUST_ARCH $ENV{TRUST_ARCH})
SET (TRUST_ROOT $ENV{TRUST_ROOT})
# OPT sert a petsc pour l instant
#SET(OPT $ENV{OPT})
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(OPT "_opt")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release_avx")
    set(OPT "_opt_avx")
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OPT "")
elseif (CMAKE_BUILD_TYPE STREQUAL "Profil")
    set(OPT "_opt_pg")
elseif (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    set(OPT "_opt_gcov")
elseif (CMAKE_BUILD_TYPE STREQUAL "semi_opt")
    set(OPT "_semi_opt")
elseif (CMAKE_BUILD_TYPE STREQUAL "custom")
    set(OPT "_custom")
    include (${TRUST_ROOT}/env/Cmake.custom)
else(CMAKE_BUILD_TYPE STREQUAL "Release")
   message(FATAL_ERROR  "unknown build_type ${CMAKE_BUILD_TYPE} !, use -DCMAKE_BUILD_TYPE=Release,Debug,Profil,Coverage,semi_opt,Release_avx,custom")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")
message("Mode: ${OPT}")


SET(COMM $ENV{COMM})
SET (PETSC_ROOT $ENV{PETSC_ROOT} )
SET (METIS_ROOT $ENV{METIS_ROOT} )
SET (TRUST_LATAFILTER $ENV{TRUST_LATAFILTER})
SET (TRUST_ICOCOAPI $ENV{TRUST_ICOCOAPI})
SET (TRUST_MED_ROOT $ENV{TRUST_MED_ROOT})
SET (TRUST_MEDCOUPLING_ROOT $ENV{TRUST_MEDCOUPLING_ROOT})



#set(COMPIL_DYN FALSE)

set(LIBRARY_OUTPUT_PATH ${TRUST_ROOT}/lib)
set(EXECUTABLE_OUTPUT_PATH ${TRUST_ROOT}/exec)


''')

    # fait maintenant au configure et mis dans Cmake.env
    f0=open(os.getenv("TRUST_ENV")+"/make."+os.getenv("TRUST_ARCH_CC"))
    lignes=f0.readlines()
    for line in lignes:
        ligne=line.split()
        if (ligne[0]=="SYSLIBS"):
            syslibs=' '.join(ligne[2:])
        elif (ligne[0]== "Defines"):
            defines= ' '.join(line.replace('(','{').replace(')','}').split()[2:])
            # out.write('# SET(ADD_CPPFLAGS '+ defines +' )\n')
        elif (ligne[0]=="CppFlags"):
            liste_f=[]
            for m in ligne[2:]:
                if (m!="-g")and (m!="-fPICuuuuuuuu") and (m.find('$')<0):
                    liste_f.append(m)
                    pass
                pass
            #print liste_f
            # out.write('# SET( CMAKE_CXX_FLAGS "'+' '.join(liste_f)+'" CACHE STRING "comme dans trio" FORCE )\n')
            pass
    out.write('include (${TRUST_ROOT}/env/Cmake.env)\n')
    # out.write('# set (CMAKE_CONFIGURATION_TYPES "Release Debug Profil Coverage semi_opt"  CACHE STRING "comme dans trio" FORCE )\n')

    out.write('include (${TRUST_ROOT}/env/Cmake.libs)\n')
    out.write('\n')
    out.write('FOREACH (liba ${list_libs})\n')
    out.write('''        set (staticlib lib${liba}.a )
         find_library( lib${liba} NAMES ${staticlib} ${liba} PATHS ${list_path_libs} NO_DEFAULT_PATH )
        if (${lib${liba}} STREQUAL lib${liba}-NOTFOUND)
           message("${liba} librairie systeme ?")
           find_library( lib${liba} NAMES ${liba} PATHS ${list_path_sys} )
        endif(${lib${liba}} STREQUAL lib${liba}-NOTFOUND)
        # pour supermuc on cherche ligfortran.so.3  en dur
        if (${liba} STREQUAL gfortran)
          if (${lib${liba}} STREQUAL libgfortran-NOTFOUND)
           find_library( lib${liba} NAMES  libgfortran.so.3 PATHS ${list_path_sys} )
          endif()
        endif()
        MARK_AS_ADVANCED( lib${liba})
        SET(libs ${libs} ${lib${liba}})
ENDFOREACH (liba )
''')

    # out.write('find_library( gfortran lib gfortran)\n')
    # out.write('SET (gfortran gfortran)\n')

    #    out.write('# MESSAGE("Monosrc ${Monosrc} ")\n')
    out.write('''

ELSE(NOT VISUAL)
  include(windows/CMake.win)
ENDIF(NOT VISUAL)


''')
    out.write('\n\nSTRING( TOUPPER ${CMAKE_BUILD_TYPE} BUILD_CONFIG)\nstring(STRIP ${CMAKE_EXE_LINKER_FLAGS_${BUILD_CONFIG}} linker_flag )\nSET(syslib ${libs} ${linker_flag} )\n\n')
    out.write('include_directories(${METIS_ROOT}/include ${TRUST_MED_ROOT}/include ${TRUST_MEDCOUPLING_ROOT}/include ${MPI_INCLUDE} ${TRUST_ROOT}/lib/src/LIBAMGX/AmgXWrapper/include ${TRUST_ROOT}/lib/src/LIBAMGX/AmgX/include ${CUDA_INC_PATH} ${PETSC_ROOT}/${TRUST_ARCH}${OPT}/include ${TRUST_LATAFILTER}/include ${TRUST_ICOCOAPI}/include ${TRUST_ROOT}/lib/src/LIBOSQP/include ${TRUST_ROOT}/lib/src/LIBVC/include )\n')
    out.write('add_definitions(${ADD_CPPFLAGS})\n')

    out.write('''


 set(trio TRUST${COMM}${OPT})
 set(libtrio_name TRUST${COMM}${ajout}${OPT})
 set(libtrio lib${libtrio_name})

IF(NOT ATELIER)
FOREACH(dir ${listdir})
   add_subdirectory(../${dir})
 ENDFOREACH(dir)

if(NOT COMPIL_DYN)
  set(my_listobj)
  foreach(_obj IN LISTS listlibs)
    LIST (APPEND my_listobj  $<TARGET_OBJECTS:obj_${_obj}>)
  endforeach()

 add_library(${libtrio} STATIC  ${special_srcs} ${my_listobj} )
 set_target_properties(${libtrio} PROPERTIES OUTPUT_NAME ${libtrio_name} PREFIX "" )
 install(TARGETS ${libtrio} DESTINATION lib)
else(NOT COMPIL_DYN)
 set(libtrio ${listlibs})

endif(NOT COMPIL_DYN)

 # on ne produit pas d executable en mode partiel
 IF((  "${ajout}" STREQUAL "" ) OR ($ENV{FORCE_LINK}))
   add_executable (${trio} MAIN/the_main.cpp MAIN/mon_main.cpp ${special_srcs}  )
   include_directories(Kernel/Utilitaires MAIN Kernel/Math)
   target_link_libraries(${trio} ${libtrio} ${syslib})
   install (TARGETS ${trio} DESTINATION exec)


   ENABLE_TESTING()
   SET (TRUST_TESTS $ENV{TRUST_TESTS})
   #file(GLOB_RECURSE LML  RELATIVE ${TRUST_TESTS} FOLLOW_SYMLINKS ${TRUST_TESTS}/[A-Z|a-z]*.lml.gz )
   file(GLOB_RECURSE LML   FOLLOW_SYMLINKS ${TRUST_TESTS}/[A-Z|a-z|0-9]*.lml.gz )
#message("${ajout} iiiii ${LML} oooo")
LIST (LENGTH  LML NN)
message("go ${NN} tests")
foreach(f ${LML})
string(REPLACE .lml.gz "" l2 ${f} )
string(FIND ${f} /  ii  REVERSE )
string(SUBSTRING ${l2} ${ii} -1 l3 )
string(SUBSTRING ${l3} 1 -1 l3 )
ADD_TEST(NAME ${l3} COMMAND trust -exe $<TARGET_FILE:${trio}> -check ${l3}  )
endforeach(f )

 ENDIF()


ELSE(NOT ATELIER)
FOREACH(dir ${listdir})
   include_directories(${TRUST_ROOT}/${dir})
 ENDFOREACH(dir)
ENDIF(NOT ATELIER)


''')


    print("libtrio ",listlib)
    pass


#lire_make_include(file)
import sys

if  __name__ == '__main__':
    listdirorg=[]
    args=sys.argv[1:]
    sans_subdir=0
    atelier=0
    if (len(args)>0) and (args[0]=="-atelier"):
        import os
        sans_subdir=1
        rep_dev=os.getenv("rep_dev")
        os.chdir(rep_dev)
        # g=open("dummy.f","a"); g.close()
        #g=open("dummy.cpp","a"); g.close()
        # os.system("cree_info_atelier.sh")
        atelier=1
        args=args[1:]
        pass
    if (len(args)>0) and (args[0]=="-avec_subdir"):
        sans_subdir=0
        args=args[1:]
        pass
    if (len(args)>0) and (args[0]=="-sans_subdir"):
        sans_subdir=1
        args=args[1:]
        pass
    if len(args)==0: # on lit dans rep.TRUST
        import os
        ENV=os.getenv("TRUST_ROOT")
        f=open(ENV+"/env/rep.TRUST","r")

        titi=1
        while (titi):
            titi=f.readline().strip()
            if (titi!=''):
                args.append(titi)
                pass
            pass
        pass


    for make in args:
        from os.path import dirname,basename
        if (basename(make)=='make.include'):
            s=dirname(make)
        else:
            s=make
            pass
        s2=s.replace('./','')
        listdirorg.append(s2)

        pass
    # print listdir
    generate_cmake(listdirorg,sans_subdir,atelier)
    if (atelier):
        out=open('CMakeLists.txt','w')
        out.write('''
cmake_minimum_required(VERSION 2.8)

include_directories(.)
set(ATELIER  ON)

include(CMakeLists.txt.trio)

include_directories(${TRUST_ROOT}/include/backward)


Project(atelier)
SET(CMAKE_SKIP_RULE_DEPENDENCY 0)


file(GLOB srcs [A-Z|a-z]*.cpp  [A-Z|a-z]*.cxx [A-Z|a-z]*.f [A-Z|a-z]*.c [A-Z|a-z]*.h  [A-Z|a-z]*.hpp  [A-Z|a-z]*.hxx)
if (srcs_atelier)
  # set(srcs_atelier "")
else()
 set(srcs_atelier "not_init")
endif()
if ("${srcs}" STREQUAL "${srcs_atelier}")
else()
  set(srcs_atelier ${srcs}  CACHE STRING "fichiers atelier" FORCE )
  #execute_process(COMMAND "${CMAKE_MAKE_PROGRAM} depend" )
  # GF bidouille importante pour forcer les dependances si ajout d un fichier .h
  execute_process(COMMAND find . -name depend.internal -exec rm {} \; RESULT_VARIABLE toto ERROR_VARIABLE err )
  message("New files in rep_dev"  )
  set(LATACOMMON ThirdPart/src/LIBLATAFILTER/commun_triou)
  if ((${kernel} STREQUAL "micro") OR (${kernel} STREQUAL "numeric"))
  set(LATACOMMON "")
  endif()
  execute_process(COMMAND env rep_dev=${CMAKE_SOURCE_DIR} ${TRUST_ROOT}/bin/KSH/cree_info_atelier.sh RESULT_VARIABLE toto OUTPUT_VARIABLE toto2 ERROR_VARIABLE error )
  message( ${toto2} ${error} "status " ${toto} )
  execute_process(COMMAND python ${TRUST_ROOT}/bin/KSH/Cherchefileatelier.py ${listdir} ${LATACOMMON} %% ${srcs} RESULT_VARIABLE toto OUTPUT_VARIABLE srcdeps_new ERROR_VARIABLE error )
  message(  ${error} "status " ${toto} )
  if (${toto})
    message(FATAL_ERROR "contact trust@cea.fr")
  endif()
  set(srcdeps ${srcdeps_new} CACHE STRING "ficher de trio dependant de l atelier " FORCE )
endif()
list(LENGTH srcs_atelier l1)
list(LENGTH srcdeps l2)

message("${l1} file(s) in rep_dev, ${l2} file(s) of TRUST depends of rep_dev")
#message("yes ${error} toto ${toto} titi ${srcdeps}")
message("Utilisation de la librairie : ${libtrio_name} " )
find_library( libTrio NAMES ${libtrio_name}.a PATHS ${TRUST_ROOT}/lib NO_DEFAULT_PATH )
if(EXISTS ${CMAKE_SOURCE_DIR}/cmake.deps)
include(cmake.deps)
endif()

add_library(list_obj  OBJECT ${srcs} ${srcdeps} ${CMAKE_SOURCE_DIR}/exec${OPT}/info_atelier.cpp )

#add_executable(exe ${srcs} ${srcdeps})
add_executable(exe  $<TARGET_OBJECTS:list_obj>)

target_link_libraries(exe ${libdeps} ${libTrio}  ${syslib})

#set(PROGRAM $ENV{PROGRAM})
''')
        prog= os.getenv("PROGRAM")
        # print "ici",prog

        if (prog):
            out.write('set(EXECUTABLE_OUTPUT_PATH %s) \n' % os.path.dirname(prog))
            out.write('set(PROGRAM %s${OPT})\n '% os.path.basename(prog))
        else:
            out.write('''
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exec${OPT})
set(PROGRAM ${trio})
''')
            pass
        out.write('''
install(TARGETS exe DESTINATION ${EXECUTABLE_OUTPUT_PATH})
set_target_properties(exe PROPERTIES  OUTPUT_NAME  ${PROGRAM} )

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exec${OPT} )


add_library(module SHARED EXCLUDE_FROM_ALL $<TARGET_OBJECTS:list_obj>  )
target_link_libraries(module ${libdeps} ${libTrio} ${syslib})
set_target_properties(module PROPERTIES OUTPUT_NAME _TRUSTModule${OPT} PREFIX "" )

add_library(lib SHARED EXCLUDE_FROM_ALL  $<TARGET_OBJECTS:list_obj>  )
# target_link_libraries(lib ${libTrio} ${syslib})
set_target_properties(lib PROPERTIES OUTPUT_NAME libatelier${OPT} PREFIX "" )

add_executable(dyn EXCLUDE_FROM_ALL ${TRUST_ROOT}/src/MAIN/the_main.cpp )
target_link_libraries(dyn lib ${libdeps} ${libTrio}  ${syslib})
set_target_properties(dyn PROPERTIES OUTPUT_NAME ${PROGRAM}_dyn PREFIX "" EXECUTABLE_OUTPUT_PATH "" )


ENABLE_TESTING()
SET (TRUST_TESTS $ENV{TRUST_TESTS})
   # file(GLOB_RECURSE LML  RELATIVE ${TRUST_TESTS} FOLLOW_SYMLINKS ${TRUST_TESTS}/[A-Z|a-z]*.lml.gz )
   file(GLOB_RECURSE LML   FOLLOW_SYMLINKS ${TRUST_TESTS}/[A-Z|a-z|0-9]*.lml.gz )
# message("iiiii ${LML} oooo ${TRUST_TESTS}")
LIST (LENGTH  LML NN)
message("go ${NN} tests")
foreach(f ${LML})
string(REPLACE .lml.gz "" l2 ${f} )
string(FIND ${f} /  ii  REVERSE )
string(SUBSTRING ${l2} ${ii} -1 l3 )
string(SUBSTRING ${l3} 1 -1 l3 )
ADD_TEST(NAME ${l3} COMMAND trust -exe $<TARGET_FILE:exe> -check ${l3}  )
endforeach(f )
''')
        out.close()
    pass
