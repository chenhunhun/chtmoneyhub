RequestExecutionLevel admin

!include "version.nsi"
!include "winver.nsh"
!include "x64.nsh"
!include "wordfunc.nsh"
!include "FileFunc.nsh"
!include "LogicLib.nsh"
!include "nsDialogs.nsh"
;!include "Image.nsh"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "财金汇"
!define PRODUCT_PUBLISHER "北京融信恒通科技有限公司"
!define PRODUCT_WEB_SITE "http://www.finantech.cn"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\MoneyHub.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"


; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "setup.ico"
!define MUI_UNICON "uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "setup.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "setup.bmp"

; Welcome page
!define MUI_WELCOMEPAGE_TITLE "欢迎使用“财金汇”升级向导"
!define MUI_WELCOMEPAGE_TEXT "\r\n这个升级向导将指引你完成“财金汇”的升级过程。\r\n\r\n在开始升级之前，建议先关闭其他所有应用程序。这将允许“升级程序”更新指定的系统文件，而不需要重新启动你的计算机。\r\n\r\n单击[升级]开始升级过程。"
!insertmacro MUI_PAGE_WELCOME
; License page
;!insertmacro MUI_PAGE_LICENSE "license.txt"
; Directory page
;!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!define MUI_PAGE_HEADER_TEXT "正在升级"
!define MUI_PAGE_HEADER_SUBTEXT "“财金汇”正在升级，请等候..."
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_TITLE "正在完成“财金汇”升级向导"
!define MUI_FINISHPAGE_TEXT "“财金汇”已升级。$_CLICK"
!define MUI_FINISHPAGE_RUN "$INSTDIR\MoneyHub.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "SimpChinese"

; MUI end ------

Name "${PRODUCT_NAME}${PRODUCT_VERSION}"
OutFile "Output\Setup-${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\MoneyHub"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails nevershow
ShowUnInstDetails show
InstallButtonText "升级"
Caption "财金汇 升级"

BrandingText "融信恒通"

VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "ProductName" "财金汇"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "Comments" "财金汇安装程序"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "CompanyName" "融信恒通"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "LegalCopyright" "融信恒通版权所有"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileDescription" "财金汇安装程序"
VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileVersion" "${PRODUCT_VERSION}"
VIProductVersion "${PRODUCT_VERSION}"

Function .onInit

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Only One Instance                                                  ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  BringToFront
  System::Call "kernel32::CreateMutexA(i 0, i 0, t '$(^Name)') i .r0 ?e"
  Pop $0
  StrCmp $0 0 launch
   StrLen $0 "$(^Name)"
   IntOp $0 $0 + 1
  loop:
    FindWindow $1 '#32770' '' 0 $1
    IntCmp $1 0 +5
    System::Call "user32::GetWindowText(i r1, t .r2, i r0) i."
    StrCmp $2 "$(^Name)" 0 loop
    System::Call "user32::ShowWindow(i r1,i 9) i."         ; If minimized then maximize
    System::Call "user32::SetForegroundWindow(i r1) i."    ; Bring to front
    Abort
  launch:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 检查中文语言                                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  System::Call "Kernel32::GetSystemDefaultLangID(v ..) i .s"
  Pop $0
  IntOp $0 $0 & 0xFFFF
  
 ${If} $0 != 2052 
   MessageBox MB_ICONSTOP|MB_OK "Moneyhub can only be installed in the Simplified Chinese version of Windows."
   Abort
 ${EndIf}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 检查操作系统                                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
  ${If} ${RunningX64}
    MessageBox MB_ICONSTOP|MB_OK "财金汇不能运行在64位操作系统上"
    Quit
  ${EndIf}

  ${IfNot} ${AtLeastWinXP}
    MessageBox MB_ICONSTOP|MB_OK "财金汇需要Windows XP SP2或更新版本的操作系统"
    Quit
  ${EndIf}
  ${If} ${IsWinXP}
  ${AndIfNot} ${AtLeastServicePack} 2
    MessageBox MB_ICONSTOP|MB_OK "财金汇需要Windows XP SP2或更新版本的操作系统"
    Quit  
  ${EndIf} 
  ${If} ${IsWin2003}
    MessageBox MB_ICONSTOP|MB_OK "财金汇不能运行在Windows 2003上"
    Quit
  ${EndIf}
  ${If} ${IsWin2008}
    MessageBox MB_ICONSTOP|MB_OK "财金汇不能运行在Windows 2008上"
    Quit
  ${EndIf}
  ${If} ${IsWin2008R2}
    MessageBox MB_ICONSTOP|MB_OK "财金汇不能运行在Windows 2008 R2上"
    Quit
  ${EndIf}
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 检查本地版本                                                       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
  Push $R0
  ClearErrors
  ReadRegStr $R0 HKLM "${PRODUCT_DIR_REGKEY}" ""
  IfErrors done

  chkver:
  ReadRegStr $R2 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion"
  ${VersionCompare} "$R2" "${PRODUCT_VERSION}" $1
  ${If} $1 == "1"
    MessageBox MB_YESNO|MB_ICONQUESTION "已经安装财金汇$R2，是否继续升级?" IDYES done IDNO nocont
    nocont:
    MessageBox MB_YESNO|MB_ICONQUESTION "真的要退出升级?" IDNO chkver
    Quit
  ${EndIf}
    
  done:
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 程序是否正在运行中                                                 ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  instchk:
  FindWindow $0 "MONEYHUB_SIGNAL_WND"
  IntCmp $0 1 isexist notexist 
  isexist: 
  MessageBox MB_ICONEXCLAMATION|MB_RETRYCANCEL "检测到财金汇正在运行，升级无法继续！$\r$\n请关闭财金汇，并单击“重试”。$\r$\n或者单击“取消”退出安装。" IDRETRY instchk
  Abort
  notexist:    
FunctionEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 检查硬盘空间
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!define sysGetDiskFreeSpaceEx 'kernel32::GetDiskFreeSpaceExA(t, *l, *l, *l) i'
Function FreeDiskSpace
	System::Call '${sysGetDiskFreeSpaceEx}(r0,.,,.r1)'
	System::Int64Op $1 / 1024
	Pop $1
FunctionEnd

Section "MainSection"
	SetDetailsView hide
	;检查硬盘空间
	StrCpy $0 'C:\'
	Call FreeDiskSpace
	StrCpy $2 100000
	System::Int64Op $1 > $2
	Pop $3
	
	IntCmp $3 1 okay
	MessageBox MB_OK "磁盘空间不足，财金汇需要至少100MB硬盘空间，请释放空间后重试。"
	Quit
okay:
	SetOutPath "$INSTDIR"

	;检查当前系统中是否有财金汇正在运行  
	StrCpy $0 "MoneyHub.exe"
	KillProc::FindProcesses
	StrCmp $0 "0" completed1
killagain1:
	Sleep 1500
	StrCpy $0 "MoneyHub.exe"
	KillProc::KillProcesses
	StrCmp $1 "0" completed1
	Goto killagain1
completed1:
	StrCpy $0 "MoneyHub_Updater.exe"
	KillProc::FindProcesses
	StrCmp $0 "0" completed2
killagain2:
	Sleep 1500
	StrCpy $0 "MoneyHub_Updater.exe"
	KillProc::KillProcesses
	StrCmp $1 "0" completed2
	Goto killagain2
completed2:
	StrCpy $0 "moneyhub_pop.exe"
	KillProc::FindProcesses
	StrCmp $0 "0" completed3
killagain3:
	Sleep 1500
	StrCpy $0 "moneyhub_pop.exe"
	KillProc::KillProcesses
	StrCmp $1 "0" completed3
	Goto killagain3
completed3:
	;开始升级
	SetDetailsPrint none
	ExecWait '"$INSTDIR\MoneyHub.exe" -d'
	DeleteRegKey HKCU "Software\Bank"

	;删除更新过的银行控件
	;RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks\alipay"
	;RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks\cmbchina"
	;RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks\icbc"
	;RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks\bjrcb"
	;RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks\ecitic"

	File /r /x .svn /x *.lib /x *.exp /x *.pdb /x *.ilk /x *.map /x BankActiveXTest.html /x BankLoaderTester.exe /x DigsigGen.exe /x ModuleVerifierTester.exe /x usbkeyinfo.xml /x VerifyList.xml /x info.xml /x CloudCheck.ini /x syslist.txt /x BlackList.txt /x BillUrl.xml ..\_Bin\Debug\*.*
	;把license.dll和pecsp.dll放到system32下
	;SetOutPath "$WINDIR\System32"
	;File /r /x .svn ..\_Bin\Debug\BankInfo\banks\cmbc\license.dll
	;File /r /x .svn ..\_Bin\Debug\BankInfo\banks\sdb\pecsp.dll
	;SetOutPath "$INSTDIR"

	ExecWait '"$INSTDIR\MoneyHub.exe" -i' $0
	${If} $0 != 0
		DetailPrint "安装驱动失败！"
		MessageBox MB_OK|MB_ICONSTOP "安装驱动失败，请退出并重装！"
		Call DoUninstall
		
		CreateDirectory "$APPDATA\MoneyHub"
		FileOpen $0 "$APPDATA\MoneyHub\Update.log" a
		FileSeek $0 0 END
		${GetTime} "" "L" $1 $2 $3 $4 $5 $6 $7
		FileWrite $0 '[$3-$2-$1 $5:$6:$7] : 安装 财金汇${PRODUCT_VERSION} 失败'
		FileWrite $0 '$\r$\n'
		FileClose $0
		
		Quit
	${EndIf}

	ClearErrors
SectionEnd

Section -Post
	Exec '"$INSTDIR\moneyhub_svc.exe"'
	WriteUninstaller "$INSTDIR\MoneyHub_Uninst.exe"
	WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\MoneyHub.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\MoneyHub_Uninst.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\MoneyHub.exe"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
	
	Delete "$DESKTOP\升级财金汇.lnk"
	DeleteRegKey HKCU "Software\Bank\Update"
	
	CreateDirectory "$APPDATA\MoneyHub"
	FileOpen $0 "$APPDATA\MoneyHub\Update.log" a
	FileSeek $0 0 END
	${GetTime} "" "L" $1 $2 $3 $4 $5 $6 $7
	FileWrite $0 '[$3-$2-$1 $5:$6:$7] : 升级 财金汇${PRODUCT_VERSION}'
	FileWrite $0 '$\r$\n'
	FileClose $0
SectionEnd
