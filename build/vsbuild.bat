# Inits environment
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"

# Generates solution
premake4 vs2015

# Builds Debug & Release
msbuild windows\vs2015\Waves.sln /p:Configuration=Debug
msbuild windows\vs2015\Waves.sln /p:Configuration=Release

# Commits changes
hg commit -m "- Windows automated build"

# Pushes them
hg push
