const symbolTalble = new Map();

const initializer = () => {
  symbolTalble.set('base', '16');
  symbolTalble.set('SP', '0');
  symbolTalble.set('LCL', '1');
  symbolTalble.set('ARG', '2');
  symbolTalble.set('THIS', '3');
  symbolTalble.set('THAT', '4');
  symbolTalble.set('SCREEN', '16384');
  symbolTalble.set('KBD', '24576');
  for (let i = 0; i  < 16; i++) {
    symbolTalble.set(`R${i}`, `${i}`);
  };
}

initializer();

module.exports = symbolTalble;
