call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"

premake4 vs2015

msbuild windows\vs2015\Waves.sln /p:Configuration=Debug
msbuild windows\vs2015\Waves.sln /p:Configuration=Release
