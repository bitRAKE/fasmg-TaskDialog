BUTTON_RESET := 0x100

align 16
Timer_Sample:
	virtual at RBP-.FRAME
		rq 4
		MY_TD

		; dialog-time state variables
		.reset dd ?

		_align 16
		.FRAME := $ - $$
	end virtual
	enter .FRAME,0
	INIT_MY_TD		; copy default structure
	mov [.reset],S_OK	; initialize dialog-time state

	mov [.cfg.pszWindowTitle],<_R "Timer Sample">
	or [.cfg.dwFlags],TDF_CALLBACK_TIMER

	.btns TD_BUTTONS \
		<"Reset">, BUTTON_RESET
	lea rax,[.btns]
	mov [.cfg.pButtons],rax
	mov [.cfg.cButtons],sizeof .btns

	lea rax,[.OnTimer]
	mov [.msg.OnTimer],rax
	lea rax,[.OnButton]
	mov [.msg.OnButton],rax
	TaskDialogIndirect ADDR .cfg,0,0,0
	leave
	retn

Timer_Sample.OnButton: ; RCX:hWnd, RDX:TDN_BUTTON_CLICKED, R8:{control id}, R9:0, object
	cmp r8d,BUTTON_RESET
	jz @F
	xor eax,eax			; S_OK, close dialog
	retn
@@:; notification dispatcher in callback function loads RAX with this value
;	mov rax,[rsp+40]		; .lpRefData ; .cfg.lpCallbackData
	label .reset:4 at RAX + sizeof Timer_Sample.msg_vTable
	mov dword [.reset],S_FALSE	; reset timer, after next update
	mov eax,1			; don't close dialog
	retn

Timer_Sample.OnTimer:
	virtual at RBP-.FRAME
				rq 4
		.hTDlg		dq ?
		.FRAME := $ - $$
				rq 7
		.lpRefData	dq ? ; .cfg.lpCallbackData
	end virtual
	push rbx
	enter .FRAME,0
	mov [.hTDlg],rcx

	xchg rax,r8
	xor edx,edx
	mov ecx,1000
	div ecx
	xchg r8,rax
	mov r9,rdx
	wsprintfW ADDR buffer,<_T "Time elapsed: %d.%.3d seconds">,r8,r9
	SendMessageW [.hTDlg],TDM_SET_ELEMENT_TEXT,TDE_MAIN_INSTRUCTION,ADDR buffer

; 0 ; S_OK	; continue to advance timer
; 1 ; S_FALSE	; reset timer
	mov rbx,[.lpRefData]
	label .reset:4 at RBX + sizeof Timer_Sample.msg_vTable
	mov eax,[.reset]
	mov [.reset],S_OK
	leave
	pop rbx
	retn
