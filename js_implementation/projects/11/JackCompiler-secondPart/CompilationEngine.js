const space = '\x20';
const escapeSymbols = ['<', '>', '&'];
const classKeywords = ['static', 'field'];
const subroutineKeywords = ['constructor', 'function', 'method'];
const statementKeywords = ['let', 'if', 'while', 'do', 'return'];
const ops = ['+', '-', '*', '/', '&', '|', '<', '>', '='];
const unaryOps = ['-', '~'];

let counter;
let layer;
let xmlArray;
let currentTokenValue;
let tokenArray;

// main function
const compilationEngine = (tokenizedData) => {
  // 初始化数据
  counter = 0;
  layer = 0;
  xmlArray = [];
  currentTokenValue = null;
  tokenArray = [];

  tokenArray = JSON.parse(JSON.stringify(tokenizedData));
  currentTokenValue = tokenArray[counter].tokenValue;
  compileClass();
  return xmlArray;
}

// helper function --->
const process = (str) => {
  if (str === currentTokenValue) {
    handleToken(tokenArray[counter]);
  } else {
    throw new Error('当前token和传入token不匹配!');
  }
  counter++;
  currentTokenValue = tokenArray[counter]?.tokenValue;
}

const handleToken = (token) => {
  const { tokenType, tokenValue } = token;
  if (tokenType === 'KEYWORD') {
    writeHierarchicalTag(`<keyword> ${tokenValue} </keyword>`);
  } else if (tokenType === 'SYMBOL') {
    if (escapeSymbols.includes(tokenValue)) {
      const parsedTokenValue = parseEscapeSymbol(tokenValue);
      writeHierarchicalTag(`<symbol> ${parsedTokenValue} </symbol>`);
    } else {
      writeHierarchicalTag(`<symbol> ${tokenValue} </symbol>`);
    }
  } else if (tokenType === 'IDENTIFIER') {
    writeHierarchicalTag(`<identifier> ${tokenValue} </identifier>`);
  } else if (tokenType === 'STRING_CONST') {
    writeHierarchicalTag(`<stringConstant> ${tokenValue} </stringConstant>`);
  } else if (tokenType === 'INT_CONST') {
    writeHierarchicalTag(`<integerConstant> ${tokenValue} </integerConstant>`);
  }
}

const getCurrentIndentation = () => {
  return {
    indens: space.repeat(layer * 2),
    currentLayer: layer,
  }
}

const writeHierarchicalTag = (str, endTagIndentation) => {
  if (endTagIndentation !== undefined) {
    xmlArray.push(`${endTagIndentation}${str}`);
  } else {
    const indentation = space.repeat(layer * 2);
    xmlArray.push(`${indentation}${str}`);
  }
}

const parseEscapeSymbol = (value) => {
  if (value === '<') {
    return '&lt;';
  } else if (value === '>') {
    return '&gt;';
  } else if (value === '&') {
    return '&amp;';
  }
}

// <--- helper function

// --> Program structure

const compileClass = () => {
  const { indens, currentLayer } = getCurrentIndentation();
  writeHierarchicalTag(`<class>`);
  // 'class'
  layer++;
  process('class');
  // className
  process(currentTokenValue);
  // '{'
  process('{');
  // classVarDec*
  while (classKeywords.includes(currentTokenValue)) {
    compileClassVarDec();
  }
  // subroutineDec*
  while (subroutineKeywords.includes(currentTokenValue)) {
    compileSubroutineDec();
  }
  // '}'
  process('}');
  writeHierarchicalTag(`</class>`, indens);
  layer = currentLayer;
}

const compileClassVarDec = () => {
  const { indens, currentLayer } = getCurrentIndentation();
  writeHierarchicalTag('<classVarDec>');
  layer++;
  // ('static' | 'field)
  process(currentTokenValue);
  // type
  process(currentTokenValue);
  // varName
  process(currentTokenValue);
  // (',' varName)*
  while (currentTokenValue === ',') {
    process(',');
    process(currentTokenValue);
  }
  // ';'
  process(';');
  writeHierarchicalTag('</classVarDec>', indens);
  layer = currentLayer;
}

const compileSubroutineDec = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<subroutineDec>');
  layer++;
  // ('constructor' | 'function' | 'method')
  process(currentTokenValue);
  // (type | 'void')
  process(currentTokenValue);
  // subroutineName
  process(currentTokenValue);
  // '('
  process('(');
  // parameterList
  compileParameterList();
  // ')'
  process(')');
  // subroutineBody
  compileSubroutineBody();
  writeHierarchicalTag('</subroutineDec>', indens);
  layer = currentLayer;
}

const compileParameterList = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<parameterList>');
  layer++;
  // ((type varName) (',' type varName)*)?
  // (type VarName)?
  if (currentTokenValue !== ')') {
    process(currentTokenValue);
    process(currentTokenValue);
    while (currentTokenValue === ',') {
      process(',');
      process(currentTokenValue);
      process(currentTokenValue);
    }
  }
  // ((',' type varName)*)?
  writeHierarchicalTag('</parameterList>', indens);
  layer = currentLayer;
}

const compileSubroutineBody = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<subroutineBody>');
  layer++;
  process('{');
  // varDec*
  while (currentTokenValue === 'var') {
    compileVarDec();
  }
  // statements
  compileStatements();
  process('}');
  writeHierarchicalTag('</subroutineBody>', indens);
  layer = currentLayer;
}

const compileVarDec = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<varDec>');
  layer++;
  // 'var'
  process('var');
  // type varName
  process(currentTokenValue);
  process(currentTokenValue);
  // (',' varName)*
  while (currentTokenValue === ',') {
    process(',');
    // varName
    process(currentTokenValue);
  }
  process(';');
  writeHierarchicalTag('</varDec>', indens);
  layer = currentLayer;
}

// <-- Program structure

// --> Statements

const compileStatements = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  // statement*
  writeHierarchicalTag('<statements>')
  layer++;
  while (statementKeywords.includes(currentTokenValue)) {
    // letStatement | ifStatement | whileStatement | doStatement | returnStatement
    switch (currentTokenValue) {
      case 'let':
        compileLetStatement();
        break;
      case 'if':
        compileIfStatement();
        break;
      case 'while':
        compileWhileStatement();
        break;
      case 'do':
        compileDoStatement();
        break;
      case 'return':
        compileReturnStatement();
        break;
    }
  }
  writeHierarchicalTag('</statements>', indens);
  layer = currentLayer;
}

const compileLetStatement = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<letStatement>');
  layer++;
  // 'let'
  process('let');
  // varName
  process(currentTokenValue);
  // ('[' expression ']')
  if (currentTokenValue === '[') {
    process('[');
    compileExpression();
    process(']');
  }
  // '='
  process('=');
  // expression
  compileExpression();
  // ';'
  process(';');
  writeHierarchicalTag('</letStatement>', indens);
  layer = currentLayer;
}

const compileIfStatement = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<ifStatement>');
  layer++;
  process('if');
  process('(');
  // expression
  compileExpression();
  process(')');
  process('{');
  compileStatements();
  process('}');
  if (currentTokenValue === 'else') {
    process('else');
    process('{');
    compileStatements();
    process('}');
  }
  writeHierarchicalTag('</ifStatement>', indens);
  layer = currentLayer;
}

const compileWhileStatement = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<whileStatement>');
  layer++;
  process('while');
  process('(');
  // expression
  compileExpression();
  process(')');
  process('{');
  compileStatements();
  process('}');
  writeHierarchicalTag('</whileStatement>', indens);
  layer = currentLayer;
}

const compileDoStatement = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<doStatement>');
  layer++;
  process('do');
  // subroutineName || (className | varName)
  process(currentTokenValue);
  if (currentTokenValue === '(') { // subroutineName'('expressionList')'
    process('(');
    const { currentLayer, indens } = getCurrentIndentation();
    writeHierarchicalTag('<expressionList>');
    layer++;
    if (currentTokenValue !== ')') {
      compileExpression();
      while (currentTokenValue === ',') {
        process(',');
        compileExpression();
      }
    }
    writeHierarchicalTag('</expressionList>', indens);
    layer = currentLayer;
    process(')');
  } else if (currentTokenValue === '.') { // (className | varName)'.'subroutineName'('expressionList')'
    process('.');
    process(currentTokenValue);
    process('(');
    const { currentLayer, indens } = getCurrentIndentation();
    writeHierarchicalTag('<expressionList>');
    layer++;
    if (currentTokenValue !== ')') {
      compileExpression();
      while (currentTokenValue === ',') {
        process(',');
        compileExpression();
      }
    }
    writeHierarchicalTag('</expressionList>', indens);
    layer = currentLayer;
    process(')');
  }
  process(';');
  writeHierarchicalTag('</doStatement>', indens);
  layer = currentLayer;
}

const compileReturnStatement = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<returnStatement>');
  layer++;
  process('return');
  if (currentTokenValue !== ';') {
    compileExpression();
  }
  process(';');
  writeHierarchicalTag('</returnStatement>', indens);
  layer = currentLayer;
}

// <-- Statements

// --> Expressions

const compileExpression = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<expression>');
  layer++;
  compileTerm();
  while (ops.includes(currentTokenValue)) {
    // op
    process(currentTokenValue);
    // term
    compileTerm();
  }
  writeHierarchicalTag('</expression>', indens);
  layer = currentLayer;
}

const compileTerm = () => {
  const { currentLayer, indens } = getCurrentIndentation();
  writeHierarchicalTag('<term>');
  layer++;
  // (unaryOp term)
  if (unaryOps.includes(currentTokenValue)) {
    // unaryOp
    process(currentTokenValue);
    // term
    compileTerm();
  } else {
    // '('expression')'
    if (currentTokenValue === '(') {
      process('(');
      // expression
      compileExpression();
      process(')');
      // integerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall
    } else {
      process(currentTokenValue);
      // subroutineCall
      if (currentTokenValue === '(') { // subroutineName'('expressionList')'
        process('(');
        const { currentLayer, indens } = getCurrentIndentation();
        writeHierarchicalTag('<expressionList>');
        layer++;
        if (currentTokenValue !== ')') {
          compileExpression();
          while (currentTokenValue === ',') {
            process(',');
            compileExpression();
          }
        }
        writeHierarchicalTag('</expressionList>', indens);
        layer = currentLayer;
        process(')');
      } else if (currentTokenValue === '.') { // (className | varName)'.'subroutineName'('expressionList')'
        process('.');
        process(currentTokenValue);
        process('(');
        const { currentLayer, indens } = getCurrentIndentation();
        writeHierarchicalTag('<expressionList>');
        layer++;
        if (currentTokenValue !== ')') {
          compileExpression();
          while (currentTokenValue === ',') {
            process(',');
            compileExpression();
          }
        }
        writeHierarchicalTag('</expressionList>', indens);
        layer = currentLayer;
        process(')');
      } else if (currentTokenValue === '[') {
        process('[');
        compileExpression();
        process(']');
      }
    }
  }
  writeHierarchicalTag('</term>', indens);
  layer = currentLayer;
}

// <-- Expressions

module.exports = compilationEngine;