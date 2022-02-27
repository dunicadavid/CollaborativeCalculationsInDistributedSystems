#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int numtasks, rank, size_0, size_1, size_2, error;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int *tasks_0, *tasks_1, *tasks_2, *v_task_part;

    error = atoi(argv[2]);

    if (rank == 0)
    {
        FILE *file = fopen("cluster0.txt", "r");
        fscanf(file, "%d", &size_0);
        tasks_0 = (int *)calloc(sizeof(int), size_0);

        for (int i = 0; i < size_0; i++)
        {
            fscanf(file, "%d", &tasks_0[i]);
            MPI_Send(&rank, 1, MPI_INT, tasks_0[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, tasks_0[i]);
        }
        fclose(file);

        int mess;
        for (int i = 0; i < size_0; i++)
        {
            MPI_Recv(&mess, 1, MPI_INT, tasks_0[i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }

        if (error == 0)
        {
            // trimite spre masteri
            MPI_Send(&size_0, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
            for (int i = 0; i < size_0; i++)
            {
                MPI_Send(&tasks_0[i], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            }

            MPI_Send(&size_0, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
            for (int i = 0; i < size_0; i++)
            {
                MPI_Send(&tasks_0[i], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            }

            // primeste de la masteri
            MPI_Recv(&size_1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
            tasks_1 = (int *)calloc(sizeof(int), size_1);
            for (int i = 0; i < size_1; i++)
            {
                MPI_Recv(&tasks_1[i], 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
            }
            MPI_Recv(&size_2, 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
            tasks_2 = (int *)calloc(sizeof(int), size_2);
            for (int i = 0; i < size_2; i++)
            {
                MPI_Recv(&tasks_2[i], 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
            }
        }
        else
        {
            // trimite spre masteri
            MPI_Send(&size_0, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
            for (int i = 0; i < size_0; i++)
            {
                MPI_Send(&tasks_0[i], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            }

            // primeste de la masteri
            MPI_Recv(&size_2, 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
            tasks_2 = (int *)calloc(sizeof(int), size_2);
            for (int i = 0; i < size_2; i++)
            {
                MPI_Recv(&tasks_2[i], 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
            }

            MPI_Recv(&size_1, 1, MPI_INT, 2, 3, MPI_COMM_WORLD, &status);
            tasks_1 = (int *)calloc(sizeof(int), size_1);
            for (int i = 0; i < size_1; i++)
            {
                MPI_Recv(&tasks_1[i], 1, MPI_INT, 2, 3, MPI_COMM_WORLD, &status);
            }
        }

        for (int i = 0; i < size_0; i++)
        {
            MPI_Send(&size_0, 1, MPI_INT, tasks_0[i], 0, MPI_COMM_WORLD);
            MPI_Send(&size_1, 1, MPI_INT, tasks_0[i], 0, MPI_COMM_WORLD);
            MPI_Send(&size_2, 1, MPI_INT, tasks_0[i], 0, MPI_COMM_WORLD);
            MPI_Send(tasks_0, size_0, MPI_INT, tasks_0[i], 0, MPI_COMM_WORLD);
            MPI_Send(tasks_1, size_1, MPI_INT, tasks_0[i], 0, MPI_COMM_WORLD);
            MPI_Send(tasks_2, size_2, MPI_INT, tasks_0[i], 0, MPI_COMM_WORLD);
        }

        // cerinta 2
        // initializare vector
        int N, CN;
        N = atoi(argv[1]);
        int *v;
        v = (int *)calloc(sizeof(int), N);
        for (int i = 0; i < N; i++)
            v[i] = i;

        // creare marime vectori pentru masteri

        int v_size_0, v_size_1, v_size_2;
        v_size_0 = N / (size_0 + size_1 + size_2) * size_0;
        v_size_1 = N / (size_0 + size_1 + size_2) * size_1;
        v_size_2 = N / (size_0 + size_1 + size_2) * size_2;

        CN = N % (size_0 + size_1 + size_2);

        if (CN >= size_0)
        {
            v_size_0 += size_0;
            CN -= size_0;
        }
        else
        {
            v_size_0 += CN;
            CN -= CN;
        }
        if (CN >= size_1)
        {
            v_size_1 += size_1;
            CN -= size_1;
        }
        else
        {
            v_size_1 += CN;
            CN -= CN;
        }
        if (CN >= size_2)
        {
            v_size_2 += size_2;
            CN -= size_2;
        }
        else
        {
            v_size_2 += CN;
            CN -= CN;
        }

        // pregatire vectorul asociat pentru masterul 0
        v_task_part = (int *)calloc(sizeof(int), v_size_0);
        for (int i = 0; i < v_size_0; i++)
            v_task_part[i] = v[i];

        // trimite vectorul asociat spre ceilalti masteri
        if (error == 0)
        {
            MPI_Send(&v_size_1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
            for (int i = 0; i < v_size_1; i++)
            {
                MPI_Send(&v[i + v_size_0], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, 1);
            }
            MPI_Send(&v_size_2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
            for (int i = 0; i < v_size_2; i++)
            {
                MPI_Send(&v[i + v_size_0 + v_size_1], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, 2);
            }
        }
        else
        {
            MPI_Send(&v_size_1, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
            for (int i = 0; i < v_size_1; i++)
            {
                MPI_Send(&v[i + v_size_0], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, 2);
            }
            MPI_Send(&v_size_2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
            for (int i = 0; i < v_size_2; i++)
            {
                MPI_Send(&v[i + v_size_0 + v_size_1], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, 2);
            }
        }
        // trimite la taskuri partile care trebuie calculate
        int send_size, index_trimitere = 0;
        int rest_impartire_v = v_size_0 % size_0;
        for (int i = 0; i < size_0; i++)
        {
            send_size = v_size_0 / size_0;
            if (rest_impartire_v)
            {
                send_size++;
                rest_impartire_v--;
            }
            MPI_Send(&send_size, 1, MPI_INT, tasks_0[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, tasks_0[i]);
            for (int j = 0; j < send_size; j++)
            {
                MPI_Send(&v_task_part[index_trimitere + j], 1, MPI_INT, tasks_0[i], 0, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, tasks_0[i]);
            }
            for (int j = 0; j < send_size; j++)
                MPI_Recv(&v_task_part[index_trimitere + j], 1, MPI_INT, tasks_0[i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            index_trimitere += send_size;
        }

        // primeste rezultate de la ceilalti masteri si completez vectorul initial + afisare

        for (int i = 0; i < v_size_0; i++)
        {
            v[i] = v_task_part[i];
        }

        if (error == 0)
        {
            for (int i = 0; i < v_size_1; i++)
            {
                MPI_Recv(&v[i + v_size_0], 1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            }
        }
        else
        {
            for (int i = 0; i < v_size_1; i++)
            {
                MPI_Recv(&v[i + v_size_0], 1, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            }
        }
        for (int i = 0; i < v_size_2; i++)
        {
            MPI_Recv(&v[i + v_size_0 + v_size_1], 1, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }

        // afisare vector
        printf("Rezultat: ");
        for (int i = 0; i < N; i++)
            printf("%d ", v[i]);
        printf("\n");
    }
    else if (rank == 1)
    {
        FILE *file = fopen("cluster1.txt", "r");
        fscanf(file, "%d", &size_1);
        tasks_1 = (int *)calloc(sizeof(int), size_1);

        for (int i = 0; i < size_1; i++)
        {
            fscanf(file, "%d", &tasks_1[i]);
            MPI_Send(&rank, 1, MPI_INT, tasks_1[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, tasks_1[i]);
        }
        fclose(file);

        int mess;
        for (int i = 0; i < size_1; i++)
        {
            MPI_Recv(&mess, 1, MPI_INT, tasks_1[i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }

        if (error == 0)
        {
            // trimite spre masteri
            MPI_Send(&size_1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 0);
            for (int i = 0; i < size_1; i++)
            {
                MPI_Send(&tasks_1[i], 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            }

            MPI_Send(&size_1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
            for (int i = 0; i < size_1; i++)
            {
                MPI_Send(&tasks_1[i], 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
            }

            // primeste de la masteri
            MPI_Recv(&size_0, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            tasks_0 = (int *)calloc(sizeof(int), size_0);
            for (int i = 0; i < size_0; i++)
            {
                MPI_Recv(&tasks_0[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            }
            MPI_Recv(&size_2, 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
            tasks_2 = (int *)calloc(sizeof(int), size_2);
            for (int i = 0; i < size_2; i++)
            {
                MPI_Recv(&tasks_2[i], 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
            }
        }
        else
        {
            // trimite spre masteri
            MPI_Send(&size_1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 2);
            for (int i = 0; i < size_1; i++)
            {
                MPI_Send(&tasks_1[i], 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
            }

            // primeste de la masteri
            MPI_Recv(&size_2, 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
            tasks_2 = (int *)calloc(sizeof(int), size_2);
            for (int i = 0; i < size_2; i++)
            {
                MPI_Recv(&tasks_2[i], 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
            }
            MPI_Recv(&size_0, 1, MPI_INT, 2, 3, MPI_COMM_WORLD, &status);
            tasks_0 = (int *)calloc(sizeof(int), size_0);
            for (int i = 0; i < size_0; i++)
            {
                MPI_Recv(&tasks_0[i], 1, MPI_INT, 2, 3, MPI_COMM_WORLD, &status);
            }
        }

        // trimite tipologia tuturor
        for (int i = 0; i < size_1; i++)
        {
            MPI_Send(&size_0, 1, MPI_INT, tasks_1[i], 1, MPI_COMM_WORLD);
            MPI_Send(&size_1, 1, MPI_INT, tasks_1[i], 1, MPI_COMM_WORLD);
            MPI_Send(&size_2, 1, MPI_INT, tasks_1[i], 1, MPI_COMM_WORLD);
            MPI_Send(tasks_0, size_0, MPI_INT, tasks_1[i], 1, MPI_COMM_WORLD);
            MPI_Send(tasks_1, size_1, MPI_INT, tasks_1[i], 1, MPI_COMM_WORLD);
            MPI_Send(tasks_2, size_2, MPI_INT, tasks_1[i], 1, MPI_COMM_WORLD);
        }

        // cerinta 2
        //  primeste de la master 0 partea de vector asociata
        int v_size_1;

        if (error == 0)
        {
            MPI_Recv(&v_size_1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            v_task_part = (int *)calloc(sizeof(int), v_size_1);
            for (int i = 0; i < v_size_1; i++)
                MPI_Recv(&v_task_part[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }
        else
        {
            MPI_Recv(&v_size_1, 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
            v_task_part = (int *)calloc(sizeof(int), v_size_1);
            for (int i = 0; i < v_size_1; i++)
                MPI_Recv(&v_task_part[i], 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
        }

        // trimite la taskuri partile care trebuie calculate
        int send_size, index_trimitere = 0;
        int rest_impartire_v = v_size_1 % size_1;
        for (int i = 0; i < size_1; i++)
        {
            send_size = v_size_1 / size_1;
            if (rest_impartire_v)
            {
                send_size++;
                rest_impartire_v--;
            }
            MPI_Send(&send_size, 1, MPI_INT, tasks_1[i], 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, tasks_1[i]);
            for (int j = 0; j < send_size; j++)
            {
                MPI_Send(&v_task_part[index_trimitere + j], 1, MPI_INT, tasks_1[i], 1, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, tasks_1[i]);
            }
            for (int j = 0; j < send_size; j++)
                MPI_Recv(&v_task_part[index_trimitere + j], 1, MPI_INT, tasks_1[i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            index_trimitere += send_size;
        }

        // trimit spre task 0 rezultatul
        if (error == 0)
            for (int i = 0; i < v_size_1; i++)
            {
                MPI_Send(&v_task_part[i], 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            }
        else
        {
            MPI_Send(&v_size_1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
            for (int i = 0; i < v_size_1; i++)
            {
                MPI_Send(&v_task_part[i], 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
            }
        }
    }
    else if (rank == 2)
    {
        FILE *file = fopen("cluster2.txt", "r");
        fscanf(file, "%d", &size_2);
        tasks_2 = (int *)calloc(sizeof(int), size_2);

        // trimite si primeste de la taskuri
        for (int i = 0; i < size_2; i++)
        {
            fscanf(file, "%d", &tasks_2[i]);
            MPI_Send(&rank, 1, MPI_INT, tasks_2[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, tasks_2[i]);
        }
        fclose(file);

        int mess;
        for (int i = 0; i < size_2; i++)
        {
            MPI_Recv(&mess, 1, MPI_INT, tasks_2[i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }

        if (error == 0)
        {
            // trimite spre masteri
            MPI_Send(&size_2, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 0);
            for (int i = 0; i < size_2; i++)
            {
                MPI_Send(&tasks_2[i], 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            }

            MPI_Send(&size_2, 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
            for (int i = 0; i < size_2; i++)
            {
                MPI_Send(&tasks_2[i], 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
            }

            // primeste de la masteri
            MPI_Recv(&size_0, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            tasks_0 = (int *)calloc(sizeof(int), size_0);
            for (int i = 0; i < size_0; i++)
            {
                MPI_Recv(&tasks_0[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            }
            MPI_Recv(&size_1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
            tasks_1 = (int *)calloc(sizeof(int), size_1);
            for (int i = 0; i < size_1; i++)
            {
                MPI_Recv(&tasks_1[i], 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
            }
        }
        else
        {
            // primeste de la masteri
            MPI_Recv(&size_0, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            tasks_0 = (int *)calloc(sizeof(int), size_0);
            for (int i = 0; i < size_0; i++)
            {
                MPI_Recv(&tasks_0[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            }
            MPI_Recv(&size_1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
            tasks_1 = (int *)calloc(sizeof(int), size_1);
            for (int i = 0; i < size_1; i++)
            {
                MPI_Recv(&tasks_1[i], 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
            }

            // trimite spre masteri
            MPI_Send(&size_2, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 0);
            for (int i = 0; i < size_2; i++)
            {
                MPI_Send(&tasks_2[i], 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            }

            MPI_Send(&size_2, 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
            for (int i = 0; i < size_2; i++)
            {
                MPI_Send(&tasks_2[i], 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
            }

            // trimite spre masteri compensand legatura rupta
            MPI_Send(&size_0, 1, MPI_INT, 1, 3, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 0);
            for (int i = 0; i < size_0; i++)
            {
                MPI_Send(&tasks_0[i], 1, MPI_INT, 1, 3, MPI_COMM_WORLD);
            }

            MPI_Send(&size_1, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
            for (int i = 0; i < size_1; i++)
            {
                MPI_Send(&tasks_1[i], 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
            }
        }

        // trimite tipologia tuturor
        for (int i = 0; i < size_2; i++)
        {
            MPI_Send(&size_0, 1, MPI_INT, tasks_2[i], 2, MPI_COMM_WORLD);
            MPI_Send(&size_1, 1, MPI_INT, tasks_2[i], 2, MPI_COMM_WORLD);
            MPI_Send(&size_2, 1, MPI_INT, tasks_2[i], 2, MPI_COMM_WORLD);
            MPI_Send(tasks_0, size_0, MPI_INT, tasks_2[i], 2, MPI_COMM_WORLD);
            MPI_Send(tasks_1, size_1, MPI_INT, tasks_2[i], 2, MPI_COMM_WORLD);
            MPI_Send(tasks_2, size_2, MPI_INT, tasks_2[i], 2, MPI_COMM_WORLD);
        }

        // cerinta 2
        //  primeste de la master 0 partea de vector asociata
        int v_size_2;
        if (error == 0)
        {
            MPI_Recv(&v_size_2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            v_task_part = (int *)calloc(sizeof(int), v_size_2);
            for (int i = 0; i < v_size_2; i++)
                MPI_Recv(&v_task_part[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }
        else
        {
            int v_size_1;
            int *v_task_part_1;

            MPI_Recv(&v_size_1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            v_task_part_1 = (int *)calloc(sizeof(int), v_size_1);
            for (int i = 0; i < v_size_1; i++)
                MPI_Recv(&v_task_part_1[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

            MPI_Recv(&v_size_2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            v_task_part = (int *)calloc(sizeof(int), v_size_2);
            for (int i = 0; i < v_size_2; i++)
                MPI_Recv(&v_task_part[i], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);


            MPI_Send(&v_size_1, 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, 1);
            for (int i = 0; i < v_size_1; i++)
            {
                MPI_Send(&v_task_part_1[i], 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, 1);
            }
        }

        // trimite la taskuri partile care trebuie calculate
        int send_size, index_trimitere = 0;
        int rest_impartire_v = v_size_2 % size_2;
        for (int i = 0; i < size_2; i++)
        {
            send_size = v_size_2 / size_2;
            if (rest_impartire_v)
            {
                send_size++;
                rest_impartire_v--;
            }
            MPI_Send(&send_size, 1, MPI_INT, tasks_2[i], 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, tasks_2[i]);
            for (int j = 0; j < send_size; j++)
            {
                MPI_Send(&v_task_part[index_trimitere + j], 1, MPI_INT, tasks_2[i], 2, MPI_COMM_WORLD);
                printf("M(%d,%d)\n", rank, tasks_2[i]);
            }
            for (int j = 0; j < send_size; j++)
                MPI_Recv(&v_task_part[index_trimitere + j], 1, MPI_INT, tasks_2[i], MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            index_trimitere += send_size;
        }

        // trimit spre task 0 rezultatul
        if (error == 0)
            for (int i = 0; i < v_size_2; i++)
            {
                MPI_Send(&v_task_part[i], 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            }
        else
        {
            int v_size_1;
            int *v_task_part_1;
            MPI_Recv(&v_size_1, 1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            v_task_part_1 = (int *)calloc(sizeof(int), v_size_1);
            for (int i = 0; i < v_size_1; i++)
            {
                MPI_Recv(&v_task_part_1[i], 1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            }

            for (int i = 0; i < v_size_1; i++)
            {
                MPI_Send(&v_task_part_1[i], 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            }

            for (int i = 0; i < v_size_2; i++)
            {
                MPI_Send(&v_task_part[i], 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            }
        }
    }
    else
    {

        int master;
        MPI_Recv(&master, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        MPI_Send(&rank, 1, MPI_INT, master, 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, master);

        MPI_Recv(&size_0, 1, MPI_INT, master, master, MPI_COMM_WORLD, &status);
        MPI_Recv(&size_1, 1, MPI_INT, master, master, MPI_COMM_WORLD, &status);
        MPI_Recv(&size_2, 1, MPI_INT, master, master, MPI_COMM_WORLD, &status);
        tasks_0 = (int *)calloc(sizeof(int), size_0);
        MPI_Recv(tasks_0, size_0, MPI_INT, master, master, MPI_COMM_WORLD, &status);
        tasks_1 = (int *)calloc(sizeof(int), size_1);
        MPI_Recv(tasks_1, size_1, MPI_INT, master, master, MPI_COMM_WORLD, &status);
        tasks_2 = (int *)calloc(sizeof(int), size_2);
        MPI_Recv(tasks_2, size_2, MPI_INT, master, master, MPI_COMM_WORLD, &status);

        // cerinta 2 primesc parte vector si o trimit inapoi in master
        int v_size;
        int *v_task;
        MPI_Recv(&v_size, 1, MPI_INT, master, master, MPI_COMM_WORLD, &status);
        v_task = (int *)calloc(sizeof(int), v_size);
        for (int i = 0; i < v_size; i++)
        {
            MPI_Recv(&v_task[i], 1, MPI_INT, master, master, MPI_COMM_WORLD, &status);
            v_task[i] *= 2;
        }

        for (int i = 0; i < v_size; i++)
        {
            MPI_Send(&v_task[i], 1, MPI_INT, master, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, master);
        }
    }

    // afisare topologie
    printf("%d -> 0:", rank);
    for (int i = 0; i < size_0 - 1; i++)
        printf("%d,", tasks_0[i]);
    printf("%d 1:", tasks_0[size_0 - 1]);
    for (int i = 0; i < size_1 - 1; i++)
        printf("%d,", tasks_1[i]);
    printf("%d 2:", tasks_1[size_1 - 1]);
    for (int i = 0; i < size_2 - 1; i++)
        printf("%d,", tasks_2[i]);
    printf("%d\n", tasks_2[size_2 - 1]);

    MPI_Finalize();
}
