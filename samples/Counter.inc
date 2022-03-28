BUTTON_INCREMENT := 0x101
BUTTON_DECREMENT := 0x102

collect _BSS_ ; :BUG used by several samples:
	m_counter dd ?
	align 64
	label buffer:512
	rb sizeof buffer
end collect

align 16
Counter_Sample:
	virtual at RBP-.FRAME
		rq 4
		MY_TD
		_align 16
		.FRAME := $ - $$
	end virtual
	enter .FRAME,0
	INIT_MY_TD ; copy default structure

	lea rax,<_T "Counter Sample">
	mov [.cfg.pszWindowTitle],rax
;: create button array, let TaskDialog know:
	.btns TD_BUTTONS \
		"Increment", BUTTON_INCREMENT,\
		"Decrement", BUTTON_DECREMENT
	lea rax,[.btns]
	mov [.cfg.pButtons],rax
	mov [.cfg.cButtons],sizeof .btns
;: handled messages:
	lea rax,[.OnButton]
	mov [.msg.OnButton],rax
	lea rax,[.OnConstructed]
	mov [.msg.OnConstructed],rax

	TaskDialogIndirect ADDR .cfg,ADDR .selectedButtonId,ADDR .selectedRadioButtonId,ADDR .verificationChecked
	leave
	retn

.OnButton: ; RCX:hWnd, RDX:TDN_BUTTON_CLICKED, R8:{control id}, R9:0, object
	or eax,-1
	cmp r8d,BUTTON_DECREMENT
	jz @F
	neg eax
	cmp r8d,BUTTON_INCREMENT
	jz @F
	xor eax,eax ; S_OK, close dialog
	retn

@@:	add [m_counter],eax
	;: ... fall through ...:

.OnConstructed: ; RCX:hWnd, RDX:TDN_DIALOG_CONSTRUCTED, R8:0, R9:0, object
	enter 48,0
	.hTDlg equ rbp-8
	mov [.hTDlg],rcx
	wvsprintfW ADDR buffer,<_T "%d">,ADDR m_counter
	SendMessageW [.hTDlg],TDM_SET_ELEMENT_TEXT,TDE_MAIN_INSTRUCTION,ADDR buffer
	mov eax,S_FALSE ; do not close dialog (only for TDN_BUTTON_CLICKED)
	leave
	retn
