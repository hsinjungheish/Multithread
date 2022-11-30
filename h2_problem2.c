#include "mpi.h" 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
   
    int n, local_num, phase;
    int comm_sz, id;
    double startwtime, endwtime;
    int *sort_ans, *temp, *sub_ans, *temp1;
    int num_temp, partner;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
	
    startwtime = MPI_Wtime();
    srand(time(NULL));

    if(id == 0){
        printf("Enter number n:");
        scanf("%d", &n);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

    local_num = n/comm_sz;
    if(id < n % comm_sz)
        local_num += 1;

    temp = (int *)malloc(sizeof(int) * local_num + 1);
    sort_ans = (int *)malloc(sizeof(int) * n);
    sub_ans = (int *)malloc(sizeof(int) * local_num);
    
    for(int i=0; i<local_num; i++)
        sub_ans[i] = rand();

    for(int i=0; i<local_num; i++)
        for(int j=0; j<local_num-i; j++)
            if(sub_ans[j]>sub_ans[j+1])
			{
                int t;
                t = sub_ans[j];
                sub_ans[j] = sub_ans[j+1];
                sub_ans[j+1] = t;
            }
        

    MPI_Barrier(MPI_COMM_WORLD);
   
    int num_of_send[comm_sz];
    int start_addr[comm_sz];
    int now_loc = 0;
    
    for(int i = 0; i < comm_sz; i++){
        num_of_send[i] = n/comm_sz;
        if(i < n % comm_sz)
            num_of_send[i]++;
        start_addr[i] = now_loc;
        now_loc += num_of_send[i];
    }


    MPI_Gatherv(sub_ans, local_num, MPI_INTEGER, sort_ans, num_of_send, start_addr, MPI_INTEGER, 0, MPI_COMM_WORLD);

    if(id == 0){
        int count = 0;
        printf("\n");
        for(int i = 0; i < comm_sz; i++){
            printf("Process %d's local sort: ", i);
            for(int j = 0; j < num_of_send[i]; j++){
                printf("%d, ", sort_ans[start_addr[i]+j]);
            }
            printf("\n");
            fflush(stdout);
            count += num_temp;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    temp1 = (int *)malloc(sizeof(int) * (local_num*2+2));
    phase = comm_sz;
    for(int i = 0; i < phase; i++){
        
        if((i % 2 == 0 && id % 2 == 1) || (i % 2 == 1 && id % 2 == 0))
        	partner = id - 1;
        else
        	partner = id + 1;
        	
        if(partner == -1 || partner == comm_sz)
            continue;

        num_temp = n / comm_sz;
        if(partner < n % comm_sz)
            num_temp++;
        if(id % 2!=0){
            MPI_Send(sub_ans, local_num, MPI_INTEGER, partner, 0, MPI_COMM_WORLD);
            MPI_Recv(temp, num_temp, MPI_INTEGER, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else{
            MPI_Recv(temp, num_temp, MPI_INTEGER, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(sub_ans, local_num, MPI_INTEGER, partner, 0, MPI_COMM_WORLD);
        }
		
		int count1 = 0, count2 = 0, count3 = 0;

        while (count3 < local_num + num_temp)
        {
            if((count2 >= num_temp) || (sub_ans[count1] < temp[count2])){
                temp1[count3] = sub_ans[count1];
                count1++; count3++;
            }
            else{
                temp1[count3] = temp[count2];
                count2++; count3++;
            }
        }

        for(int j=0; j<local_num; j++){
            if(id > partner)
                sub_ans[j] = temp1[num_temp + j];
            else
                sub_ans[j] = temp1[j];
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gatherv(sub_ans, local_num, MPI_INTEGER, sort_ans, num_of_send, start_addr, MPI_INTEGER, 0, MPI_COMM_WORLD);

    if(id == 0){
    	endwtime = MPI_Wtime() - startwtime;
        printf("Total execution time in %f secs.\n", endwtime);
        fflush(stdout);
        printf("Final answer: ");
        for(int i=0; i<n; i++){
            printf("%d, ", sort_ans[i]);
        }
        printf("\n");
        fflush(stdout);
    }

    free(sub_ans);
    free(sort_ans);
    free(temp);
    free(temp1);
    MPI_Finalize();
    
    return 0;
}
