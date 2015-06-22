TARGETDIR = $$DESTDIR
BASEDIR = $$_PRO_FILE_PWD_

#BASEDIR_WIN = $$replace(BASEDIR,"/","\\")
#TARGETDIR_WIN = $$replace(TARGETDIR,"/","\\")

QMAKE_POST_LINK += $$quote(echo "Copying files"$$escape_expand(\\n))

QMAKE_POST_LINK += $$quote(xcopy /D /Y /E /I "$$BASEDIR_WIN\\styles" "$$TARGETDIR_WIN\\styles" $$escape_expand(\\n))
