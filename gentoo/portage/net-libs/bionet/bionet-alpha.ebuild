# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $
inherit versionator

DESCRIPTION="Bionet libraries and sample apps"
HOMEPAGE="http://bioserve.colorado.edu/www/products/bionet/"
MY_PV=${PV/_alpha[0-9]*/}
MY_PR=${PV/[0-9]*_alpha/bzr}
MY_P="${PN}2-${MY_PV}+${MY_PR}"
SRC_URI="http://bioserve:bionet!@bioserve.colorado.edu/bionet/tarballs/${MY_P}.tar.bz2"
S="${WORKDIR}/${MY_P}"

LICENSE=""
SLOT="1"
KEYWORDS="~x86 ~amd64"
IUSE="bluetooth qt4 python alsa"
#Never try to download from mirror
RESTRICT="mirror"
EAPI="2"

#Build Dependencies
DEPEND="
	>=dev-libs/check-0.9.5
	dev-db/sqlite
	media-gfx/graphviz
	app-doc/doxygen
	dev-util/valgrind
	python? ( dev-lang/swig[python] )
	"

#Runtime Dependencies (Adds to build-deps)
RDEPEND="
	>=dev-lang/python-2.5
net-dns/avahi[mdnsresponder-compat]
alsa? ( media-libs/alsa-lib )
bluetooth? ( net-wireless/bluez-libs )
qt4? (
	>=x11-libs/qt-core-4.5
	>=x11-libs/qt-gui-4.5
	>=x11-libs/qt-svg-4.5
	>=x11-libs/qwt-5.0
	)
"


src_configure() {
	econf \
		$(use_enable bluetooth) \
		$(use_enable qt4) \
		$(use_enable python) \
		$(use_enable alsa) \
		|| die "Error: econf failed!"
	
}

src_compile() {
	emake || die "Error: emake failed!"
}

src_install() {
	emake DESTDIR="${D}" install
}
