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
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>

/*TODO header file for m(un)map*/

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

/***************************
 * Compile-time parameters *
 ***************************/

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
                val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
                if (val > 255)
                        val = 255;

                /* And store it in the color_val[] array */
                val = xterm_color(val);
                color_val[n] = val;
        }
}

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[])
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
}

void compute_and_output_mandel_line(int fd, int line)
{
        /*
         * A temporary array, used to hold color values for the line being drawn
         */
        int color_val[x_chars];

        compute_mandel_line(line, color_val);
        output_mandel_line(fd, color_val);
}

/*
 * Create a shared memory area, usable by all descendants of the calling
 * process.
 */
void *create_shared_memory_area(unsigned int numbytes)
{
        int pages;
        void *addr;

        if (numbytes == 0) {
                fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
                exit(1);
        }

        /*
         * Determine the number of pages needed, round up the requested number of
         * pages
         */
        pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;
 /* Create a shared, anonymous mapping for this number of pages */
        /* TODO:
                addr = mmap(...)
        */
        addr = mmap(NULL, pages * sysconf(_SC_PAGE_SIZE),
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (addr == MAP_FAILED) {
                perror("mmap");
                exit(1);
        }

        return addr;
}

        void destroy_shared_memory_area(void *addr, unsigned int numbytes) {
        int pages;

        if (numbytes == 0) {
                fprintf(stderr, "%s: internal error: called for numbytes == 0\n", __func__);
                exit(1);
        }


        // * Determine the number of pages needed, round up the requested number of
        // * pages

        pages = (numbytes - 1) / sysconf(_SC_PAGE_SIZE) + 1;

        if (munmap(addr, pages * sysconf(_SC_PAGE_SIZE)) == -1) {
                perror("destroy_shared_memory_area: munmap failed");
                exit(1);
        }
}

//struct που δηλώνω αυτά που θέλω να έχω σε shared memory
typedef struct {
    sem_t sema[50];
    int framebuffer[50][90];
} shared_data_t;

int main(int argc, char *argv[])
{


        if (argc != 2) {
                fprintf(stderr, "Usage: %s NPROCS\n", argv[0]);
                return 1;
        }


        int NPROCS = atoi(argv[1]);
        if (NPROCS <= 0 || NPROCS > y_chars) {
                fprintf(stderr, "Invalid number of processes\n");
                return 1;
        }

        xstep = (xmax - xmin) / x_chars;
        ystep = (ymax - ymin) / y_chars;


        shared_data_t *shared = create_shared_memory_area(sizeof(shared_data_t));


        for (int i = 0; i < y_chars; i++) {
    sem_init(&shared->sema[i], 1, 0);  // κλειδωμένοι
        }

        sem_post(&shared->sema[0]);  // μόνο η γραμμή 0 ξεκινά

        for (int i = 0; i < NPROCS; i++) {
                pid_t pid = fork();
                if (pid < 0) {
                        perror("fork");
                        exit(1);
                }
                else if (pid == 0) {
                        // Παιδί
                        for (int line = i; line < y_chars; line += NPROCS) {
                                compute_mandel_line(line, shared->framebuffer[line]);

                                sem_wait(&shared->sema[line]);
                                output_mandel_line(1, shared->framebuffer[line]);
                                if (line + 1 < y_chars)
                                sem_post(&shared->sema[line + 1]);
                        }
                        exit(0);
                }
        }
  for (int i = 0; i < NPROCS; i++)
                wait(NULL);

        /*
         * draw the Mandelbrot Set, one line at a time.
         * Output is sent to file descriptor '1', i.e., standard output.
         */
        //for (line = 0; line < y_chars; line++) {
        //      compute_and_output_mandel_line(1, line);
        //}
        for (int i = 0; i < y_chars; i++) {
    sem_destroy(&shared->sema[i]);
}
        destroy_shared_memory_area(shared, sizeof(shared_data_t));
        reset_xterm_color(1);
        return 0;

}