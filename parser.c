#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         translate: create a translation matrix,
                    then multiply the transform matrix by the translation matrix -
                    takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge matrix
         display: clear the screen, then
                  draw the lines of the edge matrix to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge matrix to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing

See the file script for an example of the file format

IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename, struct matrix * transform, 
                  struct matrix * edges, screen s)
{

  FILE *f;
  char line[1024];
  clear_screen(s);
  
  color c;
  c.red = MAX_COLOR / 2;
  c.blue = MAX_COLOR;
  c.green = 0;

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';

	if (!strcmp(line, "line"))
	{
		//get data from line
		fgets(line, 1024, f);
		
		//initalize and assign values to appropriate variables
		double x0, y0, z0, x1, y1, z1;
		sscanf(line, "%lf %lf %lf %lf %lf %lf", &x0, &y0, &z0, &x1, &y1, &z1);

		//add to matrix
		add_edge(edges, x0, y0, z0, x1, y1, z1);
		
	}
	else if (!strcmp(line, "ident"))
	{
		ident(transform);

	}
	else if (!strcmp(line, "scale"))
	{
		//init tmp matrix
		struct matrix *tmp;

		//get data from line
		fgets(line, 1024, f);
	
		//init variables and assign values
		double x, y, z;
		sscanf(line, "%lf %lf %lf", &x, &y, &z);

		//make tmp scale matrix, multiply it w/ transformation, then free it
		tmp = make_scale(x, y, z);
		matrix_mult(tmp, transform);
		free_matrix(tmp);

	}
	else if (!strcmp(line, "translate") || !strcmp(line, "move"))
	{
		struct matrix * tmp;

		fgets(line, 1024, f);

		double x, y, z;
		sscanf(line, "%lf %lf %lf", &x, &y, &z);

		tmp = make_translate(x, y, z);
		matrix_mult(tmp, transform);
		free_matrix(tmp);
	}
	else if (!strcmp(line, "rotate"))
	{
		//init tmp matrix
		struct matrix *tmp;
	
		//get data, init variables, assign values based on input
		fgets(line, 1024, f);
		char var;
		double deg;
		sscanf(line, "%c %lf", &var, &deg);
		
		//run respective function based on input
		if (var  == 'x')
		{
			tmp = make_rotX(deg);
		}
		else if (var == 'y')
		{
			tmp = make_rotY(deg);
		}
		else if (var == 'z')
		{
			tmp = make_rotZ(deg);
		}

		//perform matrix multiplication and free tmp matrix
		matrix_mult(tmp, transform);
		free_matrix(tmp);
	}
	else if (!strcmp(line, "apply"))
	{
		matrix_mult(transform, edges);
	}
	else if (!strcmp(line, "display"))
	{
		clear_screen(s);
		draw_lines(edges, s, c);
		printf("Displaying image\n");
		display(s);
	}
	else if (!strcmp(line, "save"))
	{
		char * filename;
		filename = malloc(255);
		fgets(filename, 255, f);

		clear_screen(s);
		draw_lines(edges, s, c);
		save_extension(s, filename);
		printf("Saved to %s\n", filename);
		free(filename);
	}
	else if (!strcmp(line, "quit"))
	{
		exit(0);
	}

	//debuggin
    //printf(":%s:\n",line);
  }
}
  
