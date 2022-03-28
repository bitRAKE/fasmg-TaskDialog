RBTN_NONE		:= 0x100
RBTN_WARNING		:= 0x101
RBTN_ERROR		:= 0x102
RBTN_INFORMATION	:= 0x103
RBTN_SHIELD		:= 0x104
RBTN_CUSTOM		:= 0x1FF ; TODO: add icon resource

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

	lea rax,<_T "Icons Sample">
	mov [.cfg.pszWindowTitle],rax
	lea rax,<_T "Main Instruction">
	mov [.cfg.pszMainInstruction],rax
	lea rax,<_T "Footer">
	mov [.cfg.pszFooter],rax
	.rbtns TD_BUTTONS \
		"None",		RBTN_NONE,\
		"Warning",	RBTN_WARNING,\
		"Information",	RBTN_INFORMATION,\
		"Error",	RBTN_ERROR,\
		"Shield",	RBTN_SHIELD,\
		"Custom",	RBTN_CUSTOM
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
	movzx r9,r8l
	neg r9w ; convert to resource ID
	mov [.hTDlg],rcx
	mov [.iid],r9
	SendMessageW rcx,TDM_UPDATE_ICON,TDIE_ICON_MAIN,r9
	SendMessageW [.hTDlg],TDM_UPDATE_ICON,TDIE_ICON_FOOTER,[.iid]
	leave
	retn
