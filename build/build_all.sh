#!/bin/sh
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

mkdir $basedir/releases/$subdir/launchpad-source
cp $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz $basedir/releases/$subdir/launchpad-source/grub-customizer_$version.tar.gz
gpg --armor --sign --detach-sig $basedir/releases/$subdir/launchpad-source/grub-customizer_$version.tar.gz

olddir=`pwd`
cd $basedir/releases/$subdir/launchpad-source
tar xzf grub-customizer_$version.tar.gz
cd grub-customizer-$version
cp -r $releasedir/grub-customizer-$version/debian debian
dpatch apply 02_gtkmm24.dpatch
if [ "$?" -ne 0 ] ; then echo 'fail'; exit; fi
dpatch apply 03_libarchive_old.dpatch
if [ "$?" -ne 0 ] ; then echo 'fail'; exit; fi
rm -rf debian
cd ..
tar -c grub-customizer-$version | gzip > $basedir/releases/$subdir/launchpad-source/grub-customizer_$version.gtk2.tar.gz
rm -rf grub-customizer-$version
gpg --armor --sign --detach-sig $basedir/releases/$subdir/launchpad-source/grub-customizer_$version.gtk2.tar.gz
cd "$olddir"

echo 7 > debian/compat
echo "01su-to-root.dpatch\n02_gtkmm24.dpatch\n03_libarchive_old.dpatch" > debian/patches/00list
cp debian/control debian/control.ori
cat debian/control.ori | sed 's/libgtkmm-3.0-dev/libgtkmm-2.4-dev/' > debian/control


# <lucid>
cp $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz $basedir/release/grub-customizer_$version.orig.tar.gz
$sourcedir/debian/changelog.d l lucid > debian/changelog
debuild -S;
mkdir $basedir/releases/$subdir/lucid
mv $basedir/release/grub-customizer_* $basedir/releases/$subdir/lucid
echo '#!/bin/sh
dput ppa:danielrichter2007/grub-customizer '$basedir'/releases/'$subdir'/lucid/grub-customizer_'$version'-0ubuntu1~ppa'$rev'l_source.changes' > $basedir/releases/$subdir/lucid/upload.sh
echo '#!/bin/sh
cd '$basedir/releases/$subdir/lucid' && export BD=build_`date +%s` && mkdir $BD && sudo mount none $BD -t tmpfs && tar -xzf *.orig.tar.gz -C $BD && tar -xzf *.debian.tar.gz -C $BD/* && cp *.orig.tar.gz $BD/ && cd $BD/* && dpkg-buildpackage && cd ../.. && mv $BD/*.deb ./ && sudo umount $BD && rm -rf $BD' > $basedir/releases/$subdir/lucid/compile.sh
chmod +x $basedir/releases/$subdir/lucid/*.sh
# </lucid>

echo 8 > debian/compat


dpatch deapply-all
echo -n "" > debian/patches/00list
mv debian/control.ori debian/control


# <precise>
cp $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz $basedir/release/grub-customizer_$version.orig.tar.gz
$sourcedir/debian/changelog.d p precise > debian/changelog
debuild -S;
mkdir $basedir/releases/$subdir/precise
mv $basedir/release/grub-customizer_* $basedir/releases/$subdir/precise
echo '#!/bin/sh
dput ppa:danielrichter2007/grub-customizer '$basedir'/releases/'$subdir'/precise/grub-customizer_'$version'-0ubuntu1~ppa'$rev'p_source.changes' > $basedir/releases/$subdir/precise/upload.sh
echo '#!/bin/sh
cd '$basedir/releases/$subdir/precise' && export BD=build_`date +%s` && mkdir $BD && sudo mount none $BD -t tmpfs && tar -xzf *.orig.tar.gz -C $BD && tar -xzf *.debian.tar.gz -C $BD/* && cp *.orig.tar.gz $BD/ && cd $BD/* && dpkg-buildpackage && cd ../.. && mv $BD/*.deb ./ && sudo umount $BD && rm -rf $BD' > $basedir/releases/$subdir/precise/compile.sh
chmod +x $basedir/releases/$subdir/precise/*.sh
# </precise>

# <quantal>
cp $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz $basedir/release/grub-customizer_$version.orig.tar.gz
$sourcedir/debian/changelog.d q quantal > debian/changelog
debuild -S;
mkdir $basedir/releases/$subdir/quantal
mv $basedir/release/grub-customizer_* $basedir/releases/$subdir/quantal
echo '#!/bin/sh
dput ppa:danielrichter2007/grub-customizer '$basedir'/releases/'$subdir'/quantal/grub-customizer_'$version'-0ubuntu1~ppa'$rev'q_source.changes' > $basedir/releases/$subdir/quantal/upload.sh
echo '#!/bin/sh
cd '$basedir/releases/$subdir/quantal' && export BD=build_`date +%s` && mkdir $BD && sudo mount none $BD -t tmpfs && tar -xzf *.orig.tar.gz -C $BD && tar -xzf *.debian.tar.gz -C $BD/* && cp *.orig.tar.gz $BD/ && cd $BD/* && dpkg-buildpackage && cd ../.. && mv $BD/*.deb ./ && sudo umount $BD && rm -rf $BD' > $basedir/releases/$subdir/quantal/compile.sh
chmod +x $basedir/releases/$subdir/quantal/*.sh
# </quantal>

# <raring>
cp $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz $basedir/release/grub-customizer_$version.orig.tar.gz
$sourcedir/debian/changelog.d r raring > debian/changelog
debuild -S;
mkdir $basedir/releases/$subdir/raring
mv $basedir/release/grub-customizer_* $basedir/releases/$subdir/raring
echo '#!/bin/sh
dput ppa:danielrichter2007/grub-customizer '$basedir'/releases/'$subdir'/raring/grub-customizer_'$version'-0ubuntu1~ppa'$rev'r_source.changes' > $basedir/releases/$subdir/raring/upload.sh
echo '#!/bin/sh
cd '$basedir/releases/$subdir/raring' && export BD=build_`date +%s` && mkdir $BD && sudo mount none $BD -t tmpfs && tar -xzf *.orig.tar.gz -C $BD && tar -xzf *.debian.tar.gz -C $BD/* && cp *.orig.tar.gz $BD/ && cd $BD/* && dpkg-buildpackage && cd ../.. && mv $BD/*.deb ./ && sudo umount $BD && rm -rf $BD' > $basedir/releases/$subdir/raring/compile.sh
chmod +x $basedir/releases/$subdir/raring/*.sh
# </raring>

# <saucy>
cp $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz $basedir/release/grub-customizer_$version.orig.tar.gz
$sourcedir/debian/changelog.d s saucy > debian/changelog
debuild -S;
mkdir $basedir/releases/$subdir/saucy
mv $basedir/release/grub-customizer_* $basedir/releases/$subdir/saucy
echo '#!/bin/sh
dput ppa:danielrichter2007/grub-customizer '$basedir'/releases/'$subdir'/saucy/grub-customizer_'$version'-0ubuntu1~ppa'$rev's_source.changes' > $basedir/releases/$subdir/saucy/upload.sh
echo '#!/bin/sh
cd '$basedir/releases/$subdir/saucy' && export BD=build_`date +%s` && mkdir $BD && sudo mount none $BD -t tmpfs && tar -xzf *.orig.tar.gz -C $BD && tar -xzf *.debian.tar.gz -C $BD/* && cp *.orig.tar.gz $BD/ && cd $BD/* && dpkg-buildpackage && cd ../.. && mv $BD/*.deb ./ && sudo umount $BD && rm -rf $BD' > $basedir/releases/$subdir/saucy/compile.sh
chmod +x $basedir/releases/$subdir/saucy/*.sh
# </saucy>

# <trusty>
cp $basedir/releases/$subdir/grub-customizer_$version.orig.tar.gz $basedir/release/grub-customizer_$version.orig.tar.gz
$sourcedir/debian/changelog.d t trusty > debian/changelog
debuild -S;
mkdir $basedir/releases/$subdir/trusty
mv $basedir/release/grub-customizer_* $basedir/releases/$subdir/trusty
echo '#!/bin/sh
dput ppa:danielrichter2007/grub-customizer '$basedir'/releases/'$subdir'/trusty/grub-customizer_'$version'-0ubuntu1~ppa'$rev't_source.changes' > $basedir/releases/$subdir/trusty/upload.sh
echo '#!/bin/sh
cd '$basedir/releases/$subdir/trusty' && export BD=build_`date +%s` && mkdir $BD && sudo mount none $BD -t tmpfs && tar -xzf *.orig.tar.gz -C $BD && tar -xzf *.debian.tar.gz -C $BD/* && cp *.orig.tar.gz $BD/ && cd $BD/* && dpkg-buildpackage && cd ../.. && mv $BD/*.deb ./ && sudo umount $BD && rm -rf $BD' > $basedir/releases/$subdir/trusty/compile.sh
chmod +x $basedir/releases/$subdir/trusty/*.sh
# </trusty>

echo '#!/bin/sh
'$basedir'/releases/'$subdir'/lucid/upload.sh
'$basedir'/releases/'$subdir'/precise/upload.sh
'$basedir'/releases/'$subdir'/quantal/upload.sh
'$basedir'/releases/'$subdir'/raring/upload.sh
'$basedir'/releases/'$subdir'/saucy/upload.sh
'$basedir'/releases/'$subdir'/trusty/upload.sh
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
