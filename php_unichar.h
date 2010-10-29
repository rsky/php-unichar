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

#ifndef _PHP_UNICHAR_H_
#define _PHP_UNICHAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <php_ini.h>
#include <SAPI.h>
#include <ext/standard/info.h>
#include <Zend/zend_extensions.h>

#ifdef ZTS
#include "TSRM.h"
#endif

#if defined(WITH_UNICHAR_ICU) && !defined(IS_UNICODE)
#include <unicode/ucnv.h>
#endif

#define PHP_UNICHAR_MODULE_VERSION "0.4.0"

ZEND_BEGIN_MODULE_GLOBALS(unichar)
	char *entity_buf;
	HashTable entity_ht;
#if defined(WITH_UNICHAR_ICU) && !defined(IS_UNICODE)
	UConverter *conv;
#endif
ZEND_END_MODULE_GLOBALS(unichar)

#ifdef ZTS
#define UNICHAR_G(v) TSRMG(unichar_globals_id, zend_unichar_globals *, v)
#else
#define UNICHAR_G(v) (unichar_globals.v)
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* _PHP_UNICHAR_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
