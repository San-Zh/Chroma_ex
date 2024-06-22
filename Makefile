#$--Makefile--$

ChromaPath ?= ${CHROMA_PATH}

LDFLAGS := -L$(ChromaPath)/lib -Wl,-rpath -Wl,$(ChromaPath)/lib
LIBS    :=  -lxml2 -lchromalib -lfiledb  -lfilehash  -llime -lqdp_lapack -lqdp  -lqio  -lqmp -lxmlWriter -lXPathReader -lmpi 
# LIBS    :+= -ljit 
# LIBS    :+= -lquda -lquda_test

CXX      := clang++

# CXXFLAGS += -DBUILD_QDPJIT_INTERFACE -DBUILD_QDP_INTERFACE -DMULTI_GPU -DQMP_COMMS -DQUDA_PRECISION=14 -DQUDA_RECONSTRUCT=7
CXXFLAGS += -D_FILEDB_LITTLE_ENDIAN -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
CXXFLAGS += -D__HIP_PLATFORM_AMD__=1 -D__HIP_PLATFORM_HCC__=1 -D__HIP_ROCclr__=1
CXXFLAGS += -Dchromalib_EXPORTS -O3 -DNDEBUG -fPIC -pthread -std=c++20 
CXXFLAGS += -I/usr/include/libxml2  -I$(ChromaPath)/include
# CXXFLAGS += -DWRITE_DEBUG


HDRS := inline_write_part_prop_w.h 
	 
OBJS := chroma_ex.o inline_write_part_prop_w.o 

all: chroma_ex

%.o: %.cc $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@


chroma_ex: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)
# $(CXX) -o $@ $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(LIBS)

chroma_ex_debug: $(OBJS)
	$(CXX) -g $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

install: chroma_ex
	cp chroma_ex $(ChromaPath)/bin/chroma_ex

uninstall:
	rm $(ChromaPath)/bin/chroma_ex
	rm -rf ./chroma_ex $(OBJS) *.o

clean:
	rm -rf ./chroma_ex $(OBJS) *.o

.PHONY: clean install