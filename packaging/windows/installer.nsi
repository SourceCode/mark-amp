; MarkAmp NSIS Installer Script
; Usage: makensis /DVERSION="1.0.0" /DBUILD_DIR="..." /DPROJECT_DIR="..." /DOUTDIR="..." installer.nsi

!ifndef VERSION
  !define VERSION "1.0.0"
!endif

!ifndef OUTDIR
  !define OUTDIR "dist"
!endif

Name "MarkAmp"
OutFile "${OUTDIR}\MarkAmp-${VERSION}-Setup.exe"
InstallDir "$PROGRAMFILES64\MarkAmp"
InstallDirRegKey HKLM "Software\MarkAmp" "InstallDir"
RequestExecutionLevel admin
SetCompressor /SOLID lzma

; ── Modern UI ──
!include "MUI2.nsh"
!include "FileFunc.nsh"

!define MUI_ABORTWARNING
!define MUI_ICON "${PROJECT_DIR}\resources\icons\markamp.ico"
!define MUI_UNICON "${PROJECT_DIR}\resources\icons\markamp.ico"

; ── Pages ──
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${PROJECT_DIR}\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

; ── Sections ──
Section "MarkAmp (required)" SecMain
  SectionIn RO

  SetOutPath "$INSTDIR"

  ; Main executable
  File "${BUILD_DIR}\src\Release\markamp.exe"

  ; Resources
  SetOutPath "$INSTDIR\resources\themes"
  File /r "${PROJECT_DIR}\resources\themes\*.*"

  SetOutPath "$INSTDIR\resources\fonts"
  File /r "${PROJECT_DIR}\resources\fonts\*.*"

  SetOutPath "$INSTDIR\resources\sample_files"
  File /nonfatal /r "${PROJECT_DIR}\resources\sample_files\*.*"

  ; Write registry keys
  WriteRegStr HKLM "Software\MarkAmp" "InstallDir" "$INSTDIR"
  WriteRegStr HKLM "Software\MarkAmp" "Version" "${VERSION}"

  ; Uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Add/Remove Programs entry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MarkAmp" \
    "DisplayName" "MarkAmp"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MarkAmp" \
    "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MarkAmp" \
    "DisplayIcon" '"$INSTDIR\markamp.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MarkAmp" \
    "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MarkAmp" \
    "Publisher" "MarkAmp"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MarkAmp" \
    "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MarkAmp" \
    "NoRepair" 1

  ; Estimated size
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MarkAmp" \
    "EstimatedSize" "$0"
SectionEnd

Section "Desktop Shortcut" SecDesktop
  CreateShortCut "$DESKTOP\MarkAmp.lnk" "$INSTDIR\markamp.exe"
SectionEnd

Section "Start Menu Shortcut" SecStartMenu
  CreateDirectory "$SMPROGRAMS\MarkAmp"
  CreateShortCut "$SMPROGRAMS\MarkAmp\MarkAmp.lnk" "$INSTDIR\markamp.exe"
  CreateShortCut "$SMPROGRAMS\MarkAmp\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
SectionEnd

Section "File Associations (.md)" SecFileAssoc
  ; Register .md file association
  WriteRegStr HKCR ".md" "" "MarkAmp.MarkdownFile"
  WriteRegStr HKCR ".markdown" "" "MarkAmp.MarkdownFile"
  WriteRegStr HKCR ".mdown" "" "MarkAmp.MarkdownFile"
  WriteRegStr HKCR "MarkAmp.MarkdownFile" "" "Markdown Document"
  WriteRegStr HKCR "MarkAmp.MarkdownFile\DefaultIcon" "" '"$INSTDIR\markamp.exe",0'
  WriteRegStr HKCR "MarkAmp.MarkdownFile\shell\open\command" "" '"$INSTDIR\markamp.exe" "%1"'

  ; "Open with MarkAmp" context menu
  WriteRegStr HKCR "*\shell\MarkAmp" "" "Open with MarkAmp"
  WriteRegStr HKCR "*\shell\MarkAmp" "Icon" '"$INSTDIR\markamp.exe"'
  WriteRegStr HKCR "*\shell\MarkAmp\command" "" '"$INSTDIR\markamp.exe" "%1"'

  ; Notify shell of association change
  System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
SectionEnd

; ── Section descriptions ──
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} "MarkAmp application files (required)."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecDesktop} "Create a desktop shortcut."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecStartMenu} "Create Start Menu entries."
  !insertmacro MUI_DESCRIPTION_TEXT ${SecFileAssoc} "Associate .md, .markdown, and .mdown files with MarkAmp."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; ── Uninstaller ──
Section "Uninstall"
  ; Remove files
  RMDir /r "$INSTDIR\resources"
  Delete "$INSTDIR\markamp.exe"
  Delete "$INSTDIR\Uninstall.exe"
  RMDir "$INSTDIR"

  ; Remove shortcuts
  Delete "$DESKTOP\MarkAmp.lnk"
  RMDir /r "$SMPROGRAMS\MarkAmp"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\MarkAmp"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MarkAmp"

  ; Remove file associations
  DeleteRegKey HKCR "MarkAmp.MarkdownFile"
  DeleteRegKey HKCR "*\shell\MarkAmp"

  ; Notify shell
  System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
SectionEnd
