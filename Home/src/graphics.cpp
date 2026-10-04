#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>

void dda(int x1, int y1, int x2, int y2)
{
    float x , y ;
    float xinc, yinc;

    int dx , dy , steps ;
    dx = x2 - x1;
    dy = y2 - y1;

    steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

    if (steps == 0) {
        putpixel(x1, y1, YELLOW);
        return;
    }

    xinc = (float)dx / steps;
    yinc = (float)dy / steps;

    x = x1;
    y = y1;

    for(int i = 0; i <= steps; i++){
        putpixel((int)(x),(int)(y), RED);
        x = x + xinc;
        y = y + yinc;
        delay(10);
    }
}


int main(){
    int gd = DETECT, gm;
    char bgiPath[] = "C:\\TDM-GCC-64\\lib\\libbgi.a";
    initgraph(&gd, &gm, bgiPath);
    dda(100, 500,700, 800);
    getch();
    closegraph();
}