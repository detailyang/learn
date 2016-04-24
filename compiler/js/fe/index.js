/*
* @Author: detailyang
* @Date:   2016-04-25 02:40:20
* @Last Modified by:   detailyang
* @Last Modified time: 2016-04-25 03:17:46
*/

'use strict';

import Tokenizer from './tokenizer';

const code = `var x = 1 + 2;
/* test */
`
const tokenizer = new Tokenizer(code, {});
tokenizer.next()

console.log(tokenizer);