#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// #define IMAGE_WIDTH  640
// #define IMAGE_HEIGHT 480
// #define IMAGE_WIDTH  170
// #define IMAGE_HEIGHT 38
// #define S (IMAGE_WIDTH/8)
#define T (0.15f)

void adaptiveThreshold(unsigned char* input, unsigned char* bin, const int IMAGE_WIDTH, const int IMAGE_HEIGHT)
{
    const int S = IMAGE_WIDTH / 8;

    unsigned long* integralImg = 0;
    int i, j;
    long sum=0;
    int count=0;
    int index;
    int x1, y1, x2, y2;
    int s2 = S/2;

    // create the integral image
    integralImg = (unsigned long*)malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(unsigned long*));

    for (i=0; i<IMAGE_WIDTH; i++)
    {
        // reset this column sum
        sum = 0;

        for (j=0; j<IMAGE_HEIGHT; j++)
        {
            index = j*IMAGE_WIDTH+i;

            sum += input[index];
            if (i==0)
                integralImg[index] = sum;
            else
                integralImg[index] = integralImg[index-1] + sum;
        }
    }

    // perform thresholding
    for (i=0; i<IMAGE_WIDTH; i++)
    {
        for (j=0; j<IMAGE_HEIGHT; j++)
        {
            index = j*IMAGE_WIDTH+i;

            // set the SxS region
            x1=i-s2; x2=i+s2;
            y1=j-s2; y2=j+s2;

            // check the border
            if (x1 < 0) x1 = 0;
            if (x2 >= IMAGE_WIDTH) x2 = IMAGE_WIDTH-1;
            if (y1 < 0) y1 = 0;
            if (y2 >= IMAGE_HEIGHT) y2 = IMAGE_HEIGHT-1;

            count = (x2-x1)*(y2-y1);

            // I(x,y)=s(x2,y2)-s(x1,y2)-s(x2,y1)+s(x1,x1)
            sum = integralImg[y2*IMAGE_WIDTH+x2] -
                  integralImg[y1*IMAGE_WIDTH+x2] -
                  integralImg[y2*IMAGE_WIDTH+x1] +
                  integralImg[y1*IMAGE_WIDTH+x1];

            if ((long)(input[index]*count) < (long)(sum*(1.0-T)))
                bin[index] = 0;
            else
                bin[index] = 255;
        }
    }

    free (integralImg);
}

int main (int argc, char** argv)
{
    IplImage* cvFrame;
    IplImage* grayImg;
    IplImage* binImg;
    int key;
    clock_t start_t, end_t;     // to evaluate processing time
    string filepath;

    // Get filepath from command-line argument
    if (argc > 1)
    {
        filepath = argv[1];
    }
    else
    {
        filepath = "image.bmp";
    }

    // Load color image
    // cvFrame = cvLoadImage(filepath.c_str(), CV_LOAD_IMAGE_UNCHANGED);
    cvFrame = cvLoadImage(filepath.c_str(), CV_LOAD_IMAGE_COLOR);
    CvSize frameSize = cvGetSize(cvFrame);

    // Warm up
    grayImg = cvCreateImage(frameSize, IPL_DEPTH_8U, 1);
    cvCvtColor(cvFrame, grayImg, CV_RGB2GRAY);

    // Convert to gray image
    start_t = clock();
    grayImg = cvCreateImage(frameSize, IPL_DEPTH_8U, 1);
    cvCvtColor(cvFrame, grayImg, CV_RGB2GRAY);
    end_t = clock();
    cout << "Convert to gray time in milliseconds: " << ((double) (end_t - start_t)) * 1000 / CLOCKS_PER_SEC << "\n";

    //cvNamedWindow("Input", 1);
    //cvNamedWindow("Output", 1);

    // Binarization
    start_t = clock();
    binImg = cvCreateImage(frameSize, 8, 1);
    // adaptiveThreshold((unsigned char*)cvFrame->imageData, (unsigned char*)binImg->imageData);
    adaptiveThreshold((unsigned char*)grayImg->imageData, (unsigned char*)binImg->imageData, frameSize.width, frameSize.height);
    end_t = clock();
    cout << "Binarization time in milliseconds: " << ((double) (end_t - start_t)) * 1000 / CLOCKS_PER_SEC << "\n";

    //cvShowImage("Input", cvFrame);
    //cvShowImage("Output", binImg);

    //key = cvWaitKey(0);

    // Release memory
    cvReleaseImage(&cvFrame);
    cvReleaseImage(&grayImg);
    cvReleaseImage(&binImg);

    return 0;
}
