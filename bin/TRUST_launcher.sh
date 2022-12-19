#!/bin/bash

# Protect from failure
# -e will exit immediatly when a command fails
# -o pipefail checks exit code from all commands composing the pipeline, not only the rightmost one
# -u treats unset variable as erros
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
                        [-c|--cores-per-task nb_cores]
                        [-g|--gpu-per-tasks nb_gpus]
                        [-r|--rocprof 0 (disable) |1 (enable)]
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
-a, --arch           Architecture to launche TRUST cpu|gpu. Default: cpu
-p, --partition      SLURM partition name                   Default: accel
-n, --ntasks         Number of MPI tasks requested          Default: 1
-c, --cores-per-task Number of requested CPU cores / task   Default: 1
-g, --gpus-per-task  Number of requested GPU / task         Default: 0
-r, --rocprof        Enable(1)/Disable(0) rocprof profiling Default: 0
Arg                  Bench path                             Mandatory
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
  SHORT=h,,a:,p:,n:,c:,g:,r:
  LONG=help,arch:,partition:,ntasks:,cores-per-task:,gpus-per-task:,--rocprof:
  OPTS=$(getopt -a -n TRUST_launcher --options $SHORT --longoptions $LONG -- "$@")
  eval set -- "$OPTS"

  # default values of variables set from params
  ARCH="cpu"
  PARTITION="accel"
  NTASKS=1
  NTASKSPERNODE=64
  CORESPERTASK=1
  GPUSPERTASK=0
  ROCROCPROF=""

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
      if [ ${ARCH} = "cpu" ]; then
        [[ ${NTASKS} -le 64 ]] && NTASKSPERNODE=${NTASKS}
        [[ ${NTASKS} -gt 64 ]] && NTASKSPERNODE=64
      else
        [[ ${NTASKS} -le 8 ]] && NTASKSPERNODE=${NTASKS}
        [[ ${NTASKS} -gt 8 ]] && NTASKSPERNODE=8
      fi
      shift 2
      ;;
    -c | --cores-per-task)
      CORESPERTASK="${2}"
      shift 2
      ;;
    -g | --gpu-per-task)
      GPUSPERTASK="${2}"
      shift 2
      ;;
    -r | --rocprof)
      [[ ${2} -eq 1 ]] && ROCPROF="rocprof --basenames on --hip-trace --timestamp on"
#      ROCPROF="rocprof --basenames on --timestamp on --stats"
      shift 2
      ;;
    --)
      shift;
      break
      ;;
    *) die "Unknown option: $1" ;;
    esac
  done

  BENCH_PATH=("$@")

  # check required argument
  [[   ${#BENCH_PATH[@]} -eq 0 ]] && die "Missing bench file name argument"
  [[ ! ${#BENCH_PATH[@]} -eq 1 ]] && die "Please enter only ONE bench file name"

  # check required conditions for correct binding
  [[ ! $((${NTASKSPERNODE}*${CORESPERTASK})) -eq 128 ]] && die "Please make sure that NTASKS*CORESPERTASK fullfills node (128) for correct binding on ADASTRA"

  return 0
}

parse_params "$@"

# Recap input parmas and arguments

echo
echo -e "# Input parameters and argument:"
echo -e "\t- arch: \t\t${ARCH}"
echo -e "\t- partition: \t\t${PARTITION}"
echo -e "\t- ntasks: \t\t${NTASKS}"
echo -e "\t- cores-per-task: \t${CORESPERTASK}"
echo -e "\t- gpus-per-task: \t${GPUSPERTASK}"
echo -e "\t- rocprof profiling: \t${ROCPROF}"
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

cat <<EOF > TRUST_sbatch.sh
#!/bin/bash

#SBATCH -J ${BENCH_NAME}
#SBATCH --partition=${PARTITION}
#SBATCH --ntasks=${NTASKS}
#SBATCH --ntasks-per-node=${NTASKSPERNODE}
#SBATCH --cpus-per-task=${CORESPERTASK}
#SBATCH --gpus-per-task=${GPUSPERTASK}
#SBATCH --gpu-bind=verbose,closest
#SBATCH --time=00:30:00
#SBATCH -o ${BENCH_NAME}_%j.out
#SBATCH -e ${BENCH_NAME}_%j.err

# TRUST environment
source ${TRUST_ROOT}/env_TRUST.sh
export CRAY_OMP_CHECK_AFFINITY=TRUE
export CRAY_ACC_DEBUG=1

module load perftools-base/22.09.0
module load perftools-lite-gpu

module list

# Informations about the current code state
echo "[DBG] Run with the following git branch:"
git -C $TRUST_ROOT rev-parse --abbrev-ref HEAD
echo "[DBG] Run with the following git commit:"
git -C $TRUST_ROOT rev-parse --short HEAD
echo "[DBG] Run with the following git diff:"
git -C $TRUST_ROOT diff
echo -e "\n\n\n"

# Create run directory and copy bench
RUN_DIR="RUN_${BENCH_NAME}_${ARCH^^}_${NTASKS}TASKS_${CORESPERTASK}CpTASK_${GPUSPERTASK}GpTASK_\${SLURM_JOB_ID}"
mkdir \${RUN_DIR}
cp ${COMPLETE_BENCH_NAME} \${RUN_DIR}
cd \${RUN_DIR}

# Prepare Mesh
echo "[DBG] Partitioning:"
[[ ! ${NTASKS} -eq 1 ]]  && time make_PAR.data ${COMPLETE_BENCH_NAME} ${NTASKS}

# Launch
if [ ${ARCH} = "cpu" ]; then
  export TRUST_DISABLE_DEVICE=1
fi

echo "[DBG] Launch:"
if [ ${NTASKS} -eq 1 ]
then
  ${ROCPROF} srun -l --mpi=cray_shasta -c ${CORESPERTASK} --cpu-bind=verbose,cores $exec ${BENCH_NAME}
else
  ${ROCPROF} srun -l --mpi=cray_shasta -c ${CORESPERTASK} --cpu-bind=verbose,cores $exec PAR_${BENCH_NAME} ${NTASKS}
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
    cp ../OpenMP_Iterateur.lml.gz .
    gunzip OpenMP_Iterateur.lml.gz
    time $TRUST_ROOT/exec/compare_lata OpenMP_Iterateur.lml PAR_OpenMP_Iterateur.lml --max_delta
  fi
else
  echo "[DBG] No numerical verification for this bench."
fi

# Get back SLURM outputs
mv ../*\${SLURM_JOB_ID}.out .
mv ../*\${SLURM_JOB_ID}.err .
EOF

echo
echo "# Subsmision"
sbatch TRUST_sbatch.sh

echo
echo "# Numerical verification:"
if [ ${COMPLETE_BENCH_NAME} == "OpenMP_Iterateur.data" ]
then
  echo "Will be done after the job."
else
  echo "No numerical verification for this bench."
fi
