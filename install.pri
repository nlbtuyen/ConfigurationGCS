TARGETDIR = $$OUT_PWD

WinBuild {
        BASEDIR_WIN = $$replace(BASEDIR,"/","\\")
        TARGETDIR_WIN = $$replace(TARGETDIR,"/","\\")

        greaterThan(QT_MAJOR_VERSION, 4) {
                QTLIBDLLPFX = "Qt5"
                QTLIBDBGDLLSFX = "d.dll"
        } else {
                QTLIBDLLPFX = "Qt"
                QTLIBDBGDLLSFX = "d4.dll"
        }

        QMAKE_POST_LINK += $$quote(xcopy /D /Y /E /I "$$BASEDIR_WIN\\aq\\bin\\aq_win_all\\*" "$$TARGETDIR_WIN\\aq\\bin" $$escape_expand(\\n))

        QMAKE_POST_LINK += $$quote(xcopy /D /Y /E /I "$$BASEDIR_WIN\\styles\\*.css" "$$TARGETDIR_WIN\\styles" $$escape_expand(\\n\\t))


        ReleaseBuild {

                COPY_DLL_LIST = \
                        $$BASEDIR_WIN\\libs\\lib\\sdl\\win32\\SDL.dll \
                        $$(QTDIR)\\bin\\icu*.dll \

                QT_DLL_LIST = Core Gui Multimedia Network OpenGL Sql Svg Test WebKit Xml XmlPatterns
                QT_PLUGIN_LIST = imageformats iconengines
                greaterThan(QT_MAJOR_VERSION, 4) {
                        QT_DLL_LIST += Concurrent MultimediaWidgets Positioning PrintSupport Qml Quick Sensors WebChannel WebKitWidgets Widgets
                        QT_PLUGIN_LIST += mediaservice platforms
                }
                for(QT_DLL, QT_DLL_LIST) {
                        COPY_DLL_LIST += $$(QTDIR)\\bin\\$${QTLIBDLLPFX}$${QT_DLL}.dll
                }

                # Copy compiler-specific DLLs
                win32-msvc* {

                        MSVC_REDIST_DLL_PATH ~= s,Common7.{1}Tools.{1},VC\redist\x86\Microsoft.VC$${120}.CRT\,ig
                        COPY_DLL_LIST += "\"$${MSVC_REDIST_DLL_PATH}msvc?1?0.dll\""
                }
                win32-g++ {
                        # we need to know where MinGW lives so we can copy some DLLs from there.
                        MINGW_PATH = $$(MINGW_PATH)
                        isEmpty(MINGW_PATH): error("MINGW_PATH not found")
                        COPY_DLL_LIST += $${MINGW_PATH}\\bin\\libwinpthread-1.dll
                        COPY_DLL_LIST += $${MINGW_PATH}\\bin\\libstdc++-6.dll
                }

                # Copy VLC files
                contains(DEFINES, QGC_USE_VLC) {
                        #QMAKE_POST_LINK += $$quote(xcopy /D /Y "$$BASEDIR_WIN\\libs\\vlc\\plugins\\*"  "$$TARGETDIR_WIN\\plugins" /E /I $$escape_expand(\\n))
                        COPY_DLL_LIST += $$BASEDIR_WIN\\libs\\vlc\\libvlccore.dll
                        COPY_DLL_LIST += $$BASEDIR_WIN\\libs\\vlc\\libvlc.dll
                }

                # Copy all DLLs to base folder
                for(COPY_DLL, COPY_DLL_LIST) {
                        QMAKE_POST_LINK += $$quote(xcopy /D /Y "$${COPY_DLL}" "$$TARGETDIR_WIN"$$escape_expand(\\n))
                }
                # Copy all QT plugin DLLs and delete the debug ones
                for(QT_PLUGIN, QT_PLUGIN_LIST) {
                        QMAKE_POST_LINK += $$quote(xcopy /D /Y /E /I "$$(QTDIR)\\plugins\\$${QT_PLUGIN}\\*.dll" "$$TARGETDIR_WIN\\$${QT_PLUGIN}" $$escape_expand(\\n))
                        QMAKE_POST_LINK += $$quote(del /Q "$$TARGETDIR_WIN\\$${QT_PLUGIN}\\*$${QTLIBDBGDLLSFX}" $$escape_expand(\\n))
                }
                # clean up stuff not needed by release versions
                QMAKE_POST_LINK += $$quote(del /Q "$$TARGETDIR_WIN\\platforms\\qminimal*" $$escape_expand(\\n))
                QMAKE_POST_LINK += $$quote(del /Q "$$TARGETDIR_WIN\\platforms\\qoffscreen*" $$escape_expand(\\n))

        }  # end if release version

}
