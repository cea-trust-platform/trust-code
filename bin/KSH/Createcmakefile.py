""" Generate CMakeLists.txt used to compile TRUST

One day we will have pure CMake rather than this ...
"""

import sys
import os

# Common header for all CMakeLists.txt (i.e. TRUST & Baltiks)
common_hdr = '''# 
# This file was generated automatically by bin/KSH/Createcmakefile.py
# !!Do not edit directly!! - changes will be overwritten!
#

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
'''

def read_libs_from_makeliba():
    """ From the make.liba file, generate Cmake.libs
    """
    f1=open(os.getenv("TRUST_ENV")+"/make.liba")
    a=' '.join(f1.readlines()).replace('\\','')
    libtrio, listL = [], []
    for mot in a.split():
        # Commence par chercher -L car parfois -l present dans le chemin (ex: /x86_64-linux/) provoque un bon bug pour -l
        if (mot.find('-L')>-1):
            P=mot[2:]
            if not P in listL:
                listL.append(P)
        elif (mot.find('-l')>-1):
            libtrio.append(mot.replace('-l',''))
        elif (mot.find('/lib')>-1)and(mot[0]!='-'):
            libtrio.append(mot[mot.rfind('/lib')+4:-2])
            P=(mot[:mot.rfind('/lib')])
            if not P in listL:
                listL.append(P)
    list1, list2 = [], []
    for L in listL:
        if (L.rfind("$")>-1):
            list1.append(L.replace('$(','$ENV{').replace(')','}').replace('ENV{OPT','{OPT'))
        else:
            list2.append(L)
    with open(os.getenv("TRUST_ENV")+"/Cmake.libs","w") as out:
        out.write('set(list_path_libs ' + '\n'.join(list1)+'\n)\n')
        out.write('set(list_path_sys ' + '\n'.join(list2)+'\n)\n')
        out.write('set(list_libs ' + '\n'.join(libtrio)+'\n)\n')
        for var in ['list_path_libs','list_path_sys','list_libs']:
            out.write('MARK_AS_ADVANCED('+var+')\n')

def add_library_for_dir(s2):
    """ Produce the small CMake piece for a sub_directory """
    tgt_nam = f"obj_{s2}"
    # Caution: double '{{' and '}}' just to escape from Python format string:
    strout = f'''add_library({tgt_nam} OBJECT  ${{srcs}} )
set(listlibs ${{listlibs}} {s2} PARENT_SCOPE)
add_custom_target(check_sources_{s2} COMMAND check_sources.sh ${{CMAKE_CURRENT_SOURCE_DIR}} )
add_dependencies(obj_{s2} check_sources_{s2})
if(COMPIL_DYN)
    add_library({s2} SHARED $<TARGET_OBJECTS:obj_{s2}>)
    install(TARGETS {s2} DESTINATION lib)
endif(COMPIL_DYN)
'''
    return strout

def lire_make_include(file,listlib):
    """ Write CMakeLists.txt files from make.include files """
    f = open(file)
    g = f.readlines()
    t=""
    for s in g:
        t+=s.replace('\\\n','').replace('\t',' ')
    t = t.split('\n')

    strout = "file(GLOB srcs [A-Z|a-z]*.cpp [A-Z|a-z]*.c [A-Z|a-z]*.f)\n"

    for s in t:
        if s.find("Includes")>=0:
            s = s.replace('-I.','-I${CMAKE_CURRENT_SOURCE_DIR}')
            s2 = s.split('-I')
            inc_dir = " ".join(s2[1:])
            strout += "include_directories(" + inc_dir.replace('(','{').replace(')','}')+ ")\n" 
        elif s.find("Lib")>=0:
            s2=s.split("/lib")
            listlib.append(s2[1])
            strout += add_library_for_dir(s2[1])
    return strout,listlib

def ajoute_subdir(dirmake,dirsdict):
    """ Add a sub-directory from main CMakeLists.txt file """
    strout=""
    for d in dirsdict[dirmake]:
        strout+="add_subdirectory("+d+")\n"
    return strout

def generate_main_cmake(listdirorg, sans_subdir, atelier):
    """ Generate the main CMakeLists.txt file and all sub CMakeLists.txt files 
    """
    listlib=[]
    if sans_subdir:
        for dirmake in listdirorg:
            cmake = os.path.join(dirmake,'CMakeLists.txt')
            if os.path.exists(cmake):
                os.remove(cmake)

    if not sans_subdir:
        for dirmake in listdirorg:
            strout=""
            make=os.path.join(dirmake,'make.include')
            if os.path.exists(make):
                strout,listlib=lire_make_include(make,listlib)
            cmake=make.replace('make.include','CMakeLists.txt')
            with open(cmake,'w') as out:
                out.write(strout)

        with open(os.path.join('src/MAIN','CMakeLists.txt'),'a') as out:
            out.write(add_library_for_dir("main"))

    if atelier:
        out = open('CMakeLists.txt.trust','w')
    else:
        out = open('src/CMakeLists.txt','w')

    out.write(common_hdr)

    if (os.getenv("TRUST_ARCH_CC")=="linux_nvc++"):
        out.write('# For cmake 3.22 and NVidia:\n')
        out.write('set(CMAKE_C_ABI_COMPILED 1)\n')
        out.write('set(CMAKE_Fortran_COMPILER_WORKS 1)\n\n')

    out.write('''# Tell the CMake makefile generator to not have rules depend on
# themselves.  This causes extra rebuilds when the include path
# changes from turning a kit on or off.
set(CMAKE_SKIP_RULE_DEPENDENCY 0 )

if(NOT VISUAL)
    # on prend les compilos choisis par configure pour retirer des blancs inutils
    set(pathcc $ENV{TRUST_CC})
    string(STRIP ${pathcc} CMAKE_CXX_COMPILER)
    set(CMAKE_C_COMPILER $ENV{TRUST_cc})
    set(CMAKE_Fortran_COMPILER $ENV{TRUST_F77})
endif(NOT VISUAL)

#
# Use a specified linker (typically mold) if provided - this greatly increase debug linking time:
#
set(TRUST_LINKER "$ENV{TRUST_LINKER}")
if(NOT TRUST_LINKER STREQUAL "")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=${TRUST_LINKER}")
endif()

#
# Project definition! Compile flags tweaks must be put before this line.
#
project(TRUST CXX)

enable_testing()

if(NOT VISUAL)
    enable_language(Fortran OPTIONAL)
endif()

# Tell cmake to create compile_commands.json for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

#
# Options
#
option(VISUAL "pour MSVC" OFF)


#
# Potential extra dependencies (baltik?)
#
if(EXISTS ${CMAKE_SOURCE_DIR}/cmake.deps)
   include(cmake.deps)
endif()


#
# Source directories
# 
''')

    out.write('set(listdir \n'+'\n'.join(listdirorg)+')\n')
    out.write('''

# Micro kernel
set(listdir_micro 
    src/Kernel/Math 
    src/Kernel/Utilitaires  
    src/MAIN
)

# Numeric kernel = micro + more
set(listdir_numeric ${listdir_micro} )
list(APPEND listdir_numeric 
    src/Kernel/Math/Matrices  
    src/Kernel/Math/SolvSys 
)

# Standard kernel = Numeric + more
set(listdir_standard ${listdir_numeric} )
list(APPEND listdir_standard 
    src/Kernel/Champs 
    src/Kernel/Champs_dis 
    src/Kernel/Cond_Lim 
    src/Kernel/Framework 
    src/Kernel/Geometrie 
    src/Kernel/Geometrie/Decoupeur 
    src/Kernel/ICoCo 
    src/Kernel/MEDimpl 
    src/Kernel/Operateurs 
    src/Kernel/Schemas_Temps 
    src/Kernel/Solveurs 
    src/Kernel/Statistiques_temps 
    src/Kernel/VF/Champs 
    src/Kernel/VF/Geometrie  
)

#
# Generate appel_c_mod_xxx.cpp file
# TODO: still necessary??
#
set(kernel "full" CACHE STRING "full,standard,numeric,micro" )

if (NOT ${kernel} STREQUAL "full")
  message(STATUS "Kernel variant: ${kernel}")
  set(listdirmod ${listdir_${kernel}})
  set(ajout _${kernel}_kernel)
  set(special_srcs ${CMAKE_CURRENT_BINARY_DIR}/appel_c_mod_${kernel}.cpp)
  set(oo "")
  foreach(d ${listdir})
    list(FIND  listdirmod ${d} trouve)
    if (${trouve} EQUAL -1)
      STRING(REPLACE "/" "_" d2 ${d})
      set(oo "${oo} void instancie_${d2}() { } ")
    endif()
  endforeach(d)
  set(oo "${oo} \n")
  if (EXISTS  ${special_srcs})
    file(READ  ${special_srcs} org)
    string(COMPARE NOTEQUAL ${org} ${oo} update_src)
  else()
    set(update_src 1)
  endif()
  if (update_src)
    message(STATUS "Updating ${special_srcs}")
    file(WRITE ${special_srcs} "${oo}")
  endif(update_src)

  set(listdir ${listdirmod})
endif()

#
# Linux compilation (Win treated below)
#
if(NOT VISUAL)
    enable_language(Fortran OPTIONAL)

    set(MPI_INCLUDE $ENV{MPI_INCLUDE})
    set(CUDA_INC_PATH $ENV{CUDA_INC_PATH})
    set(TRUST_ARCH $ENV{TRUST_ARCH})
    set(TRUST_ROOT $ENV{TRUST_ROOT})
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(OPT "_opt")
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
       message(FATAL_ERROR  "Unknown build_type ${CMAKE_BUILD_TYPE} !, use -DCMAKE_BUILD_TYPE=Release,Debug,Profil,Coverage,semi_opt,custom")
    endif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build mode: ${OPT}")


    set(COMM $ENV{COMM})
    set(PETSC_ROOT $ENV{PETSC_ROOT} )
    set(METIS_ROOT $ENV{METIS_ROOT} )
    set(TRUST_LATAFILTER $ENV{TRUST_LATAFILTER})
    set(TRUST_ICOCOAPI $ENV{TRUST_ICOCOAPI})
    set(TRUST_MED_ROOT $ENV{TRUST_MED_ROOT})
    set(TRUST_CGNS_ROOT $ENV{TRUST_CGNS_ROOT})
    set(TRUST_MEDCOUPLING_ROOT $ENV{TRUST_MEDCOUPLING_ROOT})
    set(TRUST_KOKKOS_ROOT $ENV{TRUST_KOKKOS_ROOT})

    set(LIBRARY_OUTPUT_PATH ${TRUST_ROOT}/lib)
    set(EXECUTABLE_OUTPUT_PATH ${TRUST_ROOT}/exec)

    #
    # Compilation flags
    #
    include (${TRUST_ROOT}/env/Cmake.env)

    #
    # Libraries
    #
    include (${TRUST_ROOT}/env/Cmake.libs)

    #
    # Find external (pre-requisite) libraries
    #
    foreach(liba ${list_libs})
        set(staticlib lib${liba}.a)
        find_library( lib${liba} NAMES ${staticlib} ${liba} PATHS ${list_path_libs} NO_DEFAULT_PATH )
        if (${lib${liba}} STREQUAL lib${liba}-NOTFOUND)
           find_library( lib${liba} NAMES ${liba} PATHS ${list_path_sys} REQUIRED)
        endif(${lib${liba}} STREQUAL lib${liba}-NOTFOUND)
        # pour supermuc on cherche ligfortran.so.3  en dur
        if (${liba} STREQUAL gfortran)
          if (${lib${liba}} STREQUAL libgfortran-NOTFOUND)
            find_library( lib${liba} NAMES  libgfortran.so.3 PATHS ${list_path_sys} )
          endif()
        endif()
        mark_as_advanced(lib${liba})
        set(libs ${libs} ${lib${liba}})
    endforeach()

else(NOT VISUAL)
    include(windows/CMake.win)
endif(NOT VISUAL)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_CONFIG)
string(STRIP ${CMAKE_EXE_LINKER_FLAGS_${BUILD_CONFIG}} linker_flag )
set(syslib ${libs} ${linker_flag} )

#
# Include directories and extra flags:
#

# PL: SYSTEM added to indicate thirdparty includes as system includes to avoid warnings:
include_directories(SYSTEM 
    ${METIS_ROOT}/include 
    ${TRUST_MED_ROOT}/include 
    ${TRUST_CGNS_ROOT}/include 
    ${TRUST_MEDCOUPLING_ROOT}/include 
    ${MPI_INCLUDE} 
    ${TRUST_ROOT}/lib/src/LIBAMGX/AmgXWrapper/include 
    ${TRUST_ROOT}/lib/src/LIBAMGX/AmgX/include 
    ${CUDA_INC_PATH} 
    ${PETSC_ROOT}/${TRUST_ARCH}${OPT}/include 
    ${TRUST_ROOT}/lib/src/LIBROCALUTION/include 
    ${TRUST_LATAFILTER}/include 
    ${TRUST_ICOCOAPI}/include 
    ${TRUST_ROOT}/lib/src/LIBOSQP/include 
    ${TRUST_ROOT}/lib/src/LIBVC/include 
    ${TRUST_KOKKOS_ROOT}/${TRUST_ARCH}${OPT}/include
)

#
# Extra compiler flags - coming from env/Cmake.env:
#
add_definitions(${ADD_CPPFLAGS})

set(trio TRUST${COMM}${OPT})
set(libtrio_name TRUST${COMM}${ajout}${OPT})
set(libtrio lib${libtrio_name})

if(NOT ATELIER) # Not a Baltik, TRUST itself
    foreach(dir ${listdir})
        add_subdirectory(../${dir})
    endforeach()

    #
    # The main TRUST library file generated here:
    #
    if(NOT COMPIL_DYN)
        set(my_listobj)
        foreach(_obj IN LISTS listlibs)
            list(APPEND my_listobj  $<TARGET_OBJECTS:obj_${_obj}>)
        endforeach()
        add_library(${libtrio} STATIC  ${special_srcs} ${my_listobj} )
        set_target_properties(${libtrio} PROPERTIES OUTPUT_NAME ${libtrio_name} PREFIX "" )
        install(TARGETS ${libtrio} DESTINATION lib)
    else()
        set(libtrio ${listlibs})
    endif()

    #
    # Main executable generated here:
    #
    # (no exec produced in partial mode)
    if(("${ajout}" STREQUAL "" ) OR ($ENV{FORCE_LINK}))
        include_directories(Kernel/Utilitaires MAIN Kernel/Math Kernel/Framework)
        add_executable (${trio} 
            MAIN/the_main.cpp 
            MAIN/mon_main.cpp 
            ${special_srcs}  
        )
        target_link_libraries(${trio} ${libtrio} ${syslib})
        install (TARGETS ${trio} DESTINATION exec)

        #
        # Tests generated here:
        #
        set(TRUST_TESTS $ENV{TRUST_TESTS})
        file(GLOB_RECURSE LML   FOLLOW_SYMLINKS ${TRUST_TESTS}/[A-Z|a-z|0-9]*.lml.gz )
        list(LENGTH  LML NN)
        message(STATUS "TRUST: Found ${NN} tests!")
        foreach(f ${LML})
            string(REPLACE .lml.gz "" l2 ${f} )
            string(FIND ${f} /  ii  REVERSE )
            string(SUBSTRING ${l2} ${ii} -1 l3 )
            string(SUBSTRING ${l3} 1 -1 l3 )
            add_test(NAME ${l3} COMMAND trust -exe $<TARGET_FILE:${trio}> -check ${l3}  )
            set_property(TEST ${l3}  PROPERTY SKIP_RETURN_CODE 200 )
        endforeach()
    endif()

else(NOT ATELIER)

    #
    # For a Baltik we still include the sub-dirs of TRUST, but executable and lib will be 
    # generated from another CMakeLists.txt file
    #
    foreach(dir ${listdir})
        include_directories(${TRUST_ROOT}/${dir})
    endforeach(dir)

endif(NOT ATELIER)


''')
    out.close()

def generate_baltik_cmake():
    """ Generate CMakeLists.txt for a baltik
    """
    out = open('CMakeLists.txt','w')
    out.write('''
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

include_directories(.)
set(ATELIER ON)

# Include main CMakeLists.txt
include(CMakeLists.txt.trust)

include_directories(
    ${TRUST_ROOT}/include/backward 
    ${TRUST_ROOT}/include/EOS 
    ${TRUST_ROOT}/include/CoolProp
)

project(atelier CXX)

set(CMAKE_SKIP_RULE_DEPENDENCY 0)

#
# Tell cmake to create compile_commands.json for clang-tidy
#
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Find all sources
file(GLOB srcs [A-Z|a-z]*.cpp  [A-Z|a-z]*.cxx [A-Z|a-z]*.f [A-Z|a-z]*.c [A-Z|a-z]*.h  [A-Z|a-z]*.hpp  [A-Z|a-z]*.hxx  [A-Z|a-z]*.tpp)

#
# Extract baltik source files and dependant TRUST files:
#
set(srcs_atelier ${srcs}  CACHE STRING "fichiers atelier" FORCE )
# GF bidouille importante pour forcer les dependances si ajout d un fichier .h
execute_process(COMMAND find . -name depend.internal -exec rm {} \; RESULT_VARIABLE toto ERROR_VARIABLE err )

set(LATACOMMON Outils/lata_tools/src/trust_commun)
if ((${kernel} STREQUAL "micro") OR (${kernel} STREQUAL "numeric"))
    set(LATACOMMON "")
endif()
execute_process(COMMAND env rep_dev=${CMAKE_SOURCE_DIR} ${TRUST_ROOT}/bin/KSH/cree_info_atelier.sh RESULT_VARIABLE toto OUTPUT_VARIABLE toto2 ERROR_VARIABLE error )
execute_process(COMMAND python ${TRUST_ROOT}/bin/KSH/Cherchefileatelier.py ${listdir} ${LATACOMMON} %% ${srcs} RESULT_VARIABLE toto OUTPUT_VARIABLE srcdeps_new ERROR_VARIABLE error )
if (${toto})
    message(FATAL_ERROR "contact trust@cea.fr")
endif()
set(srcdeps ${srcdeps_new} CACHE STRING "ficher de trio dependant de l atelier " FORCE )
list(LENGTH srcs_atelier l1)
list(LENGTH srcdeps l2)
message(STATUS "${l1} file(s) to compile in baltik source directories, ${l2} file(s) of TRUST depending on it")

#
# Baltik library, based on TRUST main library
#
message(STATUS "Using library: ${libtrio_name} " )
find_library( libTrio NAMES ${libtrio_name}.a PATHS ${TRUST_ROOT}/lib NO_DEFAULT_PATH REQUIRED)
if(EXISTS ${CMAKE_SOURCE_DIR}/cmake.deps)
    include(cmake.deps)
endif()

add_library(list_obj  OBJECT ${srcs} ${srcdeps} ${CMAKE_SOURCE_DIR}/exec${OPT}/info_atelier.cpp )

#
# Baltik executable generation
#
add_executable(exe  $<TARGET_OBJECTS:list_obj>)
target_link_libraries(exe ${libdeps} ${libTrio}  ${syslib})

''')
    prog = os.getenv("PROGRAM")

    if (prog):
        out.write('set(EXECUTABLE_OUTPUT_PATH %s) \n' % os.path.dirname(prog))
        out.write('set(PROGRAM %s${OPT})\n '% os.path.basename(prog))
    else:
        out.write('''
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exec${OPT})
set(PROGRAM ${trio})
''')

    out.write('''

install(TARGETS exe DESTINATION ${EXECUTABLE_OUTPUT_PATH})
set_target_properties(exe PROPERTIES  OUTPUT_NAME  ${PROGRAM} )

#
# Shared library generation
#
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

#
# Test generation
#
enable_testing()
set(TRUST_TESTS $ENV{TRUST_TESTS})
file(GLOB_RECURSE LML   FOLLOW_SYMLINKS ${TRUST_TESTS}/[A-Z|a-z|0-9]*.lml.gz )
list(LENGTH  LML NN)
foreach(f ${LML})
    string(REPLACE .lml.gz "" l2 ${f} )
    string(FIND ${f} /  ii  REVERSE )
    string(SUBSTRING ${l2} ${ii} -1 l3 )
    string(SUBSTRING ${l3} 1 -1 l3 )
    add_test(NAME ${l3} COMMAND trust -exe $<TARGET_FILE:exe> -check ${l3}  )
    set_property(TEST ${l3}  PROPERTY SKIP_RETURN_CODE 200 )
endforeach(f )
''')
    out.close()


############################
# Main
############################
if  __name__ == '__main__':
    args = sys.argv[1:]
    sans_subdir, atelier = False, False
    
        
    # TODO should use optparse:
    if len(args)>0 and args[0]=="-atelier":
        sans_subdir, atelier = True, True
        rep_dev = os.getenv("rep_dev")
        os.chdir(rep_dev)
        args = args[1:]
    if len(args)>0 and args[0]=="-avec_subdir":
        sans_subdir = False
        args = args[1:]
    if len(args)>0 and args[0]=="-sans_subdir":
        sans_subdir = True
        args = args[1:]
            
    dirs = []
    if len(args) == 0: # on lit dans rep.TRUST
        tr = os.getenv("TRUST_ROOT")
        with open(tr+"/env/rep.TRUST","r") as f:
            for lin in f.readlines():
                if lin.strip():
                    dirs.append(lin.strip())
    
    listdirorg=[]
    for make in dirs:
        if os.path.basename(make) == 'make.include':
            s = os.path.dirname(make)
        else:
            s = make
        s2 = s.replace('./','')
        listdirorg.append(s2)

    generate_main_cmake(listdirorg, sans_subdir, atelier)

    if atelier:
        generate_baltik_cmake()

