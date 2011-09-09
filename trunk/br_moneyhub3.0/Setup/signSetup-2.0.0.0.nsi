RequestExecutionLevel admin

!include "version.nsi"
!include "winver.nsh"
!include "x64.nsh"
!include "wordfunc.nsh"
!include "FileFunc.nsh"
!include "LogicLib.nsh"
!include "nsDialogs.nsh"

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "财金汇"
!define PRODUCT_PUBLISHER "北京融信恒通科技有限公司"
!define PRODUCT_WEB_SITE "http://www.finantech.cn"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\MoneyHub.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

!define PWD_DIR "."
!define BUILDTOOL "build_tool" 

!ifdef INNER
	;;=================================================================================
	;;===== INNER =====================================================================
	;;=================================================================================
	
	; MUI 1.67 compatible ------
	!include "MUI.nsh"
	
	!define MUI_ABORTWARNING
	!define MUI_ICON "setup.ico"
	!define MUI_UNICON "uninstall.ico"
	!define MUI_WELCOMEFINISHPAGE_BITMAP "setup.bmp"
	!define MUI_UNWELCOMEFINISHPAGE_BITMAP "setup.bmp"
	
	!insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_LANGUAGE "SimpChinese"
	
	; MUI end ------
	
	Name "${PRODUCT_NAME}${PRODUCT_VERSION}"
	OutFile "$%TEMP%\tempinstaller.exe"
	SetCompress off
	ShowUnInstDetails show
	
	SilentInstall silent
	
	BrandingText "融信恒通"
	
	VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "ProductName" "财金汇"
	VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "Comments" "财金汇卸载程序"
	VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "CompanyName" "融信恒通"
	VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "LegalCopyright" "融信恒通版权所有"
	VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileDescription" "财金汇卸载程序"
	VIAddVersionKey /LANG=${LANG_SIMPCHINESE} "FileVersion" "${PRODUCT_VERSION}"
	VIProductVersion "${PRODUCT_VERSION}"
	
	Function .onInit
	  WriteUninstaller "$%TEMP%\MoneyHub_Uninst.exe"
	FunctionEnd
	
	Section "MainSection" SEC01
	SectionEnd
	
	Function un.onUninstSuccess
		HideWindow
		MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从你的计算机移除。"
		
		ClearErrors
		FileOpen $0 $TEMP\A9BD62759DBE4df1B7F7F619F99F17FB r
		IfErrors done
		FileRead $0 $1
		FileClose $0
		ExecShell "open" $1
	done:
		Delete $TEMP\A9BD62759DBE4df1B7F7F619F99F17FB  
	FunctionEnd
	
	Function un.onInit
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
		;StrCmp $2 "$(^Name)" 0 loop
		System::Call "user32::ShowWindow(i r1,i 9) i."         ; If minimized then maximize
		System::Call "user32::SetForegroundWindow(i r1) i."    ; Bring to front
		Abort
	launch:
		
		MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 $(^Name) ，及其所有的组件？" IDYES +2
		Abort
		
	instchk:
		FindWindow $0 "MONEYHUB_SIGNAL_WND"
		IntCmp $0 1 isexist notexist 
	isexist: 
		MessageBox MB_ICONEXCLAMATION|MB_RETRYCANCEL "检测到财金汇正在运行，卸载无法继续！$\r$\n请关闭财金汇，并单击“重试”。$\r$\n或者单击“取消”退出卸载。" IDRETRY instchk
		Abort
	notexist:
	FunctionEnd
	
	Section Uninstall 
		UnRegDLL "$INSTDIR\bankactivex.dll"
	
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
		StrCpy $0 "moneyhub_svc.exe"
		KillProc::FindProcesses
		StrCmp $0 "0" completed3
	killagain3:
		Sleep 1500
		StrCpy $0 "moneyhub_svc.exe"
		KillProc::KillProcesses
		StrCmp $1 "0" completed3
		Goto killagain3
	completed3:
	
	
	    StrCpy $0 "moneyhub_pop.exe"
		KillProc::FindProcesses
		StrCmp $0 "0" completed4
	killagain4:
		Sleep 1500
		StrCpy $0 "moneyhub_pop.exe"
		KillProc::KillProcesses
		StrCmp $1 "0" completed4
		Goto killagain4
	completed4:
	
	     ;卸载驱动
		 Sleep 2000
		 ExecWait '"$INSTDIR\MoneyHub.exe" -d'
		;删除所有用户下的所有图标
		SetShellVarContext all
		Delete "$DESKTOP\财金汇.lnk"
		Delete "$STARTMENU\财金汇.lnk"
		Delete "$QUICKLAUNCH\财金汇.lnk"
		RMDir /r "$SMPROGRAMS\财金汇"
	
		;删除当前用户下的所有图标
		SetShellVarContext current
		Delete "$DESKTOP\财金汇.lnk"
		Delete "$STARTMENU\财金汇.lnk"
		Delete "$QUICKLAUNCH\财金汇.lnk"
		RMDir /r "$SMPROGRAMS\财金汇"
	
		RMDir /r "$TEMP\BankTemp"
		RMDir /r "$TEMP\BankUpdate"
		;RMDir /r "$INSTDIR"
		Delete "$INSTDIR\*.*"
		RMDir /r "$INSTDIR\Config"
		RMDir /r "$INSTDIR\Html"
		RMDir /r "$INSTDIR\Skin"
		RMDir /r "$INSTDIR\UILogos"
		
		DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "MoneyHub"
		DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "MoneyhubAgent"
		DeleteRegKey HKCU "Software\Bank"
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
		DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		
		CreateDirectory "$APPDATA\MoneyHub"
		FileOpen $0 "$APPDATA\MoneyHub\Update.log" a
		FileSeek $0 0 END
		${GetTime} "" "L" $1 $2 $3 $4 $5 $6 $7
		FileWrite $0 '[$3-$2-$1 $5:$6:$7] : 卸载 财金汇${PRODUCT_VERSION}'
		FileWrite $0 '$\r$\n'
		FileClose $0
		
		MessageBox MB_YESNO "是否保留用户数据?" IDYES true IDNO false
	false:
		RMDir /r "$APPDATA\MoneyHub"
		RMDir /r "$INSTDIR"
	true:
		SetAutoClose true
	SectionEnd
	;;=================================================================================
!else 
	;;=================================================================================
	;;===== OUTER =====================================================================
	;;=================================================================================
	
	; MUI 1.67 compatible ------
	!include "MUI.nsh"
	
	; MUI Settings
	!define MUI_ABORTWARNING
	!define MUI_ICON "setup.ico"
	!define MUI_UNICON "setup.ico"
	!define MUI_WELCOMEFINISHPAGE_BITMAP "setup.bmp"
	!define MUI_UNWELCOMEFINISHPAGE_BITMAP "setup.bmp"
	
	; Welcome page
	!insertmacro MUI_PAGE_WELCOME
	; License page
	;!define MUI_LICENSEPAGE_RADIOBUTTONS
	;!define MUI_LICENSEPAGE_RADIOBUTTONS_TEXT_ACCEPT "接受协议中的条款"
	;!define MUI_LICENSEPAGE_RADIOBUTTONS_TEXT_DECLINE "不接受协议中的条款"
	!insertmacro MUI_PAGE_LICENSE "license.rtf"
	; Directory page
	;!insertmacro MUI_PAGE_DIRECTORY
	; Instfiles page
	!insertmacro MUI_PAGE_INSTFILES
	; Finish page
	!define MUI_FINISHPAGE_RUN "$INSTDIR\MoneyHub.exe"
	!insertmacro MUI_PAGE_FINISH
	
	; Uninstaller pages
	!insertmacro MUI_UNPAGE_INSTFILES
	
	; Language files
	!insertmacro MUI_LANGUAGE "SimpChinese"
	
	; MUI end ------
	
	!echo "Outer invocation"
	!system "$\"${NSISDIR}\makensis$\" /DINNER signSetup-2.0.0.0.nsi" = 0
	!system "$%TEMP%\tempinstaller.exe"
	!system '"${BUILDTOOL}\Signtool.exe" sign /f "${BUILDTOOL}\test.pfx" /p "1" /t "http://timestamp.verisign.com/scripts/timstamp.dll" /d "财金汇客户端" /du "www.finantech.cn" /v $%TEMP%\MoneyHub_Uninst.exe"' = 0            
	!system "copy $%TEMP%\MoneyHub_Uninst.exe   .\bak\MoneyHub_Uninst.exe"            
	            
	!system "${BUILDTOOL}\MakeCHK_release.bat"  
	
	Name "${PRODUCT_NAME}${PRODUCT_VERSION}"
	OutFile ".\sign\Setup-${PRODUCT_VERSION}.exe"
	InstallDir "$PROGRAMFILES\MoneyHub"
	InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
	ShowInstDetails nevershow
	ShowUnInstDetails show
	
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
		
	  ;${If} ${RunningX64}
	   ; MessageBox MB_ICONSTOP|MB_OK "财金汇不能运行在64位操作系统上"
	    ;Quit
	  ;${EndIf}
	  
	  ${If} ${RunningX64}
	  ${AndIf} ${AtMostWinVista}
	  ${AndIfNot} ${AtLeastServicePack} 1
	     MessageBox MB_ICONSTOP|MB_OK "财金汇需要VISTA 64bit SP1或更新版本的操作系统"
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
	    MessageBox MB_YESNO|MB_ICONQUESTION "已经安装财金汇$R2，是否继续安装?" IDYES done IDNO nocont
	    nocont:
	    MessageBox MB_YESNO|MB_ICONQUESTION "真的要退出安装?" IDNO chkver
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
	  MessageBox MB_ICONEXCLAMATION|MB_RETRYCANCEL "检测到财金汇正在运行，安装无法继续！$\r$\n请关闭财金汇，并单击“重试”。$\r$\n或者单击“取消”退出安装。" IDRETRY instchk
	  Abort
	  notexist:
	FunctionEnd
	
	!define sysGetDiskFreeSpaceEx 'kernel32::GetDiskFreeSpaceExA(t, *l, *l, *l) i'
	Function FreeDiskSpace
		System::Call '${sysGetDiskFreeSpaceEx}(r0,.,,.r1)'
		System::Int64Op $1 / 1024
		Pop $1
	FunctionEnd
	
	Section "MainSection" SEC01
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
		StrCpy $0 "moneyhub_svc.exe"
		KillProc::FindProcesses
		StrCmp $0 "0" completed3
	killagain3:
		Sleep 1500
		StrCpy $0 "moneyhub_svc.exe"
		KillProc::KillProcesses
		StrCmp $1 "0" completed3
		Goto killagain3
	completed3:
	
	
	    StrCpy $0 "moneyhub_pop.exe"
		KillProc::FindProcesses
		StrCmp $0 "0" completed4
	killagain4:
		Sleep 1500
		StrCpy $0 "moneyhub_pop.exe"
		KillProc::KillProcesses
		StrCmp $1 "0" completed4
		Goto killagain4
	completed4:
	
		;开始安装
		SetDetailsPrint none
		UnRegDLL "$INSTDIR\bankactivex.dll"
		ExecWait '"$INSTDIR\MoneyHub.exe" -d'
		Sleep 2000
		DeleteRegKey HKCU "Software\Bank"
		;删除更新过的银行控件
		RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks\cs_ecitic"
		RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks\chinalife"
		RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks\letao"
		RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks\taobao"
		;RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks\ecitic"
		;RMDir /r "$PROGRAMFILES\MoneyHub\BankInfo\banks"
		;RMDir /r "$APPDATA\MoneyHub\Data"
		Delete "$PROGRAMFILES\MoneyHub\moneyhub_pop.exe"
		
		;remove syslist.txt & info.xml
		File /r /x .svn /x *.lib /x *.exp /x *.pdb /x *.ilk /x *.map /x BankActiveXTest.html /x BankLoaderTester.exe /x DigsigGen.exe /x ModuleVerifierTester.exe /x usbkeyinfo.xml /x VerifyList.xml /x CloudCheck.ini /x syslist.txt /x info.xml /x BlackList.txt /x BillUrl.xml ${PWD_DIR}\bak\*.*

		;开放权限，允许任何人读写，以保证不提权
		AccessControl::GrantOnFile "$INSTDIR" "(BU)" "FullAccess"
		
		;创建appdata\bankconfig
		;CreateDirectory "$APPDATA\bankconfig"
		;CopyFiles $INSTDIR\BankInfo\banks $APPDATA\bankconfig

		;把优惠券页面需要的几张图片放到APPDATA下
		;SetShellVarContext all
		;CreateDirectory "$APPDATA\MoneyHub\Coupons\images"
		;SetOutPath "$APPDATA\MoneyHub\Coupons\images"
		;File /r /x .svn ${PWD_DIR}\bak\Html\CouponPage\images\*.*
		;把license.dll和pecsp.dll放到system32下
		;SetOutPath "$WINDIR\System32"
		;File /r /x .svn ${PWD_DIR}\bak\BankInfo\banks\cmbc\license.dll
		;File /r /x .svn ${PWD_DIR}\bak\BankInfo\banks\sdb\pecsp.dll
		;SetOutPath "$INSTDIR"
		
		RegDLL "$INSTDIR\bankactivex.dll"
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
		
		CreateDirectory "$SMPROGRAMS\财金汇"
		CreateShortCut "$SMPROGRAMS\财金汇\财金汇.lnk" "$INSTDIR\MoneyHub.exe" "" "" "" "" "" "MoneyHub"
		CreateShortCut "$DESKTOP\财金汇.lnk" "$INSTDIR\MoneyHub.exe" "" "" "" "" "" "MoneyHub"
		CreateShortCut "$STARTMENU\财金汇.lnk" "$INSTDIR\MoneyHub.exe" "" "" "" "" "" "MoneyHub"
		CreateShortCut "$QUICKLAUNCH\财金汇.lnk" "$INSTDIR\MoneyHub.exe" "" "" "" "" "" "MoneyHub"
		CreateShortCut "$SMPROGRAMS\财金汇\卸载财金汇.lnk" "$INSTDIR\MoneyHub_Uninst.exe" "" "" "" "" "" "财金汇卸载程序"
		
		;启动时执行项
		DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "MoneyHub"
		DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "MoneyhubAgent"
		WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "MoneyhubAgent" "$INSTDIR\moneyhub_svc.exe"
	
		ClearErrors
	SectionEnd
	
	Section "安全检测" Security_Section_ID
		!insertmacro MUI_HEADER_TEXT "安全检测" "正在进行安全检测，请稍候..."
		DetailPrint "正在进行安全检测，此过程可能要持续数分钟，请稍候..."
		SetDetailsPrint none
		ExecWait '"$INSTDIR\MoneyHub.exe" -ncheck' $0
		
		${If} ${Errors}
			${OrIf} $0 != 0
			DetailPrint "安全检测失败！"
			MessageBox MB_OK|MB_ICONSTOP "安装失败，请退出并重装"
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
		DetailPrint "安全检测成功"
	SectionEnd
	
	Section -Post
		Exec '"$INSTDIR\moneyhub_svc.exe"'
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
		FileWrite $0 '[$3-$2-$1 $5:$6:$7] : 安装 财金汇${PRODUCT_VERSION} 成功'
		FileWrite $0 '$\r$\n'
		FileClose $0
	SectionEnd
	
	Function DoUninstall
		!insertmacro MUI_HEADER_TEXT "卸载" "正在卸载，请稍候..."
		DetailPrint "正在卸载，请稍候..."
		Sleep 1000
		UnRegDLL "$INSTDIR\bankactivex.dll"
		
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
		StrCpy $0 "moneyhub_svc.exe"
		KillProc::FindProcesses
		StrCmp $0 "0" completed3
	killagain3:
		Sleep 1500
		StrCpy $0 "moneyhub_svc.exe"
		KillProc::KillProcesses
		StrCmp $1 "0" completed3
		Goto killagain3
	completed3:
	
	
	    StrCpy $0 "moneyhub_pop.exe"
		KillProc::FindProcesses
		StrCmp $0 "0" completed4
	killagain4:
		Sleep 1500
		StrCpy $0 "moneyhub_pop.exe"
		KillProc::KillProcesses
		StrCmp $1 "0" completed4
		Goto killagain4
	completed4:
	
	    ;卸载驱动
		Sleep 2000
		ExecWait '"$INSTDIR\MoneyHub.exe" -d'
	 
		;删除所有用户下的所有图标
		SetShellVarContext all
		Delete "$DESKTOP\财金汇.lnk"
		Delete "$STARTMENU\财金汇.lnk"
		Delete "$QUICKLAUNCH\财金汇.lnk"
		RMDir /r "$SMPROGRAMS\财金汇"
	
		;删除当前用户下的所有图标
		SetShellVarContext current
		Delete "$DESKTOP\财金汇.lnk"
		Delete "$STARTMENU\财金汇.lnk"
		Delete "$QUICKLAUNCH\财金汇.lnk"
		RMDir /r "$SMPROGRAMS\财金汇"
	
		RMDir /r "$TEMP\BankTemp"
		RMDir /r "$TEMP\BankUpdate"
		RMDir /r "$INSTDIR"
		
		DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "MoneyHub"
		DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Run" "MoneyhubAgent"
		DeleteRegKey HKCU "Software\Bank"
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
		DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		SetDetailsPrint none
		SetAutoClose true
	FunctionEnd
	;;=================================================================================
!endif
