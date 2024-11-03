const compilationEngine = require('./CompilationEngine');

const symbols = ['{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'];
const keywords = ['class', 'constructor', 'function', 'method', 'field', 'static', 'var', 'int', 'char', 'boolean', 'void', 'true', 'false', 'null', 'this', 'let', 'do', 'if', 'else', 'while', 'return'];

// 主函数
const JackTokenizer = (inputFileData) => {
  const tokenArray = filterData(inputFileData);
  const tokenizedData = advande(tokenArray);
  // Add new function to output new information about identifier

  // 
  console.log('tokenizeData>>>>', JSON.stringify(tokenizedData, null, 2));
  const xmlContent = getXmlContent(tokenizedData);
  return xmlContent;
}

const filterData = (jackCode) => {
  // 过滤规则
  const commentRegex = /\/\*[\s\S]*?\*\/|\/\/.*|\/\*\*[\s\S]*?\*\//g;

  // 过滤代码comment
  const codeWithoutComments = jackCode.replace(commentRegex, '').trim();


  // 转化为array (并且让“”里面的内容不被切割)
  const codeArray = codeWithoutComments.split(/ +(?=(?:(?:[^"]*"){2})*[^"]*$)/).filter(item => item !== '');

  // 消除换行转义符
  const filterArray = codeArray.map((item) => {
    return item.replace(/[\n\r\t]/g, '');
  }).filter(data => data !== '');

  // 切割里面的symbol
  const sliceSymbol = filterArray.map((item) => {
    let symbolIndex = [];
    let dataArray = [];

    // 筛选出symbol在每一个item里面的位置
    for (let i = 0; i < item.length; i++) {
      if (symbols.includes(item[i])) {
        symbolIndex.push(i);
      }
    }
    for (let k = 0; k < symbolIndex.length; k++) {
      // 如果item只含有'.'symbol
      if (symbolIndex.length === 1) {
        let firstPart = item.slice(0, symbolIndex[k]);
        let secondPart = item.slice(symbolIndex[k], symbolIndex[k] + 1);
        let thirdPart = item.slice(symbolIndex[k] + 1);
        dataArray.push(firstPart, secondPart, thirdPart);
      } else {
        if (k === 0) {
          let firstPart = item.slice(0, symbolIndex[k]);
          let secondPart = item.slice(symbolIndex[k], symbolIndex[k] + 1);
          dataArray.push(firstPart, secondPart);
        } else if (k === symbolIndex.length - 1 && symbolIndex[k - 1] + 1 === symbolIndex[k]) { // item === '(((y' 的时候, 不一定正确
          let firstPart = item.slice(symbolIndex[k - 1] + 1, symbolIndex[k] + 1);
          let secondPart = item.slice(symbolIndex[k] + 1);    // item = (~(key
          dataArray.push(firstPart, secondPart);
        } else if (k === symbolIndex.length - 1 && symbolIndex[k] !== item.length) { // item = "Output.printInt(sum"
          let firstPart = item.slice(symbolIndex[k - 1] + 1, symbolIndex[k]);
          let secondPart = item.slice(symbolIndex[k], symbolIndex[k] + 1);
          let thirdPart = item.slice(symbolIndex[k] + 1);
          dataArray.push(firstPart, secondPart, thirdPart);
        } else {
          let firstPart = item.slice(symbolIndex[k - 1] + 1, symbolIndex[k]);
          let secondPart = item.slice(symbolIndex[k], symbolIndex[k] + 1);
          dataArray.push(firstPart, secondPart);
        }
      }
    }
    if (dataArray.length > 0) {
      return dataArray;
    } else {
      return item;
    }
  })

  const tokenArray = sliceSymbol.flat().map((item) => {
    return item.replace(/[\n\r\t]/g, '');
  }).filter(data => data !== '');

  return tokenArray;
}

const hasMoreTokens = (token) => {
  return token.length > 0 ? true : false;
}

const getTokenType = (token) => {
  if (keywords.includes(token)) {
    return 'KEYWORD';
  } else if (symbols.includes(token)) {
    return 'SYMBOL';
  } else if (token[0] === '"' && token[token.length - 1] === '"') {
    return 'STRING_CONST';
  } else if (!isNaN(token)) {
    if (+token < 0 || +token > 32767) {
      throw new Error('Unvalid number in the token!!!');
    }
    return 'INT_CONST';
  } else {
    if (isNaN(token[0])) {
      return 'IDENTIFIER';
    } else {
      throw new Error('Identifier cant start with a digit!!!');
    }
  }
}

const getTokenValue = (token, tokenType) => {
  switch (tokenType) {
    case 'KEYWORD':
    case 'SYMBOL':
    case 'IDENTIFIER':
      return token;
    case 'STRING_CONST':
      return token.slice(1, token.length - 1);
    case 'INT_CONST':
      return token;
  }
}

const advande = (tokenArray) => {
  const tokenizedData = tokenArray.map((token) => {
    if (hasMoreTokens(token)) {
      const tokenType = getTokenType(token);
      const tokenValue = getTokenValue(token, tokenType);
      return { tokenType, tokenValue };
    }
  })
  return tokenizedData;
}

const getXmlContent = (tokenizedData) => {
  let xmlArray = [];
  xmlArray = compilationEngine(tokenizedData);
  const xmlContent = xmlArray.join('\n');
  return xmlContent;
}

module.exports = JackTokenizer;
