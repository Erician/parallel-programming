#include <cv.h>
#include <highgui.h>
#include <time.h>
#include <sys/time.h>

#include <mpi.h>

using namespace cv;

//思路大致这样：由0进程进行归约，其他的用于计算
//计时的函数在0进程中运行（如何所有的程序是同时创建的，这样做是正确的）

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
	const int nChannels = image.channels();
	
	int comm_sz;
	int my_rank;
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	if(my_rank != 0)
	{	
		//计算,恰好my_rank可以作为row的标识
		//算完正好可以发给进程0
		//一共使用249个进程用于计算
		//flag标识一个消息
		int flag=0;
		//存储加工好的数据		
		uchar data[5][nChannels*image.cols];
		

		for(int i=my_rank;i<image.rows-1;i+=249)
		{
			uchar *previous = image.ptr<uchar>(i-1);
			uchar *current = image.ptr<uchar>(i);
			uchar *next = image.ptr<uchar>(i+1);
			//uchar *output = con_image.ptr<uchar>(i);
			uchar *output = data[flag];
			for(int j=nChannels;j<nChannels*(image.cols-1);j++)
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
			//算完，发走
			MPI_Send(data[flag],nChannels*image.cols,MPI_CHAR,0,flag,MPI_COMM_WORLD);
			flag++;
		}
	}
	else
	{
		//接收
		//start time
		struct timeval start,finish;
		double costTime;
		gettimeofday(&start,0);
		//创建一个图像，用来存放接收到的数据
		Mat con_image;
		con_image.create(image.size(),image.type());
		//开始接收
		for(int i=1;i<comm_sz;i++)
		{
			uchar *output = con_image.ptr<uchar>(i);
			MPI_Recv(output,nChannels*image.cols,MPI_CHAR,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

			output = con_image.ptr<uchar>(i+249);
			MPI_Recv(output,nChannels*image.cols,MPI_CHAR,i,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

			output = con_image.ptr<uchar>(i+249*2);
			MPI_Recv(output,nChannels*image.cols,MPI_CHAR,i,2,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

			output = con_image.ptr<uchar>(i+249*3);
			MPI_Recv(output,nChannels*image.cols,MPI_CHAR,i,3,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

			if(i<=82)
			{
				output = con_image.ptr<uchar>(i+249*4);
				MPI_Recv(output,nChannels*image.cols,MPI_CHAR,i,4,MPI_COMM_WORLD,MPI_STATUS_IGNORE);			
			}
		}
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
	}
	MPI_Finalize();
	return 0;
}
