/* Memory */
#define DDR_BASE 0x00000000
#define DDR_END 0x3FFFFFFF
#define A9_ONCHIP_BASE 0xFFFF0000
#define A9_ONCHIP_END 0xFFFFFFFF
#define SDRAM_BASE 0xC0000000
#define SDRAM_END 0xC3FFFFFF
#define FPGA_ONCHIP_BASE 0xC8000000
#define FPGA_ONCHIP_END 0xC803FFFF
#define FPGA_CHAR_BASE 0xC9000000
#define FPGA_CHAR_END 0xC9001FFF

/* Cyclone V FPGA devices */
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define JP1_BASE 0xFF200060
#define JP2_BASE 0xFF200070
#define PS2_BASE 0xFF200100
#define PS2_DUAL_BASE 0xFF200108
#define JTAG_UART_BASE 0xFF201000
#define JTAG_UART_2_BASE 0xFF201008
#define IrDA_BASE 0xFF201020
#define TIMER_BASE 0xFF202000
#define AV_CONFIG_BASE 0xFF203000
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030
#define AUDIO_BASE 0xFF203040
#define VIDEO_IN_BASE 0xFF203060
#define ADC_BASE 0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE 0xFF709000
#define HPS_TIMER0_BASE 0xFFC08000
#define HPS_TIMER1_BASE 0xFFC09000
#define HPS_TIMER2_BASE 0xFFD00000
#define HPS_TIMER3_BASE 0xFFD01000
#define FPGA_BRIDGE 0xFFD0501C

#define JTAG_UART_BASE ((volatile int*) 0xFF201000)
#define JTAG_UART_CONTROL ((volatile int*) (0xFF201000+4))


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

void draw_box(int x, int y, int width, int height, short int colour) {
    int x_increment,y_increment;
	for ( x_increment = 0 ; x_increment < width; x_increment++) {
		for ( y_increment = 0 ; y_increment < height; y_increment++) {
			write_pixel(x + x_increment, y + y_increment, colour);
		}
	}
}

void draw_square_centered(int x_centre,int y_centre,int padding, int half_side)
{
    int x_big = x_centre-(padding+half_side);
    int y_big = y_centre-(padding+half_side);
    int x_small = x_centre-half_side;
    int y_small = y_centre-half_side;

    for(int i = x_big; i<= (x_big+2*(half_side+padding));i++)
    {
      for(int j=y_big; j<=(y_big+2*(half_side+padding));j++)
      {
        write_pixel(i,j,0Xffff);
      }
    }

      for(int i = x_small; i<= (x_small+2*half_side);i++)
    {
      for(int j=y_small; j<=(y_small+2*half_side);j++)
      {
        write_pixel(i,j,0xff00);
      }
    }
}


void draw_table()
{


}

int main()
{

  clear_screen();
  draw_square_centered(110,120,5,20);
}