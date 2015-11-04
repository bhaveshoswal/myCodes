#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "mpi.h"

#define MAX_ITERATION 1000
#define epsilon 1E-7

double norm( double *v , int len ) /* 2 norm calculating function */
{
 int i; double total = 0.0;
 for( i = 0; i < len; i++ ) total += pow( v[i] , 2.0 ); 
 return sqrt( total );
}

int main( int argc, char *argv [] )
{
 int numpro, rank, row = 0, column = 0, i, j, k, *work, row_recv = 0, *displs, *send_count, *b_displs, *diag, *diag_part;
 
 MPI_Init( &argc, &argv );
 MPI_Comm_size( MPI_COMM_WORLD, &numpro );
 MPI_Comm_rank( MPI_COMM_WORLD, &rank );

 double *A, *A_part, *b, *b_part, *x_part;

 if( 0 == rank )
 {
  FILE *fp;
  char c; 
  fp = fopen( "jacobi_mat.txt", "r" ); /* this file contains A|b matrix*/

  while( ( c = fgetc( fp ) ) != EOF ) /* Reading the file for number of rows & columns */
  {
   if( c == ' ' && row == 0 ) column++;
   if( c == '\n' ) row++;
  }
  column++;
 
  rewind( fp ); /* reset file pointer to start of file */

  A = (double*) malloc( row*column*sizeof(double) ); /* Allocate the memory for the matrix storage */
  
  while( !feof( fp ) ) /* Storing the elements in the A as a vector */
  {
   for ( i = 0; i < row*column; i++)
    if( fscanf( fp, "%lf", &A[i] ) != 0 ) {} 
  }
  fclose( fp );
 
  b = (double*) malloc( row * sizeof(double) );

  diag = (int*) calloc( row, sizeof(int) ); /* for storing diagona; element position */

  k = 0;
  diag[k] = k; 
  fp = fopen( "jacobi_vect.txt", "r" );

  while( ( c = fgetc( fp ) ) != EOF ) 
  {
   if( c == ' ' ) k++; 
   diag[k] = k;
  }
  k++; 
  
  if( row != k ) { printf("\nError : VECTOR HAS LESS ELEMENTS THAN COLUMNS OF MATRIX\n\n"); exit( 0 ); }

  rewind( fp );

  while( !feof( fp ) ) /* Storing the vector elememts in x */
  {
   for ( i = 0; i < row; i++ )
    if( fscanf( fp, "%lf", &b[i] ) != 0 ) {}
  }
  fclose( fp );  

  work = (int*) calloc( numpro, sizeof( int ) ); /* work is a vector which contains how many rows each process will get */

  int quotient = (double) row / numpro;
  int remainder = row % numpro;

  for( i = 0; i < numpro; i++ ) work[i] = quotient;
  for( i = 0; i < remainder; i++ ) work[i]++;

  displs = (int*) malloc( numpro * sizeof( int ) ); /* Vector of starting Addr for each send_count */
  send_count = (int*) malloc( numpro * sizeof( int ) ); /* Vector of number of elements of process according to the work vector */

  int offset = 0;
  for( i = 0; i < numpro; i++ ) 
  {
   displs[i] = offset;
   send_count[i] = work[i] * column;
   offset += send_count[i];
  }
  
  b_displs = (int*) calloc( numpro, sizeof( int ) );
  for( i = 1; i < numpro; i++ ) b_displs[i] = b_displs[i-1] + work[i-1]; 
 }

 if( rank != 0 ) 
 {
  work = (int*) calloc( numpro, sizeof( int ) );
  b_displs = (int*) calloc( numpro, sizeof( int ) );
 }
 MPI_Bcast( work, numpro, MPI_INT, 0, MPI_COMM_WORLD );
 MPI_Bcast( b_displs, numpro, MPI_INT, 0, MPI_COMM_WORLD );

 MPI_Bcast( &column, 1, MPI_INT, 0, MPI_COMM_WORLD );
 MPI_Bcast( &row, 1, MPI_INT, 0, MPI_COMM_WORLD );

 double *x = (double*) calloc( row, sizeof(double) ); /* Initial Geuss of x as all zeros */

 MPI_Scatter( work, 1, MPI_INT, &row_recv, 1, MPI_INT, 0, MPI_COMM_WORLD ); /* Sacttering number of rows for each processes */

 A_part = (double*) malloc( row_recv * column * sizeof(double) );
 b_part = (double*) malloc( row_recv * sizeof(double) );
 x_part = (double*) malloc( row_recv * sizeof(double) ); 
 diag_part = (int*) malloc( row_recv * sizeof(int) );

 MPI_Scatterv( b, work, b_displs, MPI_DOUBLE, b_part, row_recv, MPI_DOUBLE, 0, MPI_COMM_WORLD ); 

 MPI_Scatterv( diag, work, b_displs, MPI_INT, diag_part, row_recv, MPI_DOUBLE, 0, MPI_COMM_WORLD );

 MPI_Scatterv( A, send_count, displs, MPI_DOUBLE, A_part, row_recv*column, MPI_DOUBLE, 0, MPI_COMM_WORLD ); /* Scatter matrix A according to number of process */

 double f = 1.0;
 int iteration = 0;
 double *x_new = (double*) calloc( row, sizeof(double) ); /* calculated x geuss */

 do
 {
  iteration++;
  /* Jacobi Algorithm */
  double local_sum = 0.0;
  for( i = 0; i < row_recv; i++ )
  {
   for( j = 0; j < diag_part[i]; j++ ) local_sum += A_part[ i * row + j ] * x[j];

   for( j = diag_part[i] + 1; j < column; j++ ) local_sum += A_part[ i * row + j ] * x[j];

   x_part[i] = ( b_part[i] - local_sum ) / (double) A_part[ i * row + diag_part[i] ];
 
   local_sum = 0.0;
  }
  /*--------------------*/
  
  MPI_Allgatherv( x_part, row_recv, MPI_DOUBLE, x_new, work, b_displs, MPI_DOUBLE, MPI_COMM_WORLD ); 

  if( 0 == rank ) f = fabs( norm( x_new, row ) - norm( x, row ) );  /* 2 Norm for evaluation measure */
 
  MPI_Bcast( &f, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD );

  for( i = 0; i < row; i++ ) x[i] = x_new[i]; /* updating old with new calculated value of x */

 }while( iteration > MAX_ITERATION || f > epsilon );
 
 if( 0 == rank )
 {
  if( iteration == MAX_ITERATION ) printf( "\n\nWarning: Result not Found Max Iteration Limit Reached\n\n");
  printf( "\nx = " );
  for( i = 0; i < row; i++ ) printf("%lf  ", x[i] );
  printf( "\nIteration = %i\n\n", iteration );
  free( A ); free( b ); free( displs ); free( send_count ); free( diag );
 }

 free( x_new ); free( x ); free( x_part ); free( b_part ); free( work ); free( b_displs );
 free( A_part ); free( diag_part );

 MPI_Finalize();
 return 0;
}
