/*--------------------------------------------------------------------------*\
| TESS-DEF.H                                                                 |
\*--------------------------------------------------------------------------*/

/*
  Beyond The Tesseract
    - adventure world definition
*/

/* Max... are for internal use.  Num... are for external use
*/
#define MaxLocs 36      /* # rooms */
#define MaxObjs 54      /* # objects */

/* constants to refer to objects by
*/
enum
{
  o_nothing,
  o_north, o_east, o_south, o_west, o_invent, o_mirror, o_crt, o_plant,
  o_group, o_hole, o_proj, o_buttons, o_zero, o_one, o_two, o_three,
  o_four, o_fractal, o_chaos, o_dust, o_flake, o_mount, o_tomb, o_stack,
  o_audio, o_book, o_pills, o_fluid, o_bottle, o_prism, o_appa, o_improb,
  o_strip, o_zeta, o_cube, o_coil, o_plasma, o_solid, o_liquid, o_sing,
  o_disk, o_supp, o_hypo, o_lemma, o_theorem, o_axiom, o_post, o_math,
  o_tetra, o_func, o_idea, o_contra, o_warr
};

