
#include <stdlib.h>
#include <time.h>



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

#define yellow 0xff00
#define white 0xffff
#define red 0xf800
#define green 0x07e0
#define gray 0x8410
#define purple 0xf81f
#define black 0x0000

int x_centres[9] = {110,160,210,110,160,210,110,160,210}; // centres of the respective squares
int y_centres[9] = {70,70,70,120,120,120,170,170,170};

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


void draw_table() // draws our initial 9 blocks
{
  draw_square_centered(x_centres[0],y_centres[0],5,20,yellow,white);
  draw_square_centered(x_centres[1],y_centres[1],5,20,yellow,white);
  draw_square_centered(x_centres[2],y_centres[2],5,20,yellow,white);
  draw_square_centered(x_centres[3],y_centres[3],5,20,yellow,white);
  draw_square_centered(x_centres[4],y_centres[4],5,20,yellow,white);
  draw_square_centered(x_centres[5],y_centres[5],5,20,yellow,white);
  draw_square_centered(x_centres[6],y_centres[6],5,20,yellow,white);
  draw_square_centered(x_centres[7],y_centres[7],5,20,yellow,white);
  draw_square_centered(x_centres[8],y_centres[8],5,20,yellow,white);
 
}

void trigger_square(char input,int correct) //checks whats the input and if its correct or wrong and turns it green or red respectively
{
    short colour;
    if(correct == 1)
    colour = green;
    else
    colour = red;

    int c;

          if(input=='7' || input=='q' || input=='Q')
      c = 0;
    else if(input=='8' || input=='w' || input=='W')
    c=1;
     else if(input=='9' || input=='e' || input=='E')
    c=2;
     else if(input=='4' || input=='a' || input=='A')
    c=3;
     else if(input=='5' || input=='s' || input=='S')
    c=4;
     else if(input=='6' || input=='d' || input=='D')
    c=5;
     else if(input=='1' || input=='z' || input=='Z')
    c=6;
     else if(input=='2' || input=='x' || input=='X')
    c=7;
     else if(input=='3' || input=='c' || input=='C')
    c=8;
    else
    c=9;

      if(c==9)
      return;
      else
      {
         draw_square_centered(x_centres[c],y_centres[c],5,20,colour,white); //render
        for(int i=0; i<(10000000/2);i++) //delay
        {
          ;
        }
        draw_square_centered(x_centres[c],y_centres[c],5,20,yellow,white); //unrender
      }
}

int check(char input, int answer) //checks if the key is valid or not if not it just asks for another input else it returns if the answer was correct or not
{

int c; // checks which block is being activated
       if(input=='7' || input=='q' || input=='Q')
      c = 0;
    else if(input=='8' || input=='w' || input=='W')
    c=1;
     else if(input=='9' || input=='e' || input=='E')
    c=2;
     else if(input=='4' || input=='a' || input=='A')
    c=3;
     else if(input=='5' || input=='s' || input=='S')
    c=4;
     else if(input=='6' || input=='d' || input=='D')
    c=5;
     else if(input=='1' || input=='z' || input=='Z')
    c=6;
     else if(input=='2' || input=='x' || input=='X')
    c=7;
     else if(input=='3' || input=='c' || input=='C')
    c=8;
    else
    c=9;

if(c==9) // none of the blocks are being activated
return 2;
else if(c==answer) // correct block activated
return 1;
else
return 0; // wrong block activated
    
}


void show_pattern(int arr_rand[],int size)
{
    for(int i = 0; i<size; i++)
    {
      int c = arr_rand[i];
      draw_square_centered(x_centres[c],y_centres[c],5,20,purple,white);
        for(int i=0; i<(10000000);i++)
        {
          ;
        }
        draw_square_centered(x_centres[c],y_centres[c],5,20,yellow,white);
      }
}


int play_level(int level)
{
  srand(time(NULL));
  int flag = 1;
  int length_of_round = level+3;
  int lives = 3;
   int arr_rand[length_of_round];
   volatile int * JTAG_UART_ptr = (int *) JTAG_UART_BASE;
  char c;
  int correct;
  for(int i = 0; i<length_of_round; i++)
  {
    arr_rand[i] = rand()%9;
  }
  
  while(1)
  {
        show_pattern(arr_rand,length_of_round);
        int current = 0;
        while(1)
        {
          if(current == length_of_round || lives==0)
          break;

            c = get_jtag(JTAG_UART_ptr); 
            int check_correct = check(c,arr_rand[current]);
            if(check_correct==2)
            {
              continue;
            }
            else if(check_correct==1)
            {
              correct = 1;
              current++;
            }
            else
            {
                correct = 0;
              lives--;
            }

            trigger_square(c,correct);
        }

        if(lives == 0)
        {
          flag = 0;
        }

        if(current== length_of_round || lives==0)
        break;
      }
        return flag;
  }

void end_game(int level)
{
  
}
    

int main()
{
  clear_screen();
  
  int level = 1;
  clear_screen();
  draw_table();
  while(1)
  {
     int flag =play_level(level);
     if(flag)
     {
      level++;
     }
     else
     end_game(level);
  
  }

}