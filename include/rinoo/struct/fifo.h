/**
 * @file   fifo.h
 * @author Reginald LIPS <reginald.l@gmail.com> - Copyright 2011
 * @date   Wed Dec 30 17:05:32 2009
 *
 * @brief  Header file for fifo functions declarations.
 *
 *
 */

#ifndef		RINOO_FIFO_H_
# define	RINOO_FIFO_H_

typedef struct		s_fifonode
{
  void			*node;
  void			(*free_func)();
  struct s_fifonode	*next;
}			t_fifonode;

typedef struct	s_fifo
{
  t_fifonode	*head;
  t_fifonode	*tail;
  u32		size;
}		t_fifo;

t_fifo		*fifo_create();
void		fifo_destroy(void *ptr);
int		fifo_push(t_fifo *fifo, void *node,
			  void (*free_func)(void *node));
void		*fifo_pop(t_fifo *fifo);
void		*fifo_get(t_fifo *fifo);

#endif		/* !RINOO_FIFO_H_ */

