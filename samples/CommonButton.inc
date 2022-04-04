align 16
Common_Buttons_Sample:
	virtual at RBP-.FRAME
		rq 4
		MY_TD
		_align 16
		.FRAME := $ - $$
	end virtual
	enter .FRAME,0
	INIT_MY_TD ; copy default structure

	mov [.cfg.pszWindowTitle],<_R "Common Buttons Sample">
	mov [.cfg.dwCommonButtons],TDCBF_OK_BUTTON or TDCBF_YES_BUTTON or TDCBF_NO_BUTTON or TDCBF_CANCEL_BUTTON or TDCBF_RETRY_BUTTON or TDCBF_CLOSE_BUTTON

	lea rax,[.OnButton]
	mov [.msg.OnButton],rax

	TaskDialogIndirect ADDR .cfg,0,0,0
	leave
	retn

.OnButton: ; RCX:hWnd, RDX:TDN_BUTTON_CLICKED, R8:{control id}, R9:0
	cmp r8,IDCANCEL ; same as message box result button ids
	jz .Cancel
	enter 32,0
	lea r9,[.table]
	cmp r8d,.LAST_COMMON
	jnc .id_unknown
	lea r9,[r9+r8*2]
.id_unknown:
	movzx r9,word [r9]
	SendMessageW rcx,TDM_SET_ELEMENT_TEXT,TDE_MAIN_INSTRUCTION,r9
	mov eax,S_FALSE ; do not close dialog
	leave
	retn
.Cancel:
	xor eax,eax
	retn

	iterate id, <>,Ok,Cancel,<>,Retry,<>,Yes,No,Close
		if % = 1
			align 2
			label .table:2
			.LAST_COMMON := %%
		end if
		match ,id
			.% RSTRING "Unknown"
		else
			.% RSTRING `id," clicked",0
		end match
		dw .%
	end iterate
