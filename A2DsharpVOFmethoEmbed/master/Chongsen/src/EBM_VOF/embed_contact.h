/**
Cell marking constants for interface reconstruction and contact line handling.
These values are stored in the mark[] field to classify cells based on their
interface configuration and proximity to the contact line. */

#define MARK_SOLID 1           // Cell is entirely in solid (cs[] = 0)
#define MARK_NO_INTERFACE 2    // Cell has no fluid interface
#define MARK_STANDARD_PLIC 3   // Standard PLIC reconstruction
#define MARK_CONTACT_2PHASE 4  // Two-phase contact line cell
#define MARK_CONTACT_3PHASE 5  // Three-phase contact line cell
#define MARK_INTERFACE_ONLY 6  // Interface cell away from contact line
#define MARK_BOUNDARY_EMBED 7  // Cell at embedded boundary
#define MARK_SPECIAL 8         // Special case cell

scalar tmp_c[], * tmp_interfaces = {tmp_c};
scalar mark[], * interfaces_mark = {mark};
scalar contact_angle[], * interfaces_contact_angle = {contact_angle};
extern scalar * interfaces;

#include "embed_correct_height.h"

/**
Calculate the extended volume fraction field and height function field at the initial time step and after each VOF step.*/

event init (i = 0) {
  for (scalar f in interfaces)
  for (scalar contact_angle in interfaces_contact_angle)
  for (scalar mark in interfaces_mark)
  for (scalar tmp_c in tmp_interfaces){
    reconstruction_tmp_c(f, contact_angle, cs, fs, tmp_c, mark);
    if (tmp_c.height.x.i)
      heights (tmp_c, mark, tmp_c.height);
    recompute_h (f, tmp_c, cs, mark, contact_angle, tmp_c.nc, tmp_c.height, tmp_c.oxyi, 1);
  }
}

event vof (i++) {
  for (scalar f in interfaces)
  for (scalar contact_angle in interfaces_contact_angle)
  for (scalar mark in interfaces_mark)
  for (scalar tmp_c in tmp_interfaces){
    if (tmp_c.height.x.i)
      heights (tmp_c, mark, tmp_c.height);
    recompute_h (f, tmp_c, cs, mark, contact_angle, tmp_c.nc, tmp_c.height, tmp_c.oxyi, 1);
  }
}


