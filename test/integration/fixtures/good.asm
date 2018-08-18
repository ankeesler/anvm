op1:                 ;; takes two parameters, a "seed", and a return address
	LOAD #1 %r1
	ADD              ;; "seed" += 1
	LOAD %r0, %r1
	MULTIPLY         ;; "seed" *= "seed"
	STORE #2 @64
	LOAD @64 %r1
	DIVIDE           ;; "seed" /= 2
	LOAD #3 %r0
	SUBTRACT         ;; "seed" -= 3
	;;BRANCH %r1
                     ;; return "seed"

main:
	LOAD %rpc %r0
	LOAD #15 %r1
	ADD
	LOAD %r0 %r1
	LOAD #5 %r0
	BRANCH $op1      ;; op1(5)
	STORE %r0 @64
	IEXIT
