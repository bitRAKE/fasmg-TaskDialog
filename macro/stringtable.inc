STRINGTABLES = 0
macro stringtable name,values&
	local base,size
	name dd RVA base,size,0,0
	base:
	iterate <sid,quoted>,values
		if %% > 16 & %% = %
			err 'stringtable: ',`name,' has more than 16 entries'
		end if
		sid := % - 1 + STRINGTABLES
		dw (@F - $)/2-1
		du quoted
		@@:
	end iterate
	size := $ - base
	STRINGTABLES = STRINGTABLES + 16
end macro
