require 'mkmf'

#`cd liblinear-1.91; make`
$LDFLAGS << " -L/usr/lib/"
$LIBS << " -llinear"
create_makefile('linear_ext')