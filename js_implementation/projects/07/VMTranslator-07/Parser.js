const fs = require('fs');

// 匹配规则
const arithmeticMatchPattern = /\b(add|sub|neg|eq|gt|lt|and|or|not)\b/g;
const popMatchPattern = /\b(pop)\b/g;
const pushMatchPattern = /\b(push)\b/g;

const parser = (filePath) => {
  if (filePath.slice(filePath.lastIndexOf('.')) !== '.vm') {
    throw new Error('编译文件后缀应为.vm');
  }
  const inputContent = initializer(filePath);
  const parsedContent = advance(inputContent);

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
  const arithMeticOperation = line.match(arithmeticMatchPattern);
  const popOperation = line.match(popMatchPattern);
  const pushOperation = line.match(pushMatchPattern);

  if (arithMeticOperation) {
    return 'C_ARITHMETIC';
  } else if (popOperation) {
    return 'C_POP';
  } else if (pushOperation) {
    return 'C_PUSH';
  } else {
    return '';
  }
  
}

const getArg1 = (line) => {
  const arithMeticOperation = line.match(arithmeticMatchPattern);
  const popOperation = line.match(popMatchPattern);
  const pushOperation = line.match(pushMatchPattern);

  if (arithMeticOperation) {
    return arithMeticOperation[0];
  } else if (popOperation) {
    const parseLine = line.split(' ').filter(str => str.length !== 0);
    return parseLine[1];
  } else if (pushOperation) {
    const parseLine = line.split(' ').filter(str => str.length !== 0);
    return parseLine[1];
  } else {
    return '';
  }
}

const getArg2 = (line) => {
  const arithMeticOperation = line.match(arithmeticMatchPattern);
  const popOperation = line.match(popMatchPattern);
  const pushOperation = line.match(pushMatchPattern);

  if (arithMeticOperation) {
    return '';
  } else if (popOperation) {
    const parseLine = line.split(' ').filter(str => str.length !== 0);
    return +parseLine[2];
  } else if (pushOperation) {
    const parseLine = line.split(' ').filter(str => str.length !== 0);
    return +parseLine[2];
  } else {
    return '';
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
          const parsedArithmeticData = { commandType: type, arg1: getArg1(line) };
          return parsedArithmeticData;
        case 'C_POP':
        case 'C_PUSH':
          const parsePopOrPushData = { commandType: type, arg1: getArg1(line), arg2: getArg2(line)};
          return parsePopOrPushData;
      }
    }
  })
  return parsedCommands;
}

module.exports = parser;