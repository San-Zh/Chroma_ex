ChromaPath=/home/Documents/Work/02_LQCDrefs/usqcd/chroma/local

CXX=mpicxx
CXXFLAGS= -std=c++11 -O3 -I/usr/include/libxml2  -I$(ChromaPath)/include
LDFLAGS= -L$(ChromaPath)/lib
LIBS= -lxml2 -lchromalib -lfiledb  -lfilehash  -llime -lqdp_lapack -lqdp  -lqio  -lqmp -lxmlWriter -lXPathReader

HDRS=inline_write_part_prop_w.h 
	 

OBJS=chroma_ex.o \
    inline_write_part_prop_w.o 

chroma_ex: $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(LIBS)


%.o: %.cc $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< 

clean:
	rm -rf ./chroma_ex $(OBJS) *~
