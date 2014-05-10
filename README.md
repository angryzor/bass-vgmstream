bass-vgmstream
==============

BASS plugin that plays vgmstream compatible music.
BASS is an easy-to-use cross platform audio interface. vgmstream is
a library that can render video game music formats.

This project is a simple plugin for BASS that allows people to
easily use vgmstream within their bass project. Simply call
`BASS_VGMSTREAM_StreamCreate` with a filename and some stream flags to
create a BASS stream from a file readable by vgmstream.

Compiling on Windows
====================
Getting set up
--------------
You'll need a few things before you can compile this:

* BASS itself. Download it from http://www.un4seen.com/
* vgmstream. Extract the repository at http://sourceforge.net/p/vgmstream/code/HEAD/tree/
* The vgmstream external libraries. They are available at http://hcs64.com/files/vgmstream_external_dlls.zip

Now, if you're planning to build with Visual Studio (probably, since i don't have
a Makefile yet), set 2 environment variables *before opening VS* (otherwise VS will still have
the old environment):

* BASS_HOME: Set this to the location of the `c` folder inside the BASS folder.
* VGMSTREAM_HOME: Set this to the location of the vgmstream repository.

Building vgmstream
------------------
Building vgmstream is pretty straightforward. Just set it to the Release configuration and
start the build. You only need to build the projects `ext_libs` and `libvgmstream`.

Building bass-vgmstream
-----------------------
Set the build configuration to Release. You should now simply be able to build the project
and get the binary.

NOTE: By default, I set the project to use the MultiThreaded DLL runtime. This is because vgmstream uses
MT DLL and since vgmstream is a static library the runtimes must be the same. If you want to use a
static runtime, you'll also have to change vgmstream's runtime.

Required binaries
=================
Your application should have the following binaries available:

* bass.dll: BASS itself
* bass_vgmstream.dll: BASS_VGMSTREAM
* libg7221_decode.dll, libmpg123-0.dll and libvorbis.dll: from the vgmstream external libraries zip.
