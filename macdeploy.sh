#!/bin/sh
#
# Configuration
#
# DESTDIR: Wherr the app bundle is located
# APPNAME: The name of the Main Application
# APPBUNDLE: The name of the Application Bundle
# EXECUTABLES: The list of the executables installed in Contents/MacOS/
# EXTRA_LIBS: The list of extra library that must be installed in the bundle
# QT_PLUGINS: The list of the QT plugins that must be installed in the bundle
# INSTALL_EXTRA_LIBS: The list of the extra libraries that must be copied manually, with the complete path
#
# The programs automatically identify the Qt Libraries to install in the bundle.
# It must be told about all the external libraries that must me installed, automatically or manually.
#

#set -e

help() {
cat << EOF
Usage: 
	macdeploy.sh [-t type] [-nb] [-nf]
	macdeploy.sh -h
	
		-t type				Define where to find the binaries. default is "release"
		-nb					No Bundle. Do not create dmg
		-nf					No Fix. Do not fix the bundle
		-h					help

EOF
}

#copy_lib
# copy a library in the application bundle Frameworks directory
#
# usage:
#	copy_lib object library appbundle
#
#	object: The library file
#	library: The library name
#	appbundle: The application bundle path
copy_lib()
{
	T=$1
	LIB=$2
	AB=$3

	#echo "checking $T against $LIB"
	libpath=`otool -L $T | grep $LIB`
	if [ $? -eq 0 ] ; then
		org_name=`otool -L $T | grep $LIB | egrep -v '\:$' | awk '{ print \$1 }'`
		echo "$org_name" | egrep '@executable_path/'
		RES=$?
		#echo "result: $RES"
		if [ $RES -ne 0 ] ; then
			lib_name=`basename $org_name`
			dest_name=$AB/Contents/Frameworks/$lib_name
			new_name=@executable_path/../Frameworks/$lib_name

			#
			# Check if the library is accessible. It could not if the library doesn't exists but is manually
			# installed by INSTALL_EXTRA_LIBS
			#
			if [ ! -r "$org_name" ] ; then
				echo "$org_name not found, trying with /usr/local/lib"
				org_name=/usr/local/lib/$org_name
			fi

			if [ -r "$org_name" ] ; then
				echo "COPYING: $org_name $dest_name"
				cp -L $org_name $dest_name || exit 1
                                chmod u+w $dest_name

				echo "Running: install_name_tool -id $new_name $dest_name"
				install_name_tool -id $new_name $dest_name || echo "- ***Error Running: install_name_tool -id $new_name $dest_name"
			else
				echo "Cannot read $org_name (`otool -L $T | grep $LIB`)"
			fi
		fi
	#else
		#echo "No $LIB found"
	fi
}

#copy_framework_qt
# copy a framework in the application bundle Frameworks directory
#
# usage:
#	copy_framework object, framework appbundle
#
# see copy_lib for details, works the same way
copy_framework_qt()
{
	T=$1
	WF=$2
	BUNDLE=$3

	src_file=`echo $WF | sed -e 's/\(Qt[^/]*\.framework\)\/.*$/\1/g'`
	dst_file=`echo $WF | sed -e 's/^.*\/\(Qt[^/]*\.framework\)\/.*$/\1/g'`
	dst_lib=`echo $WF | sed -e 's/^.*\/\(Qt[^/]*\.framework\/.*\)$/\1/g'`
	echo "Copying framework: $WF $src_file into $BUNDLE/Contents/Frameworks/$dst_file"

	if [ ! -d $BUNDLE/Contents/Frameworks/$dst_file ] ; then
		cp -Ra $src_file $BUNDLE/Contents/Frameworks/$dst_file
                chmod -R u+w $BUNDLE/Contents/Frameworks/$dst_file

		#echo "Running: install_name_tool -id @executable_path/../Frameworks/$dst_file $BUNDLE/Contents/Frameworks/$dst_lib"
		install_name_tool -id @executable_path/../Frameworks/$dst_lib $BUNDLE/Contents/Frameworks/$dst_lib \
			|| echo "- ***Error Running: install_name_tool -id @executable_path/../Frameworks/$dst_lib $BUNDLE/Contents/Frameworks/$dst_lib"
	fi
}

#copy_framework
copy_framework()
{
	src_file=$1
	dst_file=$2
	dst_lib=$3
	BUNDLE=$4

	echo "Copying framework: $src_file into $BUNDLE/Contents/Frameworks/$dst_file"

	if [ ! -d $BUNDLE/Contents/Frameworks/$dst_file ] ; then
		cp -Ra $src_file $BUNDLE/Contents/Frameworks/$dst_file || exit 1
                chmod -R u+w $BUNDLE/Contents/Frameworks/$dst_file

		echo "Running: install_name_tool -id @executable_path/../Frameworks/$dst_file $BUNDLE/Contents/Frameworks/$dst_lib"
		install_name_tool -id @executable_path/../Frameworks/$dst_lib $BUNDLE/Contents/Frameworks/$dst_lib \
			|| echo "- ***Error Running: install_name_tool -id @executable_path/../Frameworks/$dst_lib $BUNDLE/Contents/Frameworks/$dst_lib"
	fi
}


#copy_plugins
# copy the QT plugins list
#
# usage:
#	copy_plugins appbundle
#
#	appbundle: The application bundle
#
#
copy_plugins()
{
	AB=$1
	QTCONF=$AB/Contents/Resources/qt.conf

	for lib in $QT_PLUGINS; do
		srcs=`find $QT_INSTALL_PLUGINS -name \*$lib\*.dylib`

		for src in $srcs ; do
			dst=`echo $src | sed -e "s*$QT_INSTALL_PLUGINS**g"`
			dstdir=plugins/`dirname $dst`

			#echo "Installing plugin $lib: $src $dstdir"

			mkdir -p $APPBUNDLE/Contents/$dstdir
			cp $src $APPBUNDLE/Contents/$dstdir
                        chmod -R u+w $APPBUNDLE/Contents/$dstdir

			new_name=@executable_path/../plugins/$dst

			#echo "Running: install_name_tool -id $new_name $APPBUNDLE/Contents/$dst"
			install_name_tool -id $new_name $APPBUNDLE/Contents/plugins/$dst \
				|| echo "- ***Error Running: install_name_tool -id $new_name $APPBUNDLE/Contents/plugins/$dst"

		done
	done

	find $AB/Contents/plugins -name \*_debug.dylib -exec rm \{\} \;

	cat > $QTCONF << __EOF__
[Paths]
Plugins=plugins
__EOF__
}

#update_links
# updates the links to the dynamic libraries installed in the bundle, using the install_name_tool -change
# argument 4: Framework, if set, appends ".framework/name" to new_name
#
update_links()
{
	T=$1
	LIB=$2
	AB=$3
	FRAMEWORK=$4

	echo "++ Checking $LIB in $T"
	libpath=`otool -L $T | grep $LIB`
	if [ $? -eq 0 ] ; then
		echo "++ FOUND"
		#echo "Testing: otool -L $T | grep $LIB | awk '{ print \$1 }'"
		org_name=`otool -L $T | grep $LIB | egrep -v '\:$' | head -1 | awk '{ print \$1 }'`
		echo "$org_name" | egrep '@executable_path/' > /dev/null
		if [ $? -ne 0 ] ; then
			dest_name=$T
			new_name=`echo $org_name | sed -e "s/^.*$LIB/@executable_path\/..\/Frameworks\/$LIB/"`

			if [ "$FRAMEWORK" != "" ] ; then
				new_name="$new_name.framework/$LIB"
			fi

			#echo "Running: install_name_tool -change $org_name $new_name $dest_name"
			install_name_tool -change $org_name $new_name $dest_name || echo "- ***Error Running: install_name_tool -change $org_name $new_name $dest_name"
		fi
	fi
}

fix_links() {
	file=$1

	for lib in $EXECUTABLES $EXTRA_LIBS $QTLIBS ; do
		update_links $file $lib $APPBUNDLE
	done
	for lib in $EXTRA_FRAMEWORKS; do
		update_links $file $lib.framework $APPBUNDLE
	done
}

fix_bundle() {

# manually copy the libraries
if [ "$INSTALL_EXTRA_LIBS" != "" ] ; then
        echo "Installing extra libs"
        cp $INSTALL_EXTRA_LIBS $APPBUNDLE/Contents/Frameworks/
        chmod -R u+w $APPBUNDLE/Contents/Frameworks/
fi

for app in $EXECUTABLES; do
        echo "Deploying: $app"
        if [ ! -r $APPBUNDLE/Contents/MacOS/$app ] ; then
                echo "Exe Contents/MacOS/$app not readable."
                exit 1
        fi

        QTLIBS=`otool -L $APPBUNDLE/Contents/MacOS/$app | egrep 'Qt[^/]*\.framework' | egrep -v '@executable_path' | awk '{ print \$1 }' | sed -e's|@rpath/||g'`
        for qtlib in $QTLIBS; do
                copy_framework_qt $APPBUNDLE/Contents/MacOS/$app $QT_INSTALL_LIBS/$qtlib $APPBUNDLE
        done

        for qtlib in $EXTRA_QT_FRAMEWORKS; do
                copy_framework $QT_INSTALL_LIBS/$qtlib.framework $qtlib.framework $qtlib.framework/Versions/Current/$qtlib $APPBUNDLE
        done

        for fmw in $EXTRA_FRAMEWORKS ; do
                copy_framework $fmw.framework $fmw.framework $fmw.framework/Versions/Current/$fmw $APPBUNDLE
        done

        for lib in $EXTRA_LIBS; do
                copy_lib $APPBUNDLE/Contents/MacOS/$app $lib $APPBUNDLE
        done
done

for dlib in $APPBUNDLE/Contents/Frameworks/*.dylib ; do
        for lib in $EXTRA_LIBS; do
                copy_lib $dlib $lib $APPBUNDLE
        done
done

# Remove the _debug libraries
echo "Removing debug libraries"
find $APPBUNDLE/Contents/Frameworks/ -name \*_debug -exec rm \{\} \;

echo "Installing QT plugins"
copy_plugins $APPBUNDLE

QTLIBS=`ls -1d $APPBUNDLE/Contents/Frameworks/Qt*.framework 2>/dev/null || exit 0`
QTLIBS=`basename $QTLIBS 2>/dev/null || exit 0`

CEXE=""
for app in $EXECUTABLES; do
        CEXE="$CEXE $APPBUNDLE/Contents/MacOS/$app"
done

PLUGINS="`find $APPBUNDLE/Contents/plugins -name \*.dylib`"

echo "Updating QT dynamic library reference"
for file in $QTLIBS; do
        filename=`echo $file | sed -e 's/\.framework//g'`
        for lib in $QTLIBS; do
                libname=`echo $lib | sed -e 's/\.framework//g'`
                update_links $APPBUNDLE/Contents/Frameworks/$file/$filename $libname $APPBUNDLE yes
        done
done

echo "Updating dynamic library references"
for file in $CEXE $APPBUNDLE/Contents/Frameworks/*.dylib $PLUGINS ; do
        #echo $file | egrep '.*\.framework' > /dev/null
        #if [ $? -ne 0 ] ; then
        echo "Fixing: $file"
        fix_links $file
        #fi
done

for file in $EXTRA_FRAMEWORKS; do
        echo $file | egrep '.*\.framework' > /dev/null
        if [ $? -ne 0 ] ; then
                fix_links $APPBUNDLE/Contents/Frameworks/$file.framework/Versions/Current/$file
        fi
done

# Other, unrelated stuff

# Fix CGAL links
for i in chrono date_time atomic ; do
    install_name_tool -change /usr/local/opt/boost/lib/libboost_$i-mt.dylib @executable_path/../Frameworks/libboost_$i-mt.dylib $APPBUNDLE/Contents/Frameworks/libCGAL.11.dylib
done

# Fix program_options in displace

install_name_tool -change /usr/local/opt/boost/lib/libboost_program_options.dylib @executable_path/../Frameworks/libboost_program_options.dylib $APPBUNDLE/Contents/MacOS/displace

# Copy of R Scripts

mkdir $APPBUNDLE/Contents/Resources/scripts
cp $TOPDIR/scripts/*.R $APPBUNDLE/Contents/Resources/scripts
chmod -R u+w $APPBUNDLE/Contents/Resources/scripts

}

create_dmg() {
# Applying owner:group and access rights
echo "Applying permissions"
chmod -R -w $APPBUNDLE
sudo chown -R root:staff $APPBUNDLE
sudo find $APPBUNDLE -type f -exec chmod 644 \{\} \;
sudo find $APPBUNDLE -type d -exec chmod 755 \{\} \;
for app in $EXECUTABLES; do
        sudo chmod +x $APPBUNDLE/Contents/MacOS/$app
done

rm -rf $INSTALL/install
mkdir $INSTALL/install
cp -a $APPBUNDLE $INSTALL/install/

$TOOLDIR/scripts/create-dmg.sh \
    --app-drop-link 20 20 \
    --volname "Displace Project for MacOS" \
    $INSTALL/displace-mac.dmg $INSTALL/install/


}

########## Main ##########

T=release
OPT_NOBUNDLE=
OPT_NOFIX=

while [ "$1" != "" ] ; do
        case "$1" in
                -t)
                        shift
                        T=$1
                        ;;
                -nb)
                        OPT_NOBUNDLE=y
                        ;;
                -nf)
                        OPT_NOFIX=y
                        ;;
                -h)
                        help
                        exit 0
                        ;;
                *)
                        echo "Options unknown, use -h to have help."
                        exit 1
                        ;;
        esac
        shift
done

QMAKE=`which qmake`
if [ "$QMAKE" == "" ] ; then
        echo "qmake not in path."
        QMAKE=~/Qt/5.5/clang_64/bin/qmake
        if [ ! -r $QMAKE ] ; then
                echo "Cannot use wired in qmake. please fix paths."
                exit 1
        fi
fi

$QMAKE -query | sed -e 's/:/=/g' > /tmp/qtconf
. /tmp/qtconf

QT_INSTALL_LIBS=~/Qt/5.12.7/clang_64/lib/
QT_INSTALL_PLUGINS=~/Qt/5.12.7/clang_64/plugins/
TOPDIR=$PWD
DESTDIR=$PWD/build/$T/bin
INSTALL=$PWD/build
APPNAME=DisplaceProject
APPBUNDLE=$DESTDIR/$APPNAME.app

EXECUTABLES="DisplaceProject displace dtreeeditor tsereditor objeditor"
EXTRA_QT_FRAMEWORKS="QtDBus"
EXTRA_FRAMEWORKS=""
# EXTRA_LIBS must be found as dependency in EXECUTABLES
EXTRA_LIBS="libmsqlitecpp libsqlite3 libGeographic libCGAL libmpfr libgmp libboost_thread libboost_system"
QT_PLUGINS="cocoa qsqlite qgif qjpeg qmng qtiff"

# These INSTALL_EXTRA_LIBS will be installed regardless of the links
INSTALL_EXTRA_LIBS="/usr/local/lib/libmsqlitecpp.1.dylib /usr/local/lib/libCGAL.11.dylib /usr/local/opt/boost/lib/libboost_date_time-mt.dylib /usr/local/opt/boost/lib/libboost_chrono-mt.dylib /usr/local/opt/boost/lib/libboost_atomic-mt.dylib /usr/local/opt/boost/lib/libboost_system-mt.dylib /usr/local/opt/boost/lib/libboost_program_options.dylib"


TOOLDIR=`dirname $0`

if [ ! -e "$APPBUNDLE" ] ; then
	echo "$APPBUNDLE not found."
	exit
fi

echo "Deploying from: $T"

if [ "$OPT_NOFIX" == "" ] ; then
	fix_bundle
fi

if [ "$OPT_NOBUNDLE" == "" ] ; then
	create_dmg
fi

