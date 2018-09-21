#include <cv.h>
#include <highgui.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

using namespace cv;

int nChannels = 0;
int imageCols = 0;
struct ARGS{
	uchar *previous;
	uchar *current;
	uchar *next;
	uchar *output;
};

void *compute(void *args)
{
	struct ARGS *myArgs;	
	myArgs = (struct ARGS*)args;
	uchar *previous = myArgs->previous;
	uchar *current = myArgs->current;
	uchar *next = myArgs->next;
	uchar *output = myArgs->output;
	/*
	//边缘操作,这个是三个通道放在一起做卷积
	for(int j=nChannels;j<nChannels*(imageCols-1);j += nChannels)
	{//仍然舍掉一行一列
		output += nChannels;
		
		*((int*)output) =	(((*(int*)(previous+j-nChannels)>>8)&0xffffff)*(-1)+\
					((*(int*)(previous+j)>>8)&0xffffff)*(-1)+\
					((*(int*)(previous+j+nChannels)>>8)&0xffffff)*(-1)+\

					((*(int*)(current+j-nChannels)>>8)&0xffffff)*(-1)+\
					((*(int*)(current+j)>>8)&0xffffff)*(9)+\
					((*(int*)(current+j+nChannels)>>8)&0xffffff)*(-1)+\

					((*(int*)(next+j-nChannels)>>8)&0xffffff)*(-1)+\
					((*(int*)(next+j)>>8)&0xffffff)*(-1)+\
					((*(int*)(next+j+nChannels)>>8)&0xffffff)*(-1))<<8;
		//printf("%d\n",*((int*)output));
	}
	*/
	//单通道操作
	for(int j=nChannels;j<nChannels*(imageCols-1);j++)
	{//仍然舍掉一行一列
			output += nChannels;
			//边缘操作
			*output =	previous[j-nChannels]+previous[j]+previous[j+nChannels]+\
					 	current[j-nChannels]+current[j]*(-7)+current[j+nChannels]+\
						next[j-nChannels]+next[j]+next[j+nChannels];
			j++; 
			*(output+1) =	previous[j-nChannels]+previous[j]+previous[j+nChannels]+\
					 	current[j-nChannels]+current[j]*(-7)+current[j+nChannels]+\
						next[j-nChannels]+next[j]+next[j+nChannels];
			j++; 
			*(output+2) =	previous[j-nChannels]+previous[j]+previous[j+nChannels]+\
					 	current[j-nChannels]+current[j]*(-7)+current[j+nChannels]+\
						next[j-nChannels]+next[j]+next[j+nChannels]; 
	}
}

int main( int argc, char** argv )
{
	
	const char* imageName = "../TestPicture.jpg";
	Mat image;
	image = imread( imageName, CV_LOAD_IMAGE_COLOR);

	if( image.empty()==true)
	{
		printf( " No image data \n " );
		return -1;
	}
	//nChannels is global
	nChannels = image.channels();
	//imageCols is global
	imageCols = image.cols;
	Mat con_image;
	con_image.create(image.size(),image.type());
	struct timeval start,finish;
	double costTime;
	//pthread prepare
	pthread_t *thread_handles;
	//thread个数,每行一个thread
	int thread_count = image.rows-2;
	thread_handles = (pthread_t *)malloc(thread_count*sizeof(pthread_t));
	//start time
	gettimeofday(&start,0);
	//传参
	struct ARGS args[thread_count];
	for(int i=1;i<image.rows-1;i++)
	{//舍去最后一行和第一行
		//获取行地址,是中间那一行的地址
		args[i-1].previous = image.ptr<uchar>(i-1);
		args[i-1].current = image.ptr<uchar>(i);
		args[i-1].next = image.ptr<uchar>(i+1);
		args[i-1].output = con_image.ptr<uchar>(i);
		//pthread
		//值拷贝应该安全些，用指针可能会在下次循环时被修改
		pthread_create(&thread_handles[i-1],NULL,compute,(void*)(args+i-1));
	}
	for(int i=0;i<thread_count;i++)
		pthread_join(thread_handles[i],NULL);
	free(thread_handles);
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
	return 0;
}
