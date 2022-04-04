align 16
Progress_Sample:
	virtual at RBP-.FRAME
		rq 4
		MY_TD
		_align 16
		.FRAME := $ - $$
	end virtual
	enter .FRAME,0
	INIT_MY_TD ; copy default structure

	mov [.cfg.pszWindowTitle],<_R "Progress Sample">
	or [.cfg.dwFlags],TDF_SHOW_PROGRESS_BAR or TDF_CALLBACK_TIMER
	lea rax,[.OnTimer]
	mov [.msg.OnTimer],rax
	lea rax,[.OnConstructed]
	mov [.msg.OnConstructed],rax
	TaskDialogIndirect ADDR .cfg,0,0,0
	leave
	retn

.MaxRange := 5000
.MinRange := 0

.OnConstructed: ; RCX:hWnd, RDX:TDN_DIALOG_CONSTRUCTED, R8:0, R9:0, object
	enter 32,0
	SendMessageW rcx,TDM_SET_PROGRESS_BAR_RANGE,0,(.MaxRange shl 16)+.MinRange
	leave
	retn

.OnTimer:
	cmp r8,.MaxRange
	jbe @F
	mov eax,S_FALSE ; reset timer
	retn

@@:	enter 48,0
	label .hTDlg:8 at rbp-16
	label .ppos:8 at rbp-8
	mov [.hTDlg],rcx
	mov [.ppos],r8

	mov ecx,.MaxRange
	imul eax,r8d,100
	cdq
	div ecx
	mov [m_counter],eax

	wvsprintfW ADDR buffer,<_T "%d%%">,ADDR m_counter
	SendMessageW [.hTDlg],TDM_SET_ELEMENT_TEXT,TDE_MAIN_INSTRUCTION,ADDR buffer
	SendMessageW [.hTDlg],TDM_SET_PROGRESS_BAR_POS,[.ppos],0
	xor eax,eax ; S_OK ; continue to advance timer
	leave
	retn
