const fs = require('fs');

// 匹配规则
const arithmeticMatchPattern = /\b(add|sub|neg|eq|gt|lt|and|or|not)\b/g;

const popMatchPattern = /\b(pop)\b/g;
const pushMatchPattern = /\b(push)\b/g;

const labelMatchPattern = /\b(label)\b/g;
const gotoMatchPattern = /\b(goto)\b/g;
const ifGotoMatchPattern = /\b(if-goto)\b/g;

const declareFuncMatchPattern = /\b(function)\b/g;
const callFuncMatchPattern = /\b(call)\b/g;
const returnFuncMatchPattern = /\b(return)\b/g;


// const parser = (filePath) => {
//   if (filePath.slice(filePath.lastIndexOf('.')) !== '.vm') {
//     throw new Error('编译文件后缀应为.vm');
//   }
//   let inputContent = initializer(filePath);

//   const parsedContent = advance(inputContent);

//   return parsedContent;
// }

const parser = (inputFileData) => {
  const parsedContent = advance(inputFileData);
  return parsedContent;
}

const initializer = (filePath) => {
  const fileContent = fs.readFileSync(filePath, 'utf-8');
  return fileContent;
}

const hasMoreLines = (line) => {
  return line.length > 0 ? true : false;
}

const getCommandType = (line) => {
  const arithMeticOp = line.match(arithmeticMatchPattern);
  const popOp = line.match(popMatchPattern);
  const pushOp = line.match(pushMatchPattern);
  const labelOp = line.match(labelMatchPattern);
  const gotoOp = line.match(gotoMatchPattern);
  const ifGotoOp = line.match(ifGotoMatchPattern);
  const declareFuncOp = line.match(declareFuncMatchPattern);
  const callFuncOp = line.match(callFuncMatchPattern);
  const returnFuncOp = line.match(returnFuncMatchPattern);

  if (arithMeticOp) {
    return 'C_ARITHMETIC';
  } else if (popOp) {
    return 'C_POP';
  } else if (pushOp) {
    return 'C_PUSH';
  } else if (labelOp) {
    return 'C_BRANCHING_LABEL';
  } else if (ifGotoOp) {
    return 'C_BRANCHING_IFGOTO';
  } else if (gotoOp) {
    return 'C_BRANCHING_GOTO';
  } else if (declareFuncOp) {
    return 'C_FUNCTION_DECLARE';
  } else if (callFuncOp) {
    return 'C_FUNCTION_CALL';
  } else if (returnFuncOp) {
    return 'C_FUNCTION_RETURN';
  }
}

const getArg1 = (line) => {
  const arithMeticOp = line.match(arithmeticMatchPattern);
  const popOp = line.match(popMatchPattern);
  const pushOp = line.match(pushMatchPattern);
  const labelOp = line.match(labelMatchPattern);
  const gotoOp = line.match(gotoMatchPattern);
  const ifGotoOp = line.match(ifGotoMatchPattern);
  const declareFuncOp = line.match(declareFuncMatchPattern);
  const callFuncOp = line.match(callFuncMatchPattern);
  const returnFuncOp = line.match(returnFuncMatchPattern);

  if (arithMeticOp) {
    return arithMeticOp[0];
  } else if (returnFuncOp) {
    return returnFuncOp[0];
  } else if (popOp || pushOp || labelOp || gotoOp || ifGotoOp || declareFuncOp || callFuncOp) {
    const parseLine = line.split(' ').filter(str => str.length !== 0);
    return parseLine[1];
  }
}

const getArg2 = (line) => {
  const popOp = line.match(popMatchPattern);
  const pushOp = line.match(pushMatchPattern);
  const declareFuncOp = line.match(declareFuncMatchPattern);
  const callFuncOp = line.match(callFuncMatchPattern);

  if (popOp || pushOp || declareFuncOp || callFuncOp) {
    const parseLine = line.split(' ').filter(str => str.length !== 0);
    return +parseLine[2];
  }
}

const advance = (fileContent) => {
  const commandLines = fileContent.trim().split('\n');
  const filteredlines = commandLines.map((line) => {
    const filterComment = line.split('//')[0].trim();
    return filterComment;
  })
  const parsedCommands = filteredlines.filter(line => line.length > 0).map((line) => {
    const type = getCommandType(line);
    if (hasMoreLines(line)) {
      switch (type) {
        case 'C_ARITHMETIC':
        case 'C_BRANCHING_GOTO':
        case 'C_BRANCHING_IFGOTO':
        case 'C_BRANCHING_LABEL':
        case 'C_FUNCTION_RETURN':
          const parsedArithmeticData = { commandType: type, arg1: getArg1(line) };
          return parsedArithmeticData;
        case 'C_POP':
        case 'C_PUSH':
        case 'C_FUNCTION_CALL':
        case 'C_FUNCTION_DECLARE':
          const parsePopOrPushData = { commandType: type, arg1: getArg1(line), arg2: getArg2(line)};
          return parsePopOrPushData;
      }
    }
  })
  return parsedCommands;
}

module.exports = parser;