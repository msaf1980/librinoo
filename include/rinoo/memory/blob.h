/**
 * @file   blob.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Sun May  8 16:31:11 2011
 *
 * @brief  Header file for blob management
 *
 *
 */

#ifndef		RINOO_BLOB_H_
# define	RINOO_BLOB_H_

typedef struct	s_blob
{
  void		*ptr;
  size_t	size;
}		t_blob;

# define	blob_ptr(blob)			(blob)->ptr
# define	blob_size(blob)			(blob)->size
# define	blob_setsize(blob, newsize)	(blob)->size = newsize
# define	blob_static(blob, ptr, newsize)	do { blob.buf = (void *) (ptr); blob.size = newsize; } while (0)
# define	strtoblob(blob, str)		do { blob.buf = (void *) (str); blob.size = strlen((str)); } while (0)

#endif		/* !RINOO_BLOB_H_ */
