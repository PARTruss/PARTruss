#include "solveMatrix.h"
#include <cuda_runtime.h>
#include "cusolverDn.h"
double *cu_A;
double *b;
double *Workspace;
int *devIpiv;
__device__ __managed__ int devInfo;

int die(cusolverStatus_t status, int devInfo, cusolverDnHandle_t handle);
int err(cusolverStatus_t status, int devInfo);
void freeMem();

int solveMatrix(double *A_in, int n, double *b_in, double *x_out)
{
    cusolverDnHandle_t handle;
    cusolverStatus_t status;
    status = cusolverDnCreate(&handle);
    if(status!=CUSOLVER_STATUS_SUCCESS) return 0;
    int Lwork;

    cudaMallocManaged(&cu_A, (size_t) (n*n*sizeof(double)));
    cudaMallocManaged(&b, (size_t) (n*sizeof(double)));
    cudaMallocManaged(&devIpiv, (size_t) n*sizeof(int));
    cudaMemcpy(cu_A, A_in, n*n*sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(b, b_in, n*sizeof(double), cudaMemcpyHostToDevice);
    cudaDeviceSynchronize();
    status = cusolverDnDgetrf_bufferSize(handle, n, n, cu_A, n, &Lwork );
    cudaDeviceSynchronize();
    if(err(status,0))
        return die(status, 0, handle);
    cudaMallocManaged(&Workspace, (size_t) Lwork*sizeof(double));
    status = cusolverDnDgetrf(handle, n, n, cu_A, n, Workspace, devIpiv, &devInfo);
    cudaDeviceSynchronize();
    if(err(status,0))
        return die(status, devInfo, handle);
    status = cusolverDnDgetrs(handle, CUBLAS_OP_T, n, 1, cu_A, n, devIpiv, b, n, &devInfo );
    cudaDeviceSynchronize();
    if(err(status,0))
        return die(status, devInfo, handle);
    cudaMemcpy(x_out, b, n*sizeof(double), cudaMemcpyDeviceToHost);
    cusolverDnDestroy(handle);
    freeMem();
    return 0;
}

int die(cusolverStatus_t status, int devInfo, cusolverDnHandle_t handle)
{
    cusolverDnDestroy(handle);
    freeMem();
    return err(status, devInfo);
}

int err(cusolverStatus_t status, int devInfo)
{
    if(status!=CUSOLVER_STATUS_SUCCESS)
        return 1;
    if(devInfo!=0)
        return devInfo;
    return 0;
}
void freeMem()
{
    cudaFree(cu_A);
    cudaFree(b);
    cudaFree(devIpiv);
}
