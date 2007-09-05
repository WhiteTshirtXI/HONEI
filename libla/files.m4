dnl vim: set ft=m4 et :
dnl This file is used by Makefile.am.m4. You should
dnl use the provided autogen.bash script to do all the hard work.
dnl
dnl This file is used to avoid having to make lots of repetitive changes in
dnl Makefile.am every time we add a source or test file. The first parameter is
dnl the base filename with no extension; later parameters can be `hh', `cc',
dnl `test', `impl', `testscript'. Note that there isn't much error checking done
dnl on this file at present...

add(`absolute',                  `hh', `cc', `test')
add(`banded_matrix',             `hh', `test')
add(`dense_matrix',              `hh', `test')
add(`dense_vector',              `hh', `test')
add(`difference',                `hh', `test')
add(`dot_product',               `hh', `cc', `test', `benchmark')
add(`element_inverse',           `hh', `test', `benchmark')
add(`element_iterator',          `hh', `test')
add(`element_product',           `hh', `test', `benchmark')
add(`matrix',                    `hh')
add(`matrix_error',              `hh', `cc')
add(`norm',                      `hh', `test')
add(`product',                   `hh', `test', `benchmark')
add(`reduction',                 `hh', `test', `benchmark')
add(`residual',                  `hh', `test')
add(`scale',                     `hh', `test', `benchmark')
add(`scaled_sum',                `hh', `test')
add(`sparse_matrix',             `hh', `test')
add(`sparse_vector',             `hh', `test')
add(`sum',                       `hh', `test', `benchmark')
add(`vector',                    `hh')
add(`vector_error',              `hh', `cc')
add(`vector_iterator',           `hh', `test')
dnl Scheduled to be moved to a different library:
dnl add(`vector_masked_max_index',   `hh', `test')
dnl add(`vector_masked_min_index',   `hh', `test')
