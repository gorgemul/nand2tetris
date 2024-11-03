const fs = require('fs');
const path = require('path');
const parser = require('./Parser');
const codeWriter = require('./CodeWriter');

const resolveDirName = (dirPath) => {
  let fixedDirPath = dirPath;
  if (dirPath.endsWith('/')) {
    fixedDirPath = dirPath.substring(0, dirPath.length - 1);
  }
  const outputFilePrefix = fixedDirPath.slice(fixedDirPath.lastIndexOf('/') + 1);
  const outputFileName = `${outputFilePrefix}.asm`;
  const outputPath = path.resolve(dirPath, outputFileName);
  return outputPath;
}

const VMDirTranslator = (VMPath) => {
  fs.stat(VMPath, (err, stats) => {
    if (err) {
      console.error('file.stat encounter some errors:', err);
      return;
    }
    if (stats.isDirectory()) {
      fs.readdir(VMPath, (err, files) => {
        if (err) {
          console.error('fs.readdir encounter some errors:', err);
        }
        const inputFileArr = files.filter((fileName) => fileName.includes('.vm') && fileName !== 'Sys.vm');
        const initFileArr = files.filter((fileName) => fileName === 'Sys.vm'); // initFileArr = ['Sys.vm'];
        const initFileName = initFileArr[0];
        inputFileArr.push(initFileName); // push init file into the end of the module file;
        const outputFilePath = resolveDirName(VMPath);
        if (initFileArr.length === 1) {
          for (let i = 0; i < inputFileArr.length; i++) {
            try {
              const isFirstTime = i === 0 ? true : false;
              if (isFirstTime) {
                console.log('==========编译开始!==========');
              }
              const isLastTime = i === inputFileArr.length - 1 ? true : false;
              const inputFilePath = path.join(VMPath, inputFileArr[i]);
              const inputFileData = fs.readFileSync(inputFilePath, 'utf-8');
              const parsedData = parser(inputFileData);
              const outputData = codeWriter({ parsedCommands: parsedData, isFirstTime, isLastTime, currentFile: path.basename(inputFilePath, path.extname(inputFilePath))});
              if (isFirstTime) {
                fs.writeFileSync(outputFilePath, outputData);
              } else {
                fs.appendFileSync(outputFilePath, '\n');  // 在原文件末尾插入换行符
                fs.appendFileSync(outputFilePath, outputData);
              }
              if (isLastTime) {
                console.log(`成功编译init文件: ${inputFileArr[i]}.`);
                console.log(`成功生成${path.basename(outputFilePath)}文件!`);
                console.log('==========编译结束!==========');
              } else {
                console.log(`成功编译module文件: ${inputFileArr[i]}.`);
              }
            } catch (err) {
              console.error('compilation process of vm dir encounter errors:', err);
              return;
            }
          }
        } else {
          console.error('Sys.vm does not exist in the current folder');
          return;
        }
      })
    } else {
      if (VMFilePath.includes('.vm')) {
        try {
          console.log('==========编译开始!==========');
          const inputFileData = fs.readFileSync(VMPath, 'utf-8');
          const parsedData = parser(inputFileData);
          const outputData = codeWriter({ parsedCommands: parsedData, isFirstTime: false, isLastTime: true, currentFile: path.basename(VMPath, path.extname(VMPath))});
          const outputPath = VMPath.replace('.vm', '.asm');
          fs.writeFileSync(outputPath, outputData);
          console.log(`成功编译${path.basename(VMPath)}文件.`);
          console.log(`成功生成${path.basename(outputPath)}文件!`);
          console.log('==========编译结束!==========');
        } catch (err) {
          console.error('compilation process of vm file encounter errors:', err);
          return;
        }
      } else {
        console.error('编译文件必须是.vm文件!');
        return;
      }
    }
  })
}

const VMFilePath = process.argv.slice(2)[0];

VMDirTranslator(VMFilePath);




