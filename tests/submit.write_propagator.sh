#!/bin/bash
#SBATCH -J wprop
#SBATCH -p vip1
#SBATCH -N 1
#SBATCH -n 4
#SBATCH --ntasks-per-node=4
#SBATCH --gres=dcu:4
#SBATCH --comment=WRF
#SBATCH --exclusive
#SBATCH --exclude="b12r4n03,b03r4n[02-05],b04r1n[01-04],b04r4n[14-17],b04r4n03,b14r4n03"
#SBATCH -o log.write.%J
#SBATCH --time=8:00:00
## SBATCH --mem=112G

export OMP_NUM_THREADS=1
echo "Begin"
date
echo ""
echo -e "SLURM_NODELIST:\n ${SLURM_NODELIST}\n"

# source /public/home/zhangkl01/public/software/soft.lqcd/soft.usqcd/chroma_cpu/gcc-7.3.1/double/env.sh
# source /public/home/zhangkl01/public/software/soft.lqcd/soft.usqcd/chroma_qdpjit/version-dtk23.04/env.sh
source /public/home/zhangkl01/public/software/soft.lqcd/soft.usqcd/chroma_quda_qdpjit/version-dtk23.04/gcc-7.3.1/env.sh

# EXENAME=chroma
EXENAME=chroma


echo "Params:"

nx=1
ny=1
nz=1
nt=4

# IniputXmlFile="./write_part_prop.xml"
# IniputXmlFile="./write_prop_spinColorMatrix.4050t0.ini.xml"
# IniputXmlFile="./write_qdpjit_spinColorMatrix.4050t0.ini.xml"
IniputXmlFile="./write_quda_spinColorMatrix.4050t0.ini.xml"
OutputXmfFile=

GeomArray="$nx $ny $nz $nt"
echo " GeomArray = $GeomArray"

NumMpiPro=$(($nx * $ny * $nz * $nt))
echo -e " NumMpiPro = $NumMpiPro\n"

CMD="mpirun -np ${NumMpiPro} ${EXENAME} -geom ${GeomArray} -i ${IniputXmlFile}"
echo -e "CMD: \n $CMD"

$CMD

echo ""
date
echo "End"
