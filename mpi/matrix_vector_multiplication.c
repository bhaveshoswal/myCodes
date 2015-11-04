#include<stdio.h>
#include<stdlib.h>
#include "mpi.h"

int main( int argc, char* argv[] )
{
 int numpro, rank, row = 0, column = 0, i, j, k, *work, row_recv = 0, *displs, *send_count;
 
 double *A, *A_part;

 MPI_Init( &argc, &argv );
 
 MPI_Comm_size( MPI_COMM_WORLD, &numpro );
 MPI_Comm_rank( MPI_COMM_WORLD, &rank );

 if( 0 == rank )
 {
  FILE *fp;
  char c; 
  fp = fopen( "mat.txt", "r" );

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
 
 }
 
 MPI_Bcast( &column, 1, MPI_INT, 0, MPI_COMM_WORLD );
 
 double x[column]; /* Allocate the memory for the vector */

 if( rank == 0 )
 {
  FILE *fp;
  k = 0; char c;
  fp = fopen( "vect.txt", "r" );

  while( ( c = fgetc( fp ) ) != EOF ) if( c == ' ' ) k++; k++; 
  
  if( column != k ) { printf("\nError : VECTOR HAS LESS ELEMENTS THAN COLUMNS OF MATRIX\n\n"); exit( 0 ); }

  rewind( fp );

  while( !feof( fp ) ) /* Storing the vector elememts in x */
  {
   for ( i = 0; i < column; i++ )
    if( fscanf( fp, "%lf", &x[i] ) != 0 ) {}
  }

  fclose( fp );  

  work = (int*) calloc( numpro, sizeof( int ) ); /* work is a vector which contains how many rows each process will get */

  int quotient = (double) row / numpro;
  int remainder = row % numpro;

  for( i = 0; i < numpro; i++ ) work[i] = quotient;
  for( i = 0; i < remainder; i++ ) work[i]++;

/*  int count = 0;
  while( 1 )
  {  
   for( k = 0; k < numpro; k++ )
   {
    work[k]++;
    count++;
    if( count == row ) break;
   }
   if( count == row ) break;
  }
*/
  displs = (int*) malloc( numpro * sizeof( int ) ); /* Vector of starting Addr for each send_count */
  send_count = (int*) malloc( numpro * sizeof( int ) ); /* Vector of number of elements of process according to the work vector */

  int offset = 0;
  for( i = 0; i < numpro; i++ ) 
  {
   displs[i] = offset;
   send_count[i] = work[i] * column;
   offset += send_count[i];
  }
 }

 MPI_Scatter( work, 1, MPI_INT, &row_recv, 1, MPI_INT, 0, MPI_COMM_WORLD ); /* Sacttering number of rows for each processes */

 A_part = (double*) malloc( row_recv * column * sizeof(double) );

 MPI_Scatterv( A, send_count, displs, MPI_DOUBLE, A_part, row_recv*column, MPI_DOUBLE, 0, MPI_COMM_WORLD ); /* Scatter matrix A according to number of process */

 MPI_Bcast( x, column, MPI_DOUBLE, 0, MPI_COMM_WORLD ); 

 double *total = (double*) calloc( row_recv, sizeof(double) ); /* array for each process for its addition */

 j = 0; k = 0;
 for( i = 0; i < row_recv*column; i++ ) /* multiplication & addition of vectors */
 {
  A_part[i] *= x[j];
  total[k] += A_part[i];
  j++;
  
  if( j == column ) { j = 0; k++; }
 }

 if( rank == 0 ) for( i = 1; i < numpro; i++ ) displs[i] = displs[i-1] + work[i-1];

 MPI_Gatherv( total, row_recv, MPI_DOUBLE, x, work, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD ); /* collect all parts of ans vector from all vector */

 if( 0 == rank ) 
 {
  for( i = 0; i < row; i++ )  printf( "x[%i] = %lf \n", i, x[i] );
  free( work ); free( send_count ); free( displs );
 }

 free( A_part ); free( total ); /* free all memory */

 MPI_Finalize();
 return 0;
}
