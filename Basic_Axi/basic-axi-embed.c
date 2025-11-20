//#include "grid/multigrid.h"
#include "embed.h"
#include "axi.h"
#include "navier-stokes/centered.h"
#include "view.h"

#define RHO 1
#define MU  1e-3
#define U0  0.01

scalar rhov[];
face vector muv[], alphav[];

u.n[left] = dirichlet (y <= 0.2 ? 2.*U0 : U0);
u.t[left] = dirichlet (0.);
p[left] = neumann (0.);

u.n[right] = neumann (0.);
u.t[right] = neumann (0.);
p[right] = dirichlet (0.);

u.n[embed] = dirichlet (0.);
u.t[embed] = dirichlet (0.);
p[embed] = neumann (0.);

int maxlevel = 6;

int main (void) {
  rho = rhov;
  mu = muv;
  alpha = alphav;

  DT = 1e-2 [0];
  L0 = 1 [0];

  init_grid (1 << maxlevel);
  run();
}

event init (i = 0) {
  solid (cs, fs, -(y - 0.5 - 1e-2));
  cm_update (cm, cs, fs);
  fm_update (fm, cs, fs);
  restriction ({cs,fs,cm,fm});
}

event properties (i++) {
  foreach()
    rhov[] = cm[]*RHO;

  foreach_face() {
    alphav.x[] = fm.x[]/RHO;

    muv.x[] = fm.x[]*MU;
  }
}

event adapt (i++) {
  double utol = 1e-3, ctol = 1e-3;
  adapt_wavelet ({u.x,u.y,cs}, (double[]){utol,utol,ctol}, maxlevel, list = {cs,fs,cm,fm}); 
}

event stop (t = 10) {
  dump();
}
