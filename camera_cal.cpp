
#include "camera_cal.h"
#include <unistd.h>


int main(int argc,char **argv)
{
    struct system_s system_base,*p=&system_base;
    p->image.load();
    p->image.show();
    p->pMats.init();
    p->pMats.print();

    int keyStroke=0;
    while(1)
    {
        keyStroke = cv::waitKey(100);
        if(keyStroke == 'q')
           break;
    }
    return 0;
}
