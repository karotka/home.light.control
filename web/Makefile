#!/usr/bin/make -f

PROJECT=hlc
COMPONENT=web
VERSION=$(shell dpkg-parsechangelog | grep Version | cut -d' ' -f2)
FULLNAME=$(PROJECT)-$(COMPONENT)
PACKAGE="$(FULLNAME)_${VERSION}_all.deb"
DEPLOY_DEST ?=127.0.0.1

# Do not remove next row, it prevents cyclic build call.
build:

deb:
	debuild -uc -us

localtest: deb
	sudo dpkg -i ../${PACKAGE}
	sudo /etc/init.d/${PROJECT}-${COMPONENT} restart

deploy: deb
	scp ../$(PACKAGE) root@$(DEPLOY_DEST):/tmp/
	ssh root@$(DEPLOY_DEST) "dpkg -i /tmp/$(PACKAGE) && /etc/init.d/$(FULLNAME) restart"
