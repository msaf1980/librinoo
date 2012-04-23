
#line 1 "http_request_parse.rl"
/**
 * @file   http_request_parse.rl
 * @author Reginald Lips <reginald.l@gmail.com> - Copyright 2012
 * @date   Sun Apr 15 22:29:07 2012
 *
 * @brief  HTTP request parsing
 *
 *
 */

#include "rinoo/rinoo.h"


#line 17 "http_request_parse.c"
static const char _httpreq_reader_actions[] = {
	0, 1, 0, 1, 1, 1, 4, 1, 
	5, 1, 6, 1, 7, 1, 8, 1, 
	9, 1, 10, 1, 11, 1, 12, 1, 
	13, 2, 0, 1, 2, 6, 2, 2, 
	6, 7, 2, 7, 3
};

static const unsigned char _httpreq_reader_key_offsets[] = {
	0, 0, 2, 3, 4, 5, 9, 13, 
	19, 24, 29, 34, 39, 44, 49, 54, 
	60, 64, 69, 74, 75, 78, 82, 85, 
	88, 92, 97, 102, 107, 112, 117, 122, 
	126, 131, 136, 141, 146, 151, 156, 159, 
	163, 170, 177, 181, 182, 183, 184, 185
};

static const char _httpreq_reader_trans_keys[] = {
	71, 80, 69, 84, 32, 13, 32, 0, 
	127, 13, 32, 0, 127, 13, 32, 0, 
	9, 11, 127, 13, 32, 72, 0, 127, 
	13, 32, 84, 0, 127, 13, 32, 84, 
	0, 127, 13, 32, 80, 0, 127, 13, 
	32, 47, 0, 127, 13, 32, 49, 0, 
	127, 13, 32, 46, 0, 127, 13, 32, 
	48, 49, 0, 127, 13, 32, 0, 127, 
	10, 13, 32, 0, 127, 13, 67, 99, 
	33, 126, 10, 58, 33, 126, 32, 58, 
	33, 126, 13, 0, 127, 13, 0, 127, 
	10, 13, 0, 127, 58, 79, 111, 33, 
	126, 58, 78, 110, 33, 126, 58, 84, 
	116, 33, 126, 58, 69, 101, 33, 126, 
	58, 78, 110, 33, 126, 58, 84, 116, 
	33, 126, 45, 58, 33, 126, 58, 76, 
	108, 33, 126, 58, 69, 101, 33, 126, 
	58, 78, 110, 33, 126, 58, 71, 103, 
	33, 126, 58, 84, 116, 33, 126, 58, 
	72, 104, 33, 126, 58, 33, 126, 32, 
	58, 33, 126, 13, 0, 47, 48, 57, 
	58, 127, 13, 0, 47, 48, 57, 58, 
	127, 13, 32, 0, 127, 79, 83, 84, 
	32, 0
};

static const char _httpreq_reader_single_lengths[] = {
	0, 2, 1, 1, 1, 2, 2, 2, 
	3, 3, 3, 3, 3, 3, 3, 4, 
	2, 3, 3, 1, 1, 2, 1, 1, 
	2, 3, 3, 3, 3, 3, 3, 2, 
	3, 3, 3, 3, 3, 3, 1, 2, 
	1, 1, 2, 1, 1, 1, 1, 0
};

static const char _httpreq_reader_range_lengths[] = {
	0, 0, 0, 0, 0, 1, 1, 2, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 0, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	3, 3, 1, 0, 0, 0, 0, 0
};

static const unsigned char _httpreq_reader_index_offsets[] = {
	0, 0, 3, 5, 7, 9, 13, 17, 
	22, 27, 32, 37, 42, 47, 52, 57, 
	63, 67, 72, 77, 79, 82, 86, 89, 
	92, 96, 101, 106, 111, 116, 121, 126, 
	130, 135, 140, 145, 150, 155, 160, 163, 
	167, 172, 177, 181, 183, 185, 187, 189
};

static const char _httpreq_reader_indicies[] = {
	1, 2, 0, 3, 0, 4, 0, 5, 
	0, 7, 8, 6, 0, 10, 11, 9, 
	0, 10, 11, 9, 9, 0, 10, 11, 
	12, 9, 0, 10, 11, 13, 9, 0, 
	10, 11, 14, 9, 0, 10, 11, 15, 
	9, 0, 10, 11, 16, 9, 0, 10, 
	11, 17, 9, 0, 10, 11, 18, 9, 
	0, 10, 11, 19, 20, 9, 0, 21, 
	11, 9, 0, 22, 10, 11, 9, 0, 
	23, 25, 25, 24, 0, 26, 0, 28, 
	27, 0, 29, 28, 27, 0, 31, 30, 
	0, 33, 32, 0, 22, 33, 32, 0, 
	28, 34, 34, 27, 0, 28, 35, 35, 
	27, 0, 28, 36, 36, 27, 0, 28, 
	37, 37, 27, 0, 28, 38, 38, 27, 
	0, 28, 39, 39, 27, 0, 40, 28, 
	27, 0, 28, 41, 41, 27, 0, 28, 
	42, 42, 27, 0, 28, 43, 43, 27, 
	0, 28, 44, 44, 27, 0, 28, 45, 
	45, 27, 0, 28, 46, 46, 27, 0, 
	47, 27, 0, 48, 28, 27, 0, 31, 
	30, 49, 30, 0, 50, 32, 51, 32, 
	0, 52, 11, 9, 0, 53, 0, 54, 
	0, 55, 0, 56, 0, 0, 0
};

static const char _httpreq_reader_trans_targs[] = {
	0, 2, 43, 3, 4, 5, 6, 7, 
	8, 6, 7, 8, 9, 10, 11, 12, 
	13, 14, 15, 16, 42, 17, 18, 19, 
	20, 25, 47, 20, 21, 22, 23, 24, 
	23, 24, 26, 27, 28, 29, 30, 31, 
	32, 33, 34, 35, 36, 37, 38, 39, 
	40, 41, 24, 41, 17, 44, 45, 46, 
	5
};

static const char _httpreq_reader_trans_actions[] = {
	15, 0, 0, 0, 0, 17, 1, 1, 
	25, 0, 0, 3, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 21, 0, 0, 
	5, 5, 13, 0, 7, 0, 9, 31, 
	0, 11, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 7, 
	0, 28, 34, 0, 23, 0, 0, 0, 
	19
};

static const char _httpreq_reader_eof_actions[] = {
	0, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 0
};

static const int httpreq_reader_start = 1;
static const int httpreq_reader_first_final = 47;
static const int httpreq_reader_error = 0;

static const int httpreq_reader_en_main = 1;


#line 61 "http_request_parse.rl"



int rinoohttp_request_parse(t_rinoohttp *http)
{
	int cs = 0;
	char *p = buffer_ptr(http->request.buffer);
	char *pe = buffer_ptr(http->request.buffer) + buffer_len(http->request.buffer);
	char *eof = NULL;
	char *uri_start = NULL;
	char *uri_end = NULL;
	char *cl_start = NULL;
	char *cl_end = NULL;
	char *hd_start = NULL;
	char *hd_end = NULL;
	char *hdv_start = NULL;
	char *hdv_end = NULL;
	char tmp;

	
#line 175 "http_request_parse.c"
	{
	cs = httpreq_reader_start;
	}

#line 81 "http_request_parse.rl"
	
#line 182 "http_request_parse.c"
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
	_keys = _httpreq_reader_trans_keys + _httpreq_reader_key_offsets[cs];
	_trans = _httpreq_reader_index_offsets[cs];

	_klen = _httpreq_reader_single_lengths[cs];
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
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _httpreq_reader_range_lengths[cs];
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
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _httpreq_reader_indicies[_trans];
	cs = _httpreq_reader_trans_targs[_trans];

	if ( _httpreq_reader_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _httpreq_reader_actions + _httpreq_reader_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 17 "http_request_parse.rl"
	{ uri_start = p; }
	break;
	case 1:
#line 18 "http_request_parse.rl"
	{ uri_end = p; }
	break;
	case 2:
#line 19 "http_request_parse.rl"
	{ cl_start = p; }
	break;
	case 3:
#line 20 "http_request_parse.rl"
	{ cl_end = p; }
	break;
	case 4:
#line 21 "http_request_parse.rl"
	{ hd_start = p; }
	break;
	case 5:
#line 22 "http_request_parse.rl"
	{ hd_end = p; }
	break;
	case 6:
#line 23 "http_request_parse.rl"
	{ hdv_start = p; }
	break;
	case 7:
#line 24 "http_request_parse.rl"
	{
	  hdv_end = p;
	  if (hd_start != NULL && hd_end != NULL && hdv_start != NULL) {
		  tmp = *hd_end;
		  *hd_end = 0;
		  rinoohttp_header_setdata(&http->request.headers, hd_start, hdv_start, (hdv_end - hdv_start));
		  *hd_end = tmp;
	  }
  }
	break;
	case 8:
#line 33 "http_request_parse.rl"
	{
    buffer_static(http->request.uri, uri_start, uri_end - uri_start);
    if (cl_start != NULL && cl_end != NULL)
      {
	tmp = *cl_end;
	*cl_end = 0;
	http->request.content_length = atoi(cl_start);
	*cl_end = tmp;
      }
    http->request.length = p - buffer_ptr(http->request.buffer) + 1;
    return 1;
  }
	break;
	case 9:
#line 45 "http_request_parse.rl"
	{ return -1; }
	break;
	case 10:
#line 48 "http_request_parse.rl"
	{ http->request.method = RINOO_HTTP_METHOD_GET; }
	break;
	case 11:
#line 49 "http_request_parse.rl"
	{ http->request.method = RINOO_HTTP_METHOD_POST; }
	break;
	case 12:
#line 51 "http_request_parse.rl"
	{ http->version = RINOO_HTTP_VERSION_10; }
	break;
	case 13:
#line 52 "http_request_parse.rl"
	{ http->version = RINOO_HTTP_VERSION_11; }
	break;
#line 331 "http_request_parse.c"
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
	const char *__acts = _httpreq_reader_actions + _httpreq_reader_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 9:
#line 45 "http_request_parse.rl"
	{ return -1; }
	break;
#line 351 "http_request_parse.c"
		}
	}
	}

	_out: {}
	}

#line 82 "http_request_parse.rl"

	(void) httpreq_reader_en_main;
	(void) httpreq_reader_error;
	(void) httpreq_reader_first_final;
	return 0;
}
