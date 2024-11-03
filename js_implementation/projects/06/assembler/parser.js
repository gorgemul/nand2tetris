const fs = require('fs');

const translationModule = require('./code');
const symbolTalble = require('./symbolTable');

let pc = 0;
let base = +symbolTalble.get('base');
let nthPass = 1;

// 读取文件内容
const parser = (filePath) => {
  if (filePath.split('.')[1] !== 'asm') {
    throw new Error('编译文件后缀应为.asm');
  }
  const inputContent = initializer(filePath);
  advance(inputContent);
  nthPass++;
  return advance(inputContent);
}

const initializer = (filePath) => {
  const fileContent = fs.readFileSync(filePath, 'utf-8');
  return fileContent;
}

const hasMoreLines = (line) => {
  return line.length > 0 ? true : false;
}

const instructionType = (instruction) => {
  if (instruction.startsWith('@')) {
    return 'A_INSTRUCTION';
  } else if (instruction.startsWith('(')) {
    return 'L_INSTRUCTION';
  } else {
    return 'C_INSTRUCTION';
  }
}

const symbol = (instruction) => {
  const type = instructionType(instruction);
  switch(type) {
    case 'A_INSTRUCTION':
      return instruction.substring(1, instruction.length);
    case 'L_INSTRUCTION':
      return instruction.substring(1, instruction.length - 1);
  }
}

const dest = (instruction) => {
  if (instruction.includes('=')) {
    const tartget = instruction.split('=')[0];
    return tartget;
  } else {
    return '';
  }
}

const comp = (instruction) => {
  const pattern1 = /=(.*?);/; // 既有等号又有分号，捕获等号和分号中间的string
  const pattern2 = /=(.+)/; //只有等号无分号
  const pattern3 = /([^;]+);/; //只有分号无等号
  if (instruction.includes('=') && instruction.includes(';')) {
    return instruction.match(pattern1)[1];
  } else if (instruction.includes('=') && !instruction.includes(';')) {
    return instruction.match(pattern2)[1];
  } else if (!instruction.includes('=') && instruction.includes(';')) {
    return instruction.match(pattern3)[1];
  } else {
    throw new Error('格式有误，无法提取信息');
  }
}

const jump = (instruction) => {
  if (instruction.includes(';')) {
    const target = instruction.split(';')[1];
    return target;
  } else {
    return '';
  }
}

const advance = (fileContent) => {
  const lines = fileContent.split('\n').filter(str => str.trim() !== '');
  const filterdLines = lines.map(line => {
    if (hasMoreLines(line)) {
      return lineWithoutComment = line.split('//')[0].trim();
    }
  })
  while (nthPass == 1) {
    const firstResult = filterdLines.filter(str => str.trim() !== '').map((instruction) => {
      const currentType = instructionType(instruction);
      switch(currentType) {
        case 'A_INSTRUCTION':
          pc++;
          return;
        case 'C_INSTRUCTION':
          pc++;
          return;
        case 'L_INSTRUCTION':
          const label = symbol(instruction);
          symbolTalble.set(label, `${pc}`);
          return;
      }
    })
    return firstResult;
  }
  while (nthPass == 2) {
    const binaryInstruction = filterdLines.filter(str => str.trim() !== '').map((instruction) => {
      const currentType = instructionType(instruction);
      switch(currentType) {
        case 'A_INSTRUCTION':
          const ASymbol = symbol(instruction);
          let bianryAInstruction;
          if (isNaN(ASymbol)) {
            if (symbolTalble.has(ASymbol)) {
              const savedSymbol = +symbolTalble.get(ASymbol);
              bianryAInstruction = savedSymbol.toString(2).padStart(16, '0');
            } else {
              bianryAInstruction = base.toString(2).padStart(16, '0');
              symbolTalble.set(ASymbol, `${base}`);
              base++;
            }
          } else {
            const numberForm = +ASymbol;
            bianryAInstruction = numberForm.toString(2).padStart(16, '0');
          }
          pc++;
          return bianryAInstruction;
        case 'C_INSTRUCTION':
          const destInstruction = dest(instruction);
          const compInstruction = comp(instruction);
          const jumpInstruction = jump(instruction);
          const binaryComp = translationModule.comp(compInstruction);
          const binaryDest = translationModule.dest(destInstruction);
          const binaryJump = translationModule.jump(jumpInstruction);
          const binaryCInstruction = `111${binaryComp}${binaryDest}${binaryJump}`;
          pc++;
          return binaryCInstruction;
        case 'L_INSTRUCTION':
          return '';
      }
    })
    // 将过滤后的行重新拼接成一个字符串(分行)
    const result = binaryInstruction.filter(str => str.trim() !== '').join('\n');
    return result;
  }
}

module.exports = parser;