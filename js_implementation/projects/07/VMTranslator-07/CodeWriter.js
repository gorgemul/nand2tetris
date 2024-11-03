// 1. asm里面要写备注like // push local 2
// 2. 避免重复可以写helperFunction
// 3. 结束时写一个infinite loop

const base = {
  sp: 256,
  static: 16,
  temp: 5,
  judgeOp: 0,
}

const virtualMemorySegments = ['argument', 'local', 'this', 'that'];

const asmResult = [];

// 配置asm指针
const initializer = () => {
  asmResult.push('// init the stack pointer');
  asmResult.push(`  @${base.sp}`, '  D=A', '  @SP', '  M=D');
}

// MAIN
const codeWriter = (parsedCommands) => {
  initializer();
  parsedCommands.map((commandObj) => {
    if (commandObj?.commandType.indexOf('C_ARITHMETIC') !== -1) {
      writeArithmetic(commandObj);
    } else if (commandObj?.commandType.indexOf('C_PUSH') !== -1 || commandObj?.commandType.indexOf('C_POP') !== -1) {
      writePushPop(commandObj);
    }
  })
  asmResult.push('(END)', '  @END', '  0;JMP');
  const outputContent = asmResult.join('\n');
  return outputContent;
}

const writeArithmetic = (commandObj) => {
  const { arg1 } = commandObj;
  const asmOp = parseArithOp(arg1);
  leaveComment(arg1);
  if (arg1 === 'add' || arg1 === 'sub' || arg1 === 'and' || arg1 === 'or') {
    popFromStack();
    asmResult.push('  @R13', '  M=D');
    popFromStack();
    asmResult.push('  @R13', `  D=D${asmOp}M`);
    pushToStack();
  } else if (arg1 === 'neg' || arg1 === 'not') {
    popFromStack();
    asmResult.push(`  M=${asmOp}D`, '  @SP', '  M=M+1');
  } else if (arg1 === 'eq' || arg1 === 'gt' || arg1 === 'lt') {
    base.judgeOp++;
    popFromStack();
    asmResult.push('  @R13', '  M=D');
    popFromStack();
    asmResult.push('  @R13', '  D=D-M');
    asmResult.push(`  @${asmOp}${base.judgeOp}`, `  D;J${asmOp}`, '  @SP', '  A=M', '  M=0', '  @SP', '  M=M+1', `  @CONTINUE${base.judgeOp}`, '  0;JMP');
    asmResult.push(`(${asmOp}${base.judgeOp})`, '  @SP', '  A=M', '  M=-1', '  @SP', '  M=M+1', `(CONTINUE${base.judgeOp})`);
  }
}

const writePushPop = (commandObj) => {
  const { commandType, arg1, arg2 } = commandObj;
  if (commandType === 'C_PUSH') {
    leaveComment(arg1, arg2, 'push');
    if (arg1 === 'constant') {
      encounterPushConstant(arg2);
    } else if (arg1 === 'static') {
      encounterPushStatic(arg2);
    } else if (arg1 === 'pointer') {
      encounterPushPointer(arg2);
    } else if (arg1 === 'temp') {
      encounterPushTemp(arg2);
    } else if (virtualMemorySegments.includes(arg1)) {
      const segment = virtualMemorySegments.filter(item => item === arg1)[0];
      encounterPushVirtualMemorySegments(segment, arg2);
    }
  } else if (commandType === 'C_POP') {
    leaveComment(arg1, arg2, 'pop');
    if (arg1 === 'static') {
      encounterPopStatic(arg2);
    } else if (arg1 === 'pointer') {
      encounterPopPointer(arg2);
    } else if (arg1 === 'temp') {
      encounterPopTemp(arg2);
    } else if (virtualMemorySegments.includes(arg1)) {
      const segment = virtualMemorySegments.filter(item => item === arg1)[0];
      encounterPopVirtualMemorySegments(segment, arg2);
    }
  }
}

// HELPER
// 1. push or pop
const pushToStack = () => {
  asmResult.push('  @SP', '  A=M', '  M=D', '  @SP', '  M=M+1');
};

const popFromStack = () => {
  asmResult.push('  @SP', '  AM=M-1', '  D=M');
}

const encounterPushConstant = (value) => {
  asmResult.push(`  @${value}`, '  D=A');
  pushToStack();
}

const encounterPushStatic = (value) => {
  asmResult.push(`  @${base.static + value}`, '  D=M');
  pushToStack();
}

const encounterPushTemp = (value) => {
  asmResult.push(`  @${base.temp + value}`, '  D=M');
  pushToStack();
}

const encounterPushPointer = (value) => {
  if (value === 0) {
    asmResult.push('  @THIS', '  D=M');
    pushToStack();
  } else if (value === 1) {
    asmResult.push('  @THAT', '  D=M');
    pushToStack();
  }
}

const encounterPushVirtualMemorySegments = (segment, value) => {
  const asmSegment = parseSegment(segment);
  asmResult.push(`  @${value}`, '  D=A', `  @${asmSegment}`, '  A=M+D', '  D=M');
  pushToStack();
}

const encounterPopStatic = (value) => {
  popFromStack();
  asmResult.push(`  @${base.static + value}`, '  M=D');
}

const encounterPopTemp = (value) => {
  popFromStack();
  asmResult.push(`  @${base.temp + value}`, '  M=D');
}

const encounterPopPointer = (value) => {
  if (value === 0) {
    popFromStack();
    asmResult.push('  @THIS', '  M=D');
  } else if (value === 1) {
    popFromStack();
    asmResult.push('  @THAT', '  M=D');
  }
}

const encounterPopVirtualMemorySegments = (segment, value) => {
  const asmSegment = parseSegment(segment);
  asmResult.push(`  @${value}`, '  D=A', `  @${asmSegment}`, '  D=M+D', '  @R13', '  M=D');
  popFromStack();
  asmResult.push('  @R13', '  A=M', '  M=D');
}

const parseSegment = (segment) => {
  switch (segment) {
    case 'argument':
      return 'ARG';
    case 'local':
      return 'LCL';
    case 'this':
      return 'THIS';
    case 'that':
      return 'THAT';
  }
};

// 2. arithmetic

const parseArithOp = (op) => {
  switch (op) {
    case 'add':
      return '+';
    case 'sub':
      return '-';
    case 'and':
      return '&';
    case 'or':
      return '|';
    case 'neg':
      return '-';
    case 'not':
      return '!';
    case 'eq':
      return 'EQ';
    case 'lt':
      return 'LT';
    case 'gt':
      return 'GT';
  }
}

// 3. comment
const leaveComment = (arg1, arg2, op) => {
  if (arg2 !== undefined && op !== undefined) {
    asmResult.push(`// ${op} ${arg1} ${arg2}`);
  } else {
    asmResult.push(`// ${arg1}`);
  }
}

module.exports = codeWriter;

