#include <stdlib.h>
#include <time.h>

/* Cyclone V FPGA devices */
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define JTAG_UART_BASE 0xFF201000
#define JTAG_UART_2_BASE 0xFF201008
#define IrDA_BASE 0xFF201020
#define TIMER_BASE 0xFF202000



#define JTAG_UART_BASE ((volatile int*) 0xFF201000)
#define JTAG_UART_CONTROL ((volatile int*) (0xFF201000+4))

#define yellow 0xff00
#define white 0xffff
#define red 0xf800
#define green 0x07e0
#define gray 0x8410
#define purple 0xf81f
#define black 0x0000
#define Orange 0xFD20//Works
#define Blue 0x001F//works
#define Indigo 0x4810//Dark Blue
#define Cyan 0x07FF//works
#define Magenta 0xF81F//works(light purple)
#define Lime 0x07E0//light green
#define Teal 0x0410//works
#define Brown 0xA145//works
#define gold_orange 0xFE00//works
#define olive 0x8400//works
#define coral_pink 0xFBEA//works
#define peach 0xFDA0//works

short unrender = gold_orange;
int turret_xcentre[] = {36,98,160,222,284};
int turret_ycentre = 216;
int enemy_y_centres[] = {17,45,73,101,129,157};

void write_pixel(int x, int y, short colour) 
{
  volatile short *vga_addr=(volatile short*)(0x08000000 + (y<<10) + (x<<1));
  *vga_addr=colour;
}

short read_pixel(int x, int y) 
{
  volatile short *vga_addr=(volatile short*)(0x08000000 + (y<<10) + (x<<1));
  return *vga_addr;
}

/* write a single character to the character buffer at x,y
 * x in [0,79], y in [0,59]
 */
void write_char(int x, int y, char c) 
{
  // VGA character buffer
  volatile char * character_buffer = (char *) (0x09000000 + (y<<7) + x);
  *character_buffer = c;
}
void write_string(int x,int y, char s[])
{
    int a=x; int b=y;
    for(int i=0;i<strlen(s);i++)
    {
        write_char(a,b,s[i]);
    a++;
}
}

void clear_pixel() {
  int x, y;
  for (x = 0; x < 320; x++) {
    for (y = 0; y < 240; y++) {
    write_pixel(x,y,0);
  }
  }
}
void clear_char() {
  int x, y;
  for (x = 0; x < 80; x++) {
    for (y = 0; y < 60; y++) {
    write_char(x,y,(char)0);
  }
  }
  
}

/* use write_pixel to set entire screen to black (does not clear the character buffer) */
void clear_screen() 
{
  int x, y;
  for (x = 0; x < 320; x++) {
    for (y = 0; y < 240; y++) {
    write_pixel(x,y,0);
  }
  }
	
  for (x = 0; x < 80; x++) 
  {
    for (y = 0; y < 60; y++) {
    write_char(x,y,(char)0);
  }
  }
}

char get_jtag(volatile int *JTAG_UART_ptr) 
{
  int data;
  data = *(JTAG_UART_ptr);
  if (data & 0x00008000) // check INVALID
    return ((char)data & 0xFF);
  else
    return ('\0');
}

void make_circle(int x0, int y0, int radius, short color)
{
	int x, y;
	for (y = -radius; y <= radius; y++)
	{
		for (x = -radius; x <= radius; x++)
		{
			if (x * x + y * y <= radius * radius)
			{
				write_pixel(x0 + x, y0 + y, color);

			}
		}
	}

}


void draw_square_centered(int x_centre,int y_centre,int padding, int half_side,short colour,short colour_border)
{
    int x_big = x_centre-(padding+half_side);
    int y_big = y_centre-(padding+half_side);
    int x_small = x_centre-half_side;
    int y_small = y_centre-half_side;

    for(int i = x_big; i<= (x_big+2*(half_side+padding));i++)
    {
      for(int j=y_big; j<=(y_big+2*(half_side+padding));j++)
      {
        write_pixel(i,j,colour_border);
      }
    }

      for(int i = x_small; i<= (x_small+2*half_side);i++)
    {
      for(int j=y_small; j<=(y_small+2*half_side);j++)
      {
        write_pixel(i,j,colour);
      }
    }
}

void draw_rectangle_centered(int x_centre,int y_centre,int padding, int breadth_half,int height_half,short colour,short colour_border )
{
    int x_big = x_centre-(padding+breadth_half);
    int y_big = y_centre-(padding+height_half);
    int x_small = x_centre-breadth_half;
    int y_small = y_centre-height_half;

    for(int i = x_big; i<= (x_big+2*(breadth_half+padding));i++)
    {
      for(int j=y_big; j<=(y_big+2*(height_half+padding));j++)
      {
        write_pixel(i,j,colour_border);
      }
    }

      for(int i = x_small; i<= (x_small+2*breadth_half);i++)
    {
      for(int j=y_small; j<=(y_small+2*height_half);j++)
      {
        write_pixel(i,j,colour);
      }
    }
}

void level_screen() 
{
  int x, y;
  for (x = 4; x <316; x++) {
    for (y = 3; y <237; y++) {
    write_pixel(x,y,gold_orange);
  }
  }
int i;
  for(i=0;i<320;i++)
  {
   write_pixel(i,237,Brown);
   write_pixel(i,238,Brown);
   write_pixel(i,239,Brown);
  }
  for(i=0;i<320;i++) 
  {
    write_pixel(i,0,Brown);
    write_pixel(i,1,Brown);
    write_pixel(i,2,Brown);
  }
  for(i=0;i<240;i++) 
  {
      write_pixel(0,i,Brown);
      write_pixel(1,i,Brown);
      write_pixel(2,i,Brown);
      write_pixel(3,i,Brown);
  }
  for(i=0;i<240;i++) 
  {
    write_pixel(316,i,Brown);
    write_pixel(317,i,Brown);
    write_pixel(318,i,Brown);
    write_pixel(319,i,Brown);
  }
}

void make_turret(int x_centre,int y_centre,int color)
{
   if(color==gold_orange)
   { 
    draw_square_centered(x_centre,y_centre,2,10,gold_orange,gold_orange);
    draw_rectangle_centered(x_centre,y_centre-(21),2,3,8,gold_orange,gold_orange);
   }
   else
   {
    draw_square_centered(x_centre,y_centre,2,10,color,white);
    draw_rectangle_centered(x_centre,y_centre-(21),2,3,8,color,white);
   }
}

void draw_line(short color)
{
    for(int i=4;i<316;i++)
  {
    write_pixel(i,173,color);
    write_pixel(i,172,color);
  }
}

void make_grid()
{
  for(int i=3;i<237;i++)
  {
    write_pixel(67,i,red);
    write_pixel(129,i,red);
    write_pixel(191,i,red);
    write_pixel(257,i,red);
    write_pixel(319,i,red);
  }
  for(int j=0;j<5;j++)
  {
  for(int i=0;i<6;i++)
  {
    make_circle(turret_xcentre[j],enemy_y_centres[i],5,green);
  }
  }
}




int main()
{
  clear_screen();
  level_screen();
  make_grid();
  draw_line(red);
  for(int i=0; i<5; i++)
  make_turret(turret_xcentre[i],turret_ycentre,red);
   
}