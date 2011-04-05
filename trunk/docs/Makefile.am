if HAVE_DOXYGEN
directory = $(top_srcdir)/docs/man/man3/

dist_man_MANS = $(directory)/shamir_OTP.h.3 $(directory)/profile_t.3
$(directory)/shamir_OTP.h.3: doxyfile.stamp
$(directory)/profile.3: doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxywarn.txt doxyfile.stamp

all-local: doxyfile.stamp

clean-local:
	rm -rf $(top_srcdir)/docs/man $(top_srcdir)/docs/html

#docpkg = $(PACKAGE_TARNAME)-doxy-$(PACKAGE_VERSION).tar.gz

#doc_DATA = $(docpkg)

#$(docpkg): doxygen.stamp
#	tar chof - html | gzip -9 -c >$@

#doxygen.stamp: Doxyfile
#	$(DOXYGEN) $<
#$	echo Timestamp > $@

#install-data-hook:
#	cd $(DESTDIR)$(docdir) && tar xf $(docpkg)

#uninstall-hook:
#	cd $(DESTDIR)$(docdir) && rm -rf html man

endif
