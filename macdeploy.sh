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

set -e

if [ "$1" == "" ] ; then
	T=release
else
	T=$1
fi

qmake -query | sed -e 's/:/=/g' > /tmp/qtconf
. /tmp/qtconf

DESTDIR=$PWD/build/$T/bin
APPNAME=DisplaceProject
APPBUNDLE=$DESTDIR/$APPNAME.app

EXECUTABLES="DisplaceProject displace dtreeeditor tseditor vsleditor"
EXTRA_FRAMEWORKS="GDAL"
EXTRA_LIBS="libGeographic libCGAL libgmp "
QT_PLUGINS="qsqlite qgif qjpeg qmng qtiff"

INSTALL_EXTRA_LIBS=""

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
		
		echo "$org_name" | egrep '@executable_path/' > /dev/null
		if [ $? -ne 0 ] ; then	
			lib_name=`basename $org_name`
			dest_name=$AB/Contents/Frameworks/$lib_name
			new_name=@executable_path/../Frameworks/$lib_name
			
			#
			# Check if the library is accessible. It could not if the library doesn't exists but is manually
			# installed by INSTALL_EXTRA_LIBS
			#
			if [ -r "$org_name" ] ; then
				#echo "COPYING: $org_name $dest_name"
				cp -R $org_name $dest_name
		
				#echo "Running: install_name_tool -id $new_name $dest_name"
				install_name_tool -id $new_name $dest_name || echo "- ***Error Running: install_name_tool -id $new_name $dest_name"
			fi
		fi
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
	
	echo "Copying framework: $src_file into $BUNDLE/Contents/Frameworks/$dst_file"
	
	if [ ! -d $BUNDLE/Contents/Frameworks/$dst_file ] ; then 
		cp -Ra $src_file $BUNDLE/Contents/Frameworks/$dst_file
	
		#echo "Running: install_name_tool -id @executable_path/../Frameworks/$dst_file $BUNDLE/Contents/Frameworks/$dst_lib"
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

	libpath=`otool -L $T | grep $LIB`
	if [ $? -eq 0 ] ; then			
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

########## Main ##########

if [ ! -e "$APPBUNDLE" ] ; then
	echo "$APPBUNDLE not found."
	exit
fi

echo "Deploying from: $T"

# manually copy the libraries

if [ "$INSTALL_EXTRA_LIBS" != "" ] ; then
	echo "Installing extra libs"
	cp $INSTALL_EXTRA_LIBS $APPBUNDLE/Contents/Frameworks/ 
fi

for app in $EXECUTABLES; do 
	echo "Deploying: $app"
	QTLIBS=`otool -L $APPBUNDLE/Contents/MacOS/$app | egrep 'Qt[^/]*\.framework' | egrep -v '@executable_path' | awk '{ print \$1 }' | sed -e's|@rpath/||g'`
	for qtlib in $QTLIBS; do
		copy_framework_qt $APPBUNDLE/Contents/MacOS/$app $QT_INSTALL_LIBS/$qtlib $APPBUNDLE
	done

	for fmw in $EXTRA_FRAMEWORKS ; do
		copy_framework /Library/Frameworks/$fmw.framework/Versions/Current/$fmw $fmw.framework $fmw.framework
	done
	
	for lib in $EXTRA_LIBS; do
		copy_lib $APPBUNDLE/Contents/MacOS/$app $lib $APPBUNDLE
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
for file in $CEXE $APPBUNDLE/Contents/Frameworks/* $PLUGINS; do
	echo $file | egrep '.*\.framework' > /dev/null
	if [ $? -ne 0 ] ; then
		for lib in $EXTRA_LIBS $QTLIBS; do
			update_links $file $lib $APPBUNDLE
		done
	fi
done

# Other, unrelated stuff

cp extra/license.html $APPBUNDLE/Contents/Resources
cp prelude/icons/document2.icns $APPBUNDLE/Contents/Resources
install_name_tool -change libsglmac-2.29.0.0.dylib.x86_64 libsglmac-2.29.0.0.dylib $APPBUNDLE/Contents/MacOS/updater
install_name_tool -change libsglmac-2.29.0.0.dylib.x86_64 libsglmac-2.29.0.0.dylib $APPBUNDLE/Contents/MacOS/Prelude_2.0

# Applying owner:group and access rights

echo "Applying permissions"
sudo chown -R root:staff $APPBUNDLE
sudo find $APPBUNDLE -type f -exec chmod 644 \{\} \;
sudo find $APPBUNDLE -type d -exec chmod 755 \{\} \;
for app in $EXECUTABLES; do
	sudo chmod +x $APPBUNDLE/Contents/MacOS/$app
done
