dnl vim: set ft=m4 et :
dnl This file is used by Makefile.am.m4. You should
dnl use the provided autogen.bash script to do all the hard work.
dnl
dnl This file is used to avoid having to make lots of repetitive changes in
dnl Makefile.am every time we add a source or test file. The first parameter is
dnl the base filename with no extension; later parameters can be `hh', `cc',
dnl `test', `impl', `testscript'. Note that there isn't much error checking done
dnl on this file at present...

add(`apply_dirichlet_boundaries',       `hh')
add(`conjugate_gradients',              `hh', `test')
add(`defect',                           `hh',         `sse',                        `cuda')
add(`endian_swap',                      `hh')
add(`fill_matrix',                      `hh', `test')
add(`fill_vector',                      `hh', `test')
add(`hessenberg',                       `hh', `test')
add(`iterative_refinement',             `hh', `test')
add(`interpolation',                    `hh', `test')
add(`jacobi',                           `hh', `test')
add(`jacobi_kernel',                    `hh', `test', `sse')
add(`jacobi_kernel_cascade',            `hh',         `sse')
add(`matrix_io',                        `hh', `test')
add(`methods',                          `hh')
add(`multigrid',                        `hh')
add(`poisson_cg_double_dense',                `test',                   `benchmark')
add(`poisson_jac_double_dense',               `test',                   `benchmark')
add(`poisson_pcg_double_dense',               `test',                   `benchmark')
add(`poisson_iteref_cg_double_dense',         `test',                   `benchmark')
add(`poisson_iteref_pcg_double_dense',        `test',                   `benchmark')
add(`poisson_jac_float_banded',               `test',                   `benchmark')
add(`poisson_jac_float_q1',                   `test')
add(`poisson_jackernel_float_banded_sse',     `test',                   `benchmark')
add(`poisson_jackernel_float_banded',         `test',                   `benchmark')
add(`poisson_jackernel_double_banded_sse',    `test',                   `benchmark')
add(`poisson_jackernel_double_banded',        `test',                   `benchmark')
add(`poisson_jkc_float_banded',               `test')
add(`poisson_jkc_double_banded',              `test')
add(`poisson_jkc_float_banded_sse',           `test')
add(`poisson_cg_float_banded',                `test',                   `benchmark')
add(`poisson_cg_float_banded_q1',             `test')
add(`poisson_cg_double_banded_q1',             `test')
add(`poisson_pcg_float_banded',               `test',                   `benchmark')
add(`poisson_iteref_cg_float_banded',         `test',                   `benchmark')
add(`poisson_iteref_pcg_float_banded',        `test',                   `benchmark')
add(`poisson_mixedprec_cg',                   `test',                   `benchmark')
add(`poisson_mg_float_banded',                `test')
add(`poisson_mg_double_banded',               `test')
add(`poisson_mg_mixed_banded',                `test')
add(`prolongation',                     `hh', `cuda', `test')
add(`qr_decomposition',                 `hh', `test')
add(`quadrature',                       `hh', `test')
add(`restriction',                      `hh', `cuda', `test')
add(`sqrt',                             `hh', `test', `sse', `cell')
