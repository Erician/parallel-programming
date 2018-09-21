#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdlib.h>
//#include <conio.h>

#define ARRAY_SIZE 80
#define ARRAY_SIZE_IN_BYTE (sizeof(unsigned int)*(ARRAY_SIZE))

__global__ void what_is_my_id(unsigned int *const block,
	unsigned int *const thread,
	unsigned int *const warp,
	unsigned int *const calc_thread)
{
	//int x = threadIdx.x + threadIdx.y*blockDim.x;
	//int y = blockIdx.x + blockIdx.y*gridDim.x;
	//const unsigned int thread_idx = x + y*gridDim.x*blockDim.x;
	int idx = (blockIdx.x * blockDim.x) + threadIdx.x;
	int idy = (blockIdx.y * blockDim.y) + threadIdx.y;
	int thread_idx = idx + idy* blockDim.x * gridDim.x;

	block[thread_idx] = blockDim.y;
	thread[thread_idx] = idy;
	calc_thread[thread_idx] = thread_idx;
}

unsigned int cpu_block[ARRAY_SIZE];
unsigned int cpu_thread[ARRAY_SIZE];
unsigned int cpu_warp[ARRAY_SIZE];
unsigned int cpu_calc_thread[ARRAY_SIZE];

int main(void)
{
	dim3 grid(2, 5);
	dim3 block(2, 4);
	char ch;

	unsigned int *gpu_block;
	unsigned int *gpu_thread;
	unsigned int *gpu_warp;
	unsigned int *gpu_calc_thread;

	unsigned int i;

	cudaMalloc((void**)&gpu_block, ARRAY_SIZE_IN_BYTE); //需要两层指针，修改gpu_block的值
	cudaMalloc((void**)&gpu_thread, ARRAY_SIZE_IN_BYTE);
	cudaMalloc((void**)&gpu_warp, ARRAY_SIZE_IN_BYTE);
	cudaMalloc((void**)&gpu_calc_thread, ARRAY_SIZE_IN_BYTE);

	what_is_my_id << <grid, block >> >(gpu_block, gpu_thread, gpu_warp, gpu_calc_thread);

	cudaMemcpy(cpu_block, gpu_block, ARRAY_SIZE_IN_BYTE, cudaMemcpyDeviceToHost);
	cudaMemcpy(cpu_thread, gpu_thread, ARRAY_SIZE_IN_BYTE, cudaMemcpyDeviceToHost);
	cudaMemcpy(cpu_warp, gpu_warp, ARRAY_SIZE_IN_BYTE, cudaMemcpyDeviceToHost);
	cudaMemcpy(cpu_calc_thread, gpu_calc_thread, ARRAY_SIZE_IN_BYTE, cudaMemcpyDeviceToHost);

	cudaFree(gpu_block);
	cudaFree(gpu_thread);
	cudaFree(gpu_warp);
	cudaFree(gpu_calc_thread);

	for (int i = 0; i<ARRAY_SIZE; i++)
	{
		printf("Calculated Thread:%3u - Block:%2u - Warp %2u - Thread %3u\n", cpu_calc_thread[i], cpu_block[i], cpu_warp[i], cpu_thread[i]);
	}
	ch = getchar();
	return 0;
}
