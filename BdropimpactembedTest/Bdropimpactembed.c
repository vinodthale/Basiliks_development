#include "embed.h"
#include "axi.h"
#include "navier-stokes/centered.h"
#include "two-phase.h"
#include "tension.h"
#include "view.h"

//qcc -Wall -O2 -disable-dimensions Bdropimpactembed.c -o Bdropimpactembed -L$BASILISK/gl -lglutils -lfb_tiny -lm

// Max and min refinement levels
int maxlevel = 9;
int minlevel = 7;

// Boundary conditions for the embedded solid boundary
u.n[embed] = dirichlet(0.);
u.t[embed] = dirichlet(0.);
p[embed] = neumann(0.);

#define tsnap 0.01  // Time interval for snapshots
#define tmax  1.00  // Final time

// Main function
int main(int argc, char *argv[]) {
  L0 = 4.0;
  X0 = 0.;
  Y0 = 0.;
  init_grid(1 << minlevel);  // Start with a reasonably fine grid

  // Create output directory
  system("mkdir -p intermediate");
  system ("mkdir -p Images");

  // Physical properties
  rho1 = 1.0;
  mu1  = 4.888e-04;
  rho2 = 1.212e-03;
  mu2  = 8.847e-06;
  f.sigma = 3.568e-02;

  //G.x = -9.81; // Enable gravity

  run();
}

// Initialize solid, drop, and initial velocity
event init(t = 0) {
  if (!restore(file = "dump")) {
    // Embedded solid: circle of radius 0.5 at origin
    solid(cs, fs, -(sq(0.50) - sq(x) - sq(y)));

    // Use fraction function to define the drop: centered at x = 1.5, radius = 0.5
    fraction(f, sq(0.50) - (sq(x - 1.5) + sq(y)));
    refine(sq(x) + sq(y) < sq(0.52) && level < maxlevel); // refinement around solid particle  
    refine(sq(x - 1.5) + sq(y) < sq(0.52) && level < maxlevel); // refinement around Drop + Bubble
    foreach() {
      u.x[] = -1.0 * f[];  // Flow to the left if inside the drop
      u.y[] = 0.0;          // No flow in the y-direction
    }
  } else {
    fprintf(stderr, "Restarted from saved state.\n");
  }
}

// Adaptive mesh refinement
event adapt(i++) {
  adapt_wavelet({cs, f, u}, (double[]){1e-4, 1e-4, 1e-3, 1e-3}, maxlevel);
}

/*// Adaptive mesh refinement
event adapt(i++) {
  adapt_wavelet({f, u}, (double[]){1e-4, 1e-3, 1e-3}, maxlevel);
}*/

// Logging time step info
event ptintall(i++) {
  printf("i[%06d], dt[%e], t[%.2f]\n", i, dt, t);
}

// Output snapshots every tsnap seconds
event writingFiles(t = 0; t <= tmax; t += tsnap) {
  dump(file = "dump");  // Overwrite checkpoint
  char nameOut[80];
  sprintf(nameOut, "intermediate/snapshot-%5.4f", t);
  dump(file = nameOut);  // Save snapshot
}

event abc (t = 0; t <= tmax; t += tsnap)
{
  char nameBview[100];
  view (width = 1200, height = 800, quat = { 0, 0, -0.707, 0.707 }, fov = 17.9799, ty = -0.45);
  box(notics = false);
  squares("f", linear = false, min = 0.0, max = 1.0);
  draw_vof("cs", "fs", filled = -1, fc = {0.3, 0.3, 0.3}); 
  cells ();
   mirror({0, 1})
    {
      draw_vof("f", lc = {1, 1, 0}, lw = 5);  // Yellow color, thick lines
      draw_vof("cs", "fs", filled = -1, fc = {0.3, 0.3, 0.3}); 
    }
  sprintf (nameBview, "Images/yesembedVOF-%f.png", t);
  save (nameBview);
  //save ("movie.mp4");
}