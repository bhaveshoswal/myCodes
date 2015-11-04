/* By Cannon Algorithmn */

#include<stdio.h>
#include<stdlib.h>
#include "mpi.h"

void Print( double* X , int dim ) /* function to print matrix */
{
 int i, cnt = 0;
 for( i = 0; i < dim * dim; i++ )
 {
  cnt++;
  printf("\t%lf", X[i] );
  if( cnt == dim ) { printf("\n"); cnt = 0; }
 }
 printf("\n");
}

int main( int argc, char* argv[] )
{
 int numpro, rank, rowA = 0, columnA = 0, rowB = 0, columnB = 0, i;
 
 double *A, *B, *C, a, b, c;

 MPI_Init( &argc, &argv );
 
 MPI_Comm_size( MPI_COMM_WORLD, &numpro );
 MPI_Comm_rank( MPI_COMM_WORLD, &rank );

 if( 0 == rank )
 {
  FILE *fp;
  char c; 
  fp = fopen( "matA.txt", "r" );

  while( ( c = fgetc( fp ) ) != EOF ) /* Reading the file for number of rows & columns */
  {
   if( c == ' ' && rowA == 0 ) columnA++;
   if( c == '\n' ) rowA++;
  }
  columnA++;
 
  rewind( fp ); /* reset file pointer to start of file */

  A = (double*) malloc( rowA*columnA*sizeof(double) ); /* Allocate the memory for the matrix storage */
  
  while( !feof( fp ) ) /* Storing the elements in the A as a vector */
  {
   for ( i = 0; i < rowA*columnA; i++)
    if( fscanf( fp, "%lf", &A[i] ) != 0 ) {} 
  }
  fclose( fp );

  fp = fopen( "matB.txt", "r" );

  while( ( c = fgetc( fp ) ) != EOF ) /* Reading the file for number of rows & columns */
  {
   if( c == ' ' && rowB == 0 ) columnB++;
   if( c == '\n' ) rowB++;
  }
  columnB++;

  rewind( fp ); /* reset file pointer to start of file */

  B = (double*) malloc( rowB*columnB*sizeof(double) ); /* Allocate the memory for the matrix storage */

  while( !feof( fp ) ) /* Storing the elements in the A as a vector */
  {
   for ( i = 0; i < rowB*columnB; i++)
    if( fscanf( fp, "%lf", &B[i] ) != 0 ) {}
  }
  fclose( fp );

  if( columnA != rowB ) { printf("\nError: Dimensions does not match for Multi[lication\nTerminating Bye...\n\n"); exit( 0 ); }

  if( columnA != rowA  || columnB != rowB || columnA != columnB || rowA != rowB ) { printf("\nError: Both Matrix must be SQUARE & same Size\nTerminating Bye...\n\n"); exit( 0 ); }
 
 if( numpro != rowA * rowA ) { printf("\nError: Total processes should be Square of Dimension of Matrix\nTerminating Bye...\n\n"); exit( 0 ); }
 
  C = (double*) calloc( rowA * columnB, sizeof(double) ); /* Allocate the memory for result matrix */
 }
 
 MPI_Scatter( A, 1, MPI_DOUBLE, &a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD ); /* Sacttering elements of A to each processes */
 MPI_Scatter( B, 1, MPI_DOUBLE, &b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD ); /* Sacttering elements of B to each processes */

 MPI_Bcast( &rowA, 1, MPI_INT, 0, MPI_COMM_WORLD );

 int dims[] = { rowA, rowA }; /* Dimsions for Cartesian Topology */
 int periods[] = { 1, 1 }; /* 1 = for circular shifting */
 int coords[2]; /* 2 Dimension topology so 2 coordinates */
 int right_rank, left_rank, down_rank, up_rank;

 MPI_Comm cartcomm;

 MPI_Cart_create( MPI_COMM_WORLD, 2, dims, periods, 1, &cartcomm );

 MPI_Comm_rank( cartcomm, &rank );

 MPI_Cart_coords( cartcomm, rank, 2, coords );

 /* Initial Alignment */
 int srcA, destA, srcB, destB;

 MPI_Cart_shift( cartcomm, 1, (int) (float) rank / rowA, &destA, &srcA ); /* 1 = rowwise shift */
 MPI_Cart_shift( cartcomm, 0, rank % rowA, &destB, &srcB ); /* 0 = columnwise shift */

// int nlocal = (float) rowA / dims[0]

// MPI_Cart_shift( cartcomm, 0, -coords[0], &shift_src, &shift_dest );
 /* Initial Shift */
// for( i = 0; i < sqrt_numpro; i++ ) {
//  for( j = 0; j < sqrt_numpro; j++ )
//  {
//   int src = i * sqrt_numpro + ( j - i )%sqrt_numpro;
//   int dest = i * sqrt_numpro + j;
//   if( i != 0 ){
//   MPI_Sendrecv_replace( &a, 1, MPI_DOUBLE, src, 99, dest, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
//   printf( "rank = %i ", rank ); }
//  }
 // printf("\n");
// }

 if( rank >= rowA )
  MPI_Sendrecv_replace( &a, 1, MPI_DOUBLE, destA, 99, srcA, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

 if( ( rank % rowA ) != 0 )
  MPI_Sendrecv_replace( &b, 1, MPI_DOUBLE, destB, 99, srcB, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
 /* Initial sfift done here */

 /* Start Cannon */
 c += a * b;
 for( i = 1 ; i < rowA ; i++ ) 
 {
  MPI_Cart_shift( cartcomm, 1, 1, &destA, &srcA ); /* one left shift */
  MPI_Cart_shift( cartcomm, 0, 1, &destB, &srcB ); /* one up shift */

  MPI_Sendrecv_replace( &a, 1, MPI_DOUBLE, destA, 99, srcA, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

  MPI_Sendrecv_replace( &b, 1, MPI_DOUBLE, destB, 99, srcB, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
  
  c += a * b; 
 }
 /* Cannon End */

 MPI_Gather( &c, 1, MPI_DOUBLE, C, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD ); /* gathering elements from all processes*/

 if( 0 == rank ) 
 {
  printf("\nMatrix A ->\n");  Print( A, rowA );
  printf("\nMatrix B ->\n");  Print( B, rowA );
  printf("\nMatrix A*B ->\n");  Print( C, rowA );
 
  free( C ); free( B ); free( A ); 
 }

 MPI_Finalize();
 return 0;
}
