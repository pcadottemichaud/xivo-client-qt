SetCompressor /FINAL /SOLID lzma

!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!include MultiUser.nsh
!include "MUI2.nsh"
!define MUI_ICON "xivoicon.ico"
!define MUI_UNICON "xivoicon_un.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP xivo.bmp
!define MUI_HEADERIMAGE_UNBITMAP xivo.bmp

Name "XiVO Client ${XC_VERSION}"
OutFile "xivoclient-${XC_VERSION}-x86.exe"

InstallDir $PROGRAMFILES\XiVO\

; Request application privileges for Windows Vista
RequestExecutionLevel admin
ShowInstDetails show
ShowUninstDetails show

!insertmacro MULTIUSER_PAGE_INSTALLMODE
!define MUI_PAGE_HEADER_SUBTEXT $(XIVO_LICENSE_TEXT)
!insertmacro MUI_PAGE_LICENSE gpl.txt

!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
Var Dialog
Var CheckboxShortcutDesktop
Var CheckboxShortcutStart
Var CheckboxShortcutDesktopState
Var CheckboxShortcutStartState

Page custom installMiscPage installMiscPageLeave ;Custom page

Function installMiscPage
  !insertmacro MUI_HEADER_TEXT $(SHORTCUT_PAGE_TITLE) ""
  nsDialogs::Create 1018
  Pop $Dialog

  ${If} $Dialog == error
    Abort
  ${EndIf}

  ${NSD_CreateCheckbox} 0 10u 100% 10u $(SHORTCUT_DESKTOP)
  Pop $CheckboxShortcutDesktop

  ${NSD_CreateCheckbox} 0 25u 100% 10u $(SHORTCUT_STARTMENU)
  Pop $CheckboxShortcutStart

  ${NSD_Check} $CheckboxShortcutDesktop
  ${NSD_Check} $CheckboxShortcutStart

  nsDialogs::Show

FunctionEnd

Function installMiscPageLeave
  ${NSD_GetState} $CheckboxShortcutDesktop $CheckboxShortcutDesktopState
  ${NSD_GetState} $CheckboxShortcutStart $CheckboxShortcutStartState
FunctionEnd

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;Languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

Function .onInit
  !insertmacro MULTIUSER_INIT
  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

!define REG_UNINST Software\Microsoft\Windows\CurrentVersion\Uninstall\xivoclient

; The stuff to install
Section "Qt libraries" QtLib ; components page
  SectionIn RO
  SetOutPath $INSTDIR

  File /r qtlibs\*
SectionEnd ; end the section

Section "XiVO client executable" XivoclientExe ; components page
  SectionIn RO
  SetOutPath $INSTDIR

  File /r xivoclient\*

  WriteRegStr HKLM "Software\XiVO\xivoclient" "Install_Dir" "$INSTDIR"
  WriteRegStr HKLM "${REG_UNINST}" "DisplayName" "XiVO Client"
  WriteRegStr HKLM "${REG_UNINST}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "${REG_UNINST}" "NoModify" 1
  WriteRegDWORD HKLM "${REG_UNINST}" "NoRepair" 1
  WriteUninstaller "$INSTDIR\uninstall.exe"
  # Associate with tel: and callto: protocol
  WriteRegStr HKCR "tel" "" "URL:tel Protocol"
  WriteRegStr HKCR "tel" "URL Protocol" ""
  WriteRegStr HKCR "tel\shell\open\command" "" "$INSTDIR\xivoclient.exe %1"
  WriteRegStr HKCR "callto" "" "URL:callto Protocol"
  WriteRegStr HKCR "callto" "URL Protocol" ""
  WriteRegStr HKCR "callto\shell\open\command" "" "$INSTDIR\xivoclient.exe %1"

  ${If} $CheckboxShortcutDesktopState == ${BST_CHECKED}
  ${OrIf} ${Silent}
    CreateShortCut "$DESKTOP\XiVO Client.lnk" "$INSTDIR\xivoclient.exe"
  ${EndIf}

  ${If} $CheckboxShortcutStartState == ${BST_CHECKED}
  ${OrIf} ${Silent}
     CreateDirectory "$SMPROGRAMS\XiVO"
     CreateShortCut  "$SMPROGRAMS\XiVO\\$(START_XIVOCLIENT)"  "$INSTDIR\xivoclient.exe"
     CreateShortCut  "$SMPROGRAMS\XiVO\\$(REMOVE_XIVOCLIENT)" "$INSTDIR\uninstall.exe"
  ${EndIf}
SectionEnd ; end the section

Section "Uninstall"
  SetShellVarContext all
  ; remove any shortcut
  Delete "$DESKTOP\XiVO Client.lnk"
  Delete "$SMPROGRAMS\XiVO\*"
  RmDir "$SMPROGRAMS\XiVO"
  ; remove the uri association
  DeleteRegKey HKLM ${REG_UNINST}
  DeleteRegKey HKLM "Software\XiVO\xivoclient"
  DeleteRegKey HKCR "tel"
  DeleteRegKey HKCR "callto"
  ; remove the program dir
  Delete $INSTDIR\uninstall.exe
  RMDir /r $INSTDIR\imageformats
  RMDir /r $INSTDIR\sqldrivers
  RMDir /r $INSTDIR\plugins
  Delete $INSTDIR\qt.conf
  RMDir /r $INSTDIR\Qt
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\xivoclient.exe

  RMDir $INSTDIR

SectionEnd ; end the section

Function un.onInit
  !insertMacro MULTIUSER_UNINIT
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd

LangString SuccessInstall ${LANG_ENGLISH}  "The installation was successful, do you want to start the XiVO client right now ?"
LangString SuccessInstall ${LANG_FRENCH}  "L'installation a r�ussi, voulez-vous lancer le XiVO client maintenant ?"

Function .onInstSuccess
  MessageBox MB_YESNO  $(SuccessInstall) /SD IDNO IDNO NoReadme
  ExecShell "open" $INSTDIR/xivoclient.exe
  NoReadme:
FunctionEnd

;Language strings

LangString XIVO_LICENSE_TEXT ${LANG_ENGLISH} "XiVO Client is licensed under the GNU GPLv3 with a special exception to allow the link with OpenSSL."
LangString XIVO_LICENSE_TEXT ${LANG_FRENCH}  "XiVO Client est distribu� sous licence GNU GPLv3 avec une exception particuli�re vous autorisant � le lier � OpenSSL."

LangString DESC_QtLib ${LANG_ENGLISH} "Qt libraries (cross-platform application and UI framework)"
LangString DESC_QtLib ${LANG_FRENCH}  "Biblioth�que Qt (framework multi plateforme g�rant l'IHM, le r�seau, ...) "

LangString DESC_xivoclientExe ${LANG_ENGLISH} "XiVO client executable"
LangString DESC_xivoclientExe ${LANG_FRENCH}  "Ex�cutable XiVO client"

LangString SHORTCUT_PAGE_TITLE ${LANG_ENGLISH} "Shortcuts"
LangString SHORTCUT_PAGE_TITLE ${LANG_FRENCH}  "Raccourcis"

LangString SHORTCUT_DESKTOP ${LANG_ENGLISH} "Add a Shortcut on &Desktop"
LangString SHORTCUT_DESKTOP ${LANG_FRENCH}  "Ajouter un raccourci sur le &Bureau"

LangString SHORTCUT_STARTMENU ${LANG_ENGLISH} "Add a Shortcut in Start &Menu"
LangString SHORTCUT_STARTMENU ${LANG_FRENCH}  "Ajouter un raccourci dans le &Menu D�marrer"

LangString START_XIVOCLIENT ${LANG_ENGLISH} "XiVO Client.lnk"
LangString START_XIVOCLIENT ${LANG_FRENCH}  "XiVO Client.lnk"

LangString REMOVE_XIVOCLIENT ${LANG_ENGLISH} "Uninstall XiVO Client.lnk"
LangString REMOVE_XIVOCLIENT ${LANG_FRENCH}  "D�sinstaller XiVO Client.lnk"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${QtLib} $(DESC_QtLib)
  !insertmacro MUI_DESCRIPTION_TEXT ${XivoclientExe} $(DESC_xivoclientExe)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

VIProductVersion "9.4.8.6"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "XiVO client"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Computer Telephony Integration (CTI) client for XiVO"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Avencall"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "XiVO client installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${XC_VERSION}-${GIT_HASH}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "� 2007-2013, Avencall"
