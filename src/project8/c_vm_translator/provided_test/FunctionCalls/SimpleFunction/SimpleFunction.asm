// function SimpleFunction.test 2
(SimpleFunction.test)
	@0
	D=A
	@LCL
	A=D+M
	M=0
	@1
	D=A
	@LCL
	A=D+M
	M=0
// push local 0
	@LCL
	D=M
	@0
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// push local 1
	@LCL
	D=M
	@1
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// add
	@SP
	AM=M-1
	D=M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	D=D+M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// not
	@SP
	AM=M-1
	D=M
	D=!D
	@SP
	A=M
	M=D
	@SP
	M=M+1
// push argument 0
	@ARG
	D=M
	@0
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// add
	@SP
	AM=M-1
	D=M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	D=D+M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// push argument 1
	@ARG
	D=M
	@1
	A=D+A
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// sub
	@SP
	AM=M-1
	D=M
	@R13
	M=D
	@SP
	AM=M-1
	D=M
	@R13
	D=D-M
	@SP
	A=M
	M=D
	@SP
	M=M+1
// return
	@LCL
	D=M
	@R13
	M=D
	@5
	A=D-A
	D=M
	@R14
	M=D
	@SP
	AM=M-1
	D=M
	@ARG
	A=M
	M=D
	@ARG
	D=M+1
	@SP
	M=D
	@R13
	D=M
	@1
	A=D-A
	D=M
	@THAT
	M=D
	@R13
	D=M
	@2
	A=D-A
	D=M
	@THIS
	M=D
	@R13
	D=M
	@3
	A=D-A
	D=M
	@ARG
	M=D
	@R13
	D=M
	@4
	A=D-A
	D=M
	@LCL
	M=D
	@R14
	A=M
	0;JMP
