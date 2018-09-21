#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdlib.h>

#include <cv.h>
#include <highgui.h>
#include <time.h>
#include <sys/time.h>

using namespace cv;

#define ARRAY_SIZE 6220800
#define COLS 5760
#define ARRAY_SIZE_IN_BYTE (sizeof(unsigned char)*(ARRAY_SIZE))

__global__ void compute(uchar *dev_src,uchar *dev_dst)
{
	int idx = (blockIdx.x * blockDim.x) + threadIdx.x;
	int idy = (blockIdx.y * blockDim.y) + threadIdx.y;
	int thread_idx = idx + idy* blockDim.x * gridDim.x;
	//仍然忽略边界	
	//这里没有考虑计算的边界，但是也没有出现像段错误的那种错误？？？？？
	//if(idy!=0&&idy!=1079&&idx>2&&idx<5760-3)
	*(dev_dst+thread_idx) = dev_src[thread_idx-COLS-3]+dev_src[thread_idx-COLS]+dev_src[thread_idx-COLS+3]+\
							dev_src[thread_idx]*(-7)+dev_src[thread_idx-3]+dev_src[thread_idx+3]+\
							dev_src[thread_idx+COLS-3]+dev_src[thread_idx+COLS]+dev_src[thread_idx+COLS+3];
}


int main(void)
{
	
	const char* imageName = "../TestPicture.jpg";
	Mat image;
	image = imread( imageName, CV_LOAD_IMAGE_COLOR);

	if( image.empty()==true)
	{
		printf( " No image data \n " );
		return -1;
	}
	const int nChannels = image.channels();
	Mat con_image;
	con_image.create(image.size(),image.type());
	struct timeval start,finish;
	double costTime;
	//start time
	gettimeofday(&start,0);
	
	dim3 grid(81, 150);
	dim3 block(128, 4);	

	uchar *dev_dst;
	uchar *dev_src;
	cudaMalloc((void**)&dev_dst, ARRAY_SIZE_IN_BYTE);
	cudaMalloc((void**)&dev_src, ARRAY_SIZE_IN_BYTE);
		
	cudaMemcpy(dev_src, image.ptr<uchar>(0), ARRAY_SIZE_IN_BYTE, cudaMemcpyHostToDevice);
	compute<<<grid, block>>>(dev_src,dev_dst);
	cudaMemcpy(con_image.ptr<uchar>(0), dev_dst, ARRAY_SIZE_IN_BYTE, cudaMemcpyDeviceToHost);

	
	con_image.row(0).setTo(Scalar(0));
	con_image.row(con_image.rows-1).setTo(Scalar(0));
    con_image.col(0).setTo(Scalar(0));
    con_image.col(con_image.cols - 1).setTo(Scalar(0));
	//finish time
	gettimeofday(&finish,0);
	costTime = 1000000*(finish.tv_sec-start.tv_sec)+finish.tv_usec-start.tv_usec;
	printf("cost time:%lf us.\n",costTime);
	IplImage qImg;
	qImg = IplImage(con_image);
	cvSaveImage("./con.jpg", &qImg);
	
	cudaFree(dev_src);
	cudaFree(dev_dst);
	return 0;
}
