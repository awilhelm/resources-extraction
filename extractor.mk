extract: $(all.o)

CPPFLAGS +=\
	-isystem/usr/include/glib-2.0\
	-isystem/usr/lib/glib-2.0/include\

LDLIBS +=\
	-ldl\
	-lglib-2.0\

