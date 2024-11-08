// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

  @SCREEN
  D=A
  @R0 // put the current screen address to R0
  M=D
  @24575
  D=A
  @R1  // // put screen's biggest address to the R1
  M=D
(LOOP)
  @KBD
  D=M
  @BLACK
  D;JGT
  @WHITE
  D;JEQ
(BLACK)
  @R0
  D=M
  @R1
  D=M-D
  @LOOP
  D;JLT
  @R0
  D=M
  A=D
  M=-1
  @R0
  M=D+1
  @BLACK
  0;JMP
(WHITE)
  @SCREEN
  D=A
  @R0
  D=D-M
  @LOOP
  D;JGT
  @R0
  D=M
  A=D
  M=0
  @R0
  M=D-1
  @WHITE
  0;JMP