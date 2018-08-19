tuna:
	STORE %r0 @%sp
	LOAD %sp %r0
	LOAD 4 %r1
	ADD
	LOAD %r0 %rsp
