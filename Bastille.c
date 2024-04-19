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
#define unrender gold_orange
int turret_xcentre[] = {36,98,160,222,284}; // x positions of the 5 columns
int turret_ycentre = 216; // turret y alignment
int enemy_y_centres[] = {17,45,73,101,129,157};//y-positions on the map
int curr_pos; // to move the turret
int enemy_radius = 5;// radius of enemies
short turret_color = red;//turret colour and the siege colour
int enemy_map[5][6] = {{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0}};//stores location of the enemies
int spawnDelay = 1000; // Current delay between enemy spawns
int numColumns = 3; // Number of columns to spawn enemies in
int timer = 0;//timer basically
int last_shot=0;//tracks when the last shot was taken
int score = 0;//to keep track of score;



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

void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
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
   if(color==unrender)
   { 
    draw_square_centered(x_centre,y_centre,2,10,unrender,unrender);
    draw_rectangle_centered(x_centre,y_centre-(21),2,3,8,unrender,unrender);
   }
   else
   {
    draw_square_centered(x_centre,y_centre,2,10,color,white);
    draw_rectangle_centered(x_centre,y_centre-(21),2,3,8,color,white);
   }
}

void make_turret_homescreen(int x_centre,int y_centre,int color)
{
   if(color==unrender)
   { 
    draw_square_centered(x_centre,y_centre,4,20,unrender,unrender);
    draw_rectangle_centered(x_centre,y_centre-(42),4,6,16,unrender,unrender);
   }
   else
   {
    draw_square_centered(x_centre,y_centre,4,20,color,white);
    draw_rectangle_centered(x_centre,y_centre-(42),4,6,16,color,white);
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


void move_turret(char c)
{
  if(c=='a' || c=='A')
  {
    if(curr_pos==0)
    return;

    make_turret(turret_xcentre[curr_pos],turret_ycentre,unrender);
    curr_pos= curr_pos-1;
    make_turret(turret_xcentre[curr_pos],turret_ycentre,turret_color);
  }
  else if(c=='d' || c=='D')
  {
      if(curr_pos==4)
      return;
      
    make_turret(turret_xcentre[curr_pos],turret_ycentre,unrender);
    curr_pos= curr_pos+1;
    make_turret(turret_xcentre[curr_pos],turret_ycentre,turret_color);
  }
  else
  return;
}

void shoot_turret()
{ 

  int check;
  if(timer>=last_shot)
  {
  check = timer - last_shot;
  }
  else
  check=20;
  

  if(check<20)
  return;

  int x = curr_pos;
  int j=5;
  int flag=0;
  for(j;j>=0;j--)
  {
    if(enemy_map[x][j]!=0)
    {
      flag=1;
    break;
    }
  }
  if(flag)
  {
    enemy_map[x][j]--;
    score++;
    last_shot = timer;
  }
  else
  return;
}

void enemy_spawn()
{
  int k = 0 ;
  int count=0;
  while(k<numColumns && count<5)
  {
    int x = rand()%5;
    if(enemy_map[x][0]==0)
    {
      k++;
      int enemy_type = (rand()%3)+1;
      enemy_map[x][0] = enemy_type;
      count++;
    }
    else
    {
      k++;
      continue;
    }
  }
}

int enemy_update()
{
  for(int i=0;i<5;i++)
  {
    if(enemy_map[i][5]!=0)
    return 0;
  }
 
  for(int i=0;i<5;i++)
  {
    for(int j=4; j>=0;j--)
    {
        if(enemy_map[i][j]==0)
        continue;
        else
        {
          enemy_map[i][j+1] = enemy_map[i][j];
          enemy_map[i][j] = 0;
        }
    }
  }
  enemy_spawn();

  return 1;

}

void enemy_render()
{

      for(int i=0; i<5; i++)
      {
       for(int j=0; j<6;j++)
        {
          if(enemy_map[i][j]==0)
          make_circle(turret_xcentre[i],enemy_y_centres[j],enemy_radius,gold_orange);
          else
          {
            int k = enemy_map[i][j];
            if(k==1)
            make_circle(turret_xcentre[i],enemy_y_centres[j],enemy_radius,green);
            else if(k==2)
            make_circle(turret_xcentre[i],enemy_y_centres[j],enemy_radius,Blue);
            else
            make_circle(turret_xcentre[i],enemy_y_centres[j],enemy_radius,red); 
          }
        }
      }
}

void turret_action(char c)
{
  if(c=='a' || c=="A" || c=='d' || c=='D')
  {
    move_turret(c);
    return;
  }
  else if(c==' ')
  {
    shoot_turret();
  }
  else
  {
    return;
  }
}

short choose_colour(int red_count, int green_count, int blue_count)
{
  short colour_here = (red_count<<11)+(green_count<<5)+blue_count;
  return colour_here;
}

void set_turret_color(int red_count,int green_count, int blue_count)
{
     short colour_here = choose_colour(red_count,green_count,blue_count);
    make_turret_homescreen(160,170,unrender);
    make_turret_homescreen(160,170,colour_here);
    turret_color = colour_here;
}

void home_screen()
{
  clear_screen();
  level_screen();
  char c;
   volatile int * JTAG_UART_ptr = (int *) JTAG_UART_BASE;
   write_string(38,6,"BASTILLE");
   write_string(30,9,"PRESS P TO BEGIN THE GAME");
   char *heading = "COLOUR       +       -";
   char *for_red = "RED          Q       W";
   char *for_green = "GREEN        A       S";
   char *for_blue = "BLUE         Z       X";
   write_string(5,25,heading);
   write_string(5,27,for_red);
   write_string(5,29,for_blue);
   write_string(5,31,for_green);


   int red_count = 30;
  int green_count = 64;
  int blue_count = 30;

  set_turret_color(red_count,green_count,blue_count);
  
  while(1)
  {
    c = get_jtag(JTAG_UART_ptr);
    if(c=='P' || c=='p')
    return;
    else if(c=='Q' || c=='q')
    {
          if(red_count<30)
          {
          red_count+=3;
          set_turret_color(red_count,green_count,blue_count);
          }

          
    }
     else if(c=='A' || c=='a')
    {
          if(green_count<64)
          {
          green_count+=4;
          set_turret_color(red_count,green_count,blue_count);
          }

          
    }
     else if(c=='Z' || c=='z')
    {
          if(blue_count<30)
          {
          blue_count+=3;
          set_turret_color(red_count,green_count,blue_count);
          }
    }
      else if(c=='W' || c=='w')
    {
          if(red_count>0)
          {
          red_count-=3;
          set_turret_color(red_count,green_count,blue_count);
          }
    }
     else if(c=='S' || c=='s')
    {
          if(green_count>0)
          {
          green_count-=4;
          set_turret_color(red_count,green_count,blue_count);
          }
    }
     else if(c=='X' || c=='x')
    {
          if(blue_count>0)
          {
          blue_count-=3;
          set_turret_color(red_count,green_count,blue_count);
          }
    }
  }
}



void start_game()
{
  clear_screen();
  level_screen();
    draw_line(turret_color);
   make_turret(turret_xcentre[2],turret_ycentre,turret_color);
 curr_pos = 2;
 volatile int * JTAG_UART_ptr = (int *) JTAG_UART_BASE;
 
 enemy_spawn();
 while(1)
 {
    timer = (timer+1)%(100000000);
    
  char c = get_jtag(JTAG_UART_ptr);
  turret_action(c);
  enemy_render();
  if(timer%(spawnDelay)==0)
  {
    if(spawnDelay==800)
    numColumns=5;
    if(spawnDelay==700)
    numColumns=10;
    if(spawnDelay==600)
    numColumns=20;
    if(spawnDelay>500)
    {
      spawnDelay-=50;
    }
    if(spawnDelay==500)
    spawnDelay-=15;

  int check = enemy_update();
  if(!check)
  break;
  }
 }
}

void init()
{
  for(int i=0; i<5; i++)
  {
    for(int j=0; j<6; j++)
    {
      enemy_map[i][j] = 0;
    }
  }
spawnDelay = 1000; // Current delay between enemy spawns
numColumns = 3; // Number of columns to spawn enemies in
timer = 0;//timer basically
last_shot=0;//tracks when the last shot was taken
score=0;

}

void end_game()
{
   clear_screen();
  level_screen();
  char *str;
  tostring(str,score);
  write_string(34,6,"SCORE =");
  write_string(43,6,str);
  write_string(38,29,"GAME OVER");
  write_string(30,31,"PRESS P TO PLAY AGAIN");
  volatile int * JTAG_UART_ptr = (int *) JTAG_UART_BASE;
  char c;
  while(1)
  {
    c =  get_jtag(JTAG_UART_ptr);
    if(c=='p' || c=='P')
    return;
  }
}

int main()
{
  srand(time(NULL));
  home:
  home_screen();
  start_game();
  end_game();
  init();
  goto home;
}