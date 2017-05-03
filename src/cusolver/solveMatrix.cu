#include <cuda_runtime.h>
#include "cusolverDn.h"

__device__ double *A;
__device__ double *b;
__device__ double *Workspace;
__device__ int *devIpiv;
__device__ __managed__ int devInfo;

int solveMatrix(double *A_in, int n, double *b_in, double *x_out){
  cusolverDnHandle_t handle;
  cusolverStatus_t status;
  status = cusolverDnCreate(&handle);
  if(status!=CUSOLVER_STATUS_SUCCESS) return 0;
  int Lwork;

  cudaMallocManaged(&A, (size_t) (n*n*sizeof(double)));
  cudaMallocManaged(&b, (size_t) (n*sizeof(double)));
  cudaMallocManaged(&devIpiv, (size_t) n*sizeof(int));
  cudaMemcpy(A, A_in, n*n*sizeof(double), cudaMemcpyHostToDevice);
  cudaMemcpy(b, b_in, n*sizeof(double), cudaMemcpyHostToDevice);
  cudaDeviceSynchronize();
  status = cusolverDnDgetrf_bufferSize(handle, n, n, A, n, &Lwork );
  cudaDeviceSynchronize();
  if(status!=CUSOLVER_STATUS_SUCCESS){
    return 1;
    cusolverDnDestroy(handle);
  }
  cudaMallocManaged(&Workspace, (size_t) Lwork*sizeof(double));
  status = cusolverDnDgetrf(handle, n, n, A, n, Workspace, devIpiv, &devInfo);
  cudaDeviceSynchronize();
  if(status!=CUSOLVER_STATUS_SUCCESS){
    return 1;
    cusolverDnDestroy(handle);
  }
  if(devInfo!=0){
    return devInfo;
    cusolverDnDestroy(handle);
  }
  status = cusolverDnDgetrs(handle, CUBLAS_OP_T, n, 1, A, n, devIpiv, b, n, &devInfo );
  cudaDeviceSynchronize();
  if(status!=CUSOLVER_STATUS_SUCCESS){
   return 1;
   cusolverDnDestroy(handle);
  }
  if(devInfo!=0){
    return devInfo;
    cusolverDnDestroy(handle);
  }
  cudaMemcpy(x_out, b, n*sizeof(double), cudaMemcpyDeviceToHost);
  cusolverDnDestroy(handle);
  return 0;
}
