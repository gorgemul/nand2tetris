  @i
  M=0

  @n
  M=0

(LOOP)
  @R1
  D=M

  @i
  D=D-M

  @STOP
  D;JEQ

  @R0
  D=M

  @n
  M=D+M

  @i
  M=M+1

  @LOOP
  0;JMP

(STOP)
  @n
  D=M

  @R2
  M=D

(END)
  @END
  0;JMP
