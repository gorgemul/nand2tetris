	@256
	D=A
	@SP
	M=D
// call Sys.init 0
	@Sys.init$ret1
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@LCL
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@ARG
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THIS
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THAT
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@SP
	D=M
	@5
	D=D-A
	@0
	D=D-A
	@ARG
	M=D
	@SP
	D=M
	@LCL
	M=D
	@Sys.init
	0;JMP
(Sys.init$ret1)
// function Sys.init 0
(Sys.init)
// push constant 4
	@4
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
// call Main.fibonacci 1
	@Main.fibonacci$ret2
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@LCL
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@ARG
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THIS
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THAT
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@SP
	D=M
	@5
	D=D-A
	@1
	D=D-A
	@ARG
	M=D
	@SP
	D=M
	@LCL
	M=D
	@Main.fibonacci
	0;JMP
(Main.fibonacci$ret2)
// label END
(FibonacciElement_END)
// goto END
	@FibonacciElement_END
	0;JMP
// function Main.fibonacci 0
(Main.fibonacci)
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
// push constant 2
	@2
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
// lt
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
	@LT1
	D;JLT
	D=0
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@NEXT1
	0;JMP
(LT1)
	D=-1
	@SP
	A=M
	M=D
	@SP
	M=M+1
(NEXT1)
// if-goto N_LT_2
	@SP
	AM=M-1
	D=M
	@Main_N_LT_2
	D;JNE
// goto N_GE_2
	@Main_N_GE_2
	0;JMP
// label N_LT_2
(Main_N_LT_2)
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
// label N_GE_2
(Main_N_GE_2)
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
// push constant 2
	@2
	D=A
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
// call Main.fibonacci 1
	@Main.fibonacci$ret3
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@LCL
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@ARG
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THIS
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THAT
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@SP
	D=M
	@5
	D=D-A
	@1
	D=D-A
	@ARG
	M=D
	@SP
	D=M
	@LCL
	M=D
	@Main.fibonacci
	0;JMP
(Main.fibonacci$ret3)
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
// push constant 1
	@1
	D=A
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
// call Main.fibonacci 1
	@Main.fibonacci$ret4
	D=A
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@LCL
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@ARG
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THIS
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@THAT
	D=M
	@SP
	A=M
	M=D
	@SP
	M=M+1
	@SP
	D=M
	@5
	D=D-A
	@1
	D=D-A
	@ARG
	M=D
	@SP
	D=M
	@LCL
	M=D
	@Main.fibonacci
	0;JMP
(Main.fibonacci$ret4)
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
