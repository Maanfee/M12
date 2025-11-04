[bits 32]

print_pmode:
	pusha 
	mov edx, 0xb8000
	popa 
	ret 
