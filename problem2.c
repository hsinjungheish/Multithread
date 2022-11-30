#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>

int main(){
    int process_rank;
    int total_process;
    int remain, offset, toss;

    double x, y, distance_squard;
    double pi;
    double starttime, totaltime;
    long long int number_in_circle, number_of_tosses;
    number_of_tosses = 1000000;

    srand(time(NULL));

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &total_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    starttime = MPI_Wtime();
    number_in_circle = 0;

    if(process_rank == 0){
        printf("number of tosses is: %lld\n", number_of_tosses);
    }
    
    for(toss = process_rank; toss < number_of_tosses; toss += total_process){
        x = (double)rand()/RAND_MAX*2 - 1;
        y = (double)rand()/RAND_MAX*2 - 1;
        distance_squard = x*x +y*y;
        if(distance_squard <= 1){
            number_in_circle ++;
        }
    }

    remain = total_process;
    while(remain != 1){
        long long int send_count;
        offset = remain/2 + remain % 2;
        if(process_rank<remain/2){
            MPI_Recv(&send_count, 1, MPI_LONG_LONG_INT, process_rank + offset, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            number_in_circle += send_count;
        }
        else if(process_rank >= offset && process_rank < remain){
            MPI_Send(&number_in_circle, 1, MPI_LONG_LONG_INT, process_rank - offset, 0, MPI_COMM_WORLD);
        break;
        }
        remain = offset;
    }

    if(process_rank != 0){
        printf ("Process %d finished.\n", process_rank);
    	fflush (stdout);
    }
    else{
        pi = 4 * (double)number_in_circle / (double)number_of_tosses;
        totaltime = MPI_Wtime() - starttime;
        printf("Process 0 finished in %f secs.\n", totaltime);
        fflush(stdout);
        printf("PI: %.16f\n", pi);
    }

    MPI_Finalize();
    return 0;
}
