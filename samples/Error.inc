BUTTON_SENDFEEDBACK	:= 0x100
BUTTON_NOTHANKS		:= 0x101

align 16
Error_Sample:
	virtual at RBP-.FRAME
		rq 4
		MY_TD
		_align 16
		.FRAME := $ - $$
	end virtual
	enter .FRAME,0
	INIT_MY_TD ; copy default structure

	mov [.cfg.pszWindowTitle],<_R "Error Sample (page 1)">
	mov [.cfg.pszMainInstruction],<_R "An unexpected error occured. Please send feedback now!">
	mov [.cfg.pszVerificationText],<_R "Don't ask me again">
	mov [.cfg.pszContent],<_R "Error message goes here...">
	mov [.cfg.pszExpandedInformation],<_R "Stack trace goes here...">
	or [.cfg.dwFlags],TDF_USE_COMMAND_LINKS or TDF_VERIFICATION_FLAG_CHECKED or TDF_EXPAND_FOOTER_AREA

	.btns TD_BUTTONS \
		<"Send Feedback",10,"I'm in a giving mood">, BUTTON_SENDFEEDBACK,\
		<"No Thanks",10,"I don't feel like being helpful">, BUTTON_NOTHANKS
	lea rax,[.btns]
	mov [.cfg.pButtons],rax
	mov [.cfg.cButtons],sizeof .btns

	lea rax,[.OnButton]
	mov [.msg.OnButton],rax
	TaskDialogIndirect ADDR .cfg,ADDR .selectedButtonId,ADDR .selectedRadioButtonId,ADDR .verificationChecked
	leave
	retn

.OnButton: ; RCX:hWnd, RDX:TDN_BUTTON_CLICKED, R8:{control id}, R9:0, object
	cmp r8d,BUTTON_SENDFEEDBACK
	jz SendFeedbackDialog
	xor eax,eax ; S_OK, close dialog
	retn

SendFeedbackDialog:
	INIT_MY_TD ; copy default structure

	mov [.cfg.pszWindowTitle],<_R "Error Sample (page 2)">
	mov [.cfg.pszContent],<_R "Copying your passwords  ;-)">
	or [.cfg.dwFlags],TDF_SHOW_PROGRESS_BAR or TDF_CALLBACK_TIMER

	lea rax,[.OnTimer]
	mov [.msg.OnTimer],rax
	lea rax,[.OnNavigate]
	mov [.msg.OnNavigate],rax

	enter 32,0
	SendMessageW rcx,TDM_NAVIGATE_PAGE,0,ADDR .cfg
	mov eax,1 ; don't close dialog
	leave
	retn

.MaxRange := 5000
.MinRange := 0

.OnNavigate:
	enter 32,0
	SendMessageW rcx,TDM_SET_PROGRESS_BAR_RANGE,0,(.MaxRange shl 16)+.MinRange
	leave
	retn

.OnTimer:
	enter 48,0
	label .hTDlg:8 at rbp-16
	label .ppos:8 at rbp-8
	mov [.hTDlg],rcx
	cmp r8,.MaxRange
	ja .finished

	mov ecx,.MaxRange
	imul eax,r8d,100
	cdq
	div ecx
	mov [m_counter],eax
	mov [.ppos],r8

	wvsprintfW ADDR buffer,<_T "Sending... %d%%">,ADDR m_counter
	SendMessageW [.hTDlg],TDM_SET_ELEMENT_TEXT,TDE_MAIN_INSTRUCTION,ADDR buffer
	jmp @F
.finished:
	SendMessageW [.hTDlg],TDM_SET_ELEMENT_TEXT,TDE_MAIN_INSTRUCTION,\
		<_R "Thanks for the feedback!">
	SendMessageW [.hTDlg],TDM_SET_ELEMENT_TEXT,TDE_CONTENT,\
		<_R "Our developers will get right on that...">
	mov [.ppos],.MaxRange
@@:	SendMessageW [.hTDlg],TDM_SET_PROGRESS_BAR_POS,[.ppos],0
	xor eax,eax ; S_OK ; continue to advance timer
	leave
	retn
