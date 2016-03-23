#!/bin/bash
if [ "$#" -ne 1 ] ; then
	echo "build_all.sh [project_root]"
fi

export releasedir=`pwd`/release
export basedir=`pwd`
if [ ! -e "$releasedir" ] ; then
	mkdir "$releasedir"
fi

if [ ! -e "$basedir/releases" ] ; then
	mkdir "$basedir/releases"
fi

cd "$1";

#mv ../grub-customizer_* ../archive

export version=`cat debian/changelog.d | head -n 3 | grep -o '[0-9]*\.[0-9]*\.*[0-9]*'`
export rev=`cat debian/changelog.d | head -n 3 | grep -o 'ppa[0-9]*' | grep -o '[0-9]*'`
if [ "$rev" = "1" ]; then
	export subdir="$version"
else
	export subdir="$version-$rev"
fi

#rm -r $releasedir
#mkdir $releasedir

export sourcedir=`pwd`
sudo umount $releasedir 2> /dev/null
sudo mount none $releasedir -t tmpfs
#rm -rf $releasedir/* $releasedir/.*
mkdir $releasedir/grub-customizer-$version
cp -r * $releasedir/grub-customizer-$version


cd $releasedir/grub-customizer-$version

if [ $? -ne 0 ] ; then
	echo 'directory change failed'
	exit;
fi
rm -r debian/changelog.d debian/changelog.revu.d debian/changelog.mentors.d src/config.h build 2> /dev/null

if [ -d $basedir/releases/$subdir ]
then
	rm -r $basedir/releases/$subdir
fi
mkdir $basedir/releases/$subdir

mv debian ../
find -type f -print | xargs chmod -x
if [ "$rev" = "1" ]; then
	tar -C .. -c grub-customizer-$version | gzip > $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz
else
	cp $basedir/releases/$version/grub-customizer_$version.orig.tar.gz $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz
fi
rm -rf grub-customizer-$version
mv ../debian ./

mkdir "$basedir/releases/$subdir/launchpad-source"
cp "$basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz" "$basedir/releases/$subdir/launchpad-source/grub-customizer_$version.tar.gz"
gpg --armor --sign --detach-sig "$basedir/releases/$subdir/launchpad-source/grub-customizer_$version.tar.gz"

function build() {
	versionSuffix="$1"
	distribution="$2"
	
	cp $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz $basedir/release/grub-customizer_$version.orig.tar.gz

	$sourcedir/debian/changelog.d "$versionSuffix" "$distribution" > debian/changelog
	debuild -S;
	mkdir "$basedir/releases/$subdir/$distribution"
	mv "$basedir/release"/grub-customizer_* "$basedir/releases/$subdir/$distribution"
	
	uploadFile="$basedir/releases/$subdir/$distribution/upload.sh"
	
	echo '#!/bin/sh' >> "$uploadFile";
	echo 'dput ppa:danielrichter2007/grub-customizer "'$basedir'/releases/'$subdir'/'$distribution'/grub-customizer_'$version'-0ubuntu1~ppa'$rev$versionSuffix'_source.changes"' >> "$uploadFile";
	
	compileFile="$basedir/releases/$subdir/$distribution/compile.sh"
	
	echo '#!/bin/sh' >> "$compileFile"
	echo 'cd "'$basedir/releases/$subdir/$distribution'" && export BD=build_`date +%s` && mkdir $BD && sudo mount none $BD -t tmpfs && tar -xzf *.orig.tar.gz -C $BD && tar -xzf *.debian.tar.gz -C $BD/* && cp *.orig.tar.gz $BD/ && cd $BD/* && dpkg-buildpackage && cd ../.. && mv $BD/*.deb ./ && sudo umount $BD && rm -rf $BD' >> "$compileFile";
	
	chmod +x "$basedir/releases/$subdir/$distribution"/*.sh
}

build p precise
build q quantal
build s saucy
build t trusty
build v vivid
build w wily
build x xenial


echo '#!/bin/sh
'$basedir'/releases/'$subdir'/precise/upload.sh
'$basedir'/releases/'$subdir'/quantal/upload.sh
'$basedir'/releases/'$subdir'/saucy/upload.sh
'$basedir'/releases/'$subdir'/trusty/upload.sh
'$basedir'/releases/'$subdir'/vivid/upload.sh
'$basedir'/releases/'$subdir'/wily/upload.sh
'$basedir'/releases/'$subdir'/xenial/upload.sh
' > $basedir/releases/$subdir/upload_ppas.sh

chmod +x $basedir/releases/$subdir/upload_ppas.sh

# <revu>
cp $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz $basedir/release/grub-customizer_$version.orig.tar.gz
$sourcedir/debian/changelog.revu.d $version precise > debian/changelog
debuild -S;
mkdir $basedir/releases/$subdir/revu
mv $basedir/release/grub-customizer_* $basedir/releases/$subdir/revu
echo '#!/bin/sh
dput revu '$basedir'/releases/'$subdir'/revu/grub-customizer_'$version'-0ubuntu1_source.changes' > $basedir/releases/$subdir/revu/upload.sh
chmod +x $basedir/releases/$subdir/revu/upload.sh
# </revu>

# <mentors>
cp $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz $basedir/release/grub-customizer_$version.orig.tar.gz
$sourcedir/debian/changelog.mentors.d $version unstable > debian/changelog
cat $sourcedir/debian/control | grep -v '^Maintainer' | sed 's/XSBC-Original-Maintainer/Maintainer/' | sed 's/, *dpatch//' | sed 's/debhelper (>= 7.3)/debhelper (>= 8)/' > debian/control
rm -r debian/patches
cat $sourcedir/debian/rules | grep -v 'dpatch' > debian/rules
debuild -S;
mkdir $basedir/releases/$subdir/mentors
mv $basedir/release/grub-customizer_* $basedir/releases/$subdir/mentors
echo '#!/bin/sh
dput mentors '$basedir'/releases/'$subdir'/mentors/grub-customizer_'$version'-1_source.changes' > $basedir/releases/$subdir/mentors/upload.sh
chmod +x $basedir/releases/$subdir/mentors/upload.sh
# </mentors>

cd "$basedir"
sudo umount "$releasedir"
rmdir "$releasedir"
