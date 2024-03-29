BUTTON_ADMIN_STUFF := 0x101

align 16
Elevation_Required_Sample:
	virtual at RBP-.FRAME
		rq 4
		MY_TD
		_align 16
		.FRAME := $ - $$
	end virtual
	enter .FRAME,0
	INIT_MY_TD ; copy default structure

	mov [.cfg.pszWindowTitle],<_R "Elevation Required Sample">
	mov [.cfg.pszContent],<_R "Just a button with a shield ...">

	.btns TD_BUTTONS \
		"Admin Stuff", BUTTON_ADMIN_STUFF
	lea rax,[.btns]
	mov [.cfg.pButtons],rax
	mov [.cfg.cButtons],sizeof .btns

	lea rax,[.OnConstructed]
	mov [.msg.OnConstructed],rax

	TaskDialogIndirect ADDR .cfg,0,0,0
	leave
	retn

.OnConstructed: ; RCX:hWnd, RDX:TDN_DIALOG_CONSTRUCTED, R8:0, R9:0, object
	enter 32,0
	SendMessageW rcx,TDM_SET_BUTTON_ELEVATION_REQUIRED_STATE,BUTTON_ADMIN_STUFF,TRUE
	leave
	retn
