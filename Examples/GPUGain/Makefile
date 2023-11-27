# Copyright OpenFX and contributors to the OpenFX project.
# SPDX-License-Identifier: BSD-3-Clause

UNAME_SYSTEM := $(shell uname -s)

# Note: Add -DDEBUG to turn on logging
CXXFLAGS = --std=c++20 -fvisibility=hidden -I../../include -I../../Support/include -I../../Support/Plugins/include -DOFX_SUPPORTS_OPENGLRENDER

ifeq ($(UNAME_SYSTEM), Linux)
	AMDAPP_PATH ?= /opt/AMDAPP
	CXXFLAGS += -I${AMDAPP_PATH}/include -fPIC
	CUDAPATH ?= /usr/local/cuda
	NVCC = ${CUDAPATH}/bin/nvcc
	NVCCFLAGS = --compiler-options="-fPIC"
	LDFLAGS = -shared -fvisibility=hidden -L${CUDAPATH}/lib64 -lcuda -lcudart_static
	BUNDLE_DIR = GPUGain.ofx.bundle/Contents/Linux-x86-64/
	CUDA_OBJ = CudaKernel.o
else
	ARCH_FLAGS = -arch arm64 -arch x86_64
	CXXFLAGS += ${ARCH_FLAGS}
	LDFLAGS = -bundle -fvisibility=hidden -F/Library/Frameworks -framework OpenCL -framework Metal -framework AppKit
	LDFLAGS += ${ARCH_FLAGS}
	BUNDLE_DIR = GPUGain.ofx.bundle/Contents/MacOS/
	METAL_OBJ = MetalKernel.o
endif

GPUGain.ofx: GPUGain.o ${CUDA_OBJ} $(METAL_OBJ) OpenCLKernel.o ofxsCore.o ofxsImageEffect.o ofxsInteract.o ofxsLog.o ofxsMultiThread.o ofxsParams.o ofxsProperty.o ofxsPropertyValidation.o
	$(CXX) $^ -o $@ $(LDFLAGS)
	mkdir -p $(BUNDLE_DIR)
	cp GPUGain.ofx $(BUNDLE_DIR)

CudaKernel.o: CudaKernel.cu
	${NVCC} -c $< $(NVCCFLAGS)

MetalKernel.o: MetalKernel.mm
	$(CXX) -c $< $(CXXFLAGS)

%.o: ../../Support/Library/%.cpp
	$(CXX) -c $< $(CXXFLAGS)

clean:
	rm -f *.o *.ofx
	rm -fr GPUGain.ofx.bundle

install: GPUGain.ofx
	cp -fr GPUGain.ofx.bundle /Library/OFX/Plugins
