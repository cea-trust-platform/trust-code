#!/bin/bash

# Protect from failure
# -e will exit immediatly when a command fails
# -o pipefail checks exit code from all commands composing the pipeline, not only the rightmost one
# -u treats unset variable as errors
# -x outputs verbose
#set -exo pipefail

script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)

usage() {
  cat <<EOF
Usage:

      $(basename "${BASH_SOURCE[0]}") [-h|--help]
                        [-a|--arch cpu|gpu]
                        [-p|--partition partiton_name]
                        [-n|--ntasks nb_tasks]
                        [-N|--nnodes nb_nodes]
                        [-c|--cores-per-task nb_cores]
			[-g|--ngpus nb_gpus]
                        [-r|--rocprof 0 (disable) |1 (enable)]
                        [-t|--hpctoolkit 0 (disable) |1 (enable)]
                        bench_path

Script Description:

[!!!! ADASTRA SPECIFIC SCRIPT  !!!!]
TRUST_launcher.sh can be used to launch TRUST bench with SLURM scheduler.
Partitionning of the input file is performed when needed.
Submission script is generated according to input options.
Computation is launched.
Verification is performed if possible.

Available options:

-h, --help           Print this help and exit
-a, --arch           Architecture to launche TRUST cpu|gpu.    Default: cpu
-p, --partition      SLURM partition name                      Default: accel
-n, --ntasks         Number of MPI tasks requested             Default: 1
-N, --nnodes         Number of nodes requested                 Default: 1
-c, --cores-per-task Number of requested CPU cores / task      Default: 128
-g, --ngpus          Number of requested GPU                   Default: 0
-r, --rocprof        Enable(1)/Disable(0) rocprof profiling    Default: 0
-t, --hpctoolkit     Enable(1)/Disable(0) hpctoolkit profiling Default: 0
Arg                  Bench path                                Mandatory
EOF
  exit
}

die() {
  echo "=================================="
  echo "[ERROR]" $1
  echo "=================================="
  echo
  usage
  local code=${2-1} # default exit status 1
  exit "$code"
}

parse_params() {

  # define getopt options
  SHORT=h,,a:,p:,n:,N:,c:,g:,r:,t:
  LONG=help,arch:,partition:,ntasks:,nnodes:,cores-per-task:,ngpus:,rocprof:,hpctoolkit:
  OPTS=$(getopt -a -n TRUST_launcher --options $SHORT --longoptions $LONG -- "$@")
  eval set -- "$OPTS"

  # default values of variables set from params
  ARCH="cpu"
  PARTITION="GENOA"
  NTASKS=1
  NNODES=1
  NTASKSPERNODE=1
  CORESPERTASK=64
  NGPUS=0
  ROCROCPROF=""
  HPCTOOLKIT=""
  FLAG_HPCTOOLKIT=1

  while :
    do
    case "${1}" in
    -h | --help)
      usage
      shift
      ;;
    -a | --arch)
      ARCH="${2}"
      shift 2
      ;;
    -p | --partition)
      PARTITION="${2}"
      shift 2
      ;;
    -n | --ntasks)
      NTASKS="${2}"
      shift 2
      ;;
    -N | --nnodes)
      NNODES="${2}"
      shift 2
      ;;
    -c | --cores-per-task)
      CORESPERTASK="${2}"
      shift 2
      ;;
    -g | --ngpus)
      NGPUS="${2}"
      shift 2
      ;;
    -r | --rocprof)
      [[ ${2} -eq 1 ]] && ROCPROF="rocprof --basenames on --hip-trace --timestamp on"
      shift 2
      ;;
    -t | --hpctoolkit)
      [[ ${2} -eq 0 ]] && FLAG_HPCTOOLKIT=0
      [[ ${2} -eq 1 ]] && HPCTOOLKIT="hpcrun --disable-auditor -r -t"
      shift 2
      ;;
    --)
      shift;
      break
      ;;
    *) die "Unknown option: $1" ;;
    esac
  done

  NTASKSPERNODE=$((${NTASKS}/${NNODES}))
  NGPUSPERTASK=$((${NGPUS}/${NTASKS}))
  [[ ! ${NGPUS} -eq 0 ]] && NTASKSPERGPU=$((${NTASKS}/${NGPUS}))
  BENCH_PATH=("$@")

  # check required argument
  [[   ${#BENCH_PATH[@]} -eq 0 ]] && die "Missing bench file name argument"
  [[ ! ${#BENCH_PATH[@]} -eq 1 ]] && die "Please enter only ONE bench file name"

  return 0
}

parse_params "$@"

# Recap input parmas and arguments

echo "[DBG] $@"

echo
echo -e "# Input parameters and argument:"
echo -e "\t- arch: \t\t${ARCH}"
echo -e "\t- partition: \t\t${PARTITION}"
echo -e "\t- ntasks: \t\t${NTASKS}"
echo -e "\t- nnodes: \t\t${NNODES}"
echo -e "\t- cores-per-task: \t${CORESPERTASK}"
echo -e "\t- ngpus: \t\t${NGPUS}"
echo -e "\t- rocprof: \t\t${ROCPROF}"
echo -e "\t- hpctoolkit: \t\t${HPCTOOLKIT}"
echo -e "\t- bench path: \t\t${BENCH_PATH}"

# TRUST environment
echo
echo -e "# Loading TRUST environment"
source ${TRUST_ROOT}/env_TRUST.sh

# Job directory
BENCH_DIR=$(dirname ${BENCH_PATH})
cd $BENCH_DIR

# Job name
COMPLETE_BENCH_NAME=$(basename ${BENCH_PATH})
BENCH_NAME=${COMPLETE_BENCH_NAME%.data}

# Job Hash
HASH=$(md5sum<<<$RANDOM | head -c 6)

# En-tete architecture GPU MI250X
if [ ${ARCH} = "gpu" ]; then

  # Cas 1 MPI / GPU
  if [ ${NGPUSPERTASK} -ne 0 ]; then
    cat << EOF > TRUST_sbatch_${HASH}.sh
#!/bin/bash

#SBATCH -J ${BENCH_NAME}
#SBATCH --constraint=${PARTITION}
#SBATCH --account cpa2202
#SBATCH --ntasks=${NTASKS}
#SBATCH --nodes=${NNODES}
#SBATCH --ntasks-per-node=${NTASKSPERNODE}
#SBATCH --cpus-per-task=${CORESPERTASK}
#SBATCH --threads-per-core=1
#SBATCH --gpus-per-task=${NGPUSPERTASK}
#SBATCH --gpu-bind=verbose,closest
#SBATCH --time=00:30:00
#SBATCH --exclusive
#SBATCH -o ${BENCH_NAME}_%j.out
#SBATCH -e ${BENCH_NAME}_%j.err
EOF
  # Cas multi-MPI / GPU
  else
    cat << EOF > TRUST_sbatch_${HASH}.sh
#!/bin/bash

#SBATCH -J ${BENCH_NAME}
#SBATCH --constraint=${PARTITION}
#SBATCH --account cpa2202
#SBATCH --ntasks=${NTASKS}
#SBATCH --nodes=${NNODES}
#SBATCH --ntasks-per-node=${NTASKSPERNODE}
#SBATCH --cpus-per-task=${CORESPERTASK}
#SBATCH --threads-per-core=1
#SBATCH --gres=gpu:${NGPUS}
#SBATCH --gpu-bind=verbose,single:${NTASKSPERGPU}
#SBATCH --time=00:30:00
#SBATCH --exclusive
#SBATCH -o ${BENCH_NAME}_%j.out
#SBATCH -e ${BENCH_NAME}_%j.err
EOF
  fi
# En-tete architecture CPU GENOA
else
    cat << EOF > TRUST_sbatch_${HASH}.sh
#!/bin/bash

#SBATCH -J ${BENCH_NAME}
#SBATCH --constraint=${PARTITION}
#SBATCH --account cpa2202
#SBATCH --reservation=eolen_cpu
#SBATCH --ntasks=${NTASKS}
#SBATCH --nodes=${NNODES}
#SBATCH --ntasks-per-node=${NTASKSPERNODE}
#SBATCH --cpus-per-task=${CORESPERTASK}
#SBATCH --threads-per-core=1
#SBATCH --time=00:30:00
#SBATCH --exclusive
#SBATCH -o ${BENCH_NAME}_%j.out
#SBATCH -e ${BENCH_NAME}_%j.err
EOF
fi



cat <<EOF >> TRUST_sbatch_${HASH}.sh

# TRUST environment
source ${TRUST_ROOT}/env_TRUST.sh

# Load HPCTOOLKIT module if option is activated
if [ ${FLAG_HPCTOOLKIT} -eq 1 ]; then
  module unuse /opt/software/gaia/prod/0.5.2/modules/lmod/linux-rhel8-x86_64/Core
  module use /opt/software/gaia/branches/240/latest/modules/lmod/linux-rhel8-x86_64/Core
  module load CPE-22.11-gcc-11.2.0-softs
  module load hpctoolkit/2021.05.15-mpi-python3
fi
export TRUST_CLOCK_ON=1
module list

# CRAY exports
export CRAY_OMP_CHECK_AFFINITY=TRUE
export CRAY_ACC_DEBUG=2

# CRAY-MPICH exports
#export MPICH_ENV_DISPLAY=1
#export MPICH_OFI_NIC_POLICY=NUMA
#export FI_MR_CACHE_MAX_COUNT=0
if [ ${ARCH} = "gpu" ]; then
  export MPICH_GPU_SUPPORT_ENABLED=1
fi

# ROCALUTION exports
#export ROCALUTION_LAYER=1

# Informations about the current code state
echo "[DBG] Run with the following git branch:"
git -C $TRUST_ROOT rev-parse --abbrev-ref HEAD
echo "[DBG] Run with the following git commit:"
git -C $TRUST_ROOT rev-parse --short HEAD
echo "[DBG] Run with the following git diff:"
git -C $TRUST_ROOT diff
echo -e "\n\n\n"

# Create run directory and copy bench
RUN_DIR="RUN_${BENCH_NAME}_${ARCH^^}_${NTASKS}TASKS_${NNODES}NODES_${CORESPERTASK}CpTASK_${NGPUS}GPUS_\${SLURM_JOB_ID}"
mkdir \${RUN_DIR}
cp ${COMPLETE_BENCH_NAME} \${RUN_DIR}
cp extract_free_data.sh \${RUN_DIR}
cp extract_rocmsmi_data.sh \${RUN_DIR}
cp plot_command.gpt \${RUN_DIR}
cd \${RUN_DIR}

# Prepare Mesh
echo "[DBG] Partitioning:"
[[ ! ${NTASKS} -eq 1 ]]  && time make_PAR.data ${COMPLETE_BENCH_NAME} ${NTASKS}

# Launch
exec_binding="/lus/home/NAT/cpa/SHARED/TESTS/GetMapping/getMapping_cray_gpu"
if [ ${ARCH} = "cpu" ]; then
  export TRUST_DISABLE_DEVICE=1
  exec_binding="/lus/home/NAT/cpa/SHARED/TESTS/GetMapping/getMapping_cray_cpu"
fi

echo "[DBG] CPU and GPU memory profile:"
./extract_free_data.sh \${PWD} &
if [ ${ARCH} = "gpu" ]; then
  ./extract_rocmsmi_data.sh \${PWD} &
fi

echo "[DBG] Launch:"
export OMP_NUM_THREADS=1

if [ ${NTASKS} -eq 1 ]
then
  srun -l --mpi=cray_shasta -c ${CORESPERTASK} --cpu-bind=verbose,cores \${exec_binding}
  ${ROCPROF} srun -l --mpi=cray_shasta -c ${CORESPERTASK} --cpu-bind=verbose,cores ${HPCTOOLKIT} $exec ${BENCH_NAME} -journal=0
else
  srun -l --mpi=cray_shasta -c ${CORESPERTASK} --cpu-bind=verbose,cores \${exec_binding}
  ${ROCPROF} srun -l --mpi=cray_shasta -c ${CORESPERTASK} --cpu-bind=verbose,cores ${HPCTOOLKIT} $exec PAR_${BENCH_NAME} ${NTASKS} -journal=0
fi

# Process HPCTOOLKIT data if option is activated
if [ ${FLAG_HPCTOOLKIT} -eq 1 ]; then
  hpcprof hpctoolkit*
fi

if [ ${COMPLETE_BENCH_NAME} == "OpenMP_Iterateur.data" ]
then
  echo "[DBG] Numerical verification:"

  grep -qi postraitement ${COMPLETE_BENCH_NAME}
  POST=\$?
  grep -i postraitement ${COMPLETE_BENCH_NAME} | grep -q "#"
  POSTCOM=\$?
  if [[ \${POST} -eq 1 || \${POSTCOM} -eq 0 ]]; then
    echo "[DBG] No Postraitement field in data file: please add or uncomment it."
  else
    REF_LML=Ref_OpenMP_Iterateur.lml
    if [ ${NTASKS} -eq 1 ]
    then
      RUN_LML=OpenMP_Iterateur.lml
    else
      RUN_LML=PAR_OpenMP_Iterateur.lml
    fi
    cp ../OpenMP_Iterateur.lml.gz .
    gunzip -c OpenMP_Iterateur.lml.gz > \${REF_LML}
    time $TRUST_ROOT/exec/compare_lata \${REF_LML} \${RUN_LML} --max_delta
  fi
else
  echo "[DBG] No numerical verification for this bench."
fi

# Get back SLURM outputs and submission script
mv ../*\${SLURM_JOB_ID}.out .
mv ../*\${SLURM_JOB_ID}.err .
mv ../TRUST_sbatch_${HASH}.sh ./TRUST_sbatch_\${SLURM_JOB_ID}.sh
EOF

echo
echo "# Subsmision"
sbatch TRUST_sbatch_${HASH}.sh

echo
echo "# Numerical verification:"
if [ ${COMPLETE_BENCH_NAME} == "OpenMP_Iterateur.data" ]
then
  echo "Will be done after the job."
else
  echo "No numerical verification for this bench."
fi