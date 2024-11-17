  @SCREEN
  D=A

  @screen
  M=D

  @KBD
  D=A

  @kbd
  M=D

(LISTEN)
  @i
  M=0

  @KBD
  D=M

  @BLACK
  D;JNE

(WHITE)
  @screen
  D=M

  @i
  D=D+M

  @kbd
  D=D-M

  @LISTEN
  D;JEQ

  @screen
  D=M

  @i
  A=D+M
  M=0

  @i
  M=M+1

  @WHITE
  0;JMP

(BLACK)
  @screen
  D=M

  @i
  D=D+M

  @kbd
  D=D-M

  @LISTEN
  D;JEQ

  @screen
  D=M

  @i
  A=D+M
  M=-1

  @i
  M=M+1

  @BLACK
  0;JMP
