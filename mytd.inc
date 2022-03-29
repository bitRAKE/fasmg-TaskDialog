; My Task Dialog Wrapper:

macro MY_TD
	.cfg TASKDIALOGCONFIG

	.selectedButtonId	dd ?	; int
	.selectedRadioButtonId	dd ?	; int

	label .msg_vTable:.td_end-$	;: TASKDIALOG_NOTIFICATIONS:
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

		.msg.message	dq ?
	end iterate

	.verificationChecked	dd ?	; BOOL
	.td_end:
end macro

collect _BSS_
	label MyTD_Template:MyTD_Template_end - $
	MY_TD
	MyTD_Template_end:
end collect

macro INIT_MY_TD
	push rdi rsi rcx
	lea rsi,[MyTD_Template]
	lea rdi,[.cfg]
	mov ecx,((sizeof MyTD_Template)+3) shr 2
	rep movsd
	pop rcx rsi rdi
	; local notification table pointer is passed to callback
	; (could access any object data from here)
	lea rax,[.msg_vTable]
	mov [.cfg.lpCallbackData],rax
end macro


align 64
; universal callback processing (minimal environmental impact)
TaskDialog__Callback: ; HWND,UINT,WPARAM,LPARAM,LONG_PTR
	virtual at RSP
				dq ? ; RET
				rq 4
		.lpRefData	dq ?
	end virtual
	mov rax,[.lpRefData]
	cmp edx,TDN_EXPANDO_BUTTON_CLICKED + 1
	jnc .fatal
	jmp qword [rax+rdx*8]

.fatal:	int3 ; TODO: not nice

; Presently, only two notifications respond to the return value:
;	TDN_BUTTON_CLICKED:
;		S_OK		close dialog and return button id
;		S_FALSE		continue dialog processing
;	TDN_TIMER:
;		S_OK		continue to advance timer
;		S_FALSE		reset tick count
align 16
.S_OK:	xor eax,eax
	retn



; Helper to create static button arrays for TaskDialogIndirect
; (works for regular buttons and radio type)
;	. resolves to unique array pointer
;	sizeof . is button items in array
struc TD_BUTTONS line&
	local name
	struc name.struc
	iterate <quoted,id>,line
		if % = 1
			align 4
			label name:%%
			. equ name
		end if
		name.sid RSTRING quoted ; id.sid to expose
		dd id
		dq name.sid
	end iterate
	end struc
	collect _CONST_
		. name.struc
	end collect
end struc
