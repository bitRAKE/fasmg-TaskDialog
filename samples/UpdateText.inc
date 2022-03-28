BTN_UPDATECONTENT		:= 0x100 ;: low byte is TDE_* id for SendMessage
BTN_UPDATEEXPANDEDINFORMATION	:= 0x101
BTN_UPDATEFOOTER		:= 0x102
BTN_UPDATEMAININSTRUCTION	:= 0x103
BTN_UPDATEWINDOWTITLE		:= 0x3FF ; other

align 16
Update_Text_Sample:
	virtual at RBP-.FRAME
		rq 4
		MY_TD
		_align 16
		.FRAME := $ - $$
	end virtual
	enter .FRAME,0
	INIT_MY_TD

	or [.cfg.dwFlags],TDF_EXPANDED_BY_DEFAULT
	iterate <field,quoted>,\;* show text that cannot be changed at runtime
		pszWindowTitle,		"Update Text Sample (Window Title)",\
		pszMainInstruction,	"Main instruction",\
		pszContent,		"Content",\
		pszVerificationText,	"Verification text",\		;*
		pszExpandedInformation,	"Expanded information",\
		pszExpandedControlText,	"Expanded control text",\	;*
		pszCollapsedControlText,"Collapsed control text",\	;*
		pszFooter,		"Footer"

		lea rax,<_T quoted>
		mov [.cfg.field],rax
	end iterate

	.btns TD_BUTTONS \
		"Window title",		BTN_UPDATEWINDOWTITLE,\
		"Main instruction",	BTN_UPDATEMAININSTRUCTION,\
		"Content",		BTN_UPDATECONTENT,\
		"Footer",		BTN_UPDATEFOOTER,\
		"Expanded info",	BTN_UPDATEEXPANDEDINFORMATION
	lea rax,[.btns]
	mov [.cfg.pButtons],rax
	mov [.cfg.cButtons],sizeof .btns

	lea rax,[.OnButton]
	mov [.msg.OnButton],rax
	TaskDialogIndirect ADDR .cfg,ADDR .selectedButtonId,ADDR .selectedRadioButtonId,ADDR .verificationChecked
	leave
	retn

.OnButton: ; RCX:hWnd, RDX:TDN_BUTTON_CLICKED, R8:{control id}, R9:0, object
	cmp r8,IDCANCEL			; same as message box result button ids
	jnz @F
	xor eax,eax			; S_OK, close dialog
	retn
@@:	enter 32,0			;: translate button id to update message
	cmp r8,0x104
	jnc .OnButton.title
	movzx r8,r8l
	cmp r8,4
	jnc @F
	lea r9,[.Table]
	mov r9,[r9+r8*8]
	SendMessageW rcx,TDM_SET_ELEMENT_TEXT,r8,r9
	jmp @F
.OnButton.title:
	SetWindowTextW	rcx,<_T "Update Text Sample (Updated Title)">
@@:	mov eax,S_FALSE			; do not close dialog
	leave
	retn

	align sizeof.TCHAR
	iterate quoted,\
		"Content (updated)",\
		"Expanded information (updated)",\
		"Footer (updated)",\
		"Main instruction (updated)"
		if % = 1
			repeat %%
				indx %
				.%.string TCHAR quoted,0
			end repeat
			align 8
			.Table:
		end if
		dq .%.string
	end iterate
