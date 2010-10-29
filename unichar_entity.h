/**
 * Unicode Character Utility Extension for PHP
 *
 * Copyright (c) 2007-2010 Ryusuke SEKIYAMA. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @package     php-unichar
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2007-2010 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#ifndef _PHP_UNICHAR_ENTITY_H_
#define _PHP_UNICHAR_ENTITY_H_

#include <stdint.h>

/* {{{ type definitions */

typedef int8_t utf8size_t;
typedef uint32_t unichar_t;

typedef struct _unichar_entity_map_t {
	char *name;
	uint size;
	uint16_t code;
} unichar_entity_map_t;

/* }}} */

/* {{{ constants */

#define UNICHAR_UTF8_MAXLEN 4
#define UNICHAR_UTF8_MAXBUFSIZE 5
#define UNICHAR_UTF8_FAILURE 0x80000000

#define UNICHAR_ENTITY_MAXLEN 8
#define UNICHAR_ENTITY_MAXBUFSIZE 9
#define UNICHAR_ENT_STRICT        (1 << 0)
#define UNICHAR_ENT_ALLOW_CTRL     0
#define UNICHAR_ENT_CTRL_TO_SPACE (1 << 1)
#define UNICHAR_ENT_NBSP_TO_SPACE (1 << 2)
#define UNICHAR_ENT_IGNORE_NUL    (1 << 3)
#define UNICHAR_ENT_IGNORE_CTRL   (1 << 4)
#define UNICHAR_ENT_REMOVE_NUL    (1 << 5)
#define UNICHAR_ENT_REMOVE_CTRL   (1 << 6)
#define UNICHAR_ENT_WARN_NUL      (1 << 7)
#define UNICHAR_ENT_WARN_CTRL     (1 << 8)

#define UNICHAR_BOM_CODE 0xFEFF
#define UNICHAR_BOM_UTF8 "\xEF\xBB\xBF"
#define UNICHAR_BOM_UTF16BE "\xFE\xFF"
#define UNICHAR_BOM_UTF16LE "\xFF\xFE"
#define UNICHAR_BOM_UTF32BE "\x00\x00\xFE\xFF"
#define UNICHAR_BOM_UTF32LE "\xFF\xFE\x00\x00"

/* }}} */

/* {{{ utility macros */

#define UE(name, code) { name, sizeof(name), code }

#define is_utf8_tail(n) ((n) > 0x7F && (n) < 0xC0)
#define is_not_utf8_tail(n) ((n) < 0x80 || (n) > 0xBF)
/*#define is_utf8_tail(n) (((n) & 0xC0) == 0x80)
#define is_not_utf8_tail(n) (((n) & 0xC0) != 0x80)*/

#define UNICHAR_REGISTER_CONSTANT(name) \
	REGISTER_LONG_CONSTANT(#name, (long)name, CONST_PERSISTENT | CONST_CS)

#define UNICHAR_REGISTER_CONSTANT_EX(name, value) \
	REGISTER_LONG_CONSTANT(#name, (long)(value), CONST_PERSISTENT | CONST_CS)

#define UNICHAR_REGISTER_CONSTANT_STRING(name) \
	REGISTER_STRING_CONSTANT(#name, (long)name, CONST_PERSISTENT | CONST_CS)

/* }}} */

/* {{{ private function prototypes */

static char *
unichar_unichr_ex(unichar_t code, char **buf, size_t size, utf8size_t *len,
		zend_bool report_errors TSRMLS_DC);

#define unichar_unichr(code, buf, size, len) \
	unichar_unichr_ex((code), (buf), (size), (len), 1 TSRMLS_CC)

#define unichar_unichr_silent(code, buf, size, len) \
	unichar_unichr_ex((code), (buf), (size), (len), 0 TSRMLS_CC)

static unichar_t
unichar_uniord_ex(const char *str, size_t size, utf8size_t *len,
		zend_bool report_errors TSRMLS_DC);

#define unichar_uniord(str, size, len) \
	unichar_uniord_ex((str), (size), (len), 1 TSRMLS_CC)

#define unichar_uniord_silent(str, size, len) \
	unichar_uniord_ex((str), (size), (len), 0 TSRMLS_CC)

static void
unichar_init_entity_table(HashTable *ht, char **buf);

/* }}} */

/* {{{ unichar_register_ent_constants()
   */
static inline void
unichar_register_ent_constants(int module_number TSRMLS_DC)
{
	UNICHAR_REGISTER_CONSTANT(UNICHAR_ENT_STRICT);
	UNICHAR_REGISTER_CONSTANT(UNICHAR_ENT_ALLOW_CTRL);
	UNICHAR_REGISTER_CONSTANT(UNICHAR_ENT_CTRL_TO_SPACE);
	UNICHAR_REGISTER_CONSTANT(UNICHAR_ENT_NBSP_TO_SPACE);
	UNICHAR_REGISTER_CONSTANT(UNICHAR_ENT_IGNORE_NUL);
	UNICHAR_REGISTER_CONSTANT(UNICHAR_ENT_IGNORE_CTRL);
	UNICHAR_REGISTER_CONSTANT_EX(UNICHAR_ENT_REMOVE_NUL,  UNICHAR_ENT_IGNORE_NUL  | UNICHAR_ENT_REMOVE_NUL);
	UNICHAR_REGISTER_CONSTANT_EX(UNICHAR_ENT_REMOVE_CTRL, UNICHAR_ENT_IGNORE_CTRL | UNICHAR_ENT_REMOVE_CTRL);
	UNICHAR_REGISTER_CONSTANT_EX(UNICHAR_ENT_WARN_NUL,    UNICHAR_ENT_IGNORE_NUL  | UNICHAR_ENT_WARN_NUL);
	UNICHAR_REGISTER_CONSTANT_EX(UNICHAR_ENT_WARN_CTRL,   UNICHAR_ENT_IGNORE_CTRL | UNICHAR_ENT_WARN_CTRL);
	/*
	UNICHAR_REGISTER_CONSTANT(UNICHAR_BOM_CODE);
	UNICHAR_REGISTER_CONSTANT_STRING(UNICHAR_BOM_UTF8);
	UNICHAR_REGISTER_CONSTANT_STRING(UNICHAR_BOM_UTF16BE);
	UNICHAR_REGISTER_CONSTANT_STRING(UNICHAR_BOM_UTF16LE);
	UNICHAR_REGISTER_CONSTANT_STRING(UNICHAR_BOM_UTF32BE);
	UNICHAR_REGISTER_CONSTANT_STRING(UNICHAR_BOM_UTF32LE);
	*/
}
/* }}} */

#endif /* _PHP_UNICHAR_ENTITY_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
