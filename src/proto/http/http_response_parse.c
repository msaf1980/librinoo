
#line 1 "./http_response_parse.rl"
/**
 * @file   http_response_parse.rl
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2013
 * @date   Sun Apr 15 22:29:07 2012
 *
 * @brief  HTTP response parsing
 *
 *
 */

#include "rinoo/rinoo.h"


#line 17 "./http_response_parse.c"
static const char _httpres_reader_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	5, 1, 6, 1, 7, 1, 8, 1, 
	9, 1, 10, 1, 11, 1, 12, 2, 
	1, 2, 2, 7, 3, 2, 7, 8, 
	2, 8, 4
};

static const unsigned char _httpres_reader_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 5, 6, 
	7, 9, 10, 12, 14, 16, 17, 20, 
	23, 27, 32, 33, 36, 40, 43, 46, 
	50, 55, 60, 65, 70, 75, 80, 84, 
	89, 94, 99, 104, 109, 114, 117, 121, 
	128, 135, 136
};

static const char _httpres_reader_trans_keys[] = {
	72, 84, 84, 80, 47, 49, 46, 48, 
	49, 32, 48, 57, 48, 57, 48, 57, 
	32, 13, 0, 127, 13, 0, 127, 10, 
	13, 0, 127, 13, 67, 99, 33, 126, 
	10, 58, 33, 126, 32, 58, 33, 126, 
	13, 0, 127, 13, 0, 127, 10, 13, 
	0, 127, 58, 79, 111, 33, 126, 58, 
	78, 110, 33, 126, 58, 84, 116, 33, 
	126, 58, 69, 101, 33, 126, 58, 78, 
	110, 33, 126, 58, 84, 116, 33, 126, 
	45, 58, 33, 126, 58, 76, 108, 33, 
	126, 58, 69, 101, 33, 126, 58, 78, 
	110, 33, 126, 58, 71, 103, 33, 126, 
	58, 84, 116, 33, 126, 58, 72, 104, 
	33, 126, 58, 33, 126, 32, 58, 33, 
	126, 13, 0, 47, 48, 57, 58, 127, 
	13, 0, 47, 48, 57, 58, 127, 32, 
	0
};

static const char _httpres_reader_single_lengths[] = {
	0, 1, 1, 1, 1, 1, 1, 1, 
	2, 1, 0, 0, 0, 1, 1, 1, 
	2, 3, 1, 1, 2, 1, 1, 2, 
	3, 3, 3, 3, 3, 3, 2, 3, 
	3, 3, 3, 3, 3, 1, 2, 1, 
	1, 1, 0
};

static const char _httpres_reader_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 1, 0, 1, 1, 
	1, 1, 0, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 3, 
	3, 0, 0
};

static const unsigned char _httpres_reader_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12, 
	14, 17, 19, 21, 23, 25, 27, 30, 
	33, 37, 42, 44, 47, 51, 54, 57, 
	61, 66, 71, 76, 81, 86, 91, 95, 
	100, 105, 110, 115, 120, 125, 128, 132, 
	137, 142, 144
};

static const char _httpres_reader_indicies[] = {
	1, 0, 2, 0, 3, 0, 4, 0, 
	5, 0, 6, 0, 7, 0, 8, 9, 
	0, 10, 0, 11, 0, 12, 0, 13, 
	0, 14, 0, 16, 15, 0, 18, 17, 
	0, 19, 18, 17, 0, 20, 22, 22, 
	21, 0, 23, 0, 25, 24, 0, 26, 
	25, 24, 0, 28, 27, 0, 30, 29, 
	0, 19, 30, 29, 0, 25, 31, 31, 
	24, 0, 25, 32, 32, 24, 0, 25, 
	33, 33, 24, 0, 25, 34, 34, 24, 
	0, 25, 35, 35, 24, 0, 25, 36, 
	36, 24, 0, 37, 25, 24, 0, 25, 
	38, 38, 24, 0, 25, 39, 39, 24, 
	0, 25, 40, 40, 24, 0, 25, 41, 
	41, 24, 0, 25, 42, 42, 24, 0, 
	25, 43, 43, 24, 0, 44, 24, 0, 
	45, 25, 24, 0, 28, 27, 46, 27, 
	0, 47, 29, 48, 29, 0, 49, 0, 
	0, 0
};

static const char _httpres_reader_trans_targs[] = {
	0, 2, 3, 4, 5, 6, 7, 8, 
	9, 41, 10, 11, 12, 13, 14, 15, 
	16, 15, 16, 17, 18, 19, 24, 42, 
	19, 20, 21, 22, 23, 22, 23, 25, 
	26, 27, 28, 29, 30, 31, 32, 33, 
	34, 35, 36, 37, 38, 39, 40, 23, 
	40, 10
};

static const char _httpres_reader_trans_actions[] = {
	17, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 19, 1, 0, 0, 0, 3, 
	23, 0, 5, 0, 0, 7, 7, 15, 
	0, 9, 0, 11, 29, 0, 13, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 9, 0, 26, 32, 
	0, 21
};

static const char _httpres_reader_eof_actions[] = {
	0, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 0
};

static const int httpres_reader_start = 1;
static const int httpres_reader_first_final = 42;
static const int httpres_reader_error = 0;

static const int httpres_reader_en_main = 1;


#line 64 "./http_response_parse.rl"



int rinoohttp_response_parse(t_rinoohttp *http)
{
	int cs = 0;
	char *p = buffer_ptr(http->response.buffer);
	char *pe = (char *) buffer_ptr(http->response.buffer) + buffer_size(http->response.buffer);
	char *eof = NULL;
	char *code_start = NULL;
	char *msg_start = NULL;
	char *msg_end = NULL;
	char *cl_start = NULL;
	char *cl_end = NULL;
	char *hd_start = NULL;
	char *hd_end = NULL;
	char *hdv_start = NULL;
	char *hdv_end = NULL;
	char tmp;

	
#line 163 "./http_response_parse.c"
	{
	cs = httpres_reader_start;
	}

#line 85 "./http_response_parse.rl"
	
#line 170 "./http_response_parse.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _httpres_reader_trans_keys + _httpres_reader_key_offsets[cs];
	_trans = _httpres_reader_index_offsets[cs];

	_klen = _httpres_reader_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _httpres_reader_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _httpres_reader_indicies[_trans];
	cs = _httpres_reader_trans_targs[_trans];

	if ( _httpres_reader_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _httpres_reader_actions + _httpres_reader_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 17 "./http_response_parse.rl"
	{ code_start = p; }
	break;
	case 1:
#line 18 "./http_response_parse.rl"
	{ msg_start = p; }
	break;
	case 2:
#line 19 "./http_response_parse.rl"
	{ msg_end = p; }
	break;
	case 3:
#line 20 "./http_response_parse.rl"
	{ cl_start = p; }
	break;
	case 4:
#line 21 "./http_response_parse.rl"
	{ cl_end = p; }
	break;
	case 5:
#line 22 "./http_response_parse.rl"
	{ hd_start = p; }
	break;
	case 6:
#line 23 "./http_response_parse.rl"
	{ hd_end = p; }
	break;
	case 7:
#line 24 "./http_response_parse.rl"
	{ hdv_start = p; }
	break;
	case 8:
#line 25 "./http_response_parse.rl"
	{
	  hdv_end = p;
	  if (hd_start != NULL && hd_end != NULL && hdv_start != NULL) {
		  tmp = *hd_end;
		  *hd_end = 0;
		  rinoohttp_header_setdata(&http->response.headers, hd_start, hdv_start, (hdv_end - hdv_start));
		  *hd_end = tmp;
	  }
  }
	break;
	case 9:
#line 34 "./http_response_parse.rl"
	{
	  http->response.code = atoi(code_start);
	  buffer_static(&http->response.msg, msg_start, msg_end - msg_start);
	  if (cl_start != NULL && cl_end != NULL)
	  {
		  tmp = *cl_end;
		  *cl_end = 0;
		  http->response.content_length = atoi(cl_start);
		  *cl_end = tmp;
	  }
	  http->response.length = p - ((char *) buffer_ptr(http->response.buffer)) + 1;
	  return 1;
  }
	break;
	case 10:
#line 47 "./http_response_parse.rl"
	{ return -1; }
	break;
	case 11:
#line 54 "./http_response_parse.rl"
	{ http->version = RINOO_HTTP_VERSION_10; }
	break;
	case 12:
#line 55 "./http_response_parse.rl"
	{ http->version = RINOO_HTTP_VERSION_11; }
	break;
#line 316 "./http_response_parse.c"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	const char *__acts = _httpres_reader_actions + _httpres_reader_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 10:
#line 47 "./http_response_parse.rl"
	{ return -1; }
	break;
#line 336 "./http_response_parse.c"
		}
	}
	}

	_out: {}
	}

#line 86 "./http_response_parse.rl"

	(void) httpres_reader_en_main;
	(void) httpres_reader_error;
	(void) httpres_reader_first_final;
	return 0;
}
