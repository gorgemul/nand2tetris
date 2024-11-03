// 1. asm里面要写备注like // push local 2
// 2. 避免重复可以写helperFunction
// 3. 结束时写一个infinite loop
const path = require('path');

const base = {
  sp: 256,
  static: 16,
  temp: 5,
  judgeOp: 0,
  funcOp: 0,
  sysOp: 0,
}

const virtualMemorySegments = ['argument', 'local', 'this', 'that'];

let asmResult = [];

// 配置asm指针
const initializer = () => {
  base.sysOp++;
  asmResult.push('// init the stack pointer');
  asmResult.push(`  @${base.sp}`, '  D=A', '  @SP', '  M=D');
  asmResult.push(`  @Sys.INIT${base.sysOp}`, '  D=A');
  pushToStack();
  asmResult.push('  @LCL', '  D=M');
  pushToStack();
  asmResult.push('  @ARG', '  D=M');
  pushToStack();
  asmResult.push('  @THIS', '  D=M');
  pushToStack();
  asmResult.push('  @THAT', '  D=M');
  pushToStack();
  asmResult.push('  @SP', '  D=M');
  asmResult.push('  @5', '  D=D-A', '  @ARG', '  M=D');
  asmResult.push('  @SP', '  D=M', '  @LCL', '  M=D');
  asmResult.push('  @Sys.init', '  0;JMP');
  asmResult.push(`(Sys.INIT${base.sysOp})`)
}

// MAIN
const codeWriter = ({ parsedCommands, isFirstTime, isLastTime, currentFile }) => {

  asmResult = []; // 每一次进来清空数据

  if (isFirstTime) {
    initializer(); //如果第一次进入需要初始化
  }
  parsedCommands.map((commandObj) => {
    if (commandObj?.commandType.indexOf('C_ARITHMETIC') !== -1) {
      writeArithmetic(commandObj);
    } else if (commandObj?.commandType.indexOf('C_PUSH') !== -1 || commandObj?.commandType.indexOf('C_POP') !== -1) {
      writePushPop(commandObj, currentFile);
    } else if (commandObj?.commandType.indexOf('C_BRANCHING') !== -1) {
      writeBranching(commandObj);
    } else if (commandObj?.commandType.indexOf('C_FUNCTION') !== -1) {
      writeFunction(commandObj);
    }
  })
  if (isLastTime) {
    asmResult.push('(END)', '  @END', '  0;JMP');
  }
  const outputContent = asmResult.join('\n');
  return outputContent;
}

const writeArithmetic = (commandObj) => {
  const { arg1 } = commandObj;
  const asmOp = parseArithOp(arg1);
  leaveComment({ arg1 });
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

const writePushPop = (commandObj, currentFile) => {
  const { commandType, arg1, arg2 } = commandObj;
  if (commandType === 'C_PUSH') {
    leaveComment({ arg1, arg2, op: 'push' });
    if (arg1 === 'constant') {
      encounterPushConstant(arg2);
    } else if (arg1 === 'static') {
      encounterPushStatic(arg2, currentFile);
    } else if (arg1 === 'pointer') {
      encounterPushPointer(arg2);
    } else if (arg1 === 'temp') {
      encounterPushTemp(arg2);
    } else if (virtualMemorySegments.includes(arg1)) {
      const segment = virtualMemorySegments.filter(item => item === arg1)[0];
      encounterPushVirtualMemorySegments(segment, arg2);
    }
  } else if (commandType === 'C_POP') {
    leaveComment({ arg1, arg2, op: 'pop' });
    if (arg1 === 'static') {
      encounterPopStatic(arg2, currentFile);
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

const writeBranching = (commandObj) => {
  const { commandType, arg1 } = commandObj;
  if (commandType === 'C_BRANCHING_LABEL') {
    leaveComment({ arg1, op: 'label' });
    asmResult.push(`(${arg1})`);
  } else if (commandType === 'C_BRANCHING_IFGOTO') {
    leaveComment({ arg1, op: 'if-goto' });
    popFromStack();
    asmResult.push(`  @${arg1}`, '  D;JNE');
  } else if (commandType === 'C_BRANCHING_GOTO') {
    leaveComment({ arg1, op: 'goto' });
    asmResult.push(`  @${arg1}`, '  0;JMP');
  }
}

const writeFunction = (commandObj) => {
  const { commandType, arg1, arg2 } = commandObj;
  if (commandType === 'C_FUNCTION_DECLARE') {
    leaveComment({ arg1, arg2, op: 'declare' });
    asmResult.push(`(${arg1})`);
    for (let i = 0; i < arg2; i++) {
      asmResult.push(`  @${i}`, '  D=A', '  @LCL', '  A=M+D', '  M=0');
    }
  } else if (commandType === 'C_FUNCTION_RETURN') {
    leaveComment({ arg1, op: 'function' });
    asmResult.push('  @LCL', '  D=M', '  @R13', '  M=D'); //用R13(frame)存储LCL的值
    asmResult.push('  @5', '  D=A', '  @R13', '  A=M-D', '  D=M', '  @R14', '  M=D'); // R14(retAddr)存储返回的跳转地址
    popFromStack();
    asmResult.push('  @ARG', '  A=M', '  M=D'); // 让ARG[0]存储func返回值
    asmResult.push('  @ARG', '  D=M+1', '  @SP', '  M=D'); // 让SP指向返回值位置的下一个地址(ARG[1])
    asmResult.push('  @1', '  D=A', '  @R13', '  A=M-D', '  D=M', '  @THAT', '  M=D'); // reinstate THAT指针
    asmResult.push('  @2', '  D=A', '  @R13', '  A=M-D', '  D=M', '  @THIS', '  M=D'); // reinstate THIS指针
    asmResult.push('  @3', '  D=A', '  @R13', '  A=M-D', '  D=M', '  @ARG', '  M=D'); // reinstate ARG指针
    asmResult.push('  @4', '  D=A', '  @R13', '  A=M-D', '  D=M', '  @LCL', '  M=D'); // reinstate LCL指针
    asmResult.push('  @R14', '  A=M', '  0;JMP'); // 跳转返回地址
  } else if (commandType === 'C_FUNCTION_CALL') {
    leaveComment({ arg1, arg2, op: 'call' });
    base.funcOp++;
    // push frame into stack
    asmResult.push(`  @${arg1}RET${base.funcOp}`, '  D=A');
    pushToStack();
    asmResult.push('  @LCL', '  D=M');
    pushToStack();
    asmResult.push('  @ARG', '  D=M');
    pushToStack();
    asmResult.push('  @THIS', '  D=M');
    pushToStack();
    asmResult.push('  @THAT', '  D=M');
    pushToStack();
    // reposite the ARG
    asmResult.push('  @SP', '  D=M');
    asmResult.push(`  @${arg2 + 5}`, '  D=D-A', '  @ARG', '  M=D');
    // LCL points to SP
    asmResult.push('  @SP', '  D=M', '  @LCL', '  M=D'); 
    // goto the callee
    asmResult.push(`  @${arg1}`, '  0;JMP');
    asmResult.push(`(${arg1}RET${base.funcOp})`);
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

const encounterPushStatic = (value, currentFile) => {
  asmResult.push(`  @${currentFile}.${value}`, '  D=M');
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

const encounterPopStatic = (value, currentFile) => {
  popFromStack();
  asmResult.push(`  @${currentFile}.${value}`, '  M=D');
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
const leaveComment = (params) => {
  const { op, arg1, arg2 } = params;
  if (arg2 !== undefined && op !== undefined) {
    asmResult.push(`// ${op} ${arg1} ${arg2}`);
  } else if (arg1 !== undefined && op !== undefined) {
    asmResult.push(`// ${op} ${arg1}`);
  } else {
    asmResult.push(`// ${arg1}`);
  }
}

module.exports = codeWriter;

