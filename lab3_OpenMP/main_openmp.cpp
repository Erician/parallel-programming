#include <cv.h>
#include <highgui.h>
#include <time.h>
#include <sys/time.h>

#include <omp.h>

using namespace cv;

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
	Mat con_image;
	con_image.create(image.size(),image.type());
	struct timeval start,finish;
	double costTime;
	//start time
	gettimeofday(&start,0);

	#pragma omp parallel for
	for(int i=1;i<image.rows-1;i++)
	{//舍去最后一行和第一行
		//获取行地址,是中间那一行的地址
		uchar *previous = image.ptr<uchar>(i-1);
		uchar *current = image.ptr<uchar>(i);
		uchar *next = image.ptr<uchar>(i+1);

		uchar *output = con_image.ptr<uchar>(i);
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
	return 0;
}
