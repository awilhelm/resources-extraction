all: $(all.o:o=so)

$(foreach *,$(all.o),$(eval $(*:o=so): $*))

CPPFLAGS +=\
	-I$(VPATH)..\
	-isystem/usr/include/glib-2.0\
	-isystem/usr/lib/glib-2.0/include\

