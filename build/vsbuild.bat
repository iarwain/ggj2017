REM Inits environment
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"

REM Generates solution
REM premake4 vs2015

REM Builds Debug & Release
msbuild windows\vs2015\Waves.sln /p:Configuration=Debug /p:Platform=x64
msbuild windows\vs2015\Waves.sln /p:Configuration=Release /p:Platform=x64

REM Commits changes
hg commit -m "- Windows automated build"

REM Pushes them
hg push
