""" Generate CMakeLists.txt used to compile TRUST

One day we will have pure CMake rather than this ...
"""

import sys
import os

# Common header for all CMakeLists.txt (i.e. TRUST & Baltiks)
COMMON_HDR = '''# 
# This file was generated automatically by bin/KSH/Createcmakefile.py
# !!Do not edit directly!! - changes will be overwritten!
#

cmake_minimum_required(VERSION 3.5)
'''

#
# Definitions of the various TRUST modules
#
SPATIAL_DISCRET = ['src/EF', 'src/IJK', 'src/PolyMAC', 'src/VDF', 'src/VEF']
PHYSICAL_MODULES = ['src/ThHyd', 'src/ThSol']

def add_library_for_dir(libnam):
    """ Produce the the library part of the CMakeLists.txt for a sub_directory """
    tgt_nam = f"obj_{libnam}"
    # Caution: double '{{' and '}}' just to escape from Python format string:
    strout = f'''add_library({tgt_nam} OBJECT  ${{srcs}} )
set(listlibs ${{listlibs}} {libnam} PARENT_SCOPE)
add_custom_target(check_sources_{libnam} COMMAND check_sources.sh ${{CMAKE_CURRENT_SOURCE_DIR}} )
add_dependencies(obj_{libnam} check_sources_{libnam})
if(COMPIL_DYN)
    add_library({libnam} SHARED $<TARGET_OBJECTS:obj_{libnam}>)
    install(TARGETS {libnam} DESTINATION lib)
endif(COMPIL_DYN)
'''
    return strout

#
# Path manipulation ...
#
def full_path(root_dir, ze_dir):
    """ From src/titi -> /a/b/c/TRUST/src/titi """
    return os.path.join(root_dir, ze_dir)

def short_path(root_dir, ze_dir):
    """ Opposite of full_path() above """
    return ze_dir.replace(os.path.join(root_dir, ""), "")

def get_module_dir(ze_dir):
    """ From src/IJK/toto/tiit returns src/IJK 
    """
    arr = os.path.normpath(ze_dir).split(os.sep)
    return os.sep.join(arr[:2])

def is_sub_dir_of(ze_dir, top_dir_lst):
    """ Return whehter 'ze_dir' is a sub-directory of one of the entry found in the list 'top_dir_lst'
    @param ze_dir is a relative short path (src/toto)
    """
    for top in top_dir_lst:
      if ze_dir.startswith(top):
        return True
    return False

def all_sub_dirs(root_dir, ze_dir):
    """ List all sub-directories of a given top directory which contains at least one file 
    @param ze_dir must be a relative (short) path
    @return a list of relative (short) paths
    """
    dr = full_path(root_dir, ze_dir)
    lst = []
    for dirpath, dirnames, filenames in os.walk(dr):
      if filenames:
        lst.append(short_path(root_dir, dirpath))
    return lst

# All include directories for kernel and physical module - computed in 'generate_main_cmake'
_full_kern_inc = []
_full_physic_inc = []

def generate_subdir_cmake(root_dir, ze_dir, fname):
    """ Generate CMakeLists.txt for a sub-directory of the TRUST soruces.
    For the 'include_directories' directive, the rule is as follows:
      - spatial discretisation depend only on Kernel, and not on other discretisations
      - physical modules (ThHyd, ThSol) depends only on Kernel, and not on discretisations
    Last but not least, beware that sources used to compile the micro/numeric/std kernels do NOT correspond 
    to include that are needed. For example Probleme_base 
        1) is in src/Kernel/Framework (so in kernel_standard)
        2) but needs Postraitements.h (located in src/Kernel/Postraitement, so **not** in kernel_standard library) to compile
        ... !
    """
    # Sources to compile - a GLOB:
    strout = "file(GLOB srcs [A-Z|a-z]*.cpp [A-Z|a-z]*.c [A-Z|a-z]*.f)\n"

    # Include directories - see rule above:
    if is_sub_dir_of(ze_dir, PHYSICAL_MODULES):
      # include all sub-dir of the given physical module
      rd = get_module_dir(ze_dir)
      inc = _full_kern_inc + all_sub_dirs(root_dir, rd)
      # [ABN] HACK HACK HACK: ThHyd needs ThSol ... pfff:
      if rd == "src/ThHyd":
        inc.extend(all_sub_dirs(root_dir, "src/ThSol"))
    elif is_sub_dir_of(ze_dir, SPATIAL_DISCRET):
      # include all sub-dir physical modules, plus all sub-dir of the discretisation:
      mod_dir = get_module_dir(ze_dir)
      inc = _full_kern_inc + _full_physic_inc + all_sub_dirs(root_dir, mod_dir)
    elif is_sub_dir_of(ze_dir, ["src/Kernel"]) or ze_dir == "src/MAIN":
      inc = _full_kern_inc
    else:
      raise ValueError(f"The sub-directory '{ze_dir}' is not properly declared at the top of bin/KSH/Createcmakefile.py as being a spatial discretisation or a physical module!! Check this.")
    inc2 = '\n'.join(['${TRUST_ROOT}/%s' % s for s in inc])
    strout += f'''include_directories(
${{CMAKE_CURRENT_SOURCE_DIR}}
{inc2}
)
'''
    # Library name is of the form lib<rep1>_<rep2>_<rep3>... for a source directory like src/rep1/rep2/rep3:
    arr = os.path.normpath(ze_dir).split(os.sep)
    lib_nam = '_'.join(arr[1:])
    strout += add_library_for_dir(lib_nam)

    return strout

def list_dir_containing_compilable(root_dir):
    """ Generate list of all sub-directories containing sources to be compiled
    """
    def is_compilable(f):
        ext = [".cpp", ".f", ".c"]
        for e in ext:
            if f.endswith(e):
                return True
        return False

    listdirorg = []
    for dirpath, dirnames, filenames in os.walk(root_dir):
        if filenames:
            for f in filenames:
                if is_compilable(f):
                    listdirorg.append(dirpath)
                    break
    return listdirorg

def generate_cmake_files(root_dir, atelier):
    """ Generate the main CMakeLists.txt file and all sub CMakeLists.txt files 
    """
    # Get all directories containing sources to be compiled:
    src_dir = os.path.join(root_dir, "src")
    listdirorg = list_dir_containing_compilable(src_dir)

    # Generate include directories lists:
    global _full_kern_inc, _full_physic_inc
    _full_kern_inc = all_sub_dirs(root_dir, 'src/Kernel') # warning, not the same as KERNEL_STANDARD!!
    _full_kern_inc.append("src/MAIN")  # because of ICoCo/ProblemTrio.cpp ...
    _full_physic_inc = []
    for d in PHYSICAL_MODULES:
      _full_physic_inc.extend(all_sub_dirs(root_dir, d))

    # Generate sub CMakeLists.txt for each sub-directory in the TRUST sources:
    #
    if not atelier: # should not be done for baltiks, only for TRUST
        for d in listdirorg:
            d_short = short_path(root_dir, d)
            cmake_fnam = os.path.join(d, 'CMakeLists.txt')
            with open(cmake_fnam, "w") as f:
                d_short = short_path(root_dir, d)
                s = generate_subdir_cmake(root_dir, d_short, cmake_fnam)
                f.write(s)

    if atelier:
        out = open('CMakeLists.txt.trust','w')
    else:
        out = open('src/CMakeLists.txt','w')

    # Now the big central CMakeLists.txt:
    out.write(COMMON_HDR)

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
# Potential extra dependencies - WARNING, this is also where we set the kernel variant! 
#
set(kernel "full" CACHE STRING "full,standard,numeric,micro" )
if(EXISTS ${CMAKE_SOURCE_DIR}/cmake.deps)
   include(cmake.deps)
endif()


#
# Source directories
# 
''')
    listdir_short = [s.replace(root_dir + "/", "") for s in listdirorg]  # strip TRUST_ROOT to keep only src/titi/toto
    out.write('set(listdir \n'+'\n'.join(listdir_short)+')\n')
    out.write('''
set(listdir_full ${listdir})

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
    src/Kernel/Geometrie/Elem 
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
# Set Kernel type
#
if (NOT ${kernel} STREQUAL "full")
  message(STATUS "Kernel variant: ${kernel}")
  set(ajout _${kernel}_kernel)
  set(listdir ${listdir_${kernel}})
endif()

#
# Linux compilation (Win treated below)
#
if(NOT VISUAL)
    enable_language(Fortran OPTIONAL)

    set(MPI_INCLUDE $ENV{MPI_INCLUDE})
    set(CUDA_INC_PATH $ENV{CUDA_INC_PATH})
    set(ROCM_PATH $ENV{ROCM_PATH})
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
    message(STATUS "Build mode (OPT variable): '${OPT}'")


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
    # Libraries - file generated by bin/mklibs script called in env/configurer_env:
    #
    include (${TRUST_ROOT}/env/Cmake.libs)

    #
    # Find external (pre-requisite) libraries
    #
    foreach(liba ${list_libs})
        find_library( lib${liba} NAMES lib${liba}.a lib${liba}.so PATHS ${list_path_libs} NO_DEFAULT_PATH )
        if (${lib${liba}} STREQUAL lib${liba}-NOTFOUND)
           find_library( lib${liba} NAMES ${liba} PATHS ${list_path_sys} REQUIRED)
        endif(${lib${liba}} STREQUAL lib${liba}-NOTFOUND)
        # pour supermuc on cherche ligfortran.so.3  en dur
        #if (${liba} STREQUAL gfortran)
        #  if (${lib${liba}} STREQUAL libgfortran-NOTFOUND)
        #    find_library( lib${liba} NAMES  libgfortran.so.3 PATHS ${list_path_sys} )
        #  endif()
        #endif()
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

# Metis might come from PETSc or be installed standalone:
if ("$ENV{TRUST_DISABLE_PETSC}" STREQUAL "1")
  include_directories(SYSTEM ${METIS_ROOT}/include)
endif()

include_directories(SYSTEM 
    ${TRUST_MED_ROOT}/include 
    ${TRUST_CGNS_ROOT}/include 
    ${TRUST_MEDCOUPLING_ROOT}/${TRUST_ARCH}${OPT}/include 
    ${MPI_INCLUDE} 
    ${TRUST_ROOT}/lib/src/LIBAMGX/AmgXWrapper/include 
    ${TRUST_ROOT}/lib/src/LIBAMGX/AmgX/include 
    ${CUDA_INC_PATH} 
    ${ROCM_PATH}/include  
    ${PETSC_ROOT}/${TRUST_ARCH}${OPT}/include 
    ${TRUST_ROOT}/lib/src/LIBROCALUTION/include 
    ${TRUST_LATAFILTER}/include 
    ${TRUST_ICOCOAPI}/include 
    ${TRUST_ROOT}/lib/src/LIBOSQP/include 
    ${TRUST_ROOT}/lib/src/LIBVC/include 
    ${TRUST_KOKKOS_ROOT}/${TRUST_ARCH}${OPT}/include
    ${TRUST_ROOT}/include/EOS 
    ${TRUST_ROOT}/include/CoolProp
)

#
# Extra compiler flags - coming from env/Cmake.env:
#
add_definitions(${ADD_CPPFLAGS})

set(trio TRUST${COMM}${OPT})
set(libtrio_name TRUST${COMM}${ajout}${OPT})
set(libtrio lib${libtrio_name})

if(NOT ATELIER) # Not a Baltik, TRUST itself
    #
    # Unit tests - only compiled with micro kernel, and in Debug, but test added for all Kernel configuration
    #
    if("${OPT}" STREQUAL "")
        if(${kernel} STREQUAL "micro")
            add_subdirectory(${CMAKE_SOURCE_DIR}/../tests/UnitTests ${CMAKE_BINARY_DIR}/UnitTests) # Must specify binary dir cause out of tree source
        endif()
        add_test(NAME Unit_Test_Array COMMAND "$ENV{TRUST_ROOT}/exec/unit_array")
    endif()
    
    #
    # Include all TRUST (or specialized kernel) sub-directories
    #
    foreach(dir ${listdir})
        add_subdirectory(../${dir})
    endforeach()

    #
    # Update "instancie_complement_xxx.h" which contains all the *complementary* calls to the instancie methods
    # which otherwise won't be found when MAIN is including 'instancie_appel_c.h' file.
    # 
    # Recall: 
    #   - instancie_src_xxx_yyy.cpp make sure all classes derived from Objet_U are instanciated once in the code
    #   - src/MAIN/instancie_appel.h contains all the signatures for those methods
    #   - src/MAIN/instancie_appel_c.h contains all the *calls* to those methods (what the main does when it starts TRUST)
    #   - when just compiling a kernel (micro, numeric or standard) we want those calls to be valid, but some instancie_src_xx_yy.cpp are not there!
    #     -> instancie_complement_xxx.cpp contains dummy implementation to fix this.
    # Note: we could just generate a correct instancie_appel_c.h for each kernel we compile, but this would change this file over and over
    # triggering a recompile every time.
    #
    set(inst_compl "${CMAKE_BINARY_DIR}/instancie_complement_${kernel}.cpp") 
    set(new_inst "// THIS FILE IS CREATED AUTOMATICALLY BY bin/KSH/Createcmakefile.py - DO NOT EDIT\\n// TRUST_NO_INDENT\\n")
    foreach(d ${listdir_full})
        list(FIND listdir ${d} found)
            if (${found} EQUAL -1)
                string(REPLACE "/" "_" d2 ${d})
                set(new_inst "${new_inst}void instancie_${d2}() { }\\n")
            endif()
    endforeach() 
    if (EXISTS  ${inst_compl})
        file(READ  ${inst_compl} org)
        string(COMPARE NOTEQUAL ${org} ${new_inst} update_src)
    else()
        set(update_src 1)
    endif()
    if (update_src)
        message(STATUS "Updating ${inst_compl}")
        file(WRITE ${inst_compl} "${new_inst}")
    endif()    

    #
    # The main TRUST library file generated here:
    #
    if(NOT COMPIL_DYN)
        set(my_listobj)
        foreach(_obj IN LISTS listlibs)
            list(APPEND my_listobj  $<TARGET_OBJECTS:obj_${_obj}>)
        endforeach()
        add_library(${libtrio} STATIC ${inst_compl} ${my_listobj} )
        set_target_properties(${libtrio} PROPERTIES OUTPUT_NAME ${libtrio_name} PREFIX "" )
        install(TARGETS ${libtrio} DESTINATION lib)
    else()
        set(libtrio ${listlibs})
    endif()

    #
    # Main executable generated here:
    #
    # (no exec produced in partial mode)
    if((${kernel} STREQUAL "full") OR ($ENV{FORCE_LINK}))
        # If the linker was overriden, we also take this opportunity to build the executable more
        # efficiently by using directly the .o files to build the exe file.
        # On some machines (adastra GPUs) this makes a too long command line, so we use also the TRUST_LINKER
        # variable to have a way to build with the former method ... to be changed once adastra is up to date.
        if ("${TRUST_LINKER}" STREQUAL "")
            include_directories(Kernel/Utilitaires MAIN Kernel/Math Kernel/Framework)
            add_executable (${trio}
                MAIN/the_main.cpp
                MAIN/mon_main.cpp
                ${inst_compl}
            )

            target_link_libraries(${trio} ${libtrio} ${syslib})
        else()
            set(my_listobj)
            foreach(_obj IN LISTS listlibs)
                list(APPEND my_listobj  $<TARGET_OBJECTS:obj_${_obj}>)
            endforeach()
            include_directories(Kernel/Utilitaires MAIN Kernel/Math Kernel/Framework)
            add_executable (${trio} ${inst_compl})
            target_link_libraries(${trio} ${my_listobj} ${syslib})
            #
            # The flag ENABLE_EXPORTS is set on the main executable to handle this:
            #  - a dynamic library (.so) has been created which needs TRUST classes (e.g. Nom, Sortie, ...)
            #  - this dynamic library is opened using dlopen from the main TRUST (or baltik executable)
            #  - the dynamic library needs to resolve its undefined (TRUST) symbols from within the
            #  executable that loads it.
            # For all this to work, the exec needs to be linked with '-rdynamic', this is achieved in CMake 
            # with ENABLE_EXPORTS: 
            #
            set_property(TARGET ${trio} PROPERTY ENABLE_EXPORTS 1)
        endif()

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
    out.write(r'''
cmake_minimum_required(VERSION 3.5)

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
find_library( libTrio NAMES ${libtrio_name}.a PATHS ${TRUST_ROOT}/lib NO_DEFAULT_PATH)
if (NOT libTrio)
    message(FATAL_ERROR "Could not find TRUST library: ${libtrio_name}.a - did you compile TRUST correctly (potentially in debug)??")
endif()
if(EXISTS ${CMAKE_SOURCE_DIR}/cmake.deps)
    include(cmake.deps)
endif()

add_library(list_obj  OBJECT ${srcs} ${srcdeps} ${CMAKE_SOURCE_DIR}/exec${OPT}/info_atelier.cpp )

#
# Baltik executable generation
#
add_executable(exe  $<TARGET_OBJECTS:list_obj>)
# See comments above on main TRUST executable generation for this:
set_property(TARGET exe PROPERTY ENABLE_EXPORTS 1)
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
    atelier = False

    # TODO should use optparse:
    if len(args)>0 and args[0]=="-atelier":
        atelier = True
        rep_dev = os.getenv("rep_dev")
        os.chdir(rep_dev)
        args = args[1:]

    dirs = []
    if len(args) != 0:
        raise ValueError("Unexpected command line argument(s): %s" % ' '.join(args))

    root_dir = os.environ.get("TRUST_ROOT")
    generate_cmake_files(root_dir, atelier)

    if atelier:
        generate_baltik_cmake()

