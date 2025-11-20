/**
# Axisymmetric Flow with Embedded Boundary

This simulation demonstrates axisymmetric (cylindrical) flow around an embedded
solid boundary using Basilisk's embedded boundary method combined with the
axisymmetric coordinate system.

## Physical Setup
- Axisymmetric geometry (cylindrical coordinates: x = axial, y = radial)
- Embedded solid boundary at y = 0.51 (horizontal wall)
- Inlet flow from left with velocity profile
- Incompressible Navier-Stokes equations

## Parameters
- Density: RHO = 1
- Viscosity: MU = 1e-3
- Characteristic velocity: U0 = 0.01
- Reynolds number: Re = U0 * L0 / MU

## Boundary Conditions
- Left (inlet): Velocity profile with step at y = 0.2
- Right (outlet): Pressure outlet (p = 0)
- Embedded: No-slip wall
- Axis: Axisymmetric (y = 0, handled automatically by axi.h)
*/

/**
We use the embedded boundary method for solid obstacles and axisymmetric
coordinates. The grid can be either tree (AMR) or multigrid.
Note: multigrid.h is commented out - we use the default tree grid. */

// #include "grid/multigrid.h"  // Use tree grid (default) for AMR
#include "embed.h"
#include "axi.h"
#include "navier-stokes/centered.h"
#include "view.h"

/**
## Physical parameters

All quantities are dimensionless. */

#define RHO 1      // Fluid density
#define MU  1e-3   // Dynamic viscosity
#define U0  0.01   // Characteristic velocity

/**
## Field declarations

For variable density and viscosity, we need to declare fields. */

scalar rhov[];
face vector muv[], alphav[];

/**
## Boundary conditions

Left boundary: Inlet with velocity profile
- Lower region (y <= 0.2): velocity = 2*U0
- Upper region (y > 0.2): velocity = U0
This creates a shear layer at y = 0.2 */

u.n[left] = dirichlet (y <= 0.2 ? 2.*U0 : U0);
u.t[left] = dirichlet (0.);
p[left] = neumann (0.);

/**
Right boundary: Pressure outlet */

u.n[right] = neumann (0.);
u.t[right] = neumann (0.);
p[right] = dirichlet (0.);

/**
Embedded boundary: No-slip wall */

u.n[embed] = dirichlet (0.);
u.t[embed] = dirichlet (0.);
p[embed] = neumann (0.);

/**
## Simulation parameters */

int maxlevel = 6;

/**
## Main function

Set up the simulation and run. */

int main (void) {
  /**
  Assign physical properties to the solver fields. */
  rho = rhov;
  mu = muv;
  alpha = alphav;

  /**
  Set domain size and timestep. */
  DT = 1e-2 [0];   // Maximum timestep
  L0 = 1 [0];      // Domain size

  /**
  Initialize the grid and start the simulation. */
  init_grid (1 << maxlevel);
  run();
}

/**
## Initial conditions

Define the embedded solid boundary and initialize the metric. */

event init (i = 0) {
  /**
  Create a horizontal solid boundary at y = 0.51.
  The solid() function takes a signed distance function where:
  - Negative values indicate fluid region
  - Positive values indicate solid region
  The boundary is at the zero level set: y - 0.51 = 0
  We negate it: -(y - 0.51) so that y < 0.51 is fluid (negative). */

  solid (cs, fs, -(y - 0.5 - 1e-2));

  /**
  Update the metric for embedded boundaries.
  cm: cell metric (volume fractions)
  fm: face metric (area fractions) */

  cm_update (cm, cs, fs);
  fm_update (fm, cs, fs);

  /**
  Apply boundary conditions and restriction for multigrid. */
  restriction ({cs, fs, cm, fm});
}

/**
## Physical properties

Update density and viscosity at each timestep. */

event properties (i++) {
  /**
  Update cell-centered density weighted by volume fraction. */
  foreach()
    rhov[] = cm[]*RHO;

  /**
  Update face-centered properties weighted by area fraction. */
  foreach_face() {
    alphav.x[] = fm.x[]/RHO;  // Inverse density for pressure projection
    muv.x[] = fm.x[]*MU;      // Viscosity
  }
}

/**
## Adaptive mesh refinement

Adapt the grid based on velocity and geometry. */

event adapt (i++) {
  double utol = 1e-3;   // Velocity adaptation tolerance
  double ctol = 1e-3;   // Geometry adaptation tolerance

  /**
  Adapt based on velocity components and geometry.
  Also refine/coarsen the metric fields (cs, fs, cm, fm). */

  adapt_wavelet ({u.x, u.y, cs},
                 (double[]){utol, utol, ctol},
                 maxlevel,
                 list = {cs, fs, cm, fm});
}

/**
## End of simulation

Stop at t = 10 and save a snapshot. */

event stop (t = 10) {
  dump();
}
