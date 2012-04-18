
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

#include	"rinoo/rinoo.h"


#line 17 "http_request_parse.c"
static const char _httpreq_reader_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 1, 8, 1, 9, 2, 0, 1
	
};

static const unsigned char _httpreq_reader_key_offsets[] = {
	0, 0, 2, 3, 4, 5, 10, 15, 
	21, 27, 33, 39, 45, 51, 57, 64, 
	69, 75, 76, 79, 83, 86, 91, 96, 
	101, 106, 111, 116, 120, 125, 130, 135, 
	140, 145, 150, 153, 157, 164, 172, 176, 
	181, 182, 183, 184, 185
};

static const char _httpreq_reader_trans_keys[] = {
	71, 80, 69, 84, 32, 32, 0, 9, 
	11, 127, 32, 0, 9, 11, 127, 32, 
	72, 0, 9, 11, 127, 32, 84, 0, 
	9, 11, 127, 32, 84, 0, 9, 11, 
	127, 32, 80, 0, 9, 11, 127, 32, 
	47, 0, 9, 11, 127, 32, 49, 0, 
	9, 11, 127, 32, 46, 0, 9, 11, 
	127, 32, 48, 49, 0, 9, 11, 127, 
	10, 13, 32, 0, 127, 10, 13, 67, 
	99, 33, 126, 10, 58, 33, 126, 32, 
	58, 33, 126, 10, 0, 127, 58, 79, 
	111, 33, 126, 58, 78, 110, 33, 126, 
	58, 84, 116, 33, 126, 58, 69, 101, 
	33, 126, 58, 78, 110, 33, 126, 58, 
	84, 116, 33, 126, 45, 58, 33, 126, 
	58, 76, 108, 33, 126, 58, 69, 101, 
	33, 126, 58, 78, 110, 33, 126, 58, 
	71, 103, 33, 126, 58, 84, 116, 33, 
	126, 58, 72, 104, 33, 126, 58, 33, 
	126, 32, 58, 33, 126, 10, 0, 47, 
	48, 57, 58, 127, 10, 13, 0, 47, 
	48, 57, 58, 127, 10, 32, 0, 127, 
	10, 13, 32, 0, 127, 79, 83, 84, 
	32, 0
};

static const char _httpreq_reader_single_lengths[] = {
	0, 2, 1, 1, 1, 1, 1, 2, 
	2, 2, 2, 2, 2, 2, 3, 3, 
	4, 1, 1, 2, 1, 3, 3, 3, 
	3, 3, 3, 2, 3, 3, 3, 3, 
	3, 3, 1, 2, 1, 2, 2, 3, 
	1, 1, 1, 1, 0
};

static const char _httpreq_reader_range_lengths[] = {
	0, 0, 0, 0, 0, 2, 2, 2, 
	2, 2, 2, 2, 2, 2, 2, 1, 
	1, 0, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 3, 3, 1, 1, 
	0, 0, 0, 0, 0
};

static const unsigned char _httpreq_reader_index_offsets[] = {
	0, 0, 3, 5, 7, 9, 13, 17, 
	22, 27, 32, 37, 42, 47, 52, 58, 
	63, 69, 71, 74, 78, 81, 86, 91, 
	96, 101, 106, 111, 115, 120, 125, 130, 
	135, 140, 145, 148, 152, 157, 163, 167, 
	172, 174, 176, 178, 180
};

static const char _httpreq_reader_indicies[] = {
	1, 2, 0, 3, 0, 4, 0, 5, 
	0, 7, 6, 6, 0, 9, 8, 8, 
	0, 9, 10, 8, 8, 0, 9, 11, 
	8, 8, 0, 9, 12, 8, 8, 0, 
	9, 13, 8, 8, 0, 9, 14, 8, 
	8, 0, 9, 15, 8, 8, 0, 9, 
	16, 8, 8, 0, 9, 17, 18, 8, 
	8, 0, 19, 20, 9, 8, 0, 21, 
	22, 24, 24, 23, 0, 21, 0, 25, 
	23, 0, 26, 25, 23, 0, 27, 26, 
	0, 25, 28, 28, 23, 0, 25, 29, 
	29, 23, 0, 25, 30, 30, 23, 0, 
	25, 31, 31, 23, 0, 25, 32, 32, 
	23, 0, 25, 33, 33, 23, 0, 34, 
	25, 23, 0, 25, 35, 35, 23, 0, 
	25, 36, 36, 23, 0, 25, 37, 37, 
	23, 0, 25, 38, 38, 23, 0, 25, 
	39, 39, 23, 0, 25, 40, 40, 23, 
	0, 41, 23, 0, 42, 25, 23, 0, 
	27, 26, 43, 26, 0, 44, 45, 26, 
	46, 26, 0, 27, 9, 8, 0, 47, 
	48, 9, 8, 0, 49, 0, 50, 0, 
	51, 0, 52, 0, 0, 0
};

static const char _httpreq_reader_trans_targs[] = {
	0, 2, 40, 3, 4, 5, 6, 7, 
	6, 7, 8, 9, 10, 11, 12, 13, 
	14, 15, 39, 16, 38, 44, 17, 18, 
	21, 19, 20, 16, 22, 23, 24, 25, 
	26, 27, 28, 29, 30, 31, 32, 33, 
	34, 35, 36, 37, 16, 20, 37, 16, 
	38, 41, 42, 43, 5
};

static const char _httpreq_reader_trans_actions[] = {
	11, 0, 0, 0, 0, 13, 1, 21, 
	0, 3, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 17, 17, 9, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 5, 7, 7, 0, 19, 
	19, 0, 0, 0, 15
};

static const char _httpreq_reader_eof_actions[] = {
	0, 11, 11, 11, 11, 11, 11, 11, 
	11, 11, 11, 11, 11, 11, 11, 11, 
	11, 11, 11, 11, 11, 11, 11, 11, 
	11, 11, 11, 11, 11, 11, 11, 11, 
	11, 11, 11, 11, 11, 11, 11, 11, 
	11, 11, 11, 11, 0
};

static const int httpreq_reader_start = 1;
static const int httpreq_reader_first_final = 44;
static const int httpreq_reader_error = 0;

static const int httpreq_reader_en_main = 1;


#line 49 "http_request_parse.rl"



int rinoohttp_request_parse(t_rinoohttp_request *req, t_buffer *buffer)
{
	int cs = 0;
	char *p = buffer_ptr(buffer);
	char *pe = buffer_ptr(buffer) + buffer_len(buffer);
	char *eof = NULL;
	char *uri_start = NULL;
	char *uri_end = NULL;
	char *cl_start = NULL;
	char *cl_end = NULL;
	char tmp;

	
#line 167 "http_request_parse.c"
	{
	cs = httpreq_reader_start;
	}

#line 65 "http_request_parse.rl"
	
#line 174 "http_request_parse.c"
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
	{
    buffer_static(req->uri, uri_start, uri_end - uri_start);
    if (cl_start != NULL && cl_end != NULL)
      {
	tmp = *cl_end;
	*cl_end = 0;
	req->content_length = atoi(cl_start);
	*cl_end = tmp;
      }
    req->length = p - buffer_ptr(buffer) + 1;
    return 1;
  }
	break;
	case 5:
#line 33 "http_request_parse.rl"
	{ return -1; }
	break;
	case 6:
#line 36 "http_request_parse.rl"
	{ req->method = RINOO_HTTP_METHOD_GET; }
	break;
	case 7:
#line 37 "http_request_parse.rl"
	{ req->method = RINOO_HTTP_METHOD_POST; }
	break;
	case 8:
#line 39 "http_request_parse.rl"
	{ req->version = RINOO_HTTP_VERSION_10; }
	break;
	case 9:
#line 40 "http_request_parse.rl"
	{ req->version = RINOO_HTTP_VERSION_11; }
	break;
#line 299 "http_request_parse.c"
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
	case 5:
#line 33 "http_request_parse.rl"
	{ return -1; }
	break;
#line 319 "http_request_parse.c"
		}
	}
	}

	_out: {}
	}

#line 66 "http_request_parse.rl"

	(void) httpreq_reader_en_main;
	(void) httpreq_reader_error;
	(void) httpreq_reader_first_final;
	return 0;
}
