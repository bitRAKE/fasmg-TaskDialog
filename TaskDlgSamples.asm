include 'mywin.inc'
include 'mytd.inc'

; this is mainly to counter my problems naming stuff
iterate <_file,function,b_id>,\
	"CommonButton",		Common_Buttons,		COMMONBUTTONS,\
	"Counter",		Counter,		COUNTER,\
	"Elevation",		Elevation_Required,	ELEVATIONREQUIRED,\
	"EnableDisable",	Enable_Disable,		ENABLEDISABLE,\
	"Error",		Error,			ERROR,\
	"Icons",		Icons,			ICONS,\
	"Progress",		Progress,		PROGRESS,\
	"ProgressEffects",	Progress_Effects,	PROGRESSEFFECTS,\
	"Timer",		Timer,			TIMER,\
	"UpdateText",		Update_Text,		UPDATETEXT

	BUTTON_#b_id := % + 0xFF ; must be 0x100+ by design
	eval 'include "samples\',_file,'.inc"' ; bypass limitation of INCLUDE
	if % = %%
		collect _CONST_
			align 8
			label Main_Window.Table:%%
		end collect
		repeat %%
			indx %
			collect _CONST_
				dq function#_Sample
			end collect
		end repeat
	end if
end iterate

align 16
Main_Window:
	virtual at RBP-.FRAME
		rq 4
		MY_TD
		_align 16
		.FRAME := $ - $$
	end virtual
	enter .FRAME,0
	INIT_MY_TD ; copy default structure

	mov [.cfg.pszWindowTitle],\
		<_R "TaskDialog Samples (originally by Kenny Kerr)">
	mov [.cfg.pszMainInstruction],\
		<_R "Pick a sample to try:">
	mov [.cfg.pszContent],\
		<_R "Use dialog close button, ESC or Alt-F4 keys to exit a sample.">
	mov [.cfg.pszFooter],\
		<_R 'x86-64 by <a href="https://github.com/bitRAKE">bitRAKE</a> with flat assembler g.'>
	or [.cfg.dwFlags],TDF_USE_COMMAND_LINKS or TDF_CAN_BE_MINIMIZED or TDF_ENABLE_HYPERLINKS

;: create button array, let TaskDialog know:
	.btns TD_BUTTONS \
		"Common Buttons Sample",	BUTTON_COMMONBUTTONS,\
		"Counter Sample",		BUTTON_COUNTER,\
		"Elevation Required Sample",	BUTTON_ELEVATIONREQUIRED,\
		"Enable/Disable Sample",	BUTTON_ENABLEDISABLE,\
		"Error Sample",			BUTTON_ERROR,\
		"Icons Sample",			BUTTON_ICONS,\
		"Progress Sample",		BUTTON_PROGRESS,\
		"Progress Effects Sample",	BUTTON_PROGRESSEFFECTS,\
		"Timer Sample",			BUTTON_TIMER,\
		"Update Text Sample",		BUTTON_UPDATETEXT
	lea rax,[.btns]
	mov [.cfg.pButtons],rax
	mov [.cfg.cButtons],sizeof .btns

;: handled messages:
	lea rax,[.OnConstructed]
	mov [.msg.OnConstructed],rax
	lea rax,[.OnHyperlink]
	mov [.msg.OnHyperlink],rax
	lea rax,[.OnButton]
	mov [.msg.OnButton],rax

	TaskDialogIndirect ADDR .cfg,ADDR .selectedButtonId,ADDR .selectedRadioButtonId,ADDR .verificationChecked
	xchg ecx,eax
	jrcxz .S_OK
	stc ; no result
	leave
	retn

.S_OK:	; collect results to return
	mov eax,[.selectedRadioButtonId-2]
	mov ax,word[.selectedButtonId]
	leave
	retn

; modify template in order that other TaskDialogs are owned by {this}
.OnConstructed:; RCX:hWnd, RDX:TDN_DIALOG_CONSTRUCTED, R8:0, R9:0, object
	mov [MyTD_Template.cfg.hwndParent],rcx
	retn

; data driven for maximum laziness ...
.OnButton:; RCX:hWnd, RDX:TDN_BUTTON_CLICKED, R8:{control id}, R9:0, object
	lea rax,[.Table]
	movzx edx,r8l
	shr r8,9
	jnc .not_ours
	jnz .not_ours
	cmp edx,sizeof .Table
	jnc .not_ours
;	:warning: no sample function uses their shadow space
	push S_FALSE ; do not close dialog
	call qword [rax + rdx*8]
	pop rax
	retn
.not_ours:
	xor eax,eax ; S_OK ; close window
	retn

; oh, the vanity!
Main_Window.OnHyperlink:; RCX:hWnd, RDX:TDN_HYPERLINK_CLICKED, R8:0, R9:url, object
	virtual at RBP-.FRAME
		rq 4
		.P5 dq ?
		.P6 dq ?
		.FRAME := $ - $$
	end virtual
	xchg r8,r9
	enter .FRAME,0
	ShellExecuteW rcx,<_T "Open">,r8,0,0,SW_SHOWNORMAL
	leave
	retn


WinMain:ENTRY $
	pop rax

	; need to initialize MyTD_Template with application constant data
	mov [MyTD_Template.cfg.cbSize],sizeof TASKDIALOGCONFIG
	; GetDesktopWindow limits window frame! No TDF_CAN_BE_MINIMIZED!
	xor eax,eax
	mov [MyTD_Template.cfg.hwndParent],rax
	GetModuleHandleW 0 ; hModule = hInstance = BaseAddress
	mov [MyTD_Template.cfg.hInstance],rax
	mov [MyTD_Template.cfg.dwFlags],\
		\; center within parent window
		TDF_POSITION_RELATIVE_TO_WINDOW \
		\; always allow ESC, Alt-F4, Close Button
		or TDF_ALLOW_DIALOG_CANCELLATION
	lea rax,[TaskDialog__Callback] ; universal callback
	mov [MyTD_Template.cfg.pfCallback],rax
	; fill notification table with default handling
	lea rdi,[MyTD_Template.msg_vTable]
	lea rax,[TaskDialog__Callback.S_OK]
	mov ecx,TDN_EXPANDO_BUTTON_CLICKED + 1
	rep stosq

	call Main_Window
	ExitProcess 0
	int3
.end _winx.entry ; _winx.end? _end ; generate import section



section '.rdata' data readable
	_CONST_

section '.data' data readable writeable

; context for TDM_NAVIGATE_PAGE message:
namespace SendFeedbackDialog
	cfg TASKDIALOGCONFIG

	selectedButtonId	dd ?	; int
	selectedRadioButtonId	dd ?	; int

	msg_vTable:;:			TASKDIALOG_NOTIFICATIONS
	iterate message,\		; WPARAM	LPARAM
		OnCreate,\		; 0		0
		OnNavigate,\		; 0		0
		OnButton,\		; int		0
		OnHyperlink,\		; 0		*WCHAR
		OnTimer,\		; DWORD		0
		OnDestroy,\		; 0		0
		OnRadio,\		; int		0
		OnConstructed,\		; 0		0
		OnVerification,\	; BOOL		0
		OnHelp,\		; 0		0
		OnExpand		; BOOL		0

		msg.message	dq ?
	end iterate

	verificationChecked	dd ?	; BOOL
end namespace

	_BSS_


section '.rsrc' resource data readable
IDR_ICON := 0x1FF

directory \
	RT_ICON,	icons,\
	RT_GROUP_ICON,	group_icons,\
	RT_STRING,	strings,\
	RT_MANIFEST,	manifests

resource icons,\
	1,LANG_NEUTRAL,icon_m48,\
	2,LANG_NEUTRAL,icon_m32,\
	3,LANG_NEUTRAL,icon_m24,\
	4,LANG_NEUTRAL,icon_m16

resource group_icons,\
	IDR_ICON,LANG_NEUTRAL,main_icon

resource_RSTRINGS strings ; generate tables

resource manifests,\
	1,LANG_ENGLISH+SUBLANG_DEFAULT,manifest

icon main_icon,\
	icon_m48,'resource\dip_chip_48x48.ico',\
	icon_m32,'resource\dip_chip_32x32.ico',\
	icon_m24,'resource\dip_chip_24x24.ico',\
	icon_m16,'resource\dip_chip_16x16.ico'

resdata manifest
	file 'manifest.xml'
endres

section '.reloc' fixups data readable discardable
