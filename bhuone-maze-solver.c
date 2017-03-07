/* Author: Bhuwan Garbuja
 * Date: 2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* the dimension of the maze */
unsigned int g_maze_row;
unsigned int g_maze_col;

/* the initial position for the maze */
unsigned int g_start_x;
unsigned int g_start_y;

/* the end position for the maze */
unsigned int g_end_x;
unsigned int g_end_y;

/* this is the number of maze cells that were blocked */
unsigned int g_blocked_cells;

/* the main array for storing the maze detail */
unsigned int **g_main_maze;

/* file pointer for the files */
FILE *g_maze_file;
FILE *g_output_file;

struct BorderPosition
{
	/*
	 * if the value is 1 then it means there is a wall on that direction
	 * 0 will mean that there is no border
	 */
	unsigned int top, bot, right, left;
};

/*
 * procedure to read the maze
 */
void readMazeFromFile( char *maze_file_name )
{
	g_maze_file = fopen( maze_file_name, "r" );

	// if can'u find or open the file
	if ( g_maze_file == NULL )
	{
		// print the error message and end the program
		printf( "\nCannot open the file:'%s'. " , maze_file_name );
		printf( "Perhaps missing or filename different.\n" );
		exit(1);
	}
	
	//read the first line - get the maze dimension
	fscanf( g_maze_file,"%d %d/n", &g_maze_row, &g_maze_col);
	
	//allocate the number of row in the array
	g_main_maze = ( unsigned int** ) malloc( g_maze_row * sizeof( unsigned int* ) );
	
	//for each allocated row, allocate the columns in that row
	unsigned int x, y;
	for ( x = 0; x < g_maze_row; x++ )
	{
		//allocate the columns
		g_main_maze[ x ] = ( unsigned int* ) malloc( g_maze_col * sizeof( unsigned int ) );
	}
	
	//read the second line - get the start x and y coordinate
	fscanf( g_maze_file, "%d %d/n", &g_start_x, &g_start_y );
	
	//read the third file - get the ending x and y coordinate
	fscanf( g_maze_file, "%d %d/n", &g_end_x, &g_end_y );
	
	//now store numbers remaining the file
	for ( x = 0; x < g_maze_row; x++ )
	{
		for ( y = 0; y < g_maze_col; y++ )
		{
			fscanf( g_maze_file, "%d", &g_main_maze[x][y] );
		}
		
		//ignore when u reach the end of the line
		fscanf( g_maze_file,"\n" );
	}
	
	fclose( g_maze_file );
	
}

/*
 * based on the number, work out where to add the borders
 */
struct BorderPosition getBorderPosition( unsigned int value )
{
	//initialize the BorderPosition struct
	struct BorderPosition position = { 0, 0, 0, 0 };
	
	unsigned int remaining_value = value;
	
	//keep subtracting and add the borders as we go on
	if ( remaining_value >= 8 )
	{
		position.top = 1;
		remaining_value -= 8;
	}
	if ( remaining_value >= 4 )
	{
		position.left = 1;
		remaining_value -= 4;
	}
	if ( remaining_value >= 2 )
	{
		position.bot = 1;
		remaining_value -= 2;
	}
	if ( remaining_value >= 1 )
	{
		position.right = 1;
		remaining_value -= 1;
	}
	
	//if remaining_value == 0 then don't bother
	
	return position;
}

/*
 * works out the index in the maze string either top, bot, right or left
 * from the current maze_x and maze_y value in the actual maze
 */
int getMazeStringPositionIndex( unsigned int maze_x, unsigned int maze_y, char pos )
{
	unsigned int string_index;
	
	// post is always either 'u','d','r' or 'l'
	switch( pos )
	{
	case 'u':
	case 'U':
		string_index = ( 2*maze_x+1 )*( g_maze_col+1 )*2+( 2*maze_y )+1;
		break;
	case 'd':
	case 'D':
		string_index = ( 2*maze_x+2 )*( g_maze_col+1 )*2+( 2*maze_y )+1;
		break;
	case 'r':
	case 'R':
		string_index = ( 2*maze_x+1 )*( g_maze_col+1 )*2+( 2*maze_y )+2;
		break;
	default: // default when pos = 'l' or left
		string_index = ( 2*maze_x+1 )*( g_maze_col+1 )*2+( 2*maze_y );
		break;
	}
			
	return string_index;
}

/*
 * Create a basic maze with all walls closed using the global
 * dimension of the maze.
 */
void createMazeWithAllBordersClosed( char *maze_string )
{
	unsigned int x, y; //loop counters - corresponds to maze x,y coordinates
	
	//start by creating maze with all walls closed
	for ( x = 0; x < g_maze_row; x++ )
	{
		for ( y = 0; y < g_maze_col; y++ )
		{
			strcat( maze_string, "+-");
		}
		strcat(maze_string, "+\n");
		
		for ( y = 0; y < g_maze_col; y++ )
		{
			strcat( maze_string, "| ");
		}
		strcat(maze_string, "|\n");
	}
	
	//this is for making the border at the end
	for ( y = 0; y < g_maze_col; y++ )
	{
		strcat( maze_string, "+-");
	}
	strcat( maze_string, "+\n");
}

/*
 * prints out the maze with lines and borders based on the maze value
 */
void displayMazeProperly()
{
	// the total possible maze string size based on the dimension
	unsigned int str_length = ( 2*g_maze_row+1 ) * ( 2*g_maze_col+1 ) * 2;
	
	// going to build the string to display the maze
	char *maze_string = (char*) malloc( str_length*sizeof( char ) );
	
	//start with an empty string
	strcpy( maze_string, "" );
	
	// create a boxed maze initially
	createMazeWithAllBordersClosed( maze_string );
	
	unsigned int x, y;
	unsigned int current_val;
	
	// index for string character position
	unsigned int str_top, str_bot, str_right, str_left;
	
	struct BorderPosition border;
	
	// now open the walls depending the value
	for ( x = 0; x < g_maze_row; x++ )
	{
		for ( y = 0; y < g_maze_col; y++ )
		{
			current_val = g_main_maze[x][y];
			border = getBorderPosition( current_val );
			
			//eliminate borders
			if ( border.top == 0 )
			{
				str_top = getMazeStringPositionIndex( x, y, 'u' );
				maze_string[str_top] = ' ';
			}
			if ( border.bot == 0 )
			{
				str_bot = getMazeStringPositionIndex( x, y, 'd' );
				maze_string[str_bot] = ' ';
			}
			if ( border.right == 0 )
			{
				str_right = getMazeStringPositionIndex( x, y, 'r' );
				maze_string[str_right] = ' ';
			}
			if ( border.left == 0 )
			{
				str_left = getMazeStringPositionIndex( x, y, 'l' );
				maze_string[str_left] = ' ';
			}
		}
	}
	
	// add start and end marks in the maze string as well
	unsigned int start_mark, end_mark;
	
	start_mark = getMazeStringPositionIndex( g_start_x, g_start_y, 'r' ) - 1;
	end_mark = getMazeStringPositionIndex( g_end_x, g_end_y, 'r' ) - 1;
	
	maze_string[start_mark] = 'S';
	maze_string[end_mark] = 'E';
	
	//print (display) the maze
	printf( "\n%s", maze_string );
	free( maze_string );
}

/*
 * Sets the cell value to 15 and modifies the adjacent cell affected
 * by the change accordingly.
 */
void mazeCellWallCloser( unsigned int maze_x, unsigned int maze_y, unsigned int maze_value )
{
	// all wall closed means the value is 15
	g_main_maze[maze_x][maze_y] = 15;
	
	// now modify the adjacent cell affect by this change
	switch( maze_value ) // maze_value can only be 14, 13, 11 or 7
	{
	case 14:
		//adjust the cell on the right
		g_main_maze[maze_x][maze_y+1] = g_main_maze[maze_x][maze_y+1] + 4;
		break;
	
	case 13:
		//adjust the cell on the bottom
		g_main_maze[maze_x+1][maze_y] = g_main_maze[maze_x+1][maze_y] + 8;
		break;
	
	case 11:
		//adjust the cell on the left
		g_main_maze[maze_x][maze_y-1] = g_main_maze[maze_x][maze_y-1] + 1;
		break;
		
	default: //default 7
		//adjust the cell on the top
		g_main_maze[maze_x-1][maze_y] = g_main_maze[maze_x-1][maze_y] + 2;
		break;
	}
}

/*
 * Procedure is to close all maze cells that have three walls walls - in order words
 * the cells are useless so close them.
 */
void mazeSimplifier()
{
	unsigned int x, y, current_val, check_again;
	
	g_blocked_cells = 0;
	do
	{
		check_again = 0; //set the initial loop control value
		
		for ( x = 0; x < g_maze_row; x++ )
		{
			for ( y = 0; y < g_maze_col; y++ )
			{
				//ignore if the coordinates are start or end points
				if (( x == g_start_x && y == g_start_y )
					| ( x == g_end_x && y == g_end_y ))
				{
					continue;
				}
				else if (( g_main_maze[x][y] == 7 ) | ( g_main_maze[x][y] == 11 )
					| ( g_main_maze[x][y] == 13 ) | ( g_main_maze[x][y] == 14 ))
				{
					//increment the count of the cells that were blocked
					g_blocked_cells++;
					
					current_val = g_main_maze[x][y];
					mazeCellWallCloser( x, y, current_val );
					
					// modify loop variable - need to check again
					check_again = 1;
				}
			}
		}

	} while ( check_again != 0 ); // until no further walls were closed
	
}

/*
 * Uses the current maze value to check if there is a wall in the specified
 * direction or not. If there is no wall, it returns '1' (true) else 0 (false)
 */
int checkFreeDirection( unsigned int x, unsigned y, char direction )
{
	/* NOTE: 7,11,13,14 have three walls
	 * Ignoring these numbers because they would have been converted to 15
	 * by the mazeSimplifier procedure.
	 *
	 * so 7,11,13,14 is not possible
	 */
	
	//check if it is possible to go TOP direction
	int current_val = g_main_maze[x][y];
	
	switch( direction )
	{
	case 'u':
	case 'U':
		if ( current_val <= 7 )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	
	case 'd':
	case 'D':
		if ( ( current_val != 2 ) && ( current_val != 3 ) && ( current_val != 6 )
			&& ( current_val != 7 ) && ( current_val != 10 ) && ( current_val != 14 ) )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	
	case 'r':
	case 'R':
		if ( ( current_val != 1 ) && ( current_val != 3 ) && ( current_val != 5 )
			&& ( current_val != 7 ) && ( current_val != 9 ) && ( current_val != 11 )
			&& ( current_val != 13 ) )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	
	default: //case 'l':
		if ( ( current_val >= 4 ) && ( current_val <= 7 ) && ( current_val >= 12 )
			&& ( current_val <= 14 ) )
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
}

/*
 * Recursively works out the path to the end location from the start location.
 * And then writes it to the solution file
 */
void mazeStepFinder( unsigned int x, unsigned y, unsigned int step_count, char *sol_path,
	char *sol_file_name )
{
	// condition to end the recursion when it reaches 
	if ( ( x == g_end_x ) && ( y == g_end_y ) )
	{	
		printf( "\nMaze solved!\n" );
	
		// write the solution to the file
		g_output_file = fopen( sol_file_name, "w" );
		fprintf( g_output_file, "%d\n%s", step_count, sol_path );
 
		// close the file
		fclose( g_output_file );
		
		return;
	}
	
	// 'prev_step' is used to avoid going back-and-forth movement
	char prev_step = sol_path[strlen( sol_path )-1];
	
	// if possible to go top
	if ( ( checkFreeDirection( x,y,'u' ) == 1)
		&& ( checkFreeDirection( x-1,y,'d' ) == 1)
		&& ( prev_step != 'D' ) )
	{
		strcat( sol_path, "U" );
		mazeStepFinder( x-1, y, step_count+1, sol_path, sol_file_name );
	}
	
	// if possible to go bot
	else if ( ( checkFreeDirection( x,y,'d' ) == 1)
		&& ( checkFreeDirection( x+1,y,'u' ) == 1)
		&& ( prev_step != 'U' ) )
	{
		strcat( sol_path, "D" );
		mazeStepFinder( x+1, y, step_count+1, sol_path, sol_file_name );
	}
	
	// if possible to go right
	else if ( ( checkFreeDirection( x,y,'r' ) == 1)
		&& ( checkFreeDirection( x,y+1,'l' ) == 1)
		&& ( prev_step != 'L' ) )
	{
		strcat( sol_path, "R" );
		mazeStepFinder( x, y+1, step_count+1, sol_path, sol_file_name );
	}
	
	// if possible to go left
	else if ( ( checkFreeDirection( x,y,'l' ) == 1)
		&& ( checkFreeDirection( x,y-1,'r' ) == 1)
		&& ( prev_step != 'R' ) )
	{
		strcat( sol_path, "L" );
		mazeStepFinder( x, y-1, step_count+1, sol_path, sol_file_name );
	}
}

/*
 * Solve the maze.
 */
void solveTheMaze( char *sol_file_name )
{
	mazeSimplifier();
	printf( "\nMaze simplified!" );
	
	//allocate the maximum possible path of required to go from start to end
	unsigned int opened_cell = ( g_maze_row*g_maze_col ) - g_blocked_cells;
	
	char *path_string = (char*) malloc( opened_cell * sizeof( char ) );
	
	//start with an empty string
	strcpy( path_string, "" );
	
	mazeStepFinder( g_start_x, g_start_y, 0, path_string, sol_file_name );
	
	free( path_string );
}

/*
 * main method for running the program
 */
int main( int argc, char *argv[] )
{
	int x;
	int show_maze = 0;

	// allocate maximum of 50 length string for file names
	char maze_file_name[50] = {0};
	char sol_file_name[50] = {0};
	
	// work out the file names of the maze from arguments
	for( x = 1; x < argc; x++)
    {
		// check whether there is a flag to display maze or not
        if ( !strcmp( argv[x], "-d" ) )
        {
            show_maze = 1;
        }
		else
		{
			if ( !strlen( maze_file_name ) )
			{
				strcpy( maze_file_name, argv[x] );
			}
            else if ( !strlen( sol_file_name ) )
			{
				strcpy( sol_file_name, argv[x] );
			}
        }
    }
	
	// if for some reason, there user didn't specify file name correctly
	if ( !strlen( maze_file_name ) | !strlen( sol_file_name ) )
    {
		printf( "\nError in argument input\n" );
        printf( "mazesolver [-d] mazeFileName solutionFileName"  );
		exit(1);
	}
	
	// reading the maze from the file
	readMazeFromFile( maze_file_name );
	
	if ( show_maze == 1 )
	{
		printf( "\nDisplaying maze:" );
		displayMazeProperly();
	}
	
	// this is the procedure that solves the maze
	solveTheMaze( sol_file_name );
    
    if ( show_maze == 1 )
    {
        printf( "\nDisplaying maze after simplifying:" );
        displayMazeProperly();
    }
    
	// free the  maze array on each row
	for ( x = 0; x < g_maze_row; x++ )
	{
		free( g_main_maze[x] );
	}
	free( g_main_maze );
	
	// end the program nicely
	exit(0);
}
