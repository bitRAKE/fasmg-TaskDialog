RBTN_NONE		:= 0x100
RBTN_WARNING		:= 0x101
RBTN_ERROR		:= 0x102
RBTN_INFORMATION	:= 0x103
RBTN_SHIELD		:= 0x104
RBTN_CUSTOM		:= IDR_ICON ; use main icon
; undocumented
RBTN_S_BSHIELD		:= 0x105
RBTN_S_REDX		:= 0x106
RBTN_S_YELLOW		:= 0x107
RBTN_S_CHECK		:= 0x108
RBTN_S_GSHIELD		:= 0x109

align 16
Icons_Sample:
	virtual at RBP-.FRAME
		rq 4
		MY_TD
		_align 16
		.FRAME := $ - $$
	end virtual
	enter .FRAME,0
	INIT_MY_TD ; copy default structure

	mov [.cfg.pszWindowTitle],<_R "Icons Sample">
	mov [.cfg.pszMainInstruction],<_R "Main Instruction">
	mov [.cfg.pszContent],<_R "Security icons are undocumented.">
	mov [.cfg.pszFooter],<_R "Footer">
	.rbtns TD_BUTTONS \
		"None",		RBTN_NONE,\
		"Warning",	RBTN_WARNING,\
		"Information",	RBTN_INFORMATION,\
		"Error",	RBTN_ERROR,\
		"Shield",	RBTN_SHIELD,\
		"Custom",	RBTN_CUSTOM,\
		\;:		undocumented		:
		"Security Shield Blue",	RBTN_S_BSHIELD,\
		"Security Warning",	RBTN_S_REDX,\
		"Security Error",	RBTN_S_YELLOW,\
		"Security Success",	RBTN_S_CHECK,\
		"Security Shield Gray",	RBTN_S_GSHIELD
	lea rax,[.rbtns]
	mov [.cfg.pRadioButtons],rax
	mov [.cfg.cRadioButtons],sizeof .rbtns

	lea rax,[.OnRadio]
	mov [.msg.OnRadio],rax
	TaskDialogIndirect ADDR .cfg,ADDR .selectedButtonId,ADDR .selectedRadioButtonId,ADDR .verificationChecked
	leave
	retn

.OnRadio: ; RCX:hWnd, RDX:TDN_RADIO_BUTTON_CLICKED, R8:{control id}, R9:0, object
	enter 48,0
	label .hTDlg:8 at rbp-16
	label .iid:8 at rbp-8
	mov [.hTDlg],rcx
	mov r9,r8
	cmp r8,RBTN_S_GSHIELD+1
	jnc .OnRadio.custom
	movzx r9,r8l
	neg r9w ; convert to resource ID
.OnRadio.custom:
	mov [.iid],r9
	SendMessageW rcx,TDM_UPDATE_ICON,TDIE_ICON_MAIN,r9
	SendMessageW [.hTDlg],TDM_UPDATE_ICON,TDIE_ICON_FOOTER,[.iid]
	leave
	retn
