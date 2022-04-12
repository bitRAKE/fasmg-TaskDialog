BUTTON_STATE	:= 0x100
BUTTON_MODE	:= 0x101

align 16
Progress_Effects_Sample:
	virtual at RBP-.FRAME
		rq 4
		MY_TD

		;: dialog-time state variables:
		.marquee	dd ?
		.state		dd ?

		_align 16
		.FRAME := $ - $$
	end virtual
	enter .FRAME,0
	INIT_MY_TD
	mov [.marquee],0
	mov [.state],(((3 shl 11) + 2) shl 11) + 1

	mov [.cfg.pszWindowTitle],<_R "Progress Effects Sample">
	or [.cfg.dwFlags],TDF_SHOW_PROGRESS_BAR
	.btns TD_BUTTONS \
		"Cycle State", BUTTON_STATE,\
		"Toggle Mode", BUTTON_MODE
	lea rax,[.btns]
	mov [.cfg.pButtons],rax
	mov [.cfg.cButtons],sizeof .btns

	lea rax,[.OnButton]
	mov [.msg.OnButton],rax
	lea rax,[.OnConstructed]
	mov [.msg.OnConstructed],rax
	TaskDialogIndirect ADDR .cfg,0,0,0
	leave
	retn

.OnButton: ; RCX:hWnd, RDX:TDN_BUTTON_CLICKED, R8:{control id}, R9:0, object
	cmp r8d,BUTTON_STATE
	jz CycleState
	cmp r8d,BUTTON_MODE
	jz ToggleMode
	xor eax,eax ; S_OK, close dialog
	retn

Progress_Effects_Sample.OnConstructed:
	virtual at RBP-.FRAME
				rq 4
		.hTDlg		dq ?
		.FRAME := $ - $$
				rq 7
		.lpRefData	dq ? ; .cfg.lpCallbackData
	end virtual
	push rbx
	enter .FRAME,0
	mov rbx,[.lpRefData]
	mov [.hTDlg],rcx
	jmp ToggleMode.off

; three states, if we use the carry bit that is 11-bits per state! :-)
CycleState:; PBST_NORMAL -> PBST_PAUSED -> PBST_ERROR -> PBST_NORMAL
	virtual at RBP-.FRAME
				rq 4
		.hTDlg		dq ?
		.FRAME := $ - $$
				rq 7
		.lpRefData	dq ? ; .cfg.lpCallbackData
	end virtual
	push rbx
	enter .FRAME,0
	mov rbx,[.lpRefData]
	virtual at RBX + sizeof Progress_Effects_Sample.msg_vTable
		.marquee	dd ?
		.state		dd ?
	end virtual
	mov [.hTDlg],rcx
	clc
	rcl [.state],11 ; bit lost is always a zero bit
	movzx r8,byte [.state] ; only need two bits really
	SendMessageW [.hTDlg],TDM_SET_PROGRESS_BAR_STATE,r8,0
	jmp ToggleMode.xit


ToggleMode:
	virtual at RBP-.FRAME
				rq 4
		.hTDlg		dq ?
		.FRAME := $ - $$
				rq 7
		.lpRefData	dq ? ; .cfg.lpCallbackData
	end virtual
	push rbx
	enter .FRAME,0
	; verbosely get dialog-time state, but RAX already has this pointer!
	mov rbx,[.lpRefData]
	virtual at RBX + sizeof Progress_Effects_Sample.msg_vTable
		.marquee	dd ?
		.state		dd ?
	end virtual
	mov [.hTDlg],rcx
	xor [.marquee],1
	mov r8d,[.marquee]
	SendMessageW [.hTDlg],TDM_SET_MARQUEE_PROGRESS_BAR,r8,0
	test [.marquee],-1
	jz .off
	SendMessageW [.hTDlg],TDM_SET_PROGRESS_BAR_MARQUEE,1,0
	jmp .xit
.off:	SendMessageW [.hTDlg],TDM_SET_PROGRESS_BAR_POS,25,0
.xit:
	lea r8,<_T "_W_T_F!",0,"Normal",0,0,"Error",0,0,0,"Paused">
	movzx eax,byte [.state]
	shl eax,4
	add r8,rax

	lea r9,<_A "False",0,"True (only togglable in Normal state)">
	imul eax,[.marquee],6
	add r9,rax

	wsprintfW ADDR buffer,<_T "State: %s",10,"Marquee: %S" >,r8,r9
	SendMessageW [.hTDlg],TDM_SET_ELEMENT_TEXT,TDE_MAIN_INSTRUCTION,ADDR buffer
	mov eax,S_FALSE ; do not close dialog
	leave
	pop rbx
	retn
