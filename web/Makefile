#!/usr/bin/make -f

PROJECT=sklik
COMPONENT=importweb
VERSION=$(shell dpkg-parsechangelog | grep Version | cut -d' ' -f2)
FULLNAME=szn-$(PROJECT)-$(COMPONENT)
PACKAGE="$(FULLNAME)_${VERSION}_all.deb"
DEPLOY_DEST ?=sklik-webs.dev

# Do not remove next row, it prevents cyclic build call.
build:

deb:
	debuild -uc -us

localtest: deb
	sudo dpkg -i ../${PACKAGE}
	sudo /etc/init.d/szn-${PROJECT}-${COMPONENT} restart

deploy: deb
	scp ../$(PACKAGE) root@$(DEPLOY_DEST):/tmp/
	ssh root@$(DEPLOY_DEST) "dpkg -i /tmp/$(PACKAGE) && /etc/init.d/$(FULLNAME) restart"
