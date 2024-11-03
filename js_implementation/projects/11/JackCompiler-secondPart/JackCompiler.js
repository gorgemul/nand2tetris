const fs = require('fs');
const path = require('path');
const jackTokenizer = require('./JackTokenizer');

const JackAnalyzer = (compilationPath) => {
  fs.stat(compilationPath, (err, stats) => {
    if (err) {
      console.error('fs.stat encounter some errors: ', err);
      return;
    }
    if (stats.isDirectory()) {
      fs.readdir(compilationPath, (err, files) => {
        if (err) {
          console.error('fs.readdir encounter some errors: ', err);
          return;
        }
        const inputFileArr = files.filter(fileName => fileName.includes('.jack'));
        console.log('==========编译开始==========');
        for (let i = 0; i < inputFileArr.length; i++) {
          try {
            const inputFilePath = path.resolve(compilationPath, inputFileArr[i]);
            const outputFilePath = inputFilePath.replace('.jack', '.xml');
            const inputFileData = fs.readFileSync(inputFilePath, 'utf-8');
            const xmlContent = jackTokenizer(inputFileData);
            fs.writeFileSync(outputFilePath, xmlContent);
            console.log(`成功编译${path.basename(inputFilePath)}文件.`);
            console.log(`成功生成${path.basename(outputFilePath)}文件!`);
          } catch (err) {
            console.error('compile jack directory encounter some errors: ', err);
            return;
          }
        }
        console.log('==========编译结束==========');
      })
    } else {
      if (compilationPath.includes('.jack')) {
        try {
          console.log('==========编译开始==========');
          const inputFileData = fs.readFileSync(compilationPath, 'utf-8');
          const xmlContent = jackTokenizer(inputFileData);
          const outputFilePath = compilationPath.replace('.jack', '.xml');
          fs.writeFileSync(outputFilePath, xmlContent);
          console.log(`成功编译${path.basename(compilationPath)}文件.`);
          console.log(`成功生成${path.basename(outputFilePath)}文件!`);
          console.log('==========编译结束==========');
        } catch (err) {
          console.error('compile jack file encounter some errors:', err);
          return;
        }
      }
    }
  })
}

const compilationPath = process.argv.slice(2)[0];
JackAnalyzer(compilationPath);
