/*
* @Author: detailyang
* @Date:   2016-04-25 02:40:20
* @Last Modified by:   detailyang
* @Last Modified time: 2016-04-26 00:50:42
*/

'use strict';

const Uglifyjs = require('./Uglifyjs');
const tokenizer = Uglifyjs.tokenizer;

// console.log(tokenizer);
// const input = tokenizer(code);
// for (;;) {
//     const token = input();
//     if (token.type === 'eof') {
//         break;
//     }
//     console.log(token);
// }

const mytokenizer = require('./tokenizer');
const code = `
    var x = 123;
    // a ++
    /*
    abcd
     */
`;

const myinput = mytokenizer(code);
for (;;) {
    const token = myinput();
    if (token.type === 'eof') {
        break;
    }
    console.log(token);
}

