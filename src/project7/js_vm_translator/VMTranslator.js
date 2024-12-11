const fs = require('fs');
const path = require('path');
const parser = require('./Parser');
const codeWriter = require('./CodeWriter');

const createFile = (vmFilePath) => {
  const outputFilePath = vmFilePath.replace(/\.vm$/, '.asm');
  const parsedContent = parser(vmFilePath);
  const outputContent = codeWriter(parsedContent);
  const currentDir = __dirname;
  const targetDir = path.resolve(currentDir, outputFilePath); 

  fs.writeFile(targetDir, outputContent, (err) => {
    if (err) {
      console.error('写入文件时出错: ', err);
    } else {
      console.log('成功生成asm文件');
    }
  });
}

const vmFilePath = process.argv.slice(2)[0];

createFile(vmFilePath);


