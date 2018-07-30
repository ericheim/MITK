# Install MITK icon and logo

MITK_INSTALL(FILES
  "${MITK_SOURCE_DIR}/mitk.ico"
  "${MITK_SOURCE_DIR}/mitk.bmp")

# Install Qt plugins

if(MITK_USE_Qt5)
  get_property(_qmake_location TARGET ${Qt5Core_QMAKE_EXECUTABLE} PROPERTY IMPORT_LOCATION)
  get_filename_component(_qmake_path ${_qmake_location} DIRECTORY)

  if(WIN32)
    set(_prefix "")
    set(_ext ".dll")
  elseif(UNIX)
    set(_prefix "lib")
    if(APPLE)
      set(_ext ".dylib")
    else()
      set(_ext ".so")
    endif()
  endif()

  set(_install_DESTINATION "plugins/sqldrivers")
  MITK_INSTALL(FILES "${_qmake_path}/../plugins/sqldrivers/${_prefix}qsqlite${_ext}")

  set(_install_DESTINATION "plugins/imageformats")
  MITK_INSTALL(FILES "${_qmake_path}/../plugins/imageformats/${_prefix}qsvg${_ext}")

  set(_install_DESTINATION "plugins/iconengines")
  MITK_INSTALL(FILES "${_qmake_path}/../plugins/iconengines/${_prefix}qsvgicon${_ext}")

  # Install platform-specific Qt plugins

  set(_install_DESTINATION "plugins/platforms")

  if(WIN32)
    MITK_INSTALL(FILES "${_qmake_path}/../plugins/platforms/qwindows.dll")
  elseif(APPLE)
    MITK_INSTALL(FILES "${_qmake_path}/../plugins/platforms/libqcocoa.dylib")

    set(_install_DESTINATION "plugins/styles")
    MITK_INSTALL(FILES "${_qmake_path}/../plugins/styles/libqmacstyle.dylib")
  elseif(UNIX)
    MITK_INSTALL(FILES
      "${_qmake_path}/../plugins/platforms/libqxcb.so"
      "${_qmake_path}/../plugins/xcbglintegrations/libqxcb-glx-integration.so")
  endif()

  # Install Qt WebEngine

  if(APPLE)
    set(_install_DESTINATION "../Frameworks/QtWebEngineCore.framework")

    get_filename_component(_real_path "${_qmake_path}/../lib/QtWebEngineCore.framework/Helpers" REALPATH)
    MITK_INSTALL(DIRECTORY ${_real_path} USE_SOURCE_PERMISSIONS)

    # Translations are included in the Resources directory of
    # QtWebEngineCore.framework and are installed by default.
  else()
    set(_install_DESTINATION "")

    if(WIN32)
      MITK_INSTALL(FILES "${_qmake_path}/QtWebEngineProcess.exe")
    elseif(UNIX)
      MITK_INSTALL(FILES "${_qmake_path}/../libexec/QtWebEngineProcess")
    endif()

    MITK_INSTALL(DIRECTORY "${_qmake_path}/../resources")

    set(_install_DESTINATION "translations")
    MITK_INSTALL(DIRECTORY "${_qmake_path}/../translations/qtwebengine_locales")
  endif()
endif()

set(_install_DESTINATION "") # IMPORTANT: Restore default install destination!
