/*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>  // to kano include

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/
//#define NTHREADS //3 //3 νήματα
int NTHREADS;

/*
 * Output at the terminal is is x_chars wide by y_chars long
*/
int y_chars = 50;
int x_chars = 90;

/*
 * The part of the complex plane to be drawn:
 * upper left corner is (xmin, ymax), lower right corner is (xmax, ymin)
*/
double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;

/*
 * Every character in the final output is
 * xstep x ystep units wide on the complex plane.
 */
double xstep;
double ystep;

//tha oriso kai tous simaioforous
sem_t sems[50];  //orizo 50 oso kai y_chars grammes


/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
void compute_mandel_line(int line, int color_val[])
{
        /*
         * x and y traverse the complex plane.
         */
        double x, y;

        int n;
        int val;

        /* Find out the y value corresponding to this line */
        y = ymax - ystep * line;

        /* and iterate for all points on this line */
        for (x = xmin, n = 0; n < x_chars; x+= xstep, n++) {

                /* Compute the point's color value */
                val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);   //πόσες επαναλήψεις χρειάζονται
                if (val > 255)
                        val = 255;

                /* And store it in the color_val[] array */
                val = xterm_color(val);  //μετατρέπει την τιμή σε χρώμα
                color_val[n] = val;
        }
};

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[]) //εκτυπώνει μια γραμμή
{
        int i;

        char point ='@';
        char newline='\n';

        for (i = 0; i < x_chars; i++) {
                /* Set the current color, then output the point */
                set_xterm_color(fd, color_val[i]);
                if (write(fd, &point, 1) != 1) {
                        perror("compute_and_output_mandel_line: write point");
                        exit(1);
                }
        }


        /* Now that the line is done, output a newline character */
        if (write(fd, &newline, 1) != 1) {
                perror("compute_and_output_mandel_line: write newline");
                exit(1);
        }
};

void compute_and_output_mandel_line(int fd, int line)  // χρωματική τιμή για όλα τα σημεία μιας γραμμής (μιας οριζόντιας σειράς χαρακτήρων)
{
        /*
         * A temporary array, used to hold color values for the line being drawn
         */
        int color_val[x_chars];

        compute_mandel_line(line, color_val);
        output_mandel_line(fd, color_val);
};

void *thread_func(void *id){
    int tid = *(int*)id;

    for(int i=tid; i<y_chars; i+=NTHREADS){
        sem_wait(&sems[i]);
        compute_and_output_mandel_line(1, i);
        if(i+1<y_chars){
            sem_post(&sems[i+1]);
        }
    }
        return NULL;
};

int main(int argc, char *argv[])
{
        //int line;

        if (argc != 2) {
                fprintf(stderr, "Usage: %s <number_of_threads>\n", argv[0]);
                exit(1);
        }

        NTHREADS = atoi(argv[1]);

        xstep = (xmax - xmin) / x_chars;
        ystep = (ymax - ymin) / y_chars;

        //orizo threads kai ids ton threads
        pthread_t threads[NTHREADS];
        int ids[NTHREADS];


        //αρχικοποιώ τους σημαιοφόρους έτσι ώστε κανείς να μην ειναι ενεργοποιημενος στην αρχή
        for (int i = 0; i < y_chars; i++) {
                sem_init(&sems[i], 0, 0); // κλειδωμένοι όλοι
        }

        //ξεκλειδώνω πρώτη γραμμή
        sem_post(&sems[0]);

        //δημιουργώ τα νήματα
        for (int i = 0; i < NTHREADS; i++) {
                ids[i] = i;
                pthread_create(&threads[i], NULL, thread_func, &ids[i]);
        }

        /*
         * draw the Mandelbrot Set, one line at a time.
         * Output is sent to file descriptor '1', i.e., standard output.
         */
        //for (line = 0; line < y_chars; line++) {
        //      compute_and_output_mandel_line(1, line);
        //}


        //περιμένω να τλεειώσουν όλα τα threads για να τερματισω
        for (int i = 0; i < NTHREADS; i++) {
                pthread_join(threads[i], NULL);
        }

        reset_xterm_color(1);
        return 0;
}

