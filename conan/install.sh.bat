:<<BATCH
    @echo off
    echo Running batch script
    set CURRENT_DIR=%~dp0
    conan create "%CURRENT_DIR%\conanfile.py" "zhekehz/main"
    exit /b
BATCH

echo Running shell scipt
CURRENT_DIR="$( dirname -- "$0" )"
conan create "$CURRENT_DIR/conanfile.py" zhekehz/main
exit 0
