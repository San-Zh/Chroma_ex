#$--Makefile--$

ChromaPath=${CHROMA_PATH}

LDFLAGS= -L$(ChromaPath)/lib
LIBS= -lxml2 -lchromalib -lfiledb  -lfilehash  -llime -lqdp_lapack -lqdp  -lqio  -lqmp -lxmlWriter -lXPathReader -lmpi -ljit -lquda -lquda_test

CXX=clang++
CXXFLAGS=  -DBUILD_QDPJIT_INTERFACE -DBUILD_QDP_INTERFACE -DMULTI_GPU -DQMP_COMMS -DQUDA_PRECISION=14 -DQUDA_RECONSTRUCT=7 \
		   -D_FILEDB_LITTLE_ENDIAN -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
		   -D__HIP_PLATFORM_AMD__=1 -D__HIP_PLATFORM_HCC__=1 -D__HIP_ROCclr__=1 \
		   -Dchromalib_EXPORTS -O3 -DNDEBUG -fPIC -pthread -std=c++20 
CXXFLAGS+= -I/usr/include/libxml2  -I$(ChromaPath)/include
# CXXFLAGS+= -DWRITE_DEBUG


HDRS=inline_write_part_prop_w.h 
	 
OBJS=chroma_ex.o inline_write_part_prop_w.o 

chroma_ex: $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(LIBS)


%.o: %.cc $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< 

clean:
	rm -rf ./chroma_ex $(OBJS) *.o
