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

void circle(int x0, int y0, int radius, short color)
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

char get_jtag(volatile int *JTAG_UART_ptr) 
{
  int data;
  data = *(JTAG_UART_ptr);
  if (data & 0x00008000) // check INVALID
    return ((char)data & 0xFF);
  else
    return ('\0');
}