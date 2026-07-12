# Note: plugindir is set in configure

##############################################################################
# TODO: change libgstplugin.la to something else, e.g. libmysomething.la     #
##############################################################################
plugin_LTLIBRARIES = libgstipcu.la

##############################################################################
# TODO: for the next set of variables, name the prefix if you named the .la, #
#  e.g. libmysomething.la => libmysomething_la_SOURCES                       #
#                            libmysomething_la_CFLAGS                        #
#                            libmysomething_la_LIBADD                        #
#                            libmysomething_la_LDFLAGS                       #
##############################################################################

# sources used to compile this plug-in
libgstipcu_la_SOURCES = gstipcusrc.c gstipcusrc.h gst_cif_meta.c gst_cif_meta.h gstipcudemux.c gstipcudemux.h gstipcu.c gstipcusink.c gstipcusink.h

# compiler and linker flags used to compile this plugin, set in configure.ac
libgstipcu_la_CFLAGS = $(GST_CFLAGS)
libgstipcu_la_LIBADD = $(GST_LIBS)
libgstipcu_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstipcu_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstipcusrc.h
