
gladedir = $(datadir)/camorama/
glade_DATA =				\
	camorama.glade			

schema_in_files = camorama.schemas.in
schemadir = $(sysconfdir)/gconf/schemas
schema_DATA = $(schema_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

# For the sake of some users
user-install: $(app_DATA) $(keys_DATA)
	mkdir -p ~/.gnome/application-registry ~/.gnome/mime-info/
	cp $(app_DATA) ~/.gnome/application-registry
	cp $(keys_DATA) ~/.gnome/mime-info/

install-data-local:
	-GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(srcdir)/$(schema_DATA)

EXTRA_DIST = $(glade_DATA) $(schema_in_files)


