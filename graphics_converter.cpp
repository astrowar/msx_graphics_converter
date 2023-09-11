// MSX Graphics Conversion with CIEDE2000 Algorythm
// Original routine version by Leandro Correia (2019)
// port to C with SDL2 library By Eric Boez 2019
//
//  Created by Eric Boez on 15/04/2019.
//  Copyright © 2019 Eric Boez. All rights reserved.
//
// COMPIL WITH SDL2.0 LIBRARY
//
# include "converter.h"
#include "colorDiff.hpp"
#include <omp.h>

#include <algorithm>


bool is_equal(const tile_generator_t& t1, const tile_generator_t& t2)
{
    for (int i = 0; i < 8; i++)
    {
		if (t1.tile[i] != t2.tile[i])
			return false;
	}
    return true;
}

//**--------------------------------**
//   Read And Use a Specific RGB
//              Palette
//**--------------------------------**
//
void ReadPalette(t_img* e)
{
    unsigned char i;
    int id;
    id = 0;

    switch (e->UsePalette)
    {
    case 0:
        e->Nb_colors = 15;    // Number of colors in MSX0 Palette +1
        break;
    case 1:
        e->Nb_colors = 15;    // Number of colors in MSX1 Palette +1
        break;
    case 2:
        e->Nb_colors = 15;    // Number of colors in C64 Palette +1
        break;
    case 3:
        e->Nb_colors = 15;    // Number of colors in SPECTRUM Palette +1
        break;
    case 4:
        e->Nb_colors = 3;    // Number of colors in GAMEBOY Palette +1
        break;
    case 5:
        e->Nb_colors = 2;    // Number of colors in B & W Palette +1
        break;
    case 6:
        e->Nb_colors = 15;    // Number of colors in MSX B & W Palette +1
        break;

    }


    // Reads the MSX RGB color values;
    for (i = 0; i <= e->Nb_colors; i++)
    {
        switch (e->UsePalette)
        {
        case 0:
            e->msxr[i] = Palette_msx0[id];
            e->msxg[i] = Palette_msx0[id + 1];
            e->msxb[i] = Palette_msx0[id + 2];
            break;
        case 1:
            e->msxr[i] = Palette_msx1[id];
            e->msxg[i] = Palette_msx1[id + 1];
            e->msxb[i] = Palette_msx1[id + 2];
            break;
        case 2:
            e->msxr[i] = Palette_c64[id];
            e->msxg[i] = Palette_c64[id + 1];
            e->msxb[i] = Palette_c64[id + 2];
            break;
        case 3:
            e->msxr[i] = Palette_spectrum[id];
            e->msxg[i] = Palette_spectrum[id + 1];
            e->msxb[i] = Palette_spectrum[id + 2];
            break;
        case 4:
            e->msxr[i] = Palette_gameboy[id];
            e->msxg[i] = Palette_gameboy[id + 1];
            e->msxb[i] = Palette_gameboy[id + 2];
            break;
        case 5:
            e->msxr[i] = Palette_BW[id];
            e->msxg[i] = Palette_BW[id + 1];
            e->msxb[i] = Palette_BW[id + 2];
            break;
        case 6:
            e->msxr[i] = Palette_msxbw[id];
            e->msxg[i] = Palette_msxbw[id + 1];
            e->msxb[i] = Palette_msxbw[id + 2];
            break;

        }
        id = id + 3;
    }
}

//**--------------------------------**
//      Free Memory
//
//**--------------------------------**
//
void    ft_memdel(void** ap)
{
    if (ap && *ap)
    {
        free(*ap);
        *ap = NULL;
    }
}

//**--------------------------------**
//      Free SDL surfaces & render
//
//**--------------------------------**
//
void free_SDL(t_img* e)
{

    SDL_DestroyTexture(e->screen_0);
    SDL_FreeSurface(e->img);
    SDL_DestroyRenderer(e->ren);
    SDL_DestroyWindow(e->win);
    SDL_Quit();
}

//**--------------------------------**
//      Keyboard control hook
//
//**--------------------------------**
//
int   key_hook(t_img* e)
{
    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    if (e->clock > e->click_clock)
    {

        if (keystates[SDL_SCANCODE_ESCAPE])                                     // Exit/Stop
        {
            e->click_clock = clock() + (CLOCKS_PER_SEC);
            ft_exit(e, 0, "\nSee you !");
        }

        if (keystates[SDL_SCANCODE_S])                  // New slot image
        {
            e->click_clock = clock() + (CLOCKS_PER_SEC / 10);
            e->slot++;
            if (e->slot >= 9)
                e->slot = 1;
            DrawSlot(e, Slot_x[e->slot], Slot_y[e->slot]);
        }

        if (keystates[SDL_SCANCODE_1])                // Tolerance --
        {
            e->click_clock = clock() + (CLOCKS_PER_SEC / 10);
            e->tolerance = e->tolerance - STEP_TOLERANCE;
            if (e->tolerance <= TOLERANCE_MIN)
                e->tolerance = TOLERANCE_MIN;
            PrintDo(e);
            Do_it(e);

        }

        if (keystates[SDL_SCANCODE_2])                // Tolerance ++
        {
            e->click_clock = clock() + (CLOCKS_PER_SEC / 10);
            e->tolerance = e->tolerance + STEP_TOLERANCE;
            if (e->tolerance >= TOLERANCE_MAX)
                e->tolerance = TOLERANCE_MAX;
            PrintDo(e);
            Do_it(e); 

        }

        if (keystates[SDL_SCANCODE_3])                // Totlge Block Render 
        {
            e->click_clock = clock() + (CLOCKS_PER_SEC / 10);
            if (e->block_render == 1 )e->block_render = 0;
            else { e->block_render = 1; }
            PrintDo(e);
            Do_it(e);

        }

        if (keystates[SDL_SCANCODE_9])                // Detal level  --
        {
            e->click_clock = clock() + (CLOCKS_PER_SEC / 10);
            e->detaillevel = e->detaillevel + STEP_DETAIL;
            if (e->detaillevel >= DETAIL_MAX)
                e->detaillevel = DETAIL_MAX;
            PrintDo(e);
            Do_it(e);
        }

        if (keystates[SDL_SCANCODE_8])                // Detail Level --
        {
            e->click_clock = clock() + (CLOCKS_PER_SEC / 10);
            e->detaillevel = e->detaillevel - STEP_DETAIL;
            if (e->detaillevel <= DETAIL_MIN)
                e->detaillevel = DETAIL_MIN;
            PrintDo(e);
            Do_it(e);

        }

        if (keystates[SDL_SCANCODE_P])                // Use Next Palette
        {
            e->click_clock = clock() + (CLOCKS_PER_SEC / 10);
            e->UsePalette++;
            if (e->UsePalette > NB_PALETTE)
                e->UsePalette = 0;
            PrintDo(e);
            Do_it(e);

        }
    }
    return(0);
}



///**--------------------------------**
//       True Exit of the Program
//          with error keycodes
//**--------------------------------**
//
void ft_exit(t_img* e, int error, const  char* text)
{
    switch (error) {
    case 0:
        puts("\nEnding  process...");
        break;

    case 1:
        puts("\nError N°1 Opening file.\n");
        break;

    case 2:
        puts("Error n°2 Loading error.\n");
        break;

    case 3:
        puts("Error n°3 Ouch ! .\n");
        break;

    case 4:
        puts("Error n°4 Malloc error\n");
        break;

    case 5:
        puts("Error n°5 Init Error\n");
        break;
    case 6:
        puts("Parameters missing");
        break;
    default:
        break;
    }

    puts(text);
    puts("\n");
    free_SDL(e);
    SDL_Quit();

    ft_memdel((void**)&e);
    exit(EXIT_SUCCESS);
}


//**--------------------------------**
//      Initialisation of the SDL
// Window, render & surface creation
//**--------------------------------**
//
int SDL_init(t_img* e, SDL_Window* win, SDL_Renderer* ren, int width, int height)
{
    /* Initialisation de la SDL. Si ça se passe mal, on quitte */
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr, "SDL2 initialisation Error\n");
        return (EXIT_FAILURE);
    }

    /* Création de la fenêtre  */
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_MOUSE_FOCUS;
    e->win = SDL_CreateWindow(WNAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (NULL == e->win)
    {
        fprintf(stderr, "SDL_CreateWindow Error : %s", SDL_GetError());
        return (EXIT_FAILURE);
    }

    /* Création du renderer */
    e->ren = SDL_CreateRenderer(e->win, -1, SDL_RENDERER_ACCELERATED);
    if (NULL == e->ren)
    {
        fprintf(stderr, "SDL_CreateRenderer Error: %s", SDL_GetError());
        return (EXIT_FAILURE);
    }

    /* Black back ground */
    SDL_RenderClear(e->ren);
    SDL_SetRenderDrawColor(e->ren, 0, 0, 0, 255);

    //Initialisation of Main surfaces
    e->img = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGB888);
    e->img_prev = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGB888);

    // Initialisation of Texture Screen 0 & screen 1
    e->screen_0 = SDL_CreateTexture(e->ren, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);

    return(0);
}

//**--------------------------------**
//        Loading BMP image file
// & convert surface to requested color mode
//**--------------------------------**
//
SDL_Surface* SDL_loadBMP(Uint32 format, t_img* e)
{
    printf("try load image %s \n",g_argv[1]);
    SDL_Surface* tmp, * s;
   // tmp = SDL_LoadBMP(g_argv[1]);   // Load Image passed as argument
    tmp = IMG_Load(g_argv[1]); 
    if (NULL == tmp)
    {
        fprintf(stderr, "SDL_LoadBMP Error: %s", SDL_GetError());
        return (NULL);
    }
    //resize if need , keep aspect ratio 
    //create an new surface and SDL_BlitScaled intro  
    if (tmp->w > IMAGE_WIDTH || tmp->h > IMAGE_HEIGHT)
    {
        int dst_w, dst_h;
        float scale_factor_w =  (float)IMAGE_WIDTH / (float)tmp->w;
        float scale_factor_h =  (float)IMAGE_HEIGHT / (float)tmp->h;
        float scale_factor = scale_factor_w < scale_factor_h ? scale_factor_w : scale_factor_h;
        dst_w = (int)(tmp->w * scale_factor);
        dst_h = (int)(tmp->h * scale_factor);


 
        SDL_Surface* tmp2 = SDL_CreateRGBSurfaceWithFormat(0, dst_w, dst_h, 32, SDL_PIXELFORMAT_RGB888);
        SDL_BlitScaled(tmp, NULL, tmp2, NULL);
        SDL_FreeSurface(tmp);
        tmp = tmp2;
    }
  

    s = SDL_ConvertSurfaceFormat(tmp, format, 0);
    SDL_FreeSurface(tmp);
    if (NULL == s)
    {
        fprintf(stderr, "SDL_ConvertSurfaceFormat Error : %s", SDL_GetError());
        ft_exit(e, 2, "\n BMP conversion error");
    }
    return (s);
}

//**--------------------------------**
//   SDL Copy image to image process
//              surfaces
//**--------------------------------**
//
void SDL_put_img(int sx, int sy, int sw, int sh, int dx, int dy, t_img* e, SDL_Surface* surf_src, SDL_Surface* surf_dest)
{
    SDL_Rect dest = { dx, dy, 0, 0 };
    SDL_Rect src = { sx , sy , sw , sh };
    SDL_BlitSurface(surf_src, &src, surf_dest, &dest);
}

  

//**--------------------------------**
//      Load Original image
//           to main surface
//**--------------------------------**
//
int load_image(t_img* e)
{ 
	
    if (!(e->image_temp = SDL_loadBMP(SDL_PIXELFORMAT_RGB888, e)))
        ft_exit(e, 2, "Original image problem");
    SDL_put_img(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, ORG_X, ORG_Y, e, e->image_temp, e->img);
    SDL_put_img(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, ORG_X, ORG_Y, e, e->image_temp, e->img_prev);
    SDL_FreeSurface(e->image_temp);

    return(1);
}

//**--------------------------------**
//    Mem variables initialisation
//
//**--------------------------------**
//
void init_mem(t_img* e)
{
    if (SDL_init(e, e->win, e->ren, WINDOW_WIDTH, WINDOW_HEIGHT) != 0)           // SDL2 Initialisation
        ft_exit(e, 5, "SDL2 Init Problem !");
}

//**--------------------------------**
//    Plot a Pixel to SDL Image
//
//**--------------------------------**
//
void    SDL_pixel_put_to_image(t_img* e, int x, int y, unsigned int color)
{
    if (x < WINDOW_WIDTH && y < WINDOW_HEIGHT && x >= 0 && y >= 0) {
        Uint32* pixels = NULL;
        pixels = (Uint32*)e->img->pixels;
        //SDL_LockSurface(e->img);
        pixels[x + y * WINDOW_WIDTH] = color;
        pixels = NULL;
    }
}

//**--------------------------------**
//    SDL2 Rendering Full Image
//
//**--------------------------------**
//
void SDL_render(t_img* e)
{

    ///////////// Rendering Main surface to  Texture screen_0 ////////////////////

    SDL_SetRenderTarget(e->ren, e->screen_0);

    e->texture_temp = SDL_CreateTextureFromSurface(e->ren, e->img);

    SDL_Rect dest = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    SDL_Rect src = { 0 , 0 , WINDOW_WIDTH , WINDOW_HEIGHT };
    SDL_RenderCopy(e->ren, e->texture_temp, &src, &dest);
    SDL_DestroyTexture(e->texture_temp);


    SDL_SetRenderTarget(e->ren, NULL);
    SDL_RenderCopy(e->ren, e->screen_0, NULL, NULL);
    SDL_RenderPresent(e->ren);
}

//**--------------------------------**
//    SDL2 Get Pixel Data from Image
//
//**--------------------------------**
//
Uint32 SDL_getpixel(SDL_Surface* surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16*)p;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32*)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

float calcdist_yuv(int r1, int g1, int b1, int r2, int g2, int b2  , int detail_level  ) {
 
    uint8_t yt1 = ((66 * r1 + 129 * g1 + 25 * b1 + 128) >> 8) + 16;
    uint8_t ut1 = (((-38 * r1) + (-74 * g1) + 112 * b1 + 128) >> 8) + 128;
    uint8_t vt1 = ((112 * r1 + (-94 * g1) + (-18 * b1) + 128) >> 8) + 128;

    uint8_t yt2 = ((66 * r2 + 129 * g2 + 25 * b2 + 128) >> 8) + 16;
    uint8_t ut2 = (((-38 * r2) + (-74 * g2) + 112 * b2 + 128) >> 8) + 128;
    uint8_t vt2 = ((112 * r2 + (-94 * g2) + (-18 * b2) + 128) >> 8) + 128;

    float  dy = abs(yt1 - yt2);
    dy = dy * 1.2;
     
    float dist = sqrt(dy * dy  + (ut1 - ut2) * (ut1 - ut2) + (vt1 - vt2) * (vt1 - vt2));
    return   dist * detail_level;
}
 

double calcdist2000_new ( int r1, int g1, int b1, int r2, int g2, int b2 )
{

      return calcdist_yuv(r1, g1, b1, r2, g2, b2, 1.0);
      double lab1[3];
      double lab2[3];

      double  xyz1[3];
      double  xyz2[3];

      unsigned char srgb1[] = { r1, g1, b1 };
      unsigned char srgb2[] = { r2, g2, b2 };

    color::srgb_to_xyz( srgb1,  xyz1);
    color::srgb_to_xyz( srgb2, xyz2);
    //euclidian distance in XYZ 
     double dist_xyz = (xyz1[0] - xyz2[0])* (xyz1[0] - xyz2[0]) + (xyz1[1] - xyz2[1])* (xyz1[1] - xyz2[1]) + (xyz1[2] - xyz2[2])* (xyz1[2] - xyz2[2]);
     return sqrt(dist_xyz);


    color::xyz_to_lab(xyz1, lab1);
    color::xyz_to_lab(xyz2, lab2);

    double d = color::diff_de00(lab1, lab2);
    return   0.1*d;
}

//**--------------------------------**
//    CIEDE2000 Calcul routine
//
//**--------------------------------**
//
float calcdist2000 (int r1, int g1, int b1, int r2, int g2, int b2)
{
   
    if (r1 == r2 && g1 == g2 && b1 == b2)
    {
        return(3.0);
    }

    // Convert two RGB color values into Lab and uses the CIEDE2000 formula to calculate the distance between them.
    // This function first converts RGBs to XYZ and then to Lab.

    // This is not optimized, but I did my best to make it readable. In some rare cases there are some weird colors,
    // so MAYBE there's a small bug in the implementation.
    // The RGB to Lab conversion in here could easily be substituted by a giant RGB to Lab lookup table,
    // consuming much more memory, but gaining A LOT in speed.

    //	Converting RGB values into XYZ


    float r = r1 / 255.0f;
    float g = g1 / 255.0f;
    float b = b1 / 255.0f;

 
  

    //euclidian distance in sRGB
     double dist_srgb = (r1 - r2)* (r1 - r2) + (g1 - g2)* (g1 - g2) + (b1 - b2)* (b1 - b2);
     dist_srgb = 0.05* sqrt(dist_srgb);
     int dcolor_srgb = 60;

     
     if (dist_srgb < dcolor_srgb)
		 return     dist_srgb;

     
     //is a dull color ? 
     if (fabs(r1 - g1) < dcolor_srgb && fabs(r1 - b1) < dcolor_srgb && fabs(g1 - b1) < dcolor_srgb)
     {
		 return   dist_srgb;
	 }
     if (fabs(r2 - g2) < dcolor_srgb && fabs(r2 - b2) < dcolor_srgb && fabs(g2 - b2) < dcolor_srgb)
     {
           return   dist_srgb;
     }
 
      

    if (r > 0.04045f)
    {
        r = pow(((r + 0.055f) / 1.055f), 2.4);
    }
    else
    {
        r = r / 12.92f;
    }

    if (g > 0.04045)
    {
        g = pow(((g + 0.055f) / 1.055f), 2.4);
    }
    else
    {
        g = g / 12.92f;
    }

    if (b > 0.04045f)
    {
        b = pow(((b + 0.055f) / 1.055f), 2.4);
    }
    else
    {
        b = b / 12.92f;
    }

    r = r * 100.0f;
    g = g * 100.0f;
    b = b * 100.0f;

    // Observer. = 2°, Illuminant = D65
    float x = r * 0.4124 + g * 0.3576 + b * 0.1805;
    float y = r * 0.2126 + g * 0.7152 + b * 0.0722;
    float z = r * 0.0193 + g * 0.1192 + b * 0.9505;

    x = x / 95.047;   //Observer= 2°, Illuminant= D65
    y = y / 100.000;
    z = z / 108.883;

    if (x > 0.008856)
    {
        x = pow(x, (1.0 / 3.0));
    }
    else
    {
        x = (7.787 * x) + (16.0 / 116.0);
    }

    if (y > 0.008856)
    {
        y = pow(y, (1.0 / 3.0));
    }
    else
    {
        y = (7.787 * y) + (16.0 / 116.0);
    }

    if (z > 0.008856)
    {
        z = pow(z, (1.0 / 3.0));
    }
    else
    {
        z = (7.787 * z) + (16.0 / 116.0);
    }

    float l1 = (116.0 * y) - 16.0;
    float a1 = 500.0 * (x - y);
    b1 = 200.0 * (y - z);


    r = r2 / 255.0;
    g = g2 / 255.0;
    b = b2 / 255.0;

    if (r > 0.04045)
    {
        r = pow(((r + 0.055) / 1.055), 2.4);
    }
    else
    {
        r = r / 12.92;
    }

    if (g > 0.04045)
    {
        g = pow(((g + 0.055) / 1.055), 2.4);
    }
    else
    {
        g = g / 12.92;
    }

    if (b > 0.04045)
    {
        b = pow(((b + 0.055) / 1.055), 2.4);
    }
    else
    {
        b = b / 12.92;
    }

    r = r * 100.0;
    g = g * 100.0;
    b = b * 100.0;

    //Observer. = 2°, Illuminant = D65
    x = r * 0.4124 + g * 0.3576 + b * 0.1805;
    y = r * 0.2126 + g * 0.7152 + b * 0.0722;
    z = r * 0.0193 + g * 0.1192 + b * 0.9505;


    x = x / 95.047;   //Observer= 2°, Illuminant= D65
    y = y / 100.000;
    z = z / 108.883;

    if (x > 0.008856)
    {
        x = pow(x, (1.0 / 3.0));
    }
    else
    {
        x = (7.787 * x) + (16.0 / 116.0);
    }

    if (y > 0.008856)
    {
        y = pow(y, (1.0 / 3.0));
    }
    else
    {
        y = (7.787 * y) + (16.0 / 116.0);
    }

    if (z > 0.008856)
    {
        z = pow(z, (1.0 / 3.0));
    }
    else
    {
        z = (7.787 * z) + (16.0 / 116.0);
    }

    //	Converts XYZ to Lab...

    float l2 = (116.0 * y) - 16.0;
    float a2 = 500.0 * (x - y);
    b2 = 200.0 * (y - z);

    // ...and then calculates distance between Lab colors, using the CIEDE2000 formula.

    float dl = l2 - l1;
    float hl = l1 + dl * 0.5;
    float sqb1 = b1 * b1;
    float sqb2 = b2 * b2;
    float c1 = sqrtf(a1 * a1 + sqb1);
    float c2 = sqrtf(a2 * a2 + sqb2);
    float hc7 = pow(((c1 + c2) * 0.5), 7.0);
    float trc = sqrtf(hc7 / (hc7 + 6103515625.0));
    float t2 = 1.5 - trc * 0.5;
    float ap1 = a1 * t2;
    float ap2 = a2 * t2;
    c1 = sqrtf(ap1 * ap1 + sqb1);
    c2 = sqrtf(ap2 * ap2 + sqb2);
    float dc = c2 - c1;
    float hc = c1 + dc * 0.5;
    hc7 = pow(hc, 7.0);
    trc = sqrtf(hc7 / (hc7 + 6103515625.0));
    float h1 = atan2(b1, ap1);

    if (h1 < 0)
    {
        h1 = h1 + Pi * 2.0;
    }
    float h2 = atan2(b2, ap2);

    if (h2 < 0)
    {
        h2 = h2 + Pi * 2.0;
    }

    float hdiff = h2 - h1;
    float hh = h1 + h2;
    if (fabs(hdiff) > Pi)
    {
        hh = hh + Pi * 2;
        if (h2 <= h1)
        {
            hdiff = hdiff + Pi * 2.0;
        }

    }
    else
    {
        hdiff = hdiff - Pi * 2.0;
    }

    hh = hh * 0.5;
    t2 = 1.0 - 0.17 * cos(hh - Pi / 6.0) + 0.24 * cos(hh * 2.0);
    t2 = t2 + 0.32 * cos(hh * 3.0 + Pi / 30.0);
    t2 = t2 - 0.2 * cos(hh * 4.0 - Pi * 63.0 / 180.0);
    float dh = 2.0 * sqrtf(c1 * c2) * sin(hdiff * 0.5);
    float sqhl = (hl - 50.0) * (hl - 50.0);
    float fl = dl / (1.0 + (0.015 * sqhl / sqrtf(20.0 + sqhl)));
    float fc = dc / (hc * 0.045 + 1.0);
    float fh = dh / (t2 * hc * 0.015 + 1.0);
    float dt = 30 * exp(-(pow(36.0 * hh - 55.0 * Pi, 2.0)) / (25.0 * Pi * Pi));
    r = 0 - 2.0 * trc * sin(2.0 * dt * Pi / 180.0);

    return sqrtf(fl * fl + fc * fc + fh * fh + r * fc * fh);
}

double get_distance_color(int x, int y, t_img* e, double** detail , int& bestcor1, int& bestcor2, int* octetfinal ) {

    double octetdetail[8]; 
    int octetr[8], octetg[8], octetb[8];
    int toner[5], toneg[5], toneb[5];
    int i, j;
    Uint8 r, g, b, a;
    
    #pragma omp critical        
    for (i = 0; i <= 7; i++)
    {
        // Get the RGB values of 8 pixels of the original image
        SDL_GetRGBA(SDL_getpixel(e->img, ORG_X + x + i, ORG_Y + y), e->img->format, &r, &g, &b, &a);
        octetr[i] = r;
        octetg[i] = g;
        octetb[i] = b;
        octetdetail[i] = detail[x + i][y];
    }

    toner[0] = e->msxr[bestcor1];
    toneg[0] = e->msxg[bestcor1];
    toneb[0] = e->msxb[bestcor1];

    toner[2] = e->msxr[bestcor2];
    toneg[2] = e->msxg[bestcor2];
    toneb[2] = e->msxb[bestcor2];

    toner[1] = (e->msxr[bestcor1] + e->msxr[bestcor2]) / 2;
    toneg[1] = (e->msxg[bestcor1] + e->msxg[bestcor2]) / 2;
    toneb[1] = (e->msxb[bestcor1] + e->msxb[bestcor2]) / 2;

    double d = 0;
    for (i = 0; i <= 7; i++) {
        if (octetfinal[i] == 0) {
			d += (calcdist_yuv(toner[0], toneg[0], toneb[0], octetr[i], octetg[i], octetb[i] , octetdetail[i]))  ;
		}
        else if (octetfinal[i] == 1){
			d += (calcdist_yuv(toner[1], toneg[1], toneb[1], octetr[i], octetg[i], octetb[i] , octetdetail[i]))  ;
		}
        else {
            d += (calcdist_yuv(toner[2], toneg[2], toneb[2], octetr[i], octetg[i], octetb[i] , octetdetail[i]))  ;
        }
    }
    return d;
}
 
double  process_tile_line_color_in(int x, int y, t_img* e, double** detail, int& cor1, int& cor2, int* octetfinal )
{
    int i, j;
    int octetr[8], octetg[8], octetb[8];
    double octetdetail[8] ;
    int   octetvalue_dither[8] ;
    int   octetvalue_flat[8];

    int cor,  dif1, dif2, corfinal, corfinal2, id, bestdistance,  finaldist, finaldista, finaldistb;

    unsigned char byte = 0;
    int toner[5], toneg[5], toneb[5];

    Uint8 r, g, b, a;

    double distcolor[5];
    //compute the best patten for this color scheme
    #pragma omp critical        
    for (i = 0; i <= 7; i++)
    {
        // Get the RGB values of 8 pixels of the original image
        SDL_GetRGBA(SDL_getpixel(e->img, ORG_X + x + i, ORG_Y + y), e->img->format, &r, &g, &b, &a);

        octetr[i] = r;
        octetg[i] = g;
        octetb[i] = b;
        octetdetail[i] = detail[x + i][y];
    }

    toner[0] = e->msxr[cor1];
    toneg[0] = e->msxg[cor1];
    toneb[0] = e->msxb[cor1];

    toner[2] = e->msxr[cor2];
    toneg[2] = e->msxg[cor2];
    toneb[2] = e->msxb[cor2];

    toner[1] = (e->msxr[cor1] + e->msxr[cor2]) / 2;
    toneg[1] = (e->msxg[cor1] + e->msxg[cor2]) / 2;
    toneb[1] = (e->msxb[cor1] + e->msxb[cor2]) / 2;


    float dist_dither = 0;
    float dist_flat = 0;
 
   {
        // dithered
        for (i = 0; i <= 7; i++)
        {
            for (j = 0; j <= 2; j++)
            {
                distcolor[j] = (calcdist_yuv(toner[j], toneg[j], toneb[j], octetr[i], octetg[i], octetb[i], octetdetail[i]))  ;
            }
             
            finaldist = distcolor[0];
            octetvalue_dither[i] = 0;
            for (j = 0; j <= 2; j++)
            {
                if (distcolor[j] < finaldist)
                {                     
                    finaldist = distcolor[j];
                    octetvalue_dither[i] = j;
                }
            }
            dist_dither = dist_dither + finaldist;
        }
    }
    //else
    {
        // not dithered
        for (i = 0; i <= 7; i++)
        {
            finaldista = (calcdist_yuv(toner[0], toneg[0], toneb[0], octetr[i], octetg[i], octetb[i], octetdetail[i]))  ;
            finaldistb = (calcdist_yuv(toner[2], toneg[2], toneb[2], octetr[i], octetg[i], octetb[i], octetdetail[i]))  ;

            if (finaldista < finaldistb)
            {
                octetvalue_flat[i] = 0;
                finaldist = finaldista;
            }
            else
            {
                octetvalue_flat[i] = 2;
                finaldist = finaldistb;
            }
            dist_flat = dist_flat + finaldist;
        }
    }
    //copy octetvalue to octetfinal

    for (i = 0; i <= 7; i++)
    {
        if (dist_dither < dist_flat)
        {
            octetfinal[i] = octetvalue_dither[i];
        }
        else {
            octetfinal[i] = octetvalue_flat[i];
        }	 
	} 

    return std::min( dist_dither , dist_flat);

}



double  process_tile_line(int x, int y ,t_img* e , double **detail  , int &bestcor1 , int &bestcor2, int* octetfinal , double prev_dist)
{
        int i, j ;
        Uint8 r, g, b, a;
        Uint8 r0, g0, b0, a0;
        Uint8 r1, g1, b1, a1;
         int octetr[8], octetg[8], octetb[8];
         int   octetvalue_dither[8];
         int   octetvalue_flat[8];


          double octetdetail[8];
         int cor, cor1, cor2, cor3, dif1, dif2, corfinal, corfinal2, id, bestdistance, dist, finaldist, finaldista, finaldistb;
         unsigned char byte = 0;
         int toner[5], toneg[5], toneb[5];
           double distcolor[5];

        //int bestcor1 = 0;
       // int bestcor2 = 0;

        bestdistance = 99999999;
        bestdistance = prev_dist;

       #pragma omp critical        
        for (i = 0; i <= 7; i++)
        {
            // Get the RGB values of 8 pixels of the original image
            SDL_GetRGBA(SDL_getpixel(e->img, ORG_X + x + i, ORG_Y + y), e->img->format, &r, &g, &b, &a);
        
            octetr[i] = r;
            octetg[i] = g;
            octetb[i] = b;
            octetdetail[i] = detail[x + i][y];
        }

        // Brute force starts. Programs tests all 15 x 15 MSX color combinations. For each pixel octet it'll have
        // to compare the original pixel colors with three different colors:
        // two MSX colors and a mixed RGB of both. If this RGB mixed is chosen it'll later be substituted by dithering.
 
        for (cor1 = 1; cor1 <= e->Nb_colors; cor1++)
        {
                toner[0] = e->msxr[cor1];
                toneg[0] = e->msxg[cor1];
                toneb[0] = e->msxb[cor1];
             
            for (cor2 = cor1; cor2 <= e->Nb_colors; cor2++)
            {
                float dist_dither = 0;
                float dist_flat = 0;
                // If KeyHit(1) Then End 

                // Second MSX color of the octet
                toner[2] = e->msxr[cor2];
                toneg[2] = e->msxg[cor2];
                toneb[2] = e->msxb[cor2];


                // A mix of both MSX colors RGB values. Since MSX cannot mix colors,
                // later if this color is chosen it'll be substituted by a 2x2 dithering pattern.
                toner[1] = (e->msxr[cor1] + e->msxr[cor2]) / 2;
                toneg[1] = (e->msxg[cor1] + e->msxg[cor2]) / 2;
                toneb[1] = (e->msxb[cor1] + e->msxb[cor2]) / 2;

                // if colors are not too distant according to the tolerance parameter, octect will be dithered.
                  {
                    // dithered
                    for (i = 0; i <= 7; i++)
                    {
                        for (j = 0; j <= 2; j++)
                        {
                            distcolor[j] = (calcdist_yuv(toner[j], toneg[j], toneb[j], octetr[i], octetg[i], octetb[i], octetdetail[i] )) ;
                        }
                        //finaldist=distcolor(0):octetvalue(i)=0
                        finaldist = distcolor[0];
                        octetvalue_dither[i] = 0;
                        for (j = 1; j <= 2; j++)
                        {
                            if (distcolor[j] < finaldist)
                            {
                               
                                finaldist = distcolor[j];
                                octetvalue_dither[i] = j;
                            }
                        }
                        dist_dither = dist_dither + finaldist;
                        if (dist_dither > bestdistance)
                        {
                            break;
                        }
                    }
                }
              
                {
                    // not dithered
                    for (i = 0; i <= 7; i++)
                    {
                        finaldista = (calcdist_yuv(toner[0], toneg[0], toneb[0], octetr[i], octetg[i], octetb[i] , octetdetail[i]))  ;
                        finaldistb = (calcdist_yuv(toner[2], toneg[2], toneb[2], octetr[i], octetg[i], octetb[i] , octetdetail[i]))  ;

                        if (finaldista < finaldistb)
                        {
                            octetvalue_flat[i] = 0;
                            finaldist = finaldista;
                        }
                        else
                        {
                            octetvalue_flat[i] = 2;
                            finaldist = finaldistb;
                        }
                        dist_flat = dist_flat + finaldist;
                        if (dist_flat > bestdistance)
                        {
                            break;
                        }
                    }
                }

                if (dist_flat < bestdistance)
                {
                    //bestdistance=dist:bestcor1=cor1:bestcor2=cor2
                    bestdistance = dist_flat;
                    bestcor1 = cor1;
                    bestcor2 = cor2;
                    for (i = 0; i <= 7; i++)
                    {
                        octetfinal[i] = octetvalue_flat[i];
                    }
                }
                if (dist_dither < bestdistance)
                {
                    //bestdistance=dist:bestcor1=cor1:bestcor2=cor2
                    bestdistance = dist_dither;
                    bestcor1 = cor1;
                    bestcor2 = cor2;
                    for (i = 0; i <= 7; i++)
                    {
                        octetfinal[i] = octetvalue_dither[i];
                    }
                }

                if (bestdistance == 0)
                {
                    break;
                }
            }
            if (bestdistance == 0)
            {
                break;
            }
        } 
        return bestdistance;
}


 
void process_and_write(t_img* e , int x, int y , double **detail , unsigned char &byte ,int &bestcor1, int &bestcor2 , int octet_prev[8]) {

    int* Final_color;
    unsigned char buf[8];

    int i ;
    int octetfinal[8];
    //copy octet_prev to octetfinal
    for (i = 0; i <= 7; i++) octetfinal[i] = octet_prev[i];
     
 
    int bestcor1_p = bestcor1;
    int bestcor2_p = bestcor2;
   

    double distance_prev = 99999999;
    double distance  = 99999999;
    if (  bestcor1_p < 99  ) 
    {
           distance_prev = get_distance_color(x, y, e, detail, bestcor1_p, bestcor2_p, octet_prev); // distance if repeate previous line patten in this line        
          distance_prev = distance_prev/1.4;
    } 
    distance = process_tile_line(x, y, e, detail, bestcor1, bestcor2, octetfinal, distance_prev);
     bestcor1_p = bestcor1;
     bestcor2_p = bestcor2;
    byte = 0;


        for (i = 0; i <= 7; i++)
        {   
             unsigned int    Rcol;
             unsigned int    Gcol;
             unsigned int    Bcol;

            switch (octetfinal[i])
            {
            case 0:
                Rcol = e->msxr[bestcor1];
                Gcol = e->msxg[bestcor1];
                Bcol = e->msxb[bestcor1];

                break;
            case 1:
                 if (y % 2 == i % 2)
                 //if ( i % 2 == 0 )
                {

                    Rcol = e->msxr[bestcor2];
                    Gcol = e->msxg[bestcor2];
                    Bcol = e->msxb[bestcor2];
                    byte = byte + pow(2, 7 - i);

                }
                else
                {

                    Rcol = e->msxr[bestcor1];
                    Gcol = e->msxg[bestcor1];
                    Bcol = e->msxb[bestcor1];

                }
                break;

            case 2:
                Rcol = e->msxr[bestcor2];
                Gcol = e->msxg[bestcor2];
                Bcol = e->msxb[bestcor2];
                byte = byte + pow(2, 7 - i);
                break;
            default:
                Rcol = 0;
				Gcol = 0;
				Bcol = 0;
				break;

            }

            buf[0] = Bcol;
            buf[1] = Gcol;
            buf[2] = Rcol;
            buf[3] = 0xff;

            Final_color = (int*)buf;  
              #pragma omp critical      
            e->RenderImage[y][x + i] = *Final_color;
        }

        for (i = 0; i <= 7; i++) octet_prev[i] = octetfinal[i];
}
 

void write_toSDL(t_img* e, int x, int y , int& cor1, int& cor2, tile_generator_t& octetfinal )
{
    int* Final_color;
    unsigned char buf[8];

    for (int yi = 0;yi <= 7; yi++)
        for (int xi = 0; xi <= 7; xi++)
        {
            unsigned int    Rcol;
            unsigned int    Gcol;
            unsigned int    Bcol;

            int bit_ij = octetfinal.tile[yi] & (1 << (7- xi)) ;
            
            if (bit_ij >  0) {
                Rcol = e->msxr[cor1];
                Gcol = e->msxg[cor1];
                Bcol = e->msxb[cor1];
            }
            else {
                Rcol = e->msxr[cor2];
                Gcol = e->msxg[cor2];
                Bcol = e->msxb[cor2];
            } 
            buf[0] = Bcol;
            buf[1] = Gcol;
            buf[2] = Rcol; 
            buf[3] = 0xff;

            Final_color = (int*)buf;

            #pragma omp critical  
            e->RenderImage[y + yi][x + xi] = *Final_color;
        }

  
}

double get_full_process_tile_color_in(int x, int y, t_img* e, double** detail, int& cor1, int& cor2, tile_generator_t &octetfinal , double best_distance )
{
    double dist = 0;
    for (int i = 0; i < 8; ++i)  //each line in tile 
    {
        int octree_in[8];
        dist+= process_tile_line_color_in(x, y + i, e, detail, cor1, cor2, octree_in);
        if (dist > best_distance) return dist;
        
        unsigned char byte  = 0;
        //convert octree in byte 
        for (int k = 0; k < 8; k++)
        {
            if (octree_in[k] == 2) { byte = byte + (1 << (7-k)); }          
            else  if (octree_in[k] == 1)
            {
                //checker patten
                if ((  i  % 2) == (  k  % 2))
                {
                     byte = byte + (1 << (7-k));
                }
            }
		}
        octetfinal.tile[i] = byte;
    }
    return dist;
}

double get_best_color_tile(int x, int y, t_img* e, double** detail, int& cor1, int& cor2, tile_generator_t& byte_final)
{
    double best_dist = 99999999;
    tile_generator_t tmp_byte_final; 

    for (int c1 = 1; c1 <= e->Nb_colors; c1++)
    {
        for (int c2 = c1; c2 <= e->Nb_colors; c2++)
        {
		   double dist = get_full_process_tile_color_in(x, y, e, detail, c1, c2, tmp_byte_final, best_dist);
           if (dist < best_dist)
           {
               best_dist = dist;
			   cor1 = c1;
			   cor2 = c2;
               //copy octree 
               for (int i = 0; i < 8; ++i) {
                   byte_final.tile[i] = tmp_byte_final.tile[i];
                   
			   }
		   }
	   }
   }
 
    return best_dist;
}


//**--------------------------------**
//   Image processing Routine
//
//**--------------------------------**
//
void ImageProcess(t_img* e)
{
    int x, y, j, i;
    Uint8 r, g, b, a;


    int* Final_color;
    unsigned char buf[8];

    int octetr[8], octetg[8], octetb[8];
    double octetdetail[8];
    int octetfinal[8], octetvalue[8];
    int toner[5], toneg[5], toneb[5];
    double distcolor[5];
    //double detail[IMAGE_WIDTH][IMAGE_HEIGHT];        //Detail map

    double **detail = new double*[IMAGE_WIDTH];
    for (int i = 0; i < IMAGE_WIDTH; ++i)
		detail[i] = new double[IMAGE_HEIGHT];
    int imagedata[IMAGE_WIDTH][IMAGE_HEIGHT];        //Luminosity data of original image


    int cor, cor1, cor2, cor3, dif1, dif2, corfinal, corfinal2, id, bestdistance, dist, finaldist, finaldista, finaldistb;
   // int bestcor1 = 0;
   // int bestcor2 = 0;
    //unsigned char byte = 0;
    //unsigned char colbyte = 0;
    unsigned int bytepos = 0;

    y = 0;
    x = 0;
    id = 0;

    ReadPalette(e);

    //Reads all luminosity values
    for (j = 0; j <= IMAGE_HEIGHT - 1; j++)
    {
        for (i = 0; i <= IMAGE_WIDTH - 1; i++)
        {
            SDL_GetRGBA(SDL_getpixel(e->img, i, j), e->img->format, &r, &g, &b, &a);
            imagedata[i][j] = (r + g + b) / 3;
        }
    }

    if (e->detaillevel < IMAGE_WIDTH - 1)
    {
        for (j = 1; j <= IMAGE_HEIGHT - 1; j++)
        {
            for (i = 1; i <= IMAGE_WIDTH - 2; i++)
            {
                cor = imagedata[i - 1][j];
                cor2 = imagedata[i][j];
                cor3 = imagedata[i + 1][j];
                dif1 = abs(cor - cor2);
                dif2 = abs(cor2 - cor3);
                if (dif1 > dif2)
                {
                    corfinal = dif1;
                }
                else
                {
                    corfinal = dif2;
                }
                cor = imagedata[i][j - 1];
                cor3 = imagedata[i][j + 1];
                dif1 = abs(cor - cor2);
                dif2 = abs(cor2 - cor3);
                if (dif1 > dif2)
                {
                    corfinal2 = dif1;
                }
                else
                {
                    corfinal2 = dif2;
                }
                corfinal = (corfinal + corfinal2) >> 1; // Shr 1
                corfinal = corfinal;
                detail[i][j] = corfinal;
            }
        }

        for (i = 0; i <= IMAGE_WIDTH - 1; i++)
        {
            detail[i][0] = 0;
            detail[i][IMAGE_HEIGHT - 1] = 0;
        }
        for (i = 0; i <= IMAGE_HEIGHT - 1; i++)
        {
            detail[0][i] = 0;
            detail[IMAGE_WIDTH - 1][i] = 0;
        }

        for (j = 0; j <= IMAGE_HEIGHT - 1; j++)
        {
            for (i = 0; i <= IMAGE_WIDTH - 1; i++)
            {
                if (detail[i][j] < 1)
                {
                    detail[i][j] = 1;
                }
                detail[i][j] = (detail[i][j] / e->detaillevel) + 1;
            }
        }
    }
    else
    {
        for (j = 0; j <= IMAGE_HEIGHT - 1; j++)
        {
            for (i = 0; i <= IMAGE_WIDTH - 1; i++)
            {
                detail[i][j] = 1;
            }
        }
    }

    int *x_coords = new int[IMAGE_WIDTH* IMAGE_HEIGHT ];
    int *y_coords = new int[IMAGE_WIDTH* IMAGE_HEIGHT];  
    int *byte_pos_pp = new int[IMAGE_WIDTH* IMAGE_HEIGHT];   

 
    
    int k =0 ;
    while (y < IMAGE_HEIGHT)
    {       
        x_coords[k] = x;
        y_coords[k] = y;     
        byte_pos_pp[k] = bytepos; 
        y = y + 1;
        if (y % 8 == 0)
        {
            y = y - 8;
            x = x + 8;
        }
        if (x > IMAGE_WIDTH - 1) {
            x = 0;
            y = y + 8;
        }
        // Put Data in MSX Dump for saving file
       // colbyte = bestcor2 * 16 + bestcor1;
        //msxdump[bytepos] = byte;
        //msxdump[bytepos + 6144] = colbyte;
        bytepos++;
        k++;
    }
     

    int tile_num_seq = 0;
    
    for (int yi = 0; yi < TILES_NUM_ROWS; yi++)
        for(int xi =0 ; xi < TILES_NUM_COLS; xi++)
        {
            msx_tile_name[yi][xi] = tile_num_seq % 256;
            tile_num_seq++;
	    }

#pragma omp parallel for  collapse(2)
    for (int yi = 0; yi < TILES_NUM_ROWS; yi++)
        for (int xi = 0; xi < TILES_NUM_COLS; xi++)
        {
            if (e->block_render == 0) break;
        
            int global_tile_id =  xi  +  yi  * 32;
            int actual_table = global_tile_id / 256;
            int tile_id = global_tile_id % 256;
            int color_1 = 0;
            int color_2 = 0;
            tile_generator_t  byte_final;
            get_best_color_tile( xi * 8, yi * 8, e, detail, color_1, color_2, byte_final);
            for (int sub_row_j = 0; sub_row_j < 8; sub_row_j++) 
            {
                msx_tile_generator[actual_table][tile_id].tile[sub_row_j] = byte_final.tile[sub_row_j];
                msx_tile_colour[actual_table][tile_id].color[sub_row_j][0] = color_1;
                msx_tile_colour[actual_table][tile_id].color[sub_row_j][1] = color_2;
            }

            write_toSDL(e, xi * 8, yi * 8 , color_2, color_1, msx_tile_generator[actual_table][tile_id]);
        }

 

 const int  tile_size = 16;

#pragma omp parallel for 
    for (int m = 0; m < k; m+= tile_size) {
      
        if (e->block_render == 1 ) break;
         
        int bestcor1 = 99;
        int bestcor2 = 99;
        unsigned char byte = 0xCC;
        int octet_prev[8];
        for (int mm = 0; mm <= 7 ; mm++) octet_prev[mm] = 0;

        for (int mm = 0; mm < tile_size ; mm++) 
        {
            int mk = m + mm;
            int xi = x_coords[mk];
            int yi = y_coords[mk];      

            process_and_write(e, xi, yi, detail, byte, bestcor1, bestcor2, octet_prev);

            int global_tile_id = (xi/8) + (yi/8) * 32;      
            int actual_table = global_tile_id  / 256 ;
            int tile_id = global_tile_id % 256 ; 

            int sub_row_j = yi % 8;

            unsigned char colbyte = bestcor2 * 16 + bestcor1;
            {
                msxdump[mk] = byte;
                msxdump[mk + 6144] = colbyte;
                // assign to msx_tile_generator
                //map each bit to sub_row                           
                 msx_tile_generator[actual_table][tile_id].tile[sub_row_j]  =  byte  ;         
                 //msx_tile_generator[actual_table][tile_id].tile[sub_row_j]  = 0xAA;
                 msx_tile_colour[actual_table][tile_id].color[sub_row_j][0] = bestcor1;
                 msx_tile_colour[actual_table][tile_id].color[sub_row_j][1] = bestcor2;
            }
        }
    }
     

    delete x_coords;
    delete y_coords;
    delete byte_pos_pp; 

    //delete deials array 
    for (int i = 0; i < IMAGE_WIDTH; ++i)
		delete[] detail[i];
    delete[] detail;

 
}

int  get_tile_discrepance(const tile_generator_t &t1, const  tile_generator_t &t2 , const  tile_colour_t &c1 , const tile_colour_t &c2) {

	int discrepance = 0;
    for (int line_i = 0; line_i < 8; ++line_i)
    { 
		 
        for (int k = 0; k < 8; ++k) {
            int bit_color_1 = (t1.tile[line_i] >>  k) & 1;
            int bit_color_2 = (t2.tile[line_i] >>  k) & 1;

            int color_pixel_1 = c1.color[line_i][bit_color_1];
            int color_pixel_2 = c2.color[line_i][bit_color_2];
            if (color_pixel_1 != color_pixel_2)
            {
                discrepance++;
            }
        }
	}
	return discrepance;
}


int  get_tile_discrepance_mono(const tile_generator_t& t1, const  tile_generator_t& t2 ) {

    int discrepance = 0;
    for (int line_i = 0; line_i < 8; ++line_i)
    {
        if (t1.tile[line_i] != t2.tile[line_i]) discrepance++;         
    }
    return discrepance;
}

int  get_tile_discrepance_color(const tile_generator_t& t1, const  tile_generator_t& t2, const  tile_colour_t& c1, const tile_colour_t& c2 ) {

    int discrepance = 0;
    for (int line_i = 0; line_i < 8; ++line_i)
    {
        if (t1.tile[line_i] != t2.tile[line_i]) discrepance++;
        if (c1.color[line_i][0] != c2.color[line_i][0]) discrepance++;
    }
    return discrepance;
}

void swap_tile(int page , int i, int j) {
    for (int yi = 0; yi < 8; yi++)
        for (int xi = 0; xi < TILES_NUM_COLS; xi++)
        {
            int  y_global = yi + page * 8;
            int org = msx_tile_name[y_global][xi];
            if (org == i) msx_tile_name[y_global][xi] = j;
            else if (org == j) msx_tile_name[y_global][xi] = i;
        }

      //troca o gerador tambem
      std::swap(msx_tile_generator[page][i], msx_tile_generator[page][j]);
      std::swap( msx_tile_colour[page][i] , msx_tile_colour[page][j] );
}
 
void compress_tilemap() {

    //verifica tilemaps iguais 
    for (int actual_table = 0; actual_table < 3; actual_table++)
    {
        unsigned char collapse_to[256];
        for (int i = 0; i < 256; i++)  collapse_to[i] = i;

        for (int i = 0; i < 255; i++)
        {
        
            for (int j = i + 1; j < 256; j++)
            {
                int  tile_disc = get_tile_discrepance(msx_tile_generator[actual_table][i], msx_tile_generator[actual_table][j], msx_tile_colour[actual_table][i], msx_tile_colour[actual_table][j]);
                if (tile_disc == 0  )
                {                    
                    int dst = i;
                    while (collapse_to[dst] != dst) dst = collapse_to[dst];
                    collapse_to[j] = dst;
                }
            }
        }

        //remap tilemap
        for (int yi = 0; yi < 8; yi++)
            for (int xi = 0; xi < TILES_NUM_COLS; xi++)
            {
                int  y_global = yi + actual_table * 8;
                int i = msx_tile_name[y_global][xi];
                msx_tile_name[y_global][xi] = collapse_to[i];
            }
    }

 
}

void group_same_tiles() {

    for (int actual_table = 0; actual_table < 3; actual_table++)
    {
        for (int i = 0; i < 255; i++)
        {
            bool found_next = false;
            for (int j = i + 1; j < 256; j++)
            {
                			int  tile_disc = get_tile_discrepance_color(msx_tile_generator[actual_table][i], msx_tile_generator[actual_table][j] , msx_tile_colour[actual_table][i], msx_tile_colour[actual_table][j]);
                            if (tile_disc == 0)
                            {
                                found_next = true;
                                if (j != i + 1)
                                {
                                    swap_tile(actual_table, i, j);
                               
                                }
                                break;
                            }
				 
            }
            if (found_next == false) {
                for (int j = i + 1; j < 256; j++)
                {
                    int  tile_disc = get_tile_discrepance_mono(msx_tile_generator[actual_table][i], msx_tile_generator[actual_table][j]  );
                    if (tile_disc == 0)
                    {
                        found_next = true;
                        if (j != i + 1)
                        {
                            swap_tile(actual_table, i, j);
                            
                        }
                        break;
                    }
                }

            }
        }
    }
}

void clean_unused_tiles() {
    for (int actual_table = 0; actual_table < 3; actual_table++)
    {
        unsigned char used[256];
        for (int i = 0; i < 256; i++)used[i] = 0;

        for (int yi = 0; yi < 8; yi++)
            for (int xi = 0; xi < TILES_NUM_COLS; xi++)
            {
                int  y_global = yi + actual_table * 8;
                int i = msx_tile_name[y_global][xi];
                used[i] = true; 
            }

        for (int i = 0; i < 256; ++i) {
            if (used[i] == false)
            {
                for (int k = 0; k < 8; ++k)
                {
                    msx_tile_generator[actual_table][i].tile[k] = 0;
                    msx_tile_colour[actual_table][i].color[k][0] = 0;
                    msx_tile_colour[actual_table][i].color[k][1] = 0;
                }
            }
            else {
                printf("%d ", i);
            }
        }
        printf("\n");


        //move os tiles nao usados para o fim da fila
        for (int i = 0; i < 256; ++i)
            for (int j = i+1; j < 256; ++j)
            {
                if (i == j) continue;
                if (used[i] == false && used[j] == true)
                {
                    //must swap
                    swap_tile(actual_table, i, j);
                    std::swap(used[i], used[j]);
                }
                    
            }
    }
 

}

void MSXoutput_tiled_compress(t_img* e) {
    FILE* f = NULL;
    errno_t err = fopen_s(&f, "compress.sc0", "wb");
    if (err != 0)
        ft_exit(e, 1, "Writing output file problem!");

    if (f == NULL) {
        ft_exit(e, 1, "Writing output file problem!");
        return;
    }

    // Pattern generator table 1 + 2 + 3
    for (int actual_table = 0; actual_table < 3; ++actual_table)
    { 
        unsigned char used[256];
        for (int i = 0; i < 256; i++)used[i] = 0;

        for (int yi = 0; yi < 8; yi++)
            for (int xi = 0; xi < TILES_NUM_COLS; xi++)
            {
                int  y_global = yi + actual_table * 8;
                int i = msx_tile_name[y_global][xi];
                used[i] = true;
            }

        for (int tile_id = 0; tile_id < 256; tile_id++)
        {
            if (used[tile_id]) {
                tile_generator_t t = msx_tile_generator[actual_table][tile_id];
                for (int subline = 0; subline < 8; ++subline)
                {
                    unsigned char byte = t.tile[subline];
                    fputc(byte, f);     
                }
            }
            else {
                for (int subline = 0; subline < 8; ++subline)
                {
                    fputc(0x00, f);
                }
            }
        } 
        for (int tile_id = 0; tile_id < 256; tile_id++)
        {
            if (used[tile_id]) {
                int color1 = msx_tile_colour[actual_table][tile_id].color[0][0];
                int color2 = msx_tile_colour[actual_table][tile_id].color[0][1];
                unsigned char color_byte = color1 + 16 * color2;
                fputc(color_byte, f);
            }
            else {
				fputc(0x00, f);
			}
        } 
               

    }

    // Pattern name table 1 + 2 + 3
    for (int yi = 0; yi < TILES_NUM_ROWS; yi++)
    { 
        for (int xi = 0; xi < TILES_NUM_COLS; xi++)
        {
            printf("%2X ", msx_tile_name[yi][xi]);
            fputc(msx_tile_name[yi][xi], f); 
        }   
        printf("\n");
    }

    if (f != 0) fclose(f);
}

void MSXoutput_tiled(t_img* e) 
{
  
     compress_tilemap(); 
     clean_unused_tiles();
      group_same_tiles();
      compress_tilemap();
     // MSXoutput_tiled_compress(e);
    int i, j;
    unsigned char MsxHeader[7] = { 0xFE,0x00,0x00,0xFF,0x37,0x00,0x00 };

    FILE* f = NULL;
    errno_t err = fopen_s(&f, g_argv[2], "wb");
    if (err != 0)
        ft_exit(e, 1, "Writing output file problem!");

    if (f == NULL) {
        ft_exit(e, 1, "Writing output file problem!");
        return;
    }

    for (i = 0; i < 7; i++) {
       fputc(MsxHeader[i], f);
    }

    // Pattern generator table 1 + 2 + 3
    for (int actual_table = 0; actual_table < 3; ++actual_table)
    {
        for (int tile_id = 0; tile_id <  256; tile_id++)
        {
            tile_generator_t t = msx_tile_generator[actual_table][tile_id];
            for (int subline = 0; subline < 8; ++subline)
            {
                unsigned char byte = t.tile[subline];                 
                fputc(byte, f);
            }
        }

    
        //print location as hexadecimal  
         printf("Location of MSX tile generator table %d : %X\n", actual_table, ftell(f) - 1);

    }

    // Pattern name table 1 + 2 + 3
    for (int yi = 0; yi < TILES_NUM_ROWS; yi++)
        for (int xi = 0; xi < TILES_NUM_COLS; xi++)
        {           
            fputc(msx_tile_name[yi][xi], f);
        }
  
 
    printf("End Pattern names   %X\n",   ftell(f) - 1);

    //Sprite attribute table  + Palette table
    for (i = 0; i <= 1279; i++)
    {
        fputc(0, f);
    }

    printf("Sprite attribute table   %X\n", ftell(f) - 1);

    //colour tables
    for (int actual_table = 0; actual_table < 3; ++actual_table)
    {
        for (int tile_id = 0; tile_id < 256; tile_id++)
        {
            for (int yi = 0; yi < 8; ++yi)
            {
                unsigned char color = msx_tile_colour[actual_table][tile_id].color[yi][0]  +  16* msx_tile_colour[actual_table][tile_id].color[yi][1];
                fputc(color, f);
            } 
           
        }
        printf("End Color table  %X\n", ftell(f) -1 );
    }


    if (f!= 0 ) fclose(f);
}

//**--------------------------------**
//    Saving Computed image
//     To MSX Screen2 Bin Format
//**--------------------------------**
//

void MSXoutput(t_img* e)
 
{

    MSXoutput_tiled(e);
    return;
    int i, j;
    unsigned char MsxHeader[7] = { 0xFE,0x00,0x00,0xFF,0x37,0x00,0x00 };

    FILE* f = NULL;
    errno_t err  = fopen_s(&f , g_argv[2], "wb");
	if (err != 0 )
        ft_exit(e, 1, "Writing output file problem!");
	
    if (f == NULL)
        ft_exit(e, 1, "Writing output file problem!");

    for (i = 0; i < 7; i++) {
        fputc(MsxHeader[i], f);
    }

    // Pattern generator table 1 + 2 + 3
    for (i = 0; i <= 6143; i++)
    {
        fputc(msxdump[i], f);
    }

    //Pattern name table 1 + 2 + 3
    for (j = 0; j <= 2; j++)
    {
        for (i = 0; i <= 255; i++)
        {
            fputc((char)i, f);
        }
    }

    //Sprite attribute table
    for (i = 0; i <= 1279; i++)
    {
        fputc(0, f);
    }

    //Colour table 1 + 2 + 3
    for (i = 0; i <= 6143; i++)
    {
        fputc(msxdump[i + 6144], f);
    }
    if (f != 0) fclose(f);

    printf("Write out done\n");
}

//**--------------------------------**
//   Draw Computed Image on Screen
//
//**--------------------------------**
//

void DrawRenderImage(t_img* e, int Start_x, int Start_y)
{
    int x, y;

    for (y = 0; y <= IMAGE_HEIGHT - 1; y++)
    {
        for (x = 0; x <= IMAGE_WIDTH - 1; x++) {
            SDL_pixel_put_to_image(e, Start_x + x, Start_y + y, e->RenderImage[y][x]);
        }
    }
}

void DrawSlot(t_img* e, int Start_x, int Start_y)
{
    int x, y;

    for (y = 0; y <= IMAGE_HEIGHT - 1; y++)
    {
        for (x = 0; x <= IMAGE_WIDTH - 1; x++) {
            SDL_pixel_put_to_image(e, Start_x + x, Start_y + y, 0xFFFFFFFF);
        }
    }
    SDL_render(e);
}

char PrintDo(t_img* e)
{
    printf("\n-> Computing ... Using ");
    printf("%s", Palette_name[e->UsePalette]);
    return(1);
}


void Do_it(t_img* e)
{
    ImageProcess(e);

    DrawRenderImage(e, Slot_x[e->slot], Slot_y[e->slot]);


    SDL_render(e);
    printf("\n-> Interface : %d", e->interface);
    printf("\n-> Color Tolerance : %d", e->tolerance);
    printf("\n-> Detail Level : %d", e->detaillevel);
    printf("\n________________________");

    MSXoutput(e);
}
void Info(t_img* e)
{
    printf("\n "); printf(NAME); printf(VERSION); printf(DATE);
    printf("\n is converting 256 x 192 pixels BMP Images \n to a fixed color palette using CIEDE2000 Algorythm");
    printf("\n Original convesion Routine by Leandro Correia");
    printf("\n This tool was coded in C, with SDL2 Graphic Library\n by Eric Boez 2019 - BZ Prod Game Studio");
    printf("\n For MSX1 Screen2 mode with 16 colors palette support.");
    printf("\n This tool is provided as is, with no guaranty\n Under the Creative Commons CC BY_SA 4.0 license");
}
void Usage(t_img* e)
{
    char i;
    printf("\n GraphxConverter usage : \n graphxconv <Input file> <output file> [-i<n> -p<n> -t<n> -d<n> -c -h] \n");

    printf("\n<input file> : path and name of the BMP file to convert");
    printf("\n<output file> : path and name of the saved file");
    printf("\n-i<n> : if n=0, quit Graphic Interfac once conversion is done. (ex: -i0)");
    printf("\n-p<n> : Palette number to use (ex : -p3)");
    printf("\n-t<n> : Color Tolerance. <n> must be between 0 and 100 (ex: -t90)");
    printf("\n-d<n> : Details Level. <n> must be between 0 and 255 (ex: -d30)");
    printf("\n-c : Show Copyright Informations");
    printf("\n-h : the help text you are reading at the moment !");
    printf("\n----------------");
    printf("\nList of palettes:");
    for (i = 0; i <= NB_PALETTE; i++)
    {
        printf("\n%d : %s", i, Palette_name[i]);
    }
    printf("\n----------------");
    printf("\nList of Keys in Graphic Interface:");
    printf("\n1 : Color tolerance -");
    printf("\n2 : Color tolerance +");
    printf("\n8 : Detail level -");
    printf("\n9 : Detail Level +");
    printf("\n3 : Toggle Block render ");

    printf("\nP : use Next Palette");
    printf("\nS : New Slot image");
    printf("\nESC : Quit program");

    ft_exit(e, 6, "");
}


//**--------------------------------**
//              Main
//
//**--------------------------------**
//

int main(int argc, char* argv[]) {

    char C_err = 0;

    t_img* e = NULL;
    SDL_Event ev;

    printf("\n");
    printf(NAME);
    printf("\n");

    if (!(e = (t_img*)calloc(1, sizeof(t_img))))                             // Structure Init
        ft_exit(e, 4, "Malloc error: ->e");

    init_mem(e);
    e->click_clock = clock() + (CLOCKS_PER_SEC / 10);


    //Default parameters
    e->tolerance = 100;
    e->detaillevel = 32;
    e->UsePalette = 0;
    e->slot = 1;
    e->interface = 1;
    e->click_clock = e->clock;
    e->block_render = 0;


    g_argv = argv;
    ////////////////////// Parameters Checking

    if (argc < 3)
    {
        printf("\n ERROR : Missing Input file and/or Output file name. \nType the command line with -h for help\n");
        C_err = 1;
    }
    else
    {
        ++argv;
        --argc;
        ++argv;
        --argc;
    }

    while ((argc > 1) && (argv[1][0] == '-'))
    {

        switch (argv[1][1])
        {
        case 'i':
            e->interface = atoi(&argv[1][2]);
            break;

        case 'p':
            e->UsePalette = atoi(&argv[1][2]);
            if (e->UsePalette > NB_PALETTE)
                e->UsePalette = NB_PALETTE;
            break;

        case 't':
            e->tolerance = atoi(&argv[1][2]);
            if (e->tolerance > TOLERANCE_MAX || e->tolerance < TOLERANCE_MIN)
            {
                printf("\n ERROR : Wrong Color Tolerance parameter\n");
                Usage(e);
            }
            break;
        case 'd':
            e->detaillevel = atoi(&argv[1][2]);
            if (e->detaillevel > DETAIL_MAX || e->detaillevel < DETAIL_MIN)
            {
                printf("\n ERROR : Wrong Details Level parameter\n");
                Usage(e);
            }
            break;
        case 'c':
            C_err = 1;
            Info(e);
            ft_exit(e, 0, "\nSee you !");
            break;
        case 'h':
            Usage(e);
            ft_exit(e, 0, "");
            break;
        default:
            printf("\nWrong Argument: %s\n", argv[1]);
            Usage(e);
        }

        ++argv;
        --argc;
        if (C_err == 1)
            ft_exit(e, 0, "\n");
    }

    ////////////////////////////////////

    load_image(e);

    SDL_render(e);

    SDL_LockSurface(e->img);
    SDL_SetRenderTarget(e->ren, NULL);

    PrintDo(e);
    Do_it(e);

    if (e->interface == 0)
        ft_exit(e, 0, "\nSee you !");

    while (1) // infinite loop
    {
        SDL_render(e);
        e->clock = clock();
        SDL_PumpEvents();
        //if (ev.type==SDL_KEYDOWN)
        key_hook(e);

        if (SDL_PollEvent(&ev))                        //  events in the queue
        {
            if (ev.window.event == SDL_WINDOWEVENT_CLOSE)
                ft_exit(e, 0, "\nSee you !");
        }

    }
}
