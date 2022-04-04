format PE64 GUI 5.0 at 0x1234_56780000
include 'win64wxp.inc'
include 'encoding\utf8.inc'
include 'macro\collect.inc'

include 'equates\TaskDialog.inc'

include 'macro\rstrings.inc'
?dummy RSTRING ; zero id

S_FALSE := 1 ; because TRUE/FALSE is so 1900
S_OK := 0


; macros are written to process 64-bit register inputs. yet, sometimes conversion to 32-bit part is convinent.
iterate <reg,rlow>, ax,al, cx,cl, dx,dl, bx,bl, sp,spl, bp,bpl, si,sil, di,dil
define reg32.r#reg? e#reg
;define reg16low.r#reg? reg
;define reg8low.r#reg? rlow
;define reg8hi.r#reg? rlow
end iterate
repeat 8, i:8
define reg32.r#i? r#i#d
;define reg16low.r#i? r#i#w
;define reg8low.r#i? r#i#l
;define reg8hi.r#i? r#i#h
end repeat


; make DLL functions first class:
macro import? name,funs&
	local _funs
	iterate <F,S>,funs
		if % = 1
			_funs equ _x_#F,S
		else
			_funs reequ _funs,_x_#F,S
		end if
		macro F line&
			win64abi _x_#F,line
		end macro
	end iterate
	match any,_funs
		import name,any
	end match
end macro

macro win64abi api*,line&
	iterate P,line
		repeat 1,I:%%-%+1
		indx I
		win64abi_parm I,P
		end repeat
	end iterate
	call [api]
end macro

; don't try to overload x86 instructions, irregular types should be built-up prior (i.e. instead of supporting ( SIGNED {size}? [] --> MOVSX ) just do the MOVSX)
macro win64abi_parm index*,arg&
local llabl,word_id,dest
iterate reg, rcx,rdx,r8,r9,rax,r10,r11
	if index < 5
		indx index
	else
		indx 5 + ((index-4) mod (%%-4))
	end if
	match [any],arg
		if index < 5
			mov reg,[any]
		else
			push [any]
			pop [.P#index]
		end if
	else match =ADDR? any,arg
		if index < 5
			lea reg,[any]
		else
			lea reg,[any]
			mov [.P#index],reg
		end if
	else match =CONST? any,arg
; TODO: batch into granulatity group
		collect _CONST_
			label llabl
			any
		end collect
		if index < 5
			lea reg,[llabl]
		else
			lea reg,[llabl]
			mov [.P#index],reg
		end if
	else match =_T any,arg
		collect _CONST_
			align sizeof.TCHAR
			label llabl
			TCHAR any,0
		end collect
		if index < 5
			lea reg,[llabl]
		else
			lea reg,[llabl]
			mov [.P#index],reg
		end if
; TODO: not implemented
	else match =_R any,arg
		word_id RSTRING any
		if index < 5
			mov reg32.reg,word_id
		else
			mov [.P#index],word_id
		end if
	else
		x86.parse_operand@src arg
		if @src.type = 'reg'
			if index < 5
				match =reg?,arg
				else
; TODO: reg rax -> XCHG
					mov reg,arg
				end match
			else
				mov [.P#index],arg
			end if
		else if @src.type = 'imm'
			if arg = 0
				if index < 5
					xor reg32.reg,reg32.reg
				else
					and [.P#index],0
				end if
			else if arg = -1
				if index < 5
					or reg,-1
				else
					or [.P#index],-1
				end if
			else if -$81 < arg & arg < $80
				push arg
				if index < 5
					pop reg
				else
					pop [.P#index]
				end if
			else if -1 < arg & arg < $10000_0000 & index < 5
				mov reg32.reg,arg
			else if -$8000_0001 < arg & arg < $8000_0000
				if index < 5
					mov reg,arg
				else
					mov [.P#index],arg
				end if
			else ; only use proxy in extreme case
				mov reg,arg
				if index > 4
					mov [.P#index],reg
				end if
			end if
		end if
	end match
break
end iterate
end macro


; this version factors out variable terms in address space, and assumes base is infinitely alignable (use wisely)
macro _align? value*,something:db ?
	while ($-$$) and (value-1)
		something
	end while
end macro

macro lea? line&
; NOTE: spaces needed for possible line breaks! Use most general configuration!
	match any =, =< values =>,line
		local name
		match =_T V,values
			collect _CONST_
				align sizeof.TCHAR
				label name
				TCHAR V,0
			end collect
		else match =_A V,values
			collect _CONST_
				name db V,0
			end collect
		else match =_W V,values
			collect _CONST_
				name du V,0
			end collect
		else
			collect _CONST_
				label name
				values
			end collect
		end match
		lea any,[name]
	else
		lea line
	end match
end macro

macro mov? line&
; NOTE: spaces needed for possible line breaks! Use most general configuration!
	match any =, =< values =>,line
		local word_id
		match =_R V,values
			word_id RSTRING V
		else
			err ; unsupported type
		end match
		if word_id < $80
			push word_id
			pop any
		else
			mov any,word_id
		end if
	else
		mov line
	end match
end macro

section '.text' code readable executable
