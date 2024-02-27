"""
Merge several MED files together

Typically used to merge the several MED produced during a parallel computation into a single MED file.

Usage:
  python merge_meds.py MEDFilesToMerge.med NB_PROCS [MergedFileName.med]

This script will:
  - merge coincident nodes located on joints
  - merge and recreate proper face groups for the boundaries
  - merge the fields
"""

import medcoupling as mc
from optparse import OptionParser
import sys
import os


def write_mesh():
    mn = mc.GetMeshNames(FilesList[0])
    new_m = mc.MEDFileUMesh()
    for idx_m, n in enumerate(mn):
        print ("Mesh : %s, proc :" %format(n), end=" ")
        mesh_top, mesh_faces = [], []
        off_face = 0
        grps = {}
        for i, f in enumerate(FilesList):
            print (i, end=" "),
            mesh = mc.MEDFileUMesh(f, n)
            lvls = mesh.getNonEmptyLevels()
            mesh_top.append(mesh.getMeshAtLevel(0))
            if -1 in lvls: # There is a face mesh (for BC)
                # Compute the offset that we will apply to the group identifiers:
                if i > 0:
                  off_face += mesh_faces[-1].getNumberOfCells()
                fc = mesh.getMeshAtLevel(-1)
                mesh_faces.append(fc)
                grp_nams = mesh.getGroupsOnSpecifiedLev(-1)
                for gn in grp_nams:
                    arr = mesh.getGroupArr(-1, gn)
                    arr += off_face  # shift
                    grps.setdefault(gn, []).append(arr)
        # Register highest dimension
        top = mc.MEDCouplingUMesh.MergeUMeshes(mesh_top)
        top.mergeNodes(1.0e-10)
        new_m.setMeshAtLevel(0, top)

        # Register face meshes with groups
        if len(mesh_faces) != 0:
            fc = mc.MEDCouplingUMesh.MergeUMeshes(mesh_faces)
            fc.mergeNodes(1.0e-10)  # should merge exactly as above ... hmm not optimal
            fc.setCoords(top.getCoords())
            new_m.setMeshAtLevel(-1, fc)

            #    Groups management
            final_grps = []
            for gnam, sub_g in grps.items():
                da = mc.DataArrayInt.Aggregate(sub_g)
                da.setName(gnam)
                final_grps.append(da)
            new_m.setGroupsAtLevel(-1, final_grps)
        new_m.setName(mesh_top[0].getName())
        if idx_m == 0:
          write_opt = 2  # Overwrite
        else:
            write_opt = 1  # Append
        new_m.write(OutputFile, write_opt)
        new_m = mc.MEDFileUMesh() # reset object
        print ("done")

def write_fields():
    fields = {}
    for n in fn:
        print ("Field : %s" %(n))
        for t in range(n_dt):
            print ("    Timestep %s/%s, proc :" %(t + 1, n_dt), end=" "),
            f_dt = []
            for i, f in enumerate(FilesList):
                print (i, end=" "),
                f_m = mc.MEDFileFieldMultiTS.New(f, n)
                f_dt.append(f_m[t].getFieldAtLevel(mc.ON_CELLS,0))
            print ("done")
            f_dt_merge = mc.MEDCouplingFieldDouble.MergeFields(f_dt)
            f_dt_merge.setTime(f_dt[0].getTime()[0], f_dt[0].getTime()[1], 0)
            f_dt_merge.setName(f_dt[0].getName())
            fnp = mc.MEDFileField1TS.New()
            fnp.setFieldNoProfileSBT(f_dt_merge)
            fnp.write(OutputFile, 0)

if __name__=="__main__":
    parser = OptionParser()
    parser.set_usage("Merge into a single .med file MED files generated from TRUST parallel computation.\n  python merge_meds.py MEDFilesToMerge.med NB_PROCS [MergedFileName.med]")
    (opts, args) = parser.parse_args()
    parser.print_usage()

    if len(args) < 2:
        print ("Fatal error: You should specify MEDFilesToMerge.med and NB_PROCS !!!")
        exit(1)

    # get files names to merge
    InputFile = sys.argv[1]
    if InputFile.endswith('.med'):
        InputFile = os.path.splitext(os.path.basename(InputFile))[0]
    InputFile = InputFile[:-4]

    nb_procs = int(sys.argv[2])

    OutputFile = 'merged.med'
    if len(args) == 3:
        OutputFile = sys.argv[3]
        if not OutputFile.endswith('.med'):
            OutputFile += '.med'

    FilesList = ['{}{:04d}.med'.format(InputFile, i) for i in range(nb_procs)]

    for i in range(nb_procs):
        if not (os.path.isfile(FilesList[i])):
           print(("\nFatal error: %s file does not exist!" %(FilesList[i])))
           exit(1)

    mff = mc.MEDFileFields(FilesList[0])
    fn = mff.getFieldsNames()
    n_dt = len(mc.MEDFileFieldMultiTS.New(FilesList[0], fn[0]))

    write_mesh()
    write_fields()

    print ("\nParallel Med file have been merged into %s" %(OutputFile))
