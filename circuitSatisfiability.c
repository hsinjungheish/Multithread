/* circuitSatifiability.c solves the Circuit Satisfiability
 *  Problem using a brute-force sequential solution.
 *
 *   The particular circuit being tested is "wired" into the
 *   logic of function 'checkCircuit'. All combinations of
 *   inputs that satisfy the circuit are printed.
 *
 *   16-bit version by Michael J. Quinn, Sept 2002.
 *   Extended to 32 bits by Joel C. Adams, Sept 2013.
 */
#include "mpi.h"
#include <stdio.h>     // printf()
#include <limits.h>    // UINT_MAX

int checkCircuit (int, int);

int main (int argc, char *argv[]) {
    int i;               /* loop variable (32 bits) */
    int id = 0;           /* process id */
    int count = 0;        /* number of solutions */
    int numprocs;
    int sum = 0;
    int offset, received;
	
    MPI_Init(&argc, &argv);
	  MPI_Comm_rank(MPI_COMM_WORLD,&id);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	
	  double startTime = 0.0, totalTime = 0.0; 
	  startTime = MPI_Wtime(); 
 

	  for (i = id; i < USHRT_MAX; i+=numprocs) { 
    	  count += checkCircuit(id, i); 
	  } 
     
    
    int process = numprocs;
    int c = count;
    while(process > 1)
    {
      offset = process/2 + process % 2;
      if(id < process/2)
      {
         MPI_Recv(&received, 1, MPI_INT, id+offset, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         count += received;
      }
      else if(id >= offset && id < process)
      {
         MPI_Send(&count, 1, MPI_INT, id-offset, 0, MPI_COMM_WORLD);
      }
      process = offset;
    }
   
    if(id != 0){
      totalTime = MPI_Wtime() - startTime;
      printf ("Process %d finished in time %f secs. %d solutions were found.\n", id, totalTime, c);
   	  fflush (stdout);
    }
    else{
    	totalTime = MPI_Wtime() - startTime;
      printf ("Process %d finished in time %f secs. A total of %d solutions were found.\n", id, totalTime, count);
      fflush(stdout);
    }
    
    MPI_Finalize(); 
    return 0;
}

/* EXTRACT_BIT is a macro that extracts the ith bit of number n.
 *
 * parameters: n, a number;
 *             i, the position of the bit we want to know.
 *
 * return: 1 if 'i'th bit of 'n' is 1; 0 otherwise 
 */

#define EXTRACT_BIT(n,i) ( (n & (1<<i) ) ? 1 : 0)


/* checkCircuit() checks the circuit for a given input.
 * parameters: id, the id of the process checking;
 *             bits, the (long) rep. of the input being checked.
 *
 * output: the binary rep. of bits if the circuit outputs 1
 * return: 1 if the circuit outputs 1; 0 otherwise.
 */

#define SIZE 16

int checkCircuit (int id, int bits) {
   int v[SIZE];        /* Each element is a bit of bits */
   int i;

   for (i = 0; i < SIZE; i++)
     v[i] = EXTRACT_BIT(bits,i);

   if (  (v[0] || v[1]) && (!v[1] || !v[3]) && (v[2] || v[3])
       && (!v[3] || !v[4]) && (v[4] || !v[5])
       && (v[5] || !v[6]) && (v[5] || v[6])
       && (v[6] || !v[15]) && (v[7] || !v[8])
       && (!v[7] || !v[13]) && (v[8] || v[9])
       && (v[8] || !v[9]) && (!v[9] || !v[10])
       && (v[9] || v[11]) && (v[10] || v[11])
       && (v[12] || v[13]) && (v[13] || !v[14])
       && (v[14] || v[15])  )
   {
      printf ("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n", id,
         v[15],v[14],v[13],v[12],
         v[11],v[10],v[9],v[8],v[7],v[6],v[5],v[4],v[3],v[2],v[1],v[0]);
      fflush (stdout);
      return 1;
   } else {
      return 0;
   } 
}


