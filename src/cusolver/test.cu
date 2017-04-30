#include <stdio.h> 
#include <stdlib.h>
#include <cuda_runtime.h>
#include "cusolverDn.h"
#include <iostream>

__device__ __managed__
  double A[] = {0.8147,    0.6324,    0.9575,    0.9572,
                0.9058,    0.0975,    0.9649,    0.4854,
                0.1270,    0.2785,    0.1576,    0.8003,
                0.9134,    0.5469,    0.9706,    0.1419};
__device__ __managed__
  double A2[] = {0.8147,    0.9058,    0.1270,    0.9134,
                0.6324,    0.0975,    0.2785,    0.5469,
                0.9575,    0.9649,    0.1576,    0.9706,
                0.9572,    0.4854,    0.8003,    0.1419};
  __device__ __managed__
  double b[] = {0.4218,
                0.9157,
                0.7922,
                0.9595};
  __device__ __managed__
  double x[] = { 17.2819,
                  0.8395,
                -15.9067,
                  1.0883};
  __device__ __managed__ double *Workspace;
  __device__ __managed__ int *devIpiv, devInfo;

int main(){
  cusolverDnHandle_t handle;
  cusolverStatus_t status;
  status = cusolverDnCreate(&handle);
  
  int Lwork;

  status = cusolverDnDgetrf_bufferSize(handle, 4, 4, A2, 4, &Lwork );

  cudaMallocManaged(&Workspace, (size_t) Lwork*sizeof(double));
  cudaMallocManaged(&devIpiv, (size_t) 4*sizeof(int));
  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
      std::cout<<A2[j+4*i]<<"\t";
    }
    std::cout<<std::endl;
  }
  std::cout<<std::endl;
  status = cusolverDnDgetrf(handle, 4, 4, A2, 4, Workspace, devIpiv, &devInfo);
  for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
      std::cout<<A2[i+4*j]<<"\t";
    }
    std::cout<<std::endl;
  }
  // A=LU => A=U
  
//  status = cusolverDnDpotrs(handle, CUBLAS_FILL_MODE_UPPER, 4, 1, A, 4, b, 4, &devInfo);

  status = cusolverDnDgetrs(handle, CUBLAS_OP_N, 4, 1, A2, 4, devIpiv, b, 4, &devInfo );
  cudaDeviceSynchronize();
  if(status == CUSOLVER_STATUS_SUCCESS){
    std::cout<<"Yay!"<<std::endl;
    std::cout<<devInfo<<std::endl;
    for(int i=0;i<4;i++){
      std::cout<<x[i]<<"\t"<<b[i]<<std::endl;
    }
  }

 return 0;
}
