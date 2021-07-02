/*
 * TJN: Adapted from CMake Hello World Example
 * https://github.com/jameskbride/cmake-hello-world.git
 *
 */
#include <Speaker.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
int MAX_NLOOP = 100;

using namespace std;
using namespace Hello;

extern "C" {
extern int ompi_spc_value_diff(char *spc_name,
                               long long spc_prev_value,
                                                              long long *spc_new_value,
                                                                                             long long *spc_diff);
}

int main(int argc, char *argv[]) {
  int rank, size;

    int *inbuf = NULL;
    int *outbuf = NULL;
    int i, j;
    int showrank = 0;
    int nloop;

    int rc;
    int provided, num, name_len, desc_len, verbosity, bind, var_class, readonly, continuous, atomic, count, index;
    char name[256], description[256];
    MPI_Datatype datatype;
    MPI_T_enum enumtype;
    long long value;
    int found = 0;
    char label[256] = "main";
    int num_elem = 1024;
    long long _time_alltoall_past_value = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  char *counter_name = "runtime_spc_OMPI_SPC_TIME_ALLTOALL";
  MPI_T_pvar_handle handle;
  MPI_T_pvar_session session;

  MPI_T_init_thread(MPI_THREAD_SINGLE, &provided);

  //Speaker* speaker = new Speaker();

  cout << "Hello from rank: " << rank << "/" << size << endl;
  //speaker->sayHello();

    /* Determine the MPI_T pvar indices for the OMPI_BYTES_SENT/RECIEVED_USER SPCs */
    MPI_T_pvar_get_num(&num);

    rc = MPI_T_pvar_session_create(&session);

    for(i = 0; i < num; i++) {
        name_len = desc_len = 256;
        rc = PMPI_T_pvar_get_info(i, name, &name_len, &verbosity,
                                  &var_class, &datatype, &enumtype, description, &desc_len, &bind,
                                  &readonly, &continuous, &atomic);
        if( MPI_SUCCESS != rc )
            continue;

        if(strcmp(name, counter_name) == 0) {
            /* Create the MPI_T sessions/handles for the counters and start the counters */
            rc = MPI_T_pvar_handle_alloc(session, i, NULL, &handle, &count);
            rc = MPI_T_pvar_start(session, handle);
            found = 1;
            //printf("[%d] =====================================\n", rank);
            //printf("[%d] %s -> %s\n", rank, name, description);
            //printf("[%d] =====================================\n", rank);
            //fflush(stdout);
        }
    }

    /* Make sure we found the counters */
    if(found == 0) {
        fprintf(stderr, "ERROR: Couldn't find the appropriate SPC counter in the MPI_T pvars.\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    inbuf  = (int *) malloc ( size * num_elem * sizeof(int) );
    if (NULL == inbuf) {
        fprintf(stderr, "Error: malloc failed (inbuf)\n");
        goto cleanup;
    }

    outbuf  = (int *) malloc ( size * num_elem * sizeof(int) );
    if (NULL == outbuf) {
        fprintf(stderr, "Error: malloc failed (outbuf)\n");
        goto cleanup;
    }

    for (i=0; i < size * num_elem; i++) {
        inbuf[i] = 100 + rank;
        outbuf[i] = 0;
    }

#if 0
    if (rank == showrank) {
        for (i=0; i < size * num_elem; i++) {
            printf("(%d) BEFORE inbuf[%d] = %d\n",
                   rank, i, inbuf[i]);
        }
    }
#endif

    MPI_Barrier(MPI_COMM_WORLD);

    //printf("%10s: (%d of %d) Hello World\n", label, rank, size);

    MPI_Barrier(MPI_COMM_WORLD);

    for (nloop=0; nloop < MAX_NLOOP; nloop++) {
        long long tmp_max;
        int global_rc;
        long long new_value = 0;
        long long diff = 0;
        float stddev = 0.0;
        float mean   = 0.0;

        MPI_Barrier(MPI_COMM_WORLD);
        fflush(NULL);

        rc = MPI_Alltoall(inbuf, num_elem, MPI_INT, outbuf, num_elem, MPI_INT, MPI_COMM_WORLD);

        /* Check if alltoall had any problems? */
        MPI_Allreduce( &rc, &global_rc, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );
        if (rank == 0) {
            if (global_rc != 0) {
                fprintf(stderr, "Error: Alltoall failed! (rc=%d)\n", global_rc);
                goto cleanup;
            }
        }

        MPI_T_pvar_read(session, handle, &value);
        MPI_Allreduce(&value, &tmp_max, 1, MPI_LONG_LONG, MPI_MAX, MPI_COMM_WORLD);

        rc = ompi_spc_value_diff("OMPI_SPC_TIME_ALLTOALL",
        //rc = MPIX_T_spc_value_diff("OMPI_SPC_TIME_ALLTOALL",
                                 _time_alltoall_past_value,
                                 &new_value,
                                 &diff);

        MPI_Barrier(MPI_COMM_WORLD);

        //calc_stddev(MPI_COMM_WORLD, 1, &value, &stddev, &mean);

        if ((MAX_NLOOP <= 20) || ( !(nloop % 10) )) {
          #if 0
            printf("%10s: Rank: %5d  Size: %5d  Loop: %8d  %s: %d  max: %d  prev_value: %d  new_value: %d  diff: %d  stddev: %.2f  mean: %.2f\n",
                    label, rank, size, nloop, counter_name, value, tmp_max, _time_alltoall_past_value, new_value, diff, stddev, mean);
          #else
            /* TJN: If add sleep() *must* increase congest_threshold time!!! */
            //int usecs = 0;
            int usecs = 250000;   /* 0.25 sec */
            //int usecs = 100000;   /* 0.1 sec */
            //int usecs = 2000000;  /* 2 sec */
            printf("%10s: Rank: %5d  Size: %5d  Loop: %8d  %s: %d  max: %d  prev_value: %d  new_value: %d  diff: %d  stddev: %.2f  mean: %.2f -- SLEEP: %dus\n",
                    label, rank, size, nloop, counter_name, value, tmp_max, _time_alltoall_past_value, new_value, diff, stddev, mean, usecs);
            usleep(usecs);
          #endif
        }

        _time_alltoall_past_value = new_value;

//      MPI_Barrier(MPI_COMM_WORLD);
//        if (rank == 0) {
//            printf("##  END-LOOP: %d  ##########################################\n", nloop);
//        }
        fflush(NULL);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

#if 0
    if (rank == showrank) {
        for (i=0; i < size * num_elem; i++) {
            printf("(%d)    AFTER outbuf[%d] = %d\n",
                   rank, i, outbuf[i]);
        }
    }
#endif

#if 0
    printf("[%d] ==========================\n", rank);
    fflush(NULL);

    rc = MPI_T_pvar_read(session, handle, &value);
    printf("TJN: [%d] Value Read: %lld   (%s)\n", rank, value, counter_name);
    fflush(stdout);

    MPI_Barrier(MPI_COMM_WORLD);
#endif

    /* Stop the MPI_T session, free the handle, and then free the session */
    rc = MPI_T_pvar_stop(session, handle);
    rc = MPI_T_pvar_handle_free(session, &handle);

    /* Stop the MPI_T session, free the handle, and then free the session */
    rc = MPI_T_pvar_session_free(&session);

cleanup:
    if (NULL != inbuf)
        free(inbuf);

    if (NULL != outbuf)
        free(outbuf);

  MPI_T_finalize();
  MPI_Finalize();
}
