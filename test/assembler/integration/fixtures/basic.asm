tuna:
	LOAD 1 0
	LOAD 2 1
	MULTIPLY
	LOAD %r0 %r2 ;; this is a comment
	LOAD %r1 %r1

fish: ;; this is a comment
	STORE %r0 @%rsp
	LOAD %rsp %r0
	LOAD 4 %r1
	ADD
	LOAD %r0 %rsp

;; this is a comment
