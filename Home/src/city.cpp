/* ============================================================
   DYNAMIC CARTOON CITY  —  graphics.h / WinBGIm
   Features: moving cars, walking people, flying birds,
             day/night cycle, scrolling clouds, sun/moon arc,
             street lamps glow, animated building windows.
   Controls: any key = exit
   ============================================================ */

#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// ─────────────────────────────────────────────
//  SCREEN CONSTANTS
// ─────────────────────────────────────────────
const int SW  = 640;   // screen width
const int SH  = 480;   // screen height
const int GY  = 355;   // top of road / ground line

// ─────────────────────────────────────────────
//  ENTITY STRUCTS
// ─────────────────────────────────────────────
struct Car    { int x, y, spd, col; bool right; };
struct Person { int x, spd, frame, bodyCol, pantsCol; bool right; };
struct Bird   { float x, y, spd; int wing, timer; };

// ─────────────────────────────────────────────
//  GLOBALS
// ─────────────────────────────────────────────
const int NC = 8, NP = 5, NB = 6;
Car    cars[NC];
Person ppl[NP];
Bird   birds[NB];
int    ticks = 0;

// ─────────────────────────────────────────────
//  INIT
// ─────────────────────────────────────────────
void initAll()
{
    // Cars: first 4 go right (lane 1), last 4 go left (lane 2)
    int cc[] = {RED, BLUE, GREEN, WHITE, MAGENTA, CYAN, LIGHTRED, LIGHTGREEN};
    for (int i = 0; i < NC; i++) {
        cars[i].right = (i < 4);
        cars[i].y     = (i < 4) ? GY + 22 : GY + 54;
        cars[i].x     = (i % 4) * 168 + (cars[i].right ? 0 : 84);
        cars[i].spd   = 2 + (i % 3);
        cars[i].col   = cc[i];
    }
    // People walking on left sidewalk
    int bc[] = {RED, BLUE, CYAN, MAGENTA, GREEN};
    int pc[] = {BLUE, DARKGRAY, BLUE, DARKGRAY, BLUE};
    for (int i = 0; i < NP; i++) {
        ppl[i].right    = (i % 2 == 0);
        ppl[i].x        = 30 + i * 115;
        ppl[i].spd      = 1;
        ppl[i].frame    = i % 2;
        ppl[i].bodyCol  = bc[i];
        ppl[i].pantsCol = pc[i];
    }
    // Birds fly across the sky
    for (int i = 0; i < NB; i++) {
        birds[i].x     = (float)(i * 112);
        birds[i].y     = 55.0f + i * 24.0f;
        birds[i].spd   = 1.3f + i * 0.3f;
        birds[i].wing  = i % 2;
        birds[i].timer = i * 6;
    }
}

// ─────────────────────────────────────────────
//  TIME-OF-DAY helpers
//  Full cycle = 600 ticks  (day 0-299, night 300-599)
// ─────────────────────────────────────────────
int  cyclePhase(int t)  { return t % 600; }
bool isNight(int t)     { return cyclePhase(t) >= 300; }

int skyCol(int t) {
    int p = cyclePhase(t);
    if (p < 20)  return BLUE;      // pre-dawn
    if (p < 60)  return CYAN;      // morning
    if (p < 260) return CYAN;      // full day
    if (p < 300) return LIGHTRED;  // dusk / sunset
    if (p < 330) return BLUE;      // early night
    return BLACK;                   // deep night
}

const char* timeLabel(int t) {
    int p = cyclePhase(t);
    if (p < 20 || p >= 580) return "MIDNIGHT";
    if (p < 60)             return "DAWN";
    if (p < 260)            return "DAY";
    if (p < 300)            return "DUSK";
    if (p < 330)            return "EVENING";
    return "NIGHT";
}

int clockHour(int t) {
    int p = cyclePhase(t);
    return (p < 300) ? (6 + p * 12 / 300) % 24
                     : (18 + (p-300) * 12 / 300) % 24;
}

// ─────────────────────────────────────────────
//  DRAW: SKY + CELESTIAL BODIES
// ─────────────────────────────────────────────
void drawSky(int t) {
    setfillstyle(SOLID_FILL, skyCol(t));
    bar(0, 0, SW, GY - 5);
}

void drawStars(int t) {
    if (!isNight(t)) return;
    int xs[] = {15,55,100,150,195,240,285,325,370,415,
                460,505,550,595,625, 35, 80,130,175,220,
                265,310,355,400,445,490,535,580,20,70};
    int ys[] = {14, 28, 11, 44, 22, 17, 39, 19, 50, 13,
                34,  9, 24, 40, 20, 66, 82, 70, 90, 76,
                84, 64, 92, 80, 70, 84, 96, 58,103, 90};
    setcolor(WHITE);
    for (int i = 0; i < 30; i++) {
        putpixel(xs[i],   ys[i], WHITE);
        putpixel(xs[i]+1, ys[i], WHITE);
    }
}

void drawSun(int t) {
    int p = cyclePhase(t);
    if (p >= 300) return;
    // Arc left→right across the sky
    float a = 3.14159f - (float)p / 300.0f * 3.14159f;
    int sx = (int)(SW/2 + cos(a) * 290);
    int sy = (int)(GY - 40 - fabs(sin(a)) * 240);

    setcolor(YELLOW); setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(sx, sy, 22, 22);
    for (int r = 0; r < 8; r++) {
        float ang = r * 3.14159f / 4.0f + ticks * 0.01f;
        line(sx + (int)(cos(ang)*25), sy + (int)(sin(ang)*25),
             sx + (int)(cos(ang)*37), sy + (int)(sin(ang)*37));
    }
}

void drawMoon(int t) {
    int p = cyclePhase(t);
    if (p < 300) return;
    float a = 3.14159f - (float)(p-300) / 300.0f * 3.14159f;
    int mx = (int)(SW/2 + cos(a) * 270);
    int my = (int)(GY - 50 - fabs(sin(a)) * 200);

    setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(mx, my, 24, 24);
    setfillstyle(SOLID_FILL, BLACK);       // crescent shadow
    fillellipse(mx + 10, my - 5, 19, 19);
}

void drawClouds(int t, int off) {
    if (isNight(t)) return;
    setfillstyle(SOLID_FILL, WHITE); setcolor(WHITE);

    // 3 clouds, each spaced ~200px apart, scroll right with offset
    int bases[] = {0, 210, 420};
    for (int i = 0; i < 3; i++) {
        int cx = ((bases[i] + off) % (SW + 150)) - 70;
        int cy = 80 + i * 18;
        fillellipse(cx,      cy,     38, 18);
        fillellipse(cx + 30, cy - 8, 28, 15);
        fillellipse(cx - 24, cy + 4, 22, 12);
    }
}

// ─────────────────────────────────────────────
//  DRAW: BUILDINGS
// ─────────────────────────────────────────────
void drawBuilding(int x, int top, int w, int h, int wc, int rc, bool night)
{
    setfillstyle(SOLID_FILL, wc);
    bar(x, top, x + w, GY);
    setfillstyle(SOLID_FILL, rc);
    bar(x, top, x + w, top + 5);
    setcolor(BLACK);
    rectangle(x, top, x + w, GY);

    int wW=7, wH=9, gX=6, gY2=8;
    int cols = (w - 8) / (wW + gX);
    int rows = (h - 16) / (wH + gY2);
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++) {
            int wx = x + 4 + c * (wW + gX);
            int wy = top + 9 + r * (wH + gY2);
            bool lit = night ? ((r * 5 + c * 3) % 7 < 5)
                             : ((r + c) % 5 == 0);
            setfillstyle(SOLID_FILL,
                lit ? (night ? YELLOW : LIGHTBLUE)
                    : (night ? BLACK  : DARKGRAY));
            bar(wx, wy, wx + wW, wy + wH);
        }
}

void drawSkyscraper(int x, int top, int w, int h, bool night)
{
    for (int col = 0; col < w; col++) {
        setcolor((col % 10 < 5) ? (night ? BLUE : CYAN)
                                : (night ? DARKGRAY : LIGHTBLUE));
        line(x + col, top, x + col, GY);
    }
    setcolor(BLACK); rectangle(x, top, x + w, GY);

    // Blinking antenna light
    setcolor(LIGHTRED);
    line(x + w/2, top, x + w/2, top - 20);
    bool blink = (ticks / 20) % 2 == 0;
    setfillstyle(SOLID_FILL, blink ? LIGHTRED : RED);
    fillellipse(x + w/2, top - 22, 4, 4);

    // Windows
    for (int r = 0; r < (h - 10) / 16; r++)
        for (int c = 0; c < w / 14; c++) {
            bool lit = night ? ((r * 3 + c * 7) % 5 < 4) : ((r + c) % 4 == 0);
            setfillstyle(SOLID_FILL, lit ? YELLOW : BLACK);
            bar(x + 5 + c*14, top + 10 + r*16,
                x + 13 + c*14, top + 18 + r*16);
        }
}

// ─────────────────────────────────────────────
//  DRAW: ROAD, LAMPS, TREES
// ─────────────────────────────────────────────
void drawRoad() {
    setfillstyle(SOLID_FILL, DARKGRAY); bar(0, GY, SW, GY + 72);
    setcolor(YELLOW);
    for (int x = 0; x < SW; x += 40) line(x, GY+36, x+22, GY+36);
    setcolor(WHITE);
    for (int x = 0; x < SW; x += 40) {
        line(x, GY+18, x+22, GY+18);
        line(x, GY+55, x+22, GY+55);
    }
    // Kerbs
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    bar(0, GY-8, SW, GY);
    bar(0, GY+72, SW, GY+80);
}

void drawLamp(int x, bool night) {
    setcolor(LIGHTGRAY);
    line(x, GY-8, x, GY-62);
    line(x, GY-62, x+20, GY-62);
    setfillstyle(SOLID_FILL, night ? YELLOW : DARKGRAY);
    fillellipse(x+20, GY-64, 7, 5);
    if (night) {
        for (int i = 1; i <= 18; i++) {
            setcolor(i < 9 ? YELLOW : DARKGRAY);
            line(x+20-i, GY-64+i, x+20+i, GY-64+i);
        }
    }
}

void drawTree(int x) {
    setfillstyle(SOLID_FILL, 6);            // BROWN trunk
    bar(x-4, GY-42, x+4, GY-8);
    setfillstyle(SOLID_FILL, GREEN);
    fillellipse(x,     GY-60, 20, 20);
    fillellipse(x-13,  GY-48, 14, 14);
    fillellipse(x+13,  GY-48, 14, 14);
    fillellipse(x,     GY-72, 12, 12);
}

// ─────────────────────────────────────────────
//  DRAW: CARS
// ─────────────────────────────────────────────
void drawCar(Car& c) {
    int x = c.x, y = c.y;
    // Shadow
    setfillstyle(SOLID_FILL, DARKGRAY);
    fillellipse(x+26, y+7, 26, 5);
    // Body + cabin
    setfillstyle(SOLID_FILL, c.col);
    bar(x, y-13, x+52, y);
    bar(x+8, y-25, x+44, y-13);
    // Windows
    setfillstyle(SOLID_FILL, LIGHTBLUE);
    bar(x+10, y-23, x+24, y-15);
    bar(x+26, y-23, x+42, y-15);
    // Wheels
    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(x+12, y+5, 8, 8);
    fillellipse(x+40, y+5, 8, 8);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    fillellipse(x+12, y+5, 4, 4);
    fillellipse(x+40, y+5, 4, 4);
    // Lights
    if (c.right) {
        setfillstyle(SOLID_FILL, YELLOW); fillellipse(x+50, y-6, 3, 3);
        setfillstyle(SOLID_FILL, RED);    fillellipse(x+2,  y-6, 3, 3);
    } else {
        setfillstyle(SOLID_FILL, YELLOW); fillellipse(x+2,  y-6, 3, 3);
        setfillstyle(SOLID_FILL, RED);    fillellipse(x+50, y-6, 3, 3);
    }
}

// ─────────────────────────────────────────────
//  DRAW: PEOPLE (cartoon stick-people)
// ─────────────────────────────────────────────
void drawPerson(Person& p) {
    int x = p.x, y = GY - 8;
    int sw2 = (p.frame == 0) ? 5 : -5;
    if (!p.right) sw2 = -sw2;

    // Shadow
    setfillstyle(SOLID_FILL, DARKGRAY); fillellipse(x+7, y+3, 8, 3);
    // Legs
    setcolor(p.pantsCol);
    line(x+7, y-14, x+7+sw2, y);
    line(x+7, y-14, x+7-sw2, y);
    // Body (shirt)
    setfillstyle(SOLID_FILL, p.bodyCol);
    bar(x+2, y-28, x+13, y-14);
    // Arms
    setcolor(p.bodyCol);
    line(x+7, y-26, x+7-sw2, y-18);
    line(x+7, y-26, x+7+sw2, y-18);
    // Head
    setfillstyle(SOLID_FILL, 6); // skin tone
    fillellipse(x+7, y-33, 6, 7);
    // Hair
    setfillstyle(SOLID_FILL, p.bodyCol == RED ? DARKGRAY : BLACK);
    arc(x+7, y-34, 0, 180, 6);
}

// ─────────────────────────────────────────────
//  DRAW: BIRDS
// ─────────────────────────────────────────────
void drawBird(Bird& b, bool night) {
    if (night) return; // birds sleep at night
    int x = (int)b.x, y = (int)b.y;
    setcolor(BLACK);
    if (b.wing == 0) {
        arc(x,    y, 0,   180, 9);
        arc(x+18, y, 0,   180, 9);
    } else {
        arc(x,    y, 180, 360, 9);
        arc(x+18, y, 180, 360, 9);
    }
}

// ─────────────────────────────────────────────
//  HUD OVERLAY
// ─────────────────────────────────────────────
void drawHUD(int t) {
    // Time-of-day label (top-left)
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 0, 90, 18);
    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    outtextxy(5, 4, (char*)timeLabel(t));

    // Clock (top-right)
    char clk[8];
    sprintf(clk, "%02d:00", clockHour(t));
    setfillstyle(SOLID_FILL, BLACK); bar(SW-50, 0, SW, 18);
    outtextxy(SW-45, 4, clk);

    // Bottom hint
    setcolor(DARKGRAY);
    outtextxy(10, SH-15, "Press any key to exit");
}

// ─────────────────────────────────────────────
//  UPDATE: move all entities
// ─────────────────────────────────────────────
void update() {
    // Cars wrap around screen edges
    for (int i = 0; i < NC; i++) {
        if (cars[i].right) { cars[i].x += cars[i].spd; if (cars[i].x > SW+65) cars[i].x = -65; }
        else               { cars[i].x -= cars[i].spd; if (cars[i].x < -65)   cars[i].x = SW+65; }
    }
    // People walk on sidewalk
    for (int i = 0; i < NP; i++) {
        if (ppl[i].right) { ppl[i].x += ppl[i].spd; if (ppl[i].x > SW+20) ppl[i].x = -20; }
        else              { ppl[i].x -= ppl[i].spd; if (ppl[i].x < -20)   ppl[i].x = SW+20; }
        if (ticks % 10 == 0) ppl[i].frame ^= 1; // walk cycle
    }
    // Birds: float in a sine wave
    for (int i = 0; i < NB; i++) {
        birds[i].x += birds[i].spd;
        if (birds[i].x > SW + 45) birds[i].x = -45;
        birds[i].y += (float)sin(ticks * 0.06f + i * 1.2f) * 0.6f;
        birds[i].timer++;
        if (birds[i].timer % 18 == 0) birds[i].wing ^= 1; // flap
    }
    ticks++;
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────
int main()
{
    int gd = DETECT, gm;
    char bgiPath[] = "C:\\TDM-GCC-64\\lib\\libbgi.a";
    initgraph(&gd, &gm, bgiPath);
    initAll();

    int t         = 0;   // time-of-day counter (0..599)
    int cloudOff  = 0;   // cloud scroll offset
    int page      = 0;   // double-buffer page

    while (!kbhit())
    {
        bool night = isNight(t);

        // ── Draw to back buffer ──────────────────────────
        setactivepage(page ^ 1);

        drawSky(t);
        drawStars(t);
        drawSun(t);
        drawMoon(t);
        drawClouds(t, cloudOff);
        cloudOff = (cloudOff + 1) % (SW + 150);

        // Buildings (back → front order)
        drawSkyscraper(278, GY-288, 64, 288, night);
        drawBuilding(  0,  GY-175, 60, 175, DARKGRAY,  LIGHTGRAY, night);
        drawBuilding( 65,  GY-218, 78, 218, 8,         CYAN,      night);
        drawBuilding(148,  GY-150, 55, 150, LIGHTGRAY, DARKGRAY,  night);
        drawBuilding(208,  GY-235, 65, 235, 8,         BLUE,      night);
        drawBuilding(347,  GY-260, 72, 260, DARKGRAY,  LIGHTRED,  night);
        drawBuilding(424,  GY-180, 65, 180, LIGHTGRAY, DARKGRAY,  night);
        drawBuilding(494,  GY-225, 78, 225, 8,         CYAN,      night);
        drawBuilding(577,  GY-165, 63, 165, LIGHTGRAY, BLUE,      night);

        // Road, pavement, ground
        drawRoad();
        setfillstyle(SOLID_FILL, night ? DARKGRAY : LIGHTGRAY);
        bar(0, GY+80, SW, SH);

        // Street furniture
        drawTree(130); drawTree(314); drawTree(480); drawTree(622);
        drawLamp( 58, night);
        drawLamp(222, night);
        drawLamp(384, night);
        drawLamp(544, night);

        // Moving entities
        for (int i = 0; i < NC; i++) drawCar(cars[i]);
        for (int i = 0; i < NP; i++) drawPerson(ppl[i]);
        for (int i = 0; i < NB; i++) drawBird(birds[i], night);

        // HUD
        drawHUD(t);

        // ── Flip to front buffer ─────────────────────────
        setvisualpage(page ^ 1);
        page ^= 1;

        // ── Update state ─────────────────────────────────
        update();
        t = (t + 1) % 600;   // full day/night = 600 frames ≈ 21 sec
        delay(35);            // ~28 fps
    }

    getch();
    closegraph();
    return 0;
}
