 
INCLUDES = -If123inc

# define salient files for this directory

FILES = a_toe		\
	bak_fld		\
	bak_rec		\
	bak_sfld	\
	beg_ddre	\
	beg_file	\
	beg_rec		\
	bld_fmt		\
	bld_lab		\
	bldadscr	\
	c_dddir		\
	c_ddlead	\
	c_drdir		\
	c_drlead	\
	ch_size		\
	chk_fld		\
	chk_nfld	\
	chk_nrec	\
	chk_nsfl	\
	chk_rec		\
	chk_sfld	\
	cki_nfld	\
	conv_cat	\
	del_dim		\
	del_fmt		\
	del_labs	\
	deldrval	\
	e_toa		\
	end_ddre	\
	end_file	\
	end_rec		\
	er_ddfld	\
	er_ddrec	\
	er_ddsfl	\
	free_lab	\
	g_order		\
	gbstr		\
	gdstr		\
	get_dim		\
	get_dval	\
	get_fmt		\
	get_lvl		\
	gfstr		\
	gint		\
	gsstr		\
	gstr		\
	gt_adscr	\
	i_toa		\
	incr_lab	\
	is_adscr	\
	is_intgr	\
	l_tos		\
	ld_ddrec	\
	ld_rec		\
	ld_tagp		\
	load_fld	\
	load_fmt	\
	load_lab	\
	out_fmt		\
	pop_rs		\
	push_rs		\
	r_dddir		\
	r_ddlead	\
	r_drdir		\
	r_drlead	\
	rd_ddfld	\
	rd_ddrec	\
	rd_ddsfl	\
	rd_fld		\
	rd_rec		\
	rd_sfld		\
	ret_dv		\
	ret_fv		\
	ret_matc	\
	ret_pdm		\
	rt_pvfld	\
	rt_pvsfl	\
	s_tol		\
	set_stat	\
	setup_lb	\
	stc_empt	\
	stor_dv		\
	str_tok		\
	uld_ddre	\
	uld_rec		\
	v_ddtag		\
	v_drtag		\
	wint		\
	wr_ddfld	\
	wr_ddrec	\
	wr_ddsfl	\
	wr_fld		\
	wr_rec		\
	wr_sfld		
 

# define source code, header, and object file names

SRCS = $(addsuffix .c, $(FILES) )
OBJS = $(addsuffix .o, $(FILES) )


# include common variables and production rules

include common.mk


# update the Access/E library with any modified object files

$(FIPSLIB) : $(OBJS)
	ar cr fips123.a $? $(REPOSITORY_OBJ_FILES)
	$(RANLIB) $@

clean:
	rm -f *.o
