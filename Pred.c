# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <time.h>

int main ( int argc, char *argv[] );
void r8vla2_write ( char *output_filename, int m, int n, double a[m][n] );
int s_len_trim ( char *s );
int s_to_i4 ( char *s, int *last, int *error );
void timestamp ( void );

/******************************************************************************/

int main ( int argc, char *argv[] )

/******************************************************************************/

{
# define LINE_MAX_LEN 80

  double dt;
  char filename[80];
  int i;
  int ierror;
  char input[LINE_MAX_LEN];
  int length;
  double pred_init = 100.0;
  double prey_init = 5000.0;
  char *s;
  int step_num;
  double t_start;
  double t_stop;

  timestamp ( );


/*
  STEP_NUM is an input argument or else read from the user interactively.
*/
  if ( 1 < argc )
  {
    step_num = atoi ( argv[1] );
/*
  s_to_i4 ( argv[1], &length, &ierror );
*/
  }
  else
  {
    printf ( "\n" );
    printf ( "FD_PREDATOR_PREY:\n" );
    printf ( "  Please enter the number of time steps:\n" );
    scanf ( "%d", &step_num );
  }

  t_start = 0.0;
  t_stop =  5.0;
  dt = ( t_stop - t_start ) / ( double ) ( step_num );

  printf ( "\n" );
  printf ( "  Initial time    = %f\n", t_start );
  printf ( "  Final time      = %f\n", t_stop );
  printf ( "  Initial prey    = %f\n", prey_init );
  printf ( "  Initial pred    = %f\n", pred_init );
  printf ( "  Number of steps = %d\n", step_num );
/*
  Declare TRF as a VLA (Variable Length Array).
*/
  double trf[3][step_num+1];

  trf[0][0] = t_start;
  trf[1][0] = prey_init;
  trf[2][0] = pred_init;

  for ( i = 0; i < step_num; i++ )
  {
    trf[0][i+1] = trf[0][i] + dt;

    trf[1][i+1] = trf[1][i] + dt
      * (    2.0 * trf[1][i] - 0.001 * trf[1][i] * trf[2][i] );

    trf[2][i+1] = trf[2][i] + dt
      * ( - 10.0 * trf[2][i] + 0.002 * trf[1][i] * trf[2][i] );
  }
/*
  Write data to files.
*/
  sprintf ( filename, "trf_%d.txt", step_num );

  r8vla2_write ( filename, 3, step_num + 1, trf );

  printf ( "  T, R, F values written to \"%s\".\n", filename );
/*
  Terminate.
*/
  printf ( "\n" );
  printf ( "FD_PREDATOR_PREY\n" );
  printf ( "  Normal end of execution.\n" );
  printf ( "\n" );
  timestamp ( );

  return 0;
}
/******************************************************************************/

void r8vla2_write ( char *output_filename, int m, int n, double a[m][n] )

/******************************************************************************/

{
  int i;
  int j;
  FILE *output;
/*
  Open the file.
*/
  output = fopen ( output_filename, "wt" );

  if ( !output )
  {
    printf ( "\n" );
    printf ( "R8MAT_WRITE - Fatal error!\n" );
    printf ( "  Could not open the output file.\n" );
    return;
  }
/*
  Write the data.
*/
  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      fprintf ( output, "  %24.16e", a[i][j] );
    }
    fprintf ( output, "\n" );
  }
/*
  Close the file.
*/
  fclose ( output );

  return;
}
/******************************************************************************/

int s_len_trim ( char *s )

/******************************************************************************/

{
  int n;
  char *t;

  n = strlen ( s );
  t = s + strlen ( s ) - 1;

  while ( 0 < n )
  {
    if ( *t != ' ' )
    {
      return n;
    }
    t--;
    n--;
  }

  return n;
}
/******************************************************************************/

int s_to_i4 ( char *s, int *last, int *error )

/******************************************************************************/
/*

*/
{
  char c;
  int i;
  int isgn;
  int istate;
  int ival;

  *error = 0;
  istate = 0;
  isgn = 1;
  i = 0;
  ival = 0;

  while ( *s )
  {
    c = s[i];
    i = i + 1;
/*
  Haven't read anything.
*/
    if ( istate == 0 )
    {
      if ( c == ' ' )
      {
      }
      else if ( c == '-' )
      {
        istate = 1;
        isgn = -1;
      }
      else if ( c == '+' )
      {
        istate = 1;
        isgn = + 1;
      }
      else if ( '0' <= c && c <= '9' )
      {
        istate = 2;
        ival = c - '0';
      }
      else
      {
        *error = 1;
        return ival;
      }
    }
/*
  Have read the sign, expecting digits.
*/
    else if ( istate == 1 )
    {
      if ( c == ' ' )
      {
      }
      else if ( '0' <= c && c <= '9' )
      {
        istate = 2;
        ival = c - '0';
      }
      else
      {
        *error = 1;
        return ival;
      }
    }
/*
  Have read at least one digit, expecting more.
*/
    else if ( istate == 2 )
    {
      if ( '0' <= c && c <= '9' )
      {
        ival = 10 * (ival) + c - '0';
      }
      else
      {
        ival = isgn * ival;
        *last = i - 1;
        return ival;
      }
    }
  }
/*
  If we read all the characters in the string, see if we're OK.
*/
  if ( istate == 2 )
  {
    ival = isgn * ival;
    *last = s_len_trim ( s );
  }
  else
  {
    *error = 1;
    *last = 0;
  }

  return ival;
}
/******************************************************************************/

void timestamp ( void )

/******************************************************************************/

{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  size_t len;
  time_t now;

  now = time ( NULL );
  tm = localtime ( &now );

  len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  printf ( "%s\n", time_buffer );

  return;
# undef TIME_SIZE
}
