#!/bin/bash

# export CHROMA_PATH=/public/home/zhangkl01/public/software/soft.lqcd/soft.usqcd/chroma_qdpjit/version-dtk23.04
# export CHROMA_PATH=/public/home/zhangkl01/public/software/soft.lqcd/soft.usqcd/chroma_quda_qdpjit/version-dtk23.04/gcc-7.3.1
# export CHROMA_PATH=/home/Documents/Work/02_LQCDrefs/usqcd/chroma/local

# source ${CHROMA_PATH}/env.sh

if [ -z ${CHROMA_PATH} ]; then
    echo "CHROMA_PATH is not set"
    exit 1
else
    echo " set CHROMA_PATH=${CHROMA_PATH}"
fi

# check if chroma is in the PATH
if [ ${CHROMA_PATH}/bin/chroma == $(which chroma) ]; then
    echo " \${CHROMA_PATH}/bin/chroma is in the \$PATH"
fi

# export PATH=/public/home/zhangkl01/public/software/soft.lqcd/soft.usqcd/chroma_ex:${PATH}
