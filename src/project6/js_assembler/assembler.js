const fs = require('fs');
const parser = require('./parser');

const inputFilePath = process.argv.slice(2)[0]; // 从terminal接受用户传入的asm文件
const inputFileName = inputFilePath.split('.')[0];

const outputFilePath = `${inputFileName}.hack`;
const outputContent = parser(inputFilePath);

fs.writeFile(outputFilePath, outputContent, (err) => {
  if (err) {
    console.error('写入文件时出错: ', err);
  } else {
    console.log('成功生成hack文件');
  }
});